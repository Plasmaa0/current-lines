#pragma once

#include <string>
#include <vector>
#include "Node.h"
#include "Cell.h"

class Mesh {
public:
    struct MeshFormat {
        float version; // version of .msh format
        int file_type; // must be 0 ??
        int data_size; // size of float
    };
    // struct Node {
    //     uint id;
    //     double x;
    //     double y;
    //     double z;
    // };
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
        Cell cell;
    };
    struct BoundingBox{
          std::pair<double, double> x_range; // min-max
          std::pair<double, double> y_range; // min-max
          double x_min() const {return std::get<0>(x_range);}
          double x_max() const {return std::get<1>(x_range);}
          double y_min() const {return std::get<0>(y_range);}
          double y_max() const {return std::get<1>(y_range);}
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

private:
    MeshFormat meshFormat;
    std::vector<Node> nodes;
    std::vector<Element> elements;
    NodeData nodeData;
    BoundingBox boundingBox;

    void ParseMeshFormatLine(const std::string &line);

    void ParseNodesLine(const std::string &line);

    void ParseElementsLine(const std::string &line);

    void ParseNodeData(std::vector<std::string> &&NodeDataBlock);

    void CreateCells();

    void CalculateBoundingBox();
public:
    void LoadFile(const std::string &filename);

    std::vector<Cell> getCells() const;

    inline const BoundingBox& getBoundingBox() const {return boundingBox; }

};
