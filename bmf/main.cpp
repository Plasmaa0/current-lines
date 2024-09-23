#include <sstream>
#include <fstream>
#include <set>

#include "BMFFileLoader.h"
#include "SMRFileLoader.h"


void printSurfaceMesh(const Mesh& p_mesh, const NodalFields& p_fields);


int main()
{
    Mesh mesh;
    BMFFileLoader mesh_loader;
    mesh_loader.setFileName("data/sphere.bmf");
    mesh_loader.load(MeshModifier(mesh));
    NodalFields fields;
    SMRFileLoader fields_loader;
    fields_loader.setFileName("data/sphere.smr");
    fields_loader.replace(mesh, fields, 1);
    printSurfaceMesh(mesh, fields);
    return 0;
}


std::set<uint32_t> getBoundaryNodes(const Mesh& p_mesh) {
    const mesh::Region * regions = p_mesh.data().boundary_regions, *region = nullptr;
    auto bes_num_limits = p_mesh.data().bes_num_limits;
    auto boundary_domains_regions_ids = p_mesh.data().boundary_domains_regions_ids;
    auto bes_nodes = p_mesh.data().bes_nodes;
    const Apos *domain_nodes_begin = nullptr, *domain_nodes_end = nullptr;
    std::set<uint32_t> boundary_nodes;
    for (uint16_t i = 0; i < p_mesh.metric().boundary_domains_count; ++i) {
        region = regions + boundary_domains_regions_ids[i];
        domain_nodes_begin = bes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i] - region->first_fe) * region->nodes_count;
        domain_nodes_end = bes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i + 1] - region->first_fe) * region->nodes_count;
        boundary_nodes.insert(domain_nodes_begin, domain_nodes_end);
    }
    return boundary_nodes;
}

#define VAL(expr) std::cout << #expr " "  << expr << std::endl

std::set<uint32_t> getALLLL(const Mesh& p_mesh) {
    const mesh::Region * regions = p_mesh.data().regions, *region = nullptr;
    auto bes_num_limits = p_mesh.data().fes_num_limits;
    auto boundary_domains_regions_ids = p_mesh.data().domains_regions_ids;
    auto fes_nodes = p_mesh.data().fes_nodes;
    const Apos *domain_nodes_begin = nullptr, *domain_nodes_end = nullptr;
    std::set<uint32_t> boundary_nodes;
    for (uint16_t i = 0; i < p_mesh.metric().domains_count; ++i) {
        region = regions + boundary_domains_regions_ids[i];
        domain_nodes_begin = fes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i] - region->first_fe) * region->nodes_count;
        domain_nodes_end = fes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i + 1] - region->first_fe) * region->nodes_count;
        boundary_nodes.insert(domain_nodes_begin, domain_nodes_end);
    }

     regions = p_mesh.data().boundary_regions, region = nullptr;
     bes_num_limits = p_mesh.data().bes_num_limits;
     boundary_domains_regions_ids = p_mesh.data().boundary_domains_regions_ids;
    fes_nodes = p_mesh.data().bes_nodes;
    domain_nodes_begin = nullptr, domain_nodes_end = nullptr;
    for (uint16_t i = 0; i < p_mesh.metric().boundary_domains_count; ++i) {
        region = regions + boundary_domains_regions_ids[i];
        domain_nodes_begin = fes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i] - region->first_fe) * region->nodes_count;
        domain_nodes_end = fes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i + 1] - region->first_fe) * region->nodes_count;
        boundary_nodes.insert(domain_nodes_begin, domain_nodes_end);
    }
    return boundary_nodes;
}

