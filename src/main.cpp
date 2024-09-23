#include <chrono>
#include <Mesh/Mesh.h>
#include <CurrentLineGenerator/CurrentLineGenerator.h>
#include <Geometry/Geometry.h>
#include <iterator>
#include <fstream>
#include <iostream>
#include <ranges>
#include <filesystem>

#include "Loaders/Loader.h"
#include "Utils/Logger.h"


void saveCurrentLines(const std::vector<CurrentLine> &lines, const char *filename) {
    LOG_INFO("Saving {} current lines to file '{}'", lines.size(), filename);
    std::ofstream file(filename);
    uint offset = 0;
    uint savedCount = 0;
    for (auto &currentLine: lines) {
        if (currentLine.appendToFile(file, offset)) {
            offset += currentLine.size() + 1;
            ++savedCount;
        }
    }
    LOG_INFO("Saved {} current lines, average length {} points", savedCount, offset/savedCount);
    std::filesystem::remove("results_latest.geo");
    std::filesystem::create_symlink(filename, "results_latest.geo");
}

std::string CurrentDate() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    return buf;
}

void test_start_on_line() {
    auto m = Mesh();
    m.LoadFile("bmf/results/boundary_areas.msh");
    LOG_INFO("Mesh initialized : {}", m);
    CurrentLineGenerator gen(m, 1000);

    // Coords cord1 = {0.01, -0.45, 0.01}; //cylinder_tetrahedron
    // Coords cord2 = {0.01, 0.45, 0.01}; //cylinder_tetrahedron
    Coords cord1 = {-1, 0.01, -1.6}; //boundary_areas
    Coords cord2 = {1, 1.4, -1}; //boundary_areas
    // Coords cord1 = {-1, 0.31, -1.6}; //boundary_areas
    // Coords cord2 = {1, 0.31, -1.6}; //boundary_areas
    auto nLines = 50;
    LOG_INFO("Generating");
    auto lines = gen.generate_current_lines(Line(Node(Node::INVALID_ID, cord1), Node(Node::INVALID_ID, cord2)), nLines);
    LOG_INFO("Finished generating, got {} current lines", lines.size());
    auto filename = "results_" + CurrentDate() + ".geo";
    saveCurrentLines(lines, filename.c_str());
}

void test_single_line() {
    auto m = Mesh();
    m.LoadFile("bmf/results/boundary_areas.msh");
    LOG_INFO("Mesh initialized : {}", m);
    // get base point
    // Coords coords = {-0.35, 0.165}; // FM
    // Coords coords = {0.1, 0.05};//test, test-3
    // Coords coords = {0.01, 0.05};//t1
    // Coords coords = {0.01, 0.01, 0.01}; //cylinder_tetrahedron
    Coords coords = {0.3, 0.3, -1.8}; //boundary_areas
    // create current line generator
    CurrentLineGenerator gen(m, 1000);
    // generate current line
    LOG_INFO("generating current line");
    CurrentLine currentLine = gen.generate_current_line(coords);
    LOG_INFO("current line generated, size = {}", currentLine.size());

    std::ofstream file("result.geo");
    currentLine.appendToFile(file);
}

void test_bmf_smr() {
    auto m = BmfSmrMesh();
    m.loadMesh("bmf/data/sphere.bmf", "bmf/data/sphere.smr");
    LOG_INFO("Mesh initialized : {}", m);
    CurrentLineGenerator gen(m, 1000);
    LOG_INFO("Generating");
    std::set<uint32_t> ids;
    std::ranges::for_each(m.getBoudaryDomainNodesIds(0) | std::views::stride(1),
                          [&](const auto &id) { ids.insert(id); });
    auto lines = gen.generate_current_lines(ids);
    LOG_INFO("Finished generating, got {} current lines", lines.size());
    auto filename = "results_" + CurrentDate() + ".geo";
    saveCurrentLines(lines, filename.c_str());
}

int main() {
    Logger::init();
    Logger::setLogLevel(LogLevel::DEBUG);

    test_bmf_smr();

    LOG_INFO("Exit");
    return 0;
}
