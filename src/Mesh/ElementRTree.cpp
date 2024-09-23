#include "ElementRTree.h"

#include <fstream>

#include "Utils/Logger.h"

ElementRTree::ElementRTree() {
}

ElementRTree::ElementRTree(const std::vector<std::shared_ptr<FE::Element> > &elements)
    : elements(elements) {
    for (auto &element: elements) {
        boundingBox += element->getBoundingBox();
    }
}

void ElementRTree::subdivide() {
    if (left != nullptr) {
        left->subdivide();
    }
    if (right != nullptr) {
        right->subdivide();
    }
    if (elements.empty()) {
        return;
    }
    auto sizeX = boundingBox.size_x();
    auto sizeY = boundingBox.size_y();
    auto sizeZ = boundingBox.size_z();
    BoundingBox leftBoundingBox = BoundingBox({0, 0}, {0, 0}, {0, 0});
    BoundingBox rightBoundingBox = BoundingBox({0, 0}, {0, 0}, {0, 0});
    // determine subdivision side (the longest one)
    if (sizeX >= sizeY and sizeX >= sizeZ) {
        //subdivide by side X
        leftBoundingBox = BoundingBox(
            {boundingBox.x_min(), boundingBox.x_min() + boundingBox.size_x() / 2},
            boundingBox.yRange,
            boundingBox.zRange);
        rightBoundingBox = BoundingBox(
            {boundingBox.x_min() + boundingBox.size_x() / 2, boundingBox.x_max()},
            boundingBox.yRange,
            boundingBox.zRange);
    } else if (sizeY >= sizeX and sizeY >= sizeZ) {
        //subdivide by side Y
        leftBoundingBox = BoundingBox(
            boundingBox.xRange,
            {boundingBox.y_min(), boundingBox.y_min() + boundingBox.size_y() / 2},
            boundingBox.zRange);
        rightBoundingBox = BoundingBox(
            boundingBox.xRange,
            {boundingBox.y_min() + boundingBox.size_y() / 2, boundingBox.y_max()},
            boundingBox.zRange);
    } else if (sizeZ >= sizeX and sizeZ >= sizeY) {
        //subdivide by side Z
        leftBoundingBox = BoundingBox(
            boundingBox.xRange,
            boundingBox.yRange,
            {boundingBox.z_min(), boundingBox.z_min() + boundingBox.size_z() / 2});
        rightBoundingBox = BoundingBox(
            boundingBox.xRange,
            boundingBox.yRange,
            {boundingBox.z_min() + boundingBox.size_z() / 2, boundingBox.z_max()});
    }
    std::vector<std::shared_ptr<FE::Element> > leftElements, rightElements;
    for (auto &element: elements) {
        auto bBox = element->getBoundingBox();
        Coords center = {
            bBox.x_min() + bBox.size_x() / 2,
            bBox.y_min() + bBox.size_y() / 2,
            bBox.z_min() + bBox.size_z() / 2
        };
        if (leftBoundingBox.contains(center)) {
            leftElements.push_back(element);
        } else if (rightBoundingBox.contains(center)) {
            rightElements.push_back(element);
        } else {
            throw std::invalid_argument("ElementRTreeNode::subdivide()");
        }
    }
    left = not leftElements.empty() ? std::make_unique<ElementRTree>(leftElements) : nullptr;
    if (left != nullptr)
        left->boundingBox = leftBoundingBox;
    right = not rightElements.empty() ? std::make_unique<ElementRTree>(rightElements) : nullptr;
    if (right != nullptr)
        right->boundingBox = rightBoundingBox;
    elements.clear();
}

void ElementRTree::subdivide(uint level) {
    for (uint i = 0; i < level; i++)
        subdivide();
}

void ElementRTree::subdivideToBatchSize(size_t batchSize) {
    if (left != nullptr)
        left->subdivideToBatchSize(batchSize);
    if (right != nullptr)
        right->subdivideToBatchSize(batchSize);
    if (elements.size() > batchSize) {
        subdivide();
        if (left != nullptr)
            left->subdivideToBatchSize(batchSize);
        if (right != nullptr)
            right->subdivideToBatchSize(batchSize);
    }
}

std::optional<std::shared_ptr<FE::Element> > ElementRTree::findElementContainingNode(const Node &node) const {
    LOG_TRACE("Looking for {} in {}: {}", node.coords, boundingBox, boundingBox.contains(node.coords));
    if (not boundingBox.contains(node.coords)) { return std::nullopt; }
    if (not elements.empty()) {
        LOG_TRACE("Seq scan in {}", boundingBox);
        for (const auto &element: elements) {
            if (element->contains_node(node)) {
                LOG_TRACE("Seq scan success");
                return std::ref(element);
            }
        }
        LOG_TRACE("Seq scan failed");
    }
    if (left != nullptr) {
        auto foundLeft = left->findElementContainingNode(node);
        if (foundLeft.has_value())
            return foundLeft;
    }
    if (right != nullptr) {
        auto foundRight = right->findElementContainingNode(node);
        if (foundRight.has_value())
            return foundRight;
    }
    return std::nullopt;
}

int ElementRTree::saveToMSH(const std::string &filename, int boxNumber) const {
    std::ofstream meshFile(filename, boxNumber == 0 ? std::ios_base::out : std::ios_base::app);
    if (not meshFile.is_open())
        throw std::invalid_argument("ElementRTree::saveToMSH()");
    if (boxNumber == 0)
        meshFile << "SetFactory(\"OpenCASCADE\");" << std::endl;
    if (!elements.empty()) {
        // Box(1) = {0, 0, 0, 1, 1, 1};
        meshFile << "Box(" << boxNumber << ") = {" << boundingBox.x_min() << ", " << boundingBox.y_min() << ", " <<
                boundingBox.z_min() << ", " << boundingBox.size_x() << ", " << boundingBox.size_y() << ", " <<
                boundingBox.
                size_z() << "}; // " << elements.size() << std::endl << std::flush;
    }
    meshFile.close();
    if (left != nullptr)
        boxNumber = left->saveToMSH(filename, boxNumber + 1);
    if (right != nullptr)
        boxNumber = right->saveToMSH(filename, boxNumber + 1);
    return boxNumber;
}

void ElementRTree::print_nodes(const std::string &padding, const std::string &edge, ElementRTree *node,
                               bool has_left_sibling) const {
    if (node != nullptr) {
        std::cout << std::endl << padding << edge << node->children();
        if ((node->left == nullptr) && (node->right == nullptr)) {
            std::cout << std::endl << padding;
            if (has_left_sibling)
                std::cout << "|";
        } else {
            const auto new_padding = padding + (has_left_sibling ? "|    " : "     ");
            print_nodes(new_padding, "|----", node->right.get(), node->left != nullptr);
            print_nodes(new_padding, "|____", node->left.get(), false);
        }
    }
}

size_t ElementRTree::children() const {
    return elements.size() + (left != nullptr ? left->children() : 0) + (right != nullptr ? right->children() : 0);
}

void ElementRTree::print() const {
    std::cout << children();
    print_nodes("", "|----", right.get(), left != nullptr);
    print_nodes("", "|____", left.get(), false);
}