void printSurfaceMesh(const Mesh& p_mesh, const NodalFields& p_fields) {
    auto nodes_coords = p_mesh.data().nodes_coords;
    const mesh::Metric& metric = p_mesh.metric();
    uint8_t dimension = metric.dimension;
    std::set<uint32_t> boundary_nodes = getALLLL(p_mesh);
    auto rand_node = p_mesh.data().fes_nodes[123];
//    std::cout << p_mesh.data().nodes_coords[rand_node+1] << ','<< p_mesh.data().nodes_coords[rand_node+2] << ','<< p_mesh.data().nodes_coords[rand_node+3] << std::endl;
    auto x = p_mesh.data().fes_nodes;
    std::ostringstream sout;
    sout << "$MeshFormat\n" <<
            "2.0 0 8\n" <<
            "$EndMeshFormat\n" <<
            "$Nodes\n" <<
            boundary_nodes.size() << std::endl;
//    std::cout << "получено элементов: "<< boundary_nodes.size()<<std::endl;
    for (const auto& node: boundary_nodes) {
        sout << node << '\t' <<
            nodes_coords[node * dimension] << '\t' <<
            nodes_coords[node * dimension + 1] << '\t' <<
            nodes_coords[node * dimension + 2] << '\n';
    }

    auto bes_num_limits = p_mesh.data().fes_num_limits;
    Apos bes_count = 0; // количество граничных элементов
    for (uint16_t k = 0; k < metric.domains_count; ++k)
        bes_count += bes_num_limits[k + 1] - bes_num_limits[k];

    sout << "$EndNodes\n" <<
            "$Elements\n" <<
            bes_count << std::endl;

    const mesh::Region * regions = p_mesh.data().regions, *region = nullptr;
    auto boundary_domains_regions_ids = p_mesh.data().domains_regions_ids;
    auto bes_nodes = p_mesh.data().fes_nodes;
    const Apos *domain_nodes_begin = nullptr, *be_nodes = nullptr;
    Apos be_number = 0;
    for (uint16_t i = 0; i < metric.domains_count; ++i) {
        region = regions + boundary_domains_regions_ids[i];
        domain_nodes_begin = bes_nodes + region->fes_nodes_offset +
            (bes_num_limits[i] - region->first_fe) * region->nodes_count;
//        std::cout << +region->nodes_count << std::endl;
        if (region->nodes_count == 6) { // треугольники
            bes_count = bes_num_limits[i + 1] - bes_num_limits[i];
            for (Apos j = 0; j < bes_count; ++j) {
                be_nodes = domain_nodes_begin + j * region->nodes_count;
                sout << ++be_number << '\t' << 6 << '\t' << 0 << '\t' <<
                        be_nodes[5] << '\t' <<be_nodes[4] << '\t' <<be_nodes[3] << '\t' <<be_nodes[2] << '\t' << be_nodes[1] << '\t' << be_nodes[0] << std::endl;
            }
        } else if (region->nodes_count == 4) { // тетраэдры
            bes_count = bes_num_limits[i + 1] - bes_num_limits[i];
            for (Apos j = 0; j < bes_count; ++j) {
                be_nodes = domain_nodes_begin + j * region->nodes_count;
                sout << ++be_number << '\t' << 4 << '\t' << 0 << '\t' <<
                        be_nodes[3] << '\t' << be_nodes[2] << '\t' <<
                        be_nodes[1] << '\t' << be_nodes[0] << std::endl;
            }
        }
    }
    sout << "$EndElements\n";
    const Real *fields = p_fields.mainData();
    const uint8_t fields_count = p_fields.mainFieldsCount();

    // sout << "$NodeData" << std::endl;
    // sout << 1 << std::endl;
    // sout << "\"" << "density" << "\"" << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << 3 << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << boundary_nodes.size() << std::endl;
    // for (Apos j: boundary_nodes) {
    //     sout << j << " ";
    //     sout << fields[j * fields_count] << '\n';
    // }
    // sout << "$EndNodeData"<< std::endl;

    sout << "$NodeData" << std::endl;
    sout << 1 << std::endl;
    sout << "\"" << "Scalar" << "\"" << std::endl;
    sout << 1 << std::endl;
    sout << 1 << std::endl;
    sout << 3 << std::endl;
    sout << 1 << std::endl;
    sout << 3 << std::endl;
    sout << boundary_nodes.size() << std::endl;
    for (Apos j: boundary_nodes) {
        sout << j << " ";
        sout << fields[j * fields_count + 1]<<'\t'<<fields[j * fields_count + 2] <<'\t'<<fields[j * fields_count + 3]<< '\n';
    }
    sout << "$EndNodeData"<< std::endl;

    // sout << "$NodeData" << std::endl;
    // sout << 1 << std::endl;
    // sout << "\"" << "Vx" << "\"" << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << 3 << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << boundary_nodes.size() << std::endl;
    // for (Apos j: boundary_nodes) {
    //     sout << j << " ";
    //     sout << fields[j * fields_count + 1] << '\n';
    // }
    // sout << "$EndNodeData"<< std::endl;
    //
    // sout << "$NodeData" << std::endl;
    // sout << 1 << std::endl;
    // sout << "\"" << "Vy" << "\"" << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << 3 << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << boundary_nodes.size() << std::endl;
    // for (Apos j: boundary_nodes) {
    //     sout << j << " ";
    //     sout << fields[j * fields_count + 2] << '\n';
    // }
    // sout << "$EndNodeData"<< std::endl;
    //
    // sout << "$NodeData" << std::endl;
    // sout << 1 << std::endl;
    // sout << "\"" << "Vz" << "\"" << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << 3 << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << boundary_nodes.size() << std::endl;
    // for (Apos j: boundary_nodes) {
    //     sout << j << " ";
    //     sout << fields[j * fields_count + 3] << '\n';
    // }
    // sout << "$EndNodeData"<< std::endl;

    // sout << "$NodeData" << std::endl;
    // sout << 1 << std::endl;
    // sout << "\"" << "temperature" << "\"" << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << 3 << std::endl;
    // sout << 1 << std::endl;
    // sout << 1 << std::endl;
    // sout << boundary_nodes.size() << std::endl;
    // for (Apos j: boundary_nodes) {
    //     sout << j << " ";
    //     sout << fields[j * fields_count + 4] << '\n';
    // }
    // sout << "$EndNodeData"<< std::endl;

    std::ofstream fout("results/boundary_areas.msh");
    fout << sout.str();
    fout.close();
}
