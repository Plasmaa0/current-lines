#pragma once
#include <vector>
#include "VectorField.h"
#include "Node.h"
#include "Cell.h"
#include "Interpolation.h"


class CurrentLineGenerator {
public:
    CurrentLineGenerator(const std::vector<Cell>& cells, double size_x, double size_y, double min_x, double min_y, const Node& base_node)
        : cells(cells), size(size_x, size_y), min_x(min_x), min_y(min_y), current_node(base_node), current_cell(cells[0]), dx(size_x / 100.0), dy(size_y / 100.0) {}

    std::vector<Node> generate_current_line() {
        find_current_cell();
        draw_loop();
        return nodes;
    }

private:
    void find_current_cell() {
        for (const auto& cell : cells) {
            if (cell.contains_node(current_node)) {
                current_cell = cell;
                break;
            }
        }
    }

    void draw_loop() {
        double lines_counter = 0.0;
        while (lines_counter < 1000.0 * size.first) {
            if (!current_cell.contains_node(current_node)) {
                find_current_cell();
            }

            if (!current_cell.contains_node(current_node)) {
                break;
            }

            interpolation(current_cell, current_node);
            nodes.push_back(current_node);

            current_node.set_coords(Coords(
                current_node.coords.x + dx * current_node.vector_field.coords_normalize.x,
                current_node.coords.y + dy * current_node.vector_field.coords_normalize.y
            ));

            lines_counter += 1.0;
        }
    }

    std::vector<Cell> cells;
    std::vector<Node> nodes;
    std::pair<double, double> size;
    double min_x, min_y;
    Node current_node;
    Cell current_cell;
    double dx, dy;
};

