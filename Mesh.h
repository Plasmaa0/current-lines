#pragma once

#include <string>
#include <vector>

class Mesh {
public:
    void LoadFile(const std::string &filename);

private:
    struct MeshFormat {
        float version; // version of .msh format
        int file_type; // must be 0 ??
        int data_size; // size of float
    };
    struct Node {
        uint id;
        double x;
        double y;
        double z;
    };
    struct Element {
        enum class ElementType {
            Triangle,
            Quadrangle,
            Tetrahedron,
            Hexahedron,
            Prism,
            Pyramid,
        };
        uint id;
        ElementType type;
        // skip tags
        std::vector<uint> nodes;
    };
    struct NodeData {
        std::vector<std::string> str_tags;
        std::vector<double> real_tags;
        std::vector<int> int_tags;
        struct NodeValues{
            uint node_id;
            std::vector<double> values;
        };
        std::vector<NodeValues> node_values;
    };

    MeshFormat meshFormat;
    std::vector<Node> nodes;
    std::vector<Element> elements;
    NodeData nodeData;

    void ParseMeshFormatLine(const std::string &line);

    void ParseNodesLine(const std::string &line);

    void ParseElementsLine(const std::string &line);

    void ParseNodeData(std::vector<std::string> &&NodeDataBlock);
};
