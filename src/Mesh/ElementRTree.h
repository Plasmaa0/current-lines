#pragma once
#include <memory>

#include "Element/Common/Element.h"
#include "Geometry/BoundingBox.h"

class ElementRTree {
    BoundingBox boundingBox = BoundingBox({0, 0}, {0, 0}, {0, 0});
    std::unique_ptr<ElementRTree> left = nullptr;
    std::unique_ptr<ElementRTree> right = nullptr;
    std::vector<std::shared_ptr<FE::Element> > elements;

public:
    ElementRTree();

    explicit ElementRTree(const std::vector<std::shared_ptr<FE::Element> > &elements);

    void subdivide();

    void subdivide(uint level);

    void subdivideToBatchSize(size_t batchSize);

    [[nodiscard]] std::optional<std::shared_ptr<FE::Element> > findElementContainingNode(const Node &node) const;

    [[nodiscard]] int saveToMSH(const std::string &filename, int boxNumber = 0) const;

    void print() const;

    void print_nodes(const std::string &padding, const std::string &edge, ElementRTree *node,
                     bool has_left_sibling) const;

    size_t children() const;
};
