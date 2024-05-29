#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <algorithm>
#include "Utils.h"

enum class InputType {
    None,
    MeshFormat,
    Nodes,
    Elements,
    NodeData
};

void Mesh::LoadFile(const std::string &filename) {
    std::cout << "loading file: " <<  filename << std::endl;
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
                std::cout << "parsing None" << std::endl;
            } else if (line == "$MeshFormat") {
                input_type = InputType::MeshFormat;
                std::cout << "parsing MeshFormat" << std::endl;
            } else if (line == "$Nodes") {
                input_type = InputType::Nodes;
                std::cout << "parsing Nodes" << std::endl;
            } else if (line == "$Elements") {
                input_type = InputType::Elements;
                std::cout << "parsing Elements" << std::endl;
            } else if (line == "$NodeData") {
                input_type = InputType::NodeData;
                std::cout << "parsing NodeData" << std::endl;
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
                break;
        }
    }
    std::cout << "NodeData parsing" << std::endl;
    ParseNodeData(std::move(NodeDataBlock));
    std::cout << "Calculating bounding box" << std::endl;
    CalculateBoundingBox();
    std::cout << "Bounding Box X from " << boundingBox.x_min() << " to " << boundingBox.x_max() << std::endl <<
    "Y from " << boundingBox.y_min() << " to " << boundingBox.y_max() << std::endl;
    std::cout << "file: " <<  filename << " loaded" << std::endl;
}

void Mesh::CalculateBoundingBox(){
    boundingBox.x_range = {0,0};
    boundingBox.y_range = {0,0};
    std::for_each(nodes.begin(), nodes.end(), [&](const Node& node){
        if (std::get<0>(boundingBox.x_range) > node.coords.x) {
            std::get<0>(boundingBox.x_range) = node.coords.x;
        }
        if (std::get<0>(boundingBox.y_range) > node.coords.y) {
            std::get<0>(boundingBox.y_range) = node.coords.y;
        }
        if (std::get<1>(boundingBox.x_range) < node.coords.x) {
            std::get<1>(boundingBox.x_range) = node.coords.x;
        }
        if (std::get<1>(boundingBox.y_range) < node.coords.y) {
            std::get<1>(boundingBox.y_range) = node.coords.y;
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
    nodes.push_back(Node(id, x, y, z));
}

void Mesh::ParseElementsLine(const std::string &line) {
    // elm-number elm-type number-of-tags < tag > … node-number-list
    //                     _______skipping this_____
    Element elem;
    std::istringstream iss(line);
    iss >> elem.id;
    int type_id;
    iss >> type_id;
    uint nodes_to_get = 0;
    switch (type_id) {
        case 2: // 3 node triangle
            elem.type = Element::ElementType::Triangle;
            nodes_to_get = 3;
            break;
        case 3: // 4 node quadrangle
            elem.type = Element::ElementType::Quadrangle;
            nodes_to_get = 4;
            break;
        case 4: // 4 node tetrahedron
            elem.type = Element::ElementType::Tetrahedron;
            nodes_to_get = 4;
            break;
        case 5: // 8 node hexahedron
            elem.type = Element::ElementType::Hexahedron;
            nodes_to_get = 8;
            break;
        case 6: // 6 node prism
            elem.type = Element::ElementType::Prism;
            nodes_to_get = 6;
            break;
        case 7: // 5 node pyramid
            elem.type = Element::ElementType::Pyramid;
            nodes_to_get = 5;
            break;
        default:
            NOT_IMPLEMENTED;
            break;
    }
    for (int i = 0; i < nodes_to_get; ++i) {
        uint node_id;
        iss >> node_id;
        elem.nodes.push_back(node_id);
        // nodes_for_cell.push_back();
    }

    if(elem.type != Element::ElementType::Quadrangle){
        NOT_IMPLEMENTED; // пока что адаптируюсь под говнокод девочки
    }
    std::vector<Node> nodes_for_cell;
    // взять те ноды id которых находятся в elem.nodes
    std::copy_if(nodes.begin(), nodes.end(), std::back_inserter(nodes_for_cell), [&elem](const Node& node){
                 return std::find(elem.nodes.begin(), elem.nodes.end(), node.id) != elem.nodes.end();
             });
    elem.cell = Cell::create(nodes_for_cell); 
    elements.push_back(std::move(elem));
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
        nodeData.str_tags.push_back(std::move(pop_str()));
    }
    auto num_real_tags = pop_int();
    for (int i = 0; i < num_real_tags; ++i) {
        nodeData.real_tags.push_back(pop_real());
    }
    auto num_int_tags = pop_int();
    for (int i = 0; i < num_int_tags; ++i) {
        nodeData.int_tags.push_back(pop_int());
    }
    // main part
    while (not NodeDataBlock.empty()) {
        auto &&line = pop_str();
        std::istringstream iss(line);
        uint node_id;
        iss >> node_id;
        std::vector<double> values;
        std::copy(std::istream_iterator<double>(iss), std::istream_iterator<double>(), std::back_inserter(values));
        // nodeData.node_values.push_back(NodeData::NodeValues(node_id, std::move(values)));
        // TODO: проверить точно ли values всего 3 штуки всегда?
        // std::cout << node_id << std::endl;
        auto &node_by_id  = *std::find_if(nodes.begin(), nodes.end(), [&node_id](const Node& node){return node.id==node_id;});
        node_by_id.vector_field.set_coords(Coords(values[0],values[1],values[2]));
    }
}
