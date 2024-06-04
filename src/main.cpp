#include <Mesh/Mesh.h>
#include <CurrentLineGenerator/CurrentLineGenerator.h>
#include <iterator>
#include <fstream>
#include <iostream>

int main() {
    // load mesh
    auto m = Mesh();
    m.LoadFile("test-3.msh");

    const Mesh::BoundingBox &bBox = m.getBoundingBox();
    auto size_x = bBox.x_max() - bBox.x_min();
    auto size_y = bBox.y_max() - bBox.y_min();

    // get base point
//    Coords coords = {-0.35, 0.145}; // FM
//    Coords coords = {0.1, 0.05};//test, test-3
    Coords coords = {0.045, 0.01};//ell
    // create current line generator
    CurrentLineGenerator gen(m);
    // generate current line
    std::cout << "generating current line" << std::endl;
    std::vector<Node> currentLine = gen.generate_current_line(coords);
    std::cout << "current line generated, size = " << currentLine.size() << std::endl;
    // for(int i = 0; i<currentLine.size(); ++i){
    //     const auto& node = currentLine[i];
    //     std::cout << node.id << ", " << node.coords.x << ", " << node.coords.y << ", " << node.coords.z << std::endl;
    // }

    std::ofstream file("myresult.geo");
    if (not file.is_open()) {
        throw std::invalid_argument("FILE NOT OPEN");
    }
    for (int i = 0; i < currentLine.size(); ++i) {
        const auto &node = currentLine[i];
        file << "Point(" << i + 1 << ") = {" << node.coords.x << ", " << node.coords.y << ", " << node.coords.z << ", "
             << size_x << "};" << std::endl;
    }
    file << "Line(" << currentLine.size() << ") = {";
    for (int i = 0; i < currentLine.size(); ++i) {
        file << i + 1;
        if (i != currentLine.size() - 1)
            file << ", ";
    }
    file << "};";
    return 0;
}
