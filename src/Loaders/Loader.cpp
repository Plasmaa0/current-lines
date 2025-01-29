#include "Loader.h"
#include "bmf/bmf.h"
#include "bmf/BMFFileLoader.h"
#include "bmf/SMRFileLoader.h"
#include "bmf/NodalFields.h"
#include "Utils/Logger.h"

void BmfSmrMesh::convertToGmsh(const std::string &p_bmf_filename, const std::string &p_smr_filename,
                               const std::string &p_tmp_filename) {
    bmf::BMFFileLoader mesh_loader;
    mesh_loader.setFileName(p_bmf_filename);
    mesh_loader.load(bmf::MeshModifier(mesh));
    NodalFields fields;
    bmf::SMRFileLoader fields_loader;
    fields_loader.setFileName(p_smr_filename);
    fields_loader.replace(mesh, fields, 1);
    LOG_DEBUG("Boundary domain count: {}", mesh.metric().boundary_domains_count);
    printSurfaceMesh(mesh, fields, p_tmp_filename);
    printBoundaryMesh(mesh, fields, "boundary_" + p_tmp_filename);
}

void BmfSmrMesh::loadMesh(const std::string &p_bmf_filename, const std::string &p_smr_filename) {
    convertToGmsh(p_bmf_filename, p_smr_filename, "converted.msh");
    LOG_INFO("Initializing BmfSmrMesh from {}, {}", p_bmf_filename, p_smr_filename);
    LoadFile("converted.msh");
}

std::set<uint32_t> BmfSmrMesh::getBoundaryNodesIds() const {
    return getBoundaryNodes(mesh);
}

std::set<uint32_t> BmfSmrMesh::getBoundaryAllNodesIds() const {
    return getAllNodes(mesh);
}

std::set<uint32_t> BmfSmrMesh::getBoudaryDomainNodesIds(const uint16_t p_boundaryDomainId) const {
    return getDomainBoundaryNodes(mesh, p_boundaryDomainId);
}
