#pragma once
#include "Mesh/Mesh.h"
#include <set>

#include "bmf/Mesh.h"

class BmfSmrMesh : public Mesh {
public:
    void convertToGmsh(const std::string &p_bmf_filename, const std::string &p_smr_filename,
                       const std::string &p_tmp_filename);

    void loadMesh(const std::string &p_bmf_filename, const std::string &p_smr_filename);

    [[nodiscard]] std::set<uint32_t> getBoundaryNodesIds() const;

    [[nodiscard]] std::set<uint32_t> getBoundaryAllNodesIds() const;

    [[nodiscard]] std::set<uint32_t> getBoudaryDomainNodesIds(uint16_t p_boundaryDomainId) const;

private:
    bmf::Mesh mesh;
};
