#include "Mesh.h"

#include <Utils/Nimpl.h>
#include "Element/Common/ElementFactory.h"

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>
#include <complex>
#include <iostream>
#include <iterator>

#include "Utils/Logger.h"

enum class InputType {
    None,
    MeshFormat,
    Nodes,
    Elements,
    NodeData
};

void Mesh::LoadFile(const std::string &filename) {
    initializeLUTs();
    LOG_INFO("Loading file '{}'", filename);
    std::ifstream file(filename);
    if (not file.is_open()) {
        throw std::invalid_argument("FILE NOT OPEN");
    }

    auto input_type = InputType::None;
    std::string line;
    std::vector<std::string> NodeDataBlock;
    while (std::getline(file, line)) {
        //        std::istringstream iss(line);

        // update input mode
        if (line.starts_with('$')) {
            if (line.starts_with("$End")) {
                input_type = InputType::None;
                LOG_DEBUG("done");
            } else if (line.starts_with("$MeshFormat")) {
                input_type = InputType::MeshFormat;
                LOG_DEBUG("parsing MeshFormat ... ");
            } else if (line.starts_with("$Nodes")) {
                input_type = InputType::Nodes;
                LOG_DEBUG("parsing Nodes ... ");
            } else if (line.starts_with("$Elements")) {
                input_type = InputType::Elements;
                LOG_DEBUG("parsing Elements ... ");
            } else if (line.starts_with("$NodeData")) {
                input_type = InputType::NodeData;
                LOG_DEBUG("parsing NodeData ... ");
            }
            continue;
        }

        // parse line
        switch (input_type) {
            case InputType::MeshFormat:
                ParseMeshFormatLine(line);
                break;

            case InputType::Nodes:
                // if line does not contain space therefore it's first of '$Nodes' block with total number of nodes
                // and total number of nodes will be calculated during node parsing, so we'll skip first line
                if (line.contains(' ') or line.contains('\t')) {
                    ParseNodesLine(line);
                }
                break;

            case InputType::Elements:
                // if line does not contain space therefore it's first of '$Nodes' block with total number of nodes
                // and total number of nodes will be calculated during node parsing, so we'll skip first line
                if (line.contains(' ') or line.contains('\t')) {
                    ParseElementsLine(line);
                }
                break;

            case InputType::NodeData:
                NodeDataBlock.push_back(line); // we'll parse that later
                break;

            case InputType::None:
                break;

            default:
                std::unreachable();
        }
    }
    LOG_INFO("Loaded {} nodes, {} elements", nodes.size(), elements.size());
    LOG_DEBUG("NodeData parsing");
    ParseNodeData(std::move(NodeDataBlock));
    LOG_DEBUG("Calculating bounding box");
    CalculateBoundingBox();
    LOG_DEBUG("Calculated bounding box: {}", getBoundingBox());
    uint rTreeLevels = std::max(static_cast<uint>(std::round(std::log(elements.size()) / log(10))), 1u);
    LOG_INFO("Building R-tree with {} levels", rTreeLevels);
    elementRTree = ElementRTree(elements);
    elementRTree.subdivide(rTreeLevels);
    elementRTree.subdivideToBatchSize(200);
    elementRTree.saveToMSH("rtree.geo");
    // elementRTree.print();
    LOG_INFO("File '{}' loaded", filename);
}

constexpr void Mesh::initializeLUTs() const {
    elementTypeLUT[2] = {FE::Element::Type::Triangle, 3}; // 3 node triangle
    elementTypeLUT[3] = {FE::Element::Type::Quadrangle, 4}; // 4 node quadrangle
    elementTypeLUT[4] = {FE::Element::Type::Tetrahedron, 4}; // 4 node tetrahedron
    elementTypeLUT[5] = {FE::Element::Type::Hexahedron, 8}; // 8 node hexahedron
    elementTypeLUT[6] = {FE::Element::Type::Prism, 6}; // 6 node prism
    // elementTypeLUT[7] = {FE::Element::Type::Pyramid, 5}; // 5 node pyramid

    elementTypeNameLUT[FE::Element::Type::Triangle] = "Triangle";
    elementTypeNameLUT[FE::Element::Type::Quadrangle] = "Quadrangle";
    elementTypeNameLUT[FE::Element::Type::Tetrahedron] = "Tetrahedron";
    elementTypeNameLUT[FE::Element::Type::Hexahedron] = "Hexahedron";
    elementTypeNameLUT[FE::Element::Type::Prism] = "Prism";
    // elementTypeNameLUT[FE::Element::Type::Pyramid] = "Pyramid";
}

