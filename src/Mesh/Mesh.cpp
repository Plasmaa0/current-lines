#include "Mesh.h"

#include <Utils/Nimpl.h>
#include "Element/Common/ElementFactory.h"

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>
#include <iostream>
#include <iterator>

enum class InputType {
    None,
    MeshFormat,
    Nodes,
    Elements,
    NodeData
};

void Mesh::LoadFile(const std::string &filename) {
    initializeElementTypeLUT();
    std::cout << "loading file: " << filename << std::endl;
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
                std::cout << "done" << std::endl;
            } else if (line.starts_with("$MeshFormat")) {
                input_type = InputType::MeshFormat;
                std::cout << "parsing MeshFormat ... ";
            } else if (line.starts_with("$Nodes")) {
                input_type = InputType::Nodes;
                std::cout << "parsing Nodes ... ";
            } else if (line.starts_with("$Elements")) {
                input_type = InputType::Elements;
                std::cout << "parsing Elements ... ";
            } else if (line.starts_with("$NodeData")) {
                input_type = InputType::NodeData;
                std::cout << "parsing NodeData ... ";
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
                if (line.contains(' ')) {
                    ParseNodesLine(line);
                }
                break;

            case InputType::Elements:
                // if line does not contain space therefore it's first of '$Nodes' block with total number of nodes
                // and total number of nodes will be calculated during node parsing, so we'll skip first line
                if (line.contains(' ')) {
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
    std::cout << "NodeData parsing" << std::endl;
    ParseNodeData(std::move(NodeDataBlock));
    std::cout << "Calculating bounding box" << std::endl;
    CalculateBoundingBox();
    std::cout << "Bounding Box X from " << boundingBox.x_min() << " to " << boundingBox.x_max() << std::endl <<
              "Y from " << boundingBox.y_min() << " to " << boundingBox.y_max() << std::endl;
    std::cout << "file: " << filename << " loaded" << std::endl;
}

constexpr void Mesh::initializeElementTypeLUT() {
    elementTypeLUT[2] = {FE::Element::Type::Triangle, 3}; // 3 node triangle
    elementTypeLUT[3] = {FE::Element::Type::Quadrangle, 4}; // 4 node quadrangle
    elementTypeLUT[4] = {FE::Element::Type::Tetrahedron, 4}; // 4 node tetrahedron
    elementTypeLUT[5] = {FE::Element::Type::Hexahedron, 8}; // 8 node hexahedron
    elementTypeLUT[6] = {FE::Element::Type::Prism, 6}; // 6 node prism
    elementTypeLUT[7] = {FE::Element::Type::Pyramid, 5}; // 5 node pyramid

}

void Mesh::CalculateBoundingBox() {
    boundingBox.xRange = {0, 0};
    boundingBox.yRange = {0, 0};
    std::for_each(nodes.begin(), nodes.end(), [&](const Node &node) {
        if (std::get<0>(boundingBox.xRange) > node.coords.x) {
            std::get<0>(boundingBox.xRange) = node.coords.x;
        }
        if (std::get<0>(boundingBox.yRange) > node.coords.y) {
            std::get<0>(boundingBox.yRange) = node.coords.y;
        }
        if (std::get<1>(boundingBox.xRange) < node.coords.x) {
            std::get<1>(boundingBox.xRange) = node.coords.x;
        }
        if (std::get<1>(boundingBox.yRange) < node.coords.y) {
            std::get<1>(boundingBox.yRange) = node.coords.y;
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
        return;
        NOT_IMPLEMENTED;
    }
    auto &[elementType, nodes_to_get] = elementTypeLUT[type_id];
    std::vector<std::reference_wrapper<Node>> elementNodes;
//    std::cout << "FE size: " << nodes_to_get << " nodes." << std::endl;
    for (int i = 0; i < nodes_to_get; ++i) {
        uint node_id;
        iss >> node_id;
        elementNodes.push_back(std::ref(nodes[node_id - 1]));
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
        return std::move(str);
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
        nodes[node_id - 1].vector_field.set_coords(Coords(values[0], values[1], values[2]));
    }
}

std::optional<std::shared_ptr<FE::Element>> Mesh::findElementByNode(const Node &node_p) const {
    for (const auto &element: elements) {
        if (element->contains_node(node_p)) {
            return std::ref(element);
        }
    }
    std::cout << "current cell not found" << std::endl << std::flush;
    return std::nullopt;
}