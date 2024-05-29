#pragma once
#include <numeric>
#include <vector>
#include <algorithm>
#include "Line.h"
#include "Node.h"
#include "CrossPoints.h"

class Cell {
public:
    std::vector<Node> nodes;

    static Cell create(const Node& node1, const Node& node2, const Node& node3, const Node& node4) {
        Cell cell;
        cell.nodes = {node1, node2, node3, node4};
        cell.sort_nodes();
        return cell;
    }

    static Cell create(const std::vector<Node>& nodes_p) {
            Cell cell;
            cell.nodes = nodes_p;
            cell.sort_nodes();
            return cell;
    }

    void sort_nodes() {
        while (true) {
            if (nodes[0].coords.x > nodes[1].coords.x) {
                std::swap(nodes[0], nodes[1]);
            }

            if (nodes[0].coords.x == nodes[1].coords.x && nodes[0].coords.y < nodes[1].coords.y) {
                std::swap(nodes[0], nodes[1]);
            }

            if (nodes[1].coords.y < nodes[2].coords.y) {
                std::swap(nodes[1], nodes[2]);
            }
            if (nodes[2].coords.x < nodes[3].coords.x) {
                std::swap(nodes[2], nodes[3]);
            }
            if (nodes[3].coords.y > nodes[0].coords.y) {
                std::swap(nodes[3], nodes[0]);
            }

            if (nodes[0].coords.x <= nodes[1].coords.x && nodes[1].coords.y >= nodes[2].coords.y && nodes[2].coords.x >= nodes[3].coords.x && nodes[3].coords.y <= nodes[0].coords.y) {
                break;
            }
        }
    }

    bool the_same(const Cell& other) const {
        return nodes[0].the_same(other.nodes[0]) && nodes[1].the_same(other.nodes[1]) && nodes[2].the_same(other.nodes[2]) && nodes[3].the_same(other.nodes[3]);
    }

    double find_biggest_x() const {
      return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].coords.x,
                      [](double max, const Node &next) {
                        return std::max(max, next.coords.x);
                      });
    }

    double find_biggest_y() const {
      return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].coords.y,
                      [](double max, const Node &next) {
                        return std::max(max, next.coords.y);
                      });
    }

    double find_smallest_x() const {
      return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].coords.x,
                      [](double min, const Node &next) {
                        return std::min(min, next.coords.x);
                      });
    }

    double find_smallest_y() const {
      return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].coords.y,
                      [](double min, const Node &next) {
                        return std::min(min, next.coords.y);
                      });
    }

    bool is_in_x_range(const Node& node) const {
        return find_smallest_x() <=node.coords.x  && node.coords.x <= find_biggest_x();
    }

    bool is_in_y_range(const Node& node) const {
        return find_smallest_y() <= node.coords.y && node.coords.y <= find_biggest_y();
    }

    bool contains_node(const Node& node) const {
        return is_in_x_range(node) and is_in_y_range(node) and
               Line(nodes[0], nodes[1]).find_position(node) >= 0.0 and
               Line(nodes[1], nodes[2]).find_position(node) >= 0.0 and
               Line(nodes[2], nodes[3]).find_position(node) >= 0.0 and
               Line(nodes[3], nodes[0]).find_position(node) >= 0.0;
    }

    std::optional<CrossPoints> find_cross_point_x(const Node& node) const {
        // Implement the logic to find the cross point x
        Line scan_line(node, Node(-1, node.coords.x + 1.0, node.coords.y));

        if (node.coords.x == find_smallest_x() || node.coords.x == find_biggest_x()) {
            return std::nullopt;
        }

        std::optional<Node> small = scan_line.lines_intersect(Line(nodes[3], nodes[0]));
        std::optional<Node> big = scan_line.lines_intersect(Line(nodes[1], nodes[2]));

        if (small.has_value() && big.has_value()) {
            return CrossPoints(small.value(), big.value());
        }

        return std::nullopt;
    }
};


