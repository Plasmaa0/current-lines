#pragma once

#include <Geometry/Node.h>
#include <Geometry/Element.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <optional>

class Mesh {
public:
    struct MeshFormat {
        float version; // version of .msh format
        int file_type; // must be 0 ??
        int data_size; // size of float
    };

    struct BoundingBox {
        std::pair<double, double> x_range; // min-max
        std::pair<double, double> y_range; // min-max
        [[nodiscard]] double x_min() const { return std::get<0>(x_range); }

        [[nodiscard]] double x_max() const { return std::get<1>(x_range); }

        [[nodiscard]] double y_min() const { return std::get<0>(y_range); }

        [[nodiscard]] double y_max() const { return std::get<1>(y_range); }
    };

    struct NodeData {
        std::vector<std::string> str_tags;
        std::vector<double> real_tags;
        std::vector<int> int_tags;
        struct NodeValues {
            uint node_id;
            std::vector<double> values;
        };
        std::vector<NodeValues> node_values;
    };

    void LoadFile(const std::string &filename);

    [[nodiscard]] inline const BoundingBox &getBoundingBox() const { return boundingBox; }

    [[nodiscard]] inline const std::vector<Element> &getElement() const { return elements; }

    [[nodiscard]] std::optional<std::reference_wrapper<const Element>> findElementByNode(const Node &node_p) const;

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

    void CalculateBoundingBox();

};
