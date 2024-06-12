#pragma once

#include <Geometry/Node.h>
#include <Geometry/BoundingBox.h>
#include "Element/Common/Element.h"

#include <vector>
#include <string>
#include <cstdlib>
#include <optional>
#include <memory>
#include <ostream>

class Mesh {
public:
    struct MeshFormat {
        float version; // version of .msh format
        int file_type; // must be 0 ??
        int data_size; // size of float
    };

    struct NodeData {
        std::vector<std::string> strTags;
        std::vector<double> realTags;
        std::vector<int> intTags;

        struct NodeValue {
            uint node_id;
            std::vector<double> values;
        };

        std::vector<NodeValue> nodeValues;
    };

    void LoadFile(const std::string &filename);

    [[nodiscard]] inline const BoundingBox &getBoundingBox() const { return boundingBox; }

    [[nodiscard]] inline const std::vector<std::shared_ptr<FE::Element> > &getElements() const { return elements; }

    [[nodiscard]] std::optional<std::shared_ptr<FE::Element> > findElementByNode(const Node &node_p) const;

private:
    MeshFormat meshFormat;
    std::vector<Node> nodes;
    std::vector<std::shared_ptr<FE::Element> > elements;
    NodeData nodeData;
    BoundingBox boundingBox;
    mutable std::unordered_map<uint, std::pair<FE::Element::Type, uint> > elementTypeLUT;
    mutable std::unordered_map<FE::Element::Type, std::string> elementTypeNameLUT;

    void ParseMeshFormatLine(const std::string &line);

    void ParseNodesLine(const std::string &line);

    void ParseElementsLine(const std::string &line);

    void ParseNodeData(std::vector<std::string> &&NodeDataBlock);

    void CalculateBoundingBox();

    constexpr void initializeLUTs() const;

    friend std::ostream &operator<<(std::ostream &os, const Mesh &obj);
};

DEFINE_FORMATTER(Mesh)
