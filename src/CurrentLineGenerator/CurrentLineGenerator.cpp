#include "CurrentLineGenerator.h"

#include <Geometry/VectorField.h>
#include <CurrentLineGenerator/Interpolation.h>

#include <iostream>

CurrentLineGenerator::CurrentLineGenerator(const Mesh &mesh_p) : mesh(mesh_p) {
    const Mesh::BoundingBox &bBox = mesh.getBoundingBox();
    auto size_x = bBox.x_max() - bBox.x_min();
    auto size_y = bBox.y_max() - bBox.y_min();
    size = {size_x, size_y};
    dx = size_x / 1000.0;
    dy = size_y / 1000.0;
}

std::vector<Node> CurrentLineGenerator::generate_current_line(const Coords &baseCoords_p) {
    auto currentNode = Node(Node::INVALID_ID, baseCoords_p);
    auto currentElement = mesh.findElementByNode(currentNode);
    std::vector<Node> currentLine;
    if (not currentElement.has_value()) {
        std::cout << "base node not found" << std::endl;
        return std::move(currentLine);
    }
    std::cout << "draw loop" << std::endl;
    double linesCounter = 0.0;
    while (linesCounter < 1000.0 * size.first) {
        if (not currentElement->get().contains_node(currentNode)) {
            currentElement = mesh.findElementByNode(currentNode);
        }

//        if (not currentElement->get().contains_node(currentNode)) {
//            std::cout << "current line out of bounds" << std::endl;
//            break;
//        }
        if (not currentElement.has_value()) {
            std::cout << "current line out of bounds" << std::endl;
            break;
        }

        bool success = interpolation(*currentElement, currentNode);
        if (not success) {
            std::cout << "interpolation failure" << std::endl;
        }
        currentLine.push_back(currentNode);

        currentNode.set_coords(Coords(
                currentNode.coords.x + dx * currentNode.vector_field.coords_normalize.x,
                currentNode.coords.y + dy * currentNode.vector_field.coords_normalize.y
        ));

        linesCounter += 1.0;
    }
    return std::move(currentLine);
}
