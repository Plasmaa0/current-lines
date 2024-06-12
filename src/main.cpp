#include <Mesh/Mesh.h>
#include <CurrentLineGenerator/CurrentLineGenerator.h>
#include <Geometry/Line.h>
#include <iterator>
#include <fstream>
#include <iostream>
#include "Utils/Logger.h"


int main() {
    Logger::init();
    // load mesh
    auto m = Mesh();
    m.LoadFile("models/cylinder_tetrahedron.msh");

    // get base point
    // Coords coords = {-0.35, 0.165}; // FM
    // Coords coords = {0.1, 0.05};//test, test-3
    // Coords coords = {0.01, 0.05};//t1
    // Coords coords = {0.01, 0.01, 0.01}; //cylinder_tetrahedron
    // create current line generator
    CurrentLineGenerator gen(m);
    // generate current line
    // std::cout << "generating current line" << std::endl;
    // CurrentLine currentLine = gen.generate_current_line(coords);
    // std::cout << "current line generated, size = " << currentLine.size() << std::endl;

    // std::ofstream file("myresult.geo");
    // currentLine.appendToFile(file);

    Coords cord1 = {0.01, -0.45, 0.01}; //cylinder_tetrahedron
    Coords cord2 = {0.01, 0.45, 0.01}; //cylinder_tetrahedron
    auto nLines = 50;
    auto lines = gen.generate_current_lines(Line(Node(Node::INVALID_ID, cord1), Node(Node::INVALID_ID, cord2)), nLines);
    std::ofstream file("myresults.geo");
    uint offset = 0;
    for (auto &currentLine: lines) {
        currentLine.appendToFile(file, offset);
        offset += currentLine.size();
    }
    return 0;
}