void Mesh::CalculateBoundingBox() {
    boundingBox.xRange = {0, 0};
    boundingBox.yRange = {0, 0};
    boundingBox.zRange = {0, 0};
    std::for_each(nodes.begin(), nodes.end(), [&](const Node &node) {
        if (std::get<0>(boundingBox.xRange) > node.coords.x) {
            std::get<0>(boundingBox.xRange) = node.coords.x;
        }
        if (std::get<0>(boundingBox.yRange) > node.coords.y) {
            std::get<0>(boundingBox.yRange) = node.coords.y;
        }
        if (std::get<0>(boundingBox.zRange) > node.coords.z) {
            std::get<0>(boundingBox.zRange) = node.coords.z;
        }
        if (std::get<1>(boundingBox.xRange) < node.coords.x) {
            std::get<1>(boundingBox.xRange) = node.coords.x;
        }
        if (std::get<1>(boundingBox.yRange) < node.coords.y) {
            std::get<1>(boundingBox.yRange) = node.coords.y;
        }
        if (std::get<1>(boundingBox.zRange) < node.coords.z) {
            std::get<1>(boundingBox.zRange) = node.coords.z;
        }
    });
}

void Mesh::ParseMeshFormatLine(const std::string &line) {
    // version-number file-type data-size
    std::istringstream iss(line);
    float version; // version of .msh format
    int file_type; // must be 0 ??
    int data_size; // size of float
    iss >> version >> file_type >> data_size;
    this->meshFormat = {version, file_type, data_size};
}

void Mesh::ParseNodesLine(const std::string &line) {
    // node-number x-coord y-coord z-coord
    std::istringstream iss(line);
    uint id;
    double x, y, z;
    iss >> id >> x >> y >> z;
    nodes.emplace_back(id, x, y, z);
}

void Mesh::ParseElementsLine(const std::string &line) {
    // elm-number elm-type number-of-tags < tag > … node-number-list
    //                     _______skipping this_____
    std::istringstream iss(line);
    uint elementID;
    iss >> elementID;
    uint type_id;
    iss >> type_id;
    //    uint nodes_to_get = 0;
    //    FE::Type elementType;
    int number_of_tags = -1;
    iss >> number_of_tags;
    if (number_of_tags != 0) {
        int tag;
        for (int i = 0; i < number_of_tags; ++i) {
            iss >> tag;
        }
        //        NOT_IMPLEMENTED;
    }

    if (not elementTypeLUT.contains(type_id)) {
        LOG_WARNING("Encountered unsupported element type. id={}", type_id);
        return; // TODO скип неподдерживаемых типов элементов
        NOT_IMPLEMENTED;
    }
    auto &[elementType, nodes_to_get] = elementTypeLUT[type_id];
    std::vector<std::reference_wrapper<Node> > elementNodes;
    for (uint i = 0; i < nodes_to_get; ++i) {
        uint node_id;
        iss >> node_id;
        elementNodes.push_back(std::ref(getNodeById(node_id))); // TODO делать [node_id-1] если нумерация от 1
    }

    elements.push_back(FE::Factory::createElement(elementNodes, elementID, elementType));
}

