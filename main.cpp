#include <iostream>
#include "Mesh.h"
#include "CurrentLineGenerator.h"
#include <iterator>
#include <fstream>
#include <sstream>

int main(){
    // load mesh
    auto m = Mesh();
    m.LoadFile("../FM.msh");
    const Mesh::BoundingBox& bBox = m.getBoundingBox();
    auto size_x = bBox.x_max() - bBox.x_min();
    auto size_y = bBox.y_max() - bBox.y_min();
    // get base point
    double x = -0.35;
    double y = 0.145;
    // double x = 0.1;
    // double y = 0.1;
    // create current line generator
    CurrentLineGenerator gen(m.getCells(), 
                             size_x, size_y, 
                             bBox.x_min(), bBox.y_min(),
                             Node(-1, x, y));
    // generate current line
    std::cout << "generating current line" << std::endl;
    std::vector<Node> currentLine = gen.generate_current_line();
    std::cout << "current line generated, size = " << currentLine.size() << std::endl;
    // for(int i = 0; i<currentLine.size(); ++i){
    //     const auto& node = currentLine[i];
    //     std::cout << node.id << ", " << node.coords.x << ", " << node.coords.y << ", " << node.coords.z << std::endl;
    // }


    std::ofstream file("myresult.geo");
    if (not file.is_open()) {
        throw std::invalid_argument("FILE NOT OPEN");
    }
    for(int i = 0; i<currentLine.size(); ++i){
        const auto& node = currentLine[i];
        file << "Point("<<i+1<<") = {" << node.coords.x << ", " << node.coords.y << ", " << node.coords.z << ", " << size_x << "};" << std::endl;
    }
    file << "Line(" << currentLine.size() << ") = {";
    for(int i = 0; i<currentLine.size(); ++i){
        file << i+1;
        if(i!=currentLine.size()-1)
            file << ", ";
    }
    file << "};";
    return 0;
}
