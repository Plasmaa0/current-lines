#pragma once

#include <sstream>
#include <fstream>
#include <set>

#include "Mesh.h"
#include "NodalFields.h"

void convert(const std::string &p_bmf_filename, const std::string &p_smr_filename, const std::string &p_tmp_filename);

void printSurfaceMesh(const bmf::Mesh &p_mesh, const NodalFields &p_fields, const std::string &p_file_name);

void printBoundaryMesh(const bmf::Mesh &p_mesh, const NodalFields &p_fields, const std::string &p_file_name);

std::set<uint32_t> getBoundaryNodes(const bmf::Mesh &p_mesh);

std::set<uint32_t> getDomainBoundaryNodes(const bmf::Mesh &p_mesh, const uint16_t p_boundaryDomainId);

std::set<uint32_t> getAllNodes(const bmf::Mesh &p_mesh);