void Mesh::ParseNodeData(std::vector<std::string> &&NodeDataBlock) {
    // number-of-string-tags
    //< "string-tag" >
    //…

    //number-of-real-tags
    //< real-tag >
    //…

    //number-of-integer-tags
    //< integer-tag >
    //…

    //node-number value …
    //…
    std::reverse(NodeDataBlock.begin(), NodeDataBlock.end());
    auto pop_real = [&NodeDataBlock]() {
        auto real = std::stod(NodeDataBlock.back());
        NodeDataBlock.pop_back();
        return real;
    };
    auto pop_str = [&NodeDataBlock]() {
        auto str = NodeDataBlock.back();
        NodeDataBlock.pop_back();
        return str;
    };
    auto pop_int = [&NodeDataBlock]() {
        auto num = std::stoi(NodeDataBlock.back());
        NodeDataBlock.pop_back();
        return num;
    };
    auto num_str_tags = pop_int();
    for (int i = 0; i < num_str_tags; ++i) {
        nodeData.strTags.push_back(std::move(pop_str()));
    }
    auto num_real_tags = pop_int();
    for (int i = 0; i < num_real_tags; ++i) {
        nodeData.realTags.push_back(pop_real());
    }
    auto num_int_tags = pop_int();
    for (int i = 0; i < num_int_tags; ++i) {
        nodeData.intTags.push_back(pop_int());
    }
    // main part
    while (not NodeDataBlock.empty()) {
        auto &&line = pop_str();
        std::istringstream iss(line);
        uint node_id;
        iss >> node_id;
        std::vector<double> values;
        std::copy(std::istream_iterator<double>(iss), std::istream_iterator<double>(), std::back_inserter(values));
        // nodeData.nodeValues.push_back(NodeData::NodeValue(node_id, std::move(values)));
        // TODO: проверить точно ли values всего 3 штуки всегда?
        // LOG_DEBUG("node id: {}", node_id);
        getNodeById(node_id).vector_field.coords = Coords(values[0], values[1], values[2]);
        // TODO делать [node_id-1] если нумерация от 1
    }
}

std::optional<std::shared_ptr<FE::Element> > Mesh::findElementByNode(const Node &node_p) const {
    return elementRTree.findElementContainingNode(node_p);
    for (const auto &element: elements) {
        if (element->contains_node(node_p)) {
            return std::ref(element);
        }
    }
    LOG_WARNING("current cell not found");
    return std::nullopt;
}

const Node &Mesh::getNodeById(uint p_id) const noexcept {
    auto node = std::ranges::lower_bound(nodes, Node(p_id, {}),
                                         [](const Node &a, const Node &b) { return a.id < b.id; });
    if (node == nodes.cend()) {
        LOG_ERROR("Не существует узла с id {}", p_id);
        throw std::out_of_range(std::format("Не существует узла с id {}", p_id));
    }
    return std::ref(*node);
}

Node &Mesh::getNodeById(uint p_id) noexcept {
    auto node = std::ranges::lower_bound(nodes, Node(p_id, {}),
                                         [](const Node &a, const Node &b) { return a.id < b.id; });
    if (node == nodes.end()) {
        LOG_ERROR("Не существует узла с id {}", p_id);
        throw std::out_of_range(std::format("Не существует узла с id {}", p_id));
    }
    return std::ref(*node);
}

std::optional<std::shared_ptr<FE::Element> > Mesh::getElementContainingNodeId(uint nodeId_p) const {
    auto res = std::ranges::find_if(elements,
                                    [nodeId_p](const std::shared_ptr<FE::Element> &element) {
                                        const auto &nodes = element->getNodes();
                                        return std::ranges::any_of(
                                            nodes, [nodeId_p](const Node &node) { return node.id == nodeId_p; });
                                    });
    if (res != elements.cend())
        return {*res};
    return std::nullopt;
}

std::ostream &operator<<(std::ostream &os, const Mesh &obj) {
    return os << std::format("Mesh(Nodes={}, Elements={}, Type={}, {})", obj.nodes.size(), obj.elements.size(),
                             obj.elementTypeNameLUT[obj.elements[0]->getType()], obj.getBoundingBox());
}
