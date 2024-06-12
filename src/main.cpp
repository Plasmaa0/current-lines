#include <Mesh/Mesh.h>
#include <CurrentLineGenerator/CurrentLineGenerator.h>
#include <Geometry/Line.h>
#include <iterator>
#include <fstream>
#include <iostream>
#include "Utils/Logger.h"


int main() {
    Logger::init();
    Logger::setLogLevel(LogLevel::INFO);
    LOG_INFO("Initializing mesh");
    // load mesh
    auto m = Mesh();
    m.LoadFile("models/cylinder_tetrahedron.msh");
    LOG_INFO("Mesh init finish: {}", m);
    // get base point
    // Coords coords = {-0.35, 0.165}; // FM
    // Coords coords = {0.1, 0.05};//test, test-3
    // Coords coords = {0.01, 0.05};//t1
    // Coords coords = {0.01, 0.01, 0.01}; //cylinder_tetrahedron
    // create current line generator
    CurrentLineGenerator gen(m);
    // generate current line
    // LOG_INFO("generating current line");
    // CurrentLine currentLine = gen.generate_current_line(coords);
    // LOG_INFO("current line generated, size = {}", currentLine.size())

    // std::ofstream file("myresult.geo");
    // currentLine.appendToFile(file);

    Coords cord1 = {0.01, -0.45, 0.01}; //cylinder_tetrahedron
    Coords cord2 = {0.01, 0.45, 0.01}; //cylinder_tetrahedron
    auto nLines = 50;
    LOG_INFO("Generating");
    auto lines = gen.generate_current_lines(Line(Node(Node::INVALID_ID, cord1), Node(Node::INVALID_ID, cord2)), nLines);
    LOG_INFO("Finished generating, got {} current lines", lines.size());
    auto filename = "myresults.geo";
    LOG_INFO("Saving to file '{}'", filename);
    std::ofstream file(filename);
    uint offset = 0;
    for (auto &currentLine: lines) {
        currentLine.appendToFile(file, offset);
        offset += currentLine.size();
    }
    LOG_INFO("Exit");
    return 0;
}
