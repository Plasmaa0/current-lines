// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <thread>
#include <omp.h>

#include "Mesh.h"
#include "MeshStructures.h"

/// Выделение памяти под массив
/// с проверкой размера

template<typename _Type>
void allocate(_Type *&p_pointer, size_t p_old_size, size_t p_new_size)
{
    if (p_old_size != p_new_size) {
        delete[] p_pointer;

        if (p_new_size != 0)
            p_pointer = new _Type[p_new_size];
        else
            p_pointer = nullptr;
    }
}

// Получение сигнатуры типа КЭ региона

inline bool FETypeSignature(const mesh::Region    &p_region,
                            mesh::FETypeSignature &o_signature) noexcept
{
    const mesh::FEType fe_type = p_region.type;
    const mesh::FETypeMetric metric = Mesh::FETypeMetric(fe_type);

    o_signature.is_inner_nodes =
        (p_region.nodes_count - metric.vertices_count) % metric.edges_count;

    uint8_t fe_nodes_count;
    bool res{ false };

    if (o_signature.is_inner_nodes) {
        fe_nodes_count = metric.vertices_count;

        for (uint8_t e = 1; e < UINT8_MAX; ++e) {
            switch (fe_type)
            {
                case mesh::FEType::triangle:
                    fe_nodes_count = (e + 2) * (e + 3) / 2;
                break;

                case mesh::FEType::quadrilateral:
                    fe_nodes_count = (e + 2) * (e + 2);
                break;

                case mesh::FEType::tetrahedron:
                    fe_nodes_count = (e + 2) * (e + 3) * (e + 4) / 6;
                break;

                case mesh::FEType::quadrilateral_pyramid:
                    fe_nodes_count = (e + 2) * (e + 2) * (e + 3) + 1;
                break;

                case mesh::FEType::triangular_prism:
                    fe_nodes_count = (e + 2) * (e + 2) * (e + 3) / 2;
                break;

                case mesh::FEType::hexahedron:
                    fe_nodes_count = (e + 2) * (e + 2) * (e + 2);
                break;

                default:
                break;
            }

            if (p_region.nodes_count == fe_nodes_count) {
                o_signature.edge_inner_nodes_count = e;
                res = true;
                break;
            }
        }
    } else {
        o_signature.edge_inner_nodes_count =
            (p_region.nodes_count - metric.vertices_count) / metric.edges_count;
        res = true;
    }

    return res;
}

// Реализация методов Mesh

void Mesh::operator=(Mesh &&p_mesh) noexcept
{
    clear();

    m_metric = p_mesh.m_metric;
    m_fv_metric = p_mesh.m_fv_metric;

    m_data = p_mesh.m_data;
    m_fv_data = p_mesh.m_fv_data;

    p_mesh.m_data = {};
    p_mesh.m_fv_data = {};

    p_mesh.m_metric = {};
    p_mesh.m_fv_metric = {};
}


mesh::FETypeMetric Mesh::FETypeMetric(mesh::FEType p_type) noexcept
{
    switch (p_type)
    {
        case mesh::FEType::edge:
            return { 2, 0, 1 };

        case mesh::FEType::triangle:
            return { 3, 1, 3 };

        case mesh::FEType::quadrilateral:
            return { 4, 1, 4 };

        case mesh::FEType::tetrahedron:
            return { 4, 4, 6 };

        case mesh::FEType::quadrilateral_pyramid:
            return { 5, 5, 8 };

        case mesh::FEType::triangular_prism:
            return { 6, 5, 9 };

        case mesh::FEType::hexahedron:
            return { 8, 6, 12 };

        default:
        break;
    }

    return {};
}


mesh::FETypeMasks Mesh::FETypeMasks(mesh::FEType p_type)
{
    constexpr uint8_t quadrilateral_vertices_count = 4;
    constexpr uint8_t triangle_vertices_count = 3;
    constexpr uint8_t edge_vertices_count = 2;

    mesh::FETypeMasks res;

    switch (p_type)
    {
        case mesh::FEType::edge:
        {
            constexpr uint8_t edges_count = 1;

            res.edges_mask.resize(edge_vertices_count * edges_count);

            res.edges_mask[0] = 0;
            res.edges_mask[1] = 1;
        }
        break;

        case mesh::FEType::triangle:
        {
            res.faces_mask.resize(triangle_vertices_count);

            for (uint8_t nd = 0; nd < triangle_vertices_count; ++nd)
                res.faces_mask[nd] = nd;

            constexpr uint8_t edges_count = 3;
            res.edges_mask.resize(edge_vertices_count * edges_count);

            uint8_t e(0), i;

            for (uint8_t nd = 0; nd < triangle_vertices_count; ++nd)
                for (i = nd + 1; i < triangle_vertices_count; ++i, ++e) {
                    res.edges_mask[edge_vertices_count * e] = nd;
                    res.edges_mask[edge_vertices_count * e + 1] = i;
                }
        }
        break;

        case mesh::FEType::quadrilateral:
        {
            res.faces_mask.resize(quadrilateral_vertices_count);

            for (uint8_t nd = 0; nd < quadrilateral_vertices_count; ++nd)
                res.faces_mask[nd] = nd;

            constexpr uint8_t edges_count = 4;
            res.edges_mask.resize(edge_vertices_count * edges_count);

            for (uint8_t v = 0; v < quadrilateral_vertices_count; ++v) {
                res.edges_mask[edge_vertices_count * v] = v;
                res.edges_mask[edge_vertices_count * v + 1] =
                    (v + 1) % quadrilateral_vertices_count;
            }
        }
        break;

        case mesh::FEType::tetrahedron:
        {
            constexpr uint8_t vertices_count = 4;
            constexpr uint8_t faces_count = 4;
            constexpr uint8_t edges_count = 6;

            res.faces_mask.resize(faces_count * triangle_vertices_count);

            uint8_t i;

            for (uint8_t f = 0; f < faces_count; ++f) {
                for (i = 0; i < triangle_vertices_count; ++i)
                    res.faces_mask[f * triangle_vertices_count + i] = (f + i) % vertices_count;
            }

            res.edges_mask.resize(edge_vertices_count * edges_count);

            uint8_t e(0);

            for (uint8_t nd = 0; nd < vertices_count; ++nd)
                for (i = nd + 1; i < vertices_count; ++i, ++e) {
                    res.edges_mask[edge_vertices_count * e] = nd;
                    res.edges_mask[edge_vertices_count * e + 1] = i;
                }
        }
        break;

        case mesh::FEType::quadrilateral_pyramid:
        {
            constexpr uint8_t vertices_count = 5;
            constexpr uint8_t quadrilateral_faces_count = 1;
            constexpr uint8_t triangle_faces_count = 4;
            constexpr uint8_t edges_count = 6;

            res.faces_mask.resize(quadrilateral_faces_count * quadrilateral_vertices_count +
                                  triangle_faces_count * triangle_vertices_count);
            uint8_t *faces_mask = res.faces_mask.data();

            uint8_t i;

            for(i = 0; i < quadrilateral_vertices_count; ++i)
                faces_mask[i] = i;

            faces_mask += quadrilateral_faces_count * quadrilateral_vertices_count;

            for (uint8_t f = 0;
                 f < triangle_faces_count;
                 ++f, faces_mask += triangle_vertices_count) {
                for (i = 0; i < edge_vertices_count; ++i)
                    faces_mask[i] = (f + i) % quadrilateral_vertices_count;

                faces_mask[triangle_vertices_count - 1] = vertices_count - 1;
            }

            res.edges_mask.resize(edge_vertices_count * edges_count);

            uint8_t e(0);

            for (uint8_t nd = 0; nd < vertices_count; ++nd)
                for (i = nd + 1; i < vertices_count; ++i, ++e) {
                    res.edges_mask[2 * e] = nd;
                    res.edges_mask[2 * e + 1] = i;
                }
        }
        break;

        case mesh::FEType::triangular_prism:
        {
            constexpr uint8_t quadrilateral_faces_count = 3;
            constexpr uint8_t triangle_faces_count = 2;
            constexpr uint8_t edges_count = 9;
            UNUSED(edges_count);

            res.faces_mask.resize(quadrilateral_faces_count * quadrilateral_vertices_count +
                                  triangle_faces_count * triangle_vertices_count);
            uint8_t *faces_mask = res.faces_mask.data();

            uint8_t i;

            for(i = 0; i < triangle_vertices_count; ++i) {
                faces_mask[i] = i;
                faces_mask[i + triangle_vertices_count] = i + triangle_vertices_count;
            }

            faces_mask += triangle_faces_count * triangle_vertices_count;

            for (uint8_t f = 0;
                 f < quadrilateral_faces_count;
                 ++f, faces_mask += quadrilateral_vertices_count) {
                for (i = 0; i < edge_vertices_count; ++i) {
                    faces_mask[i] = (f + i) % triangle_vertices_count;
                    faces_mask[i + edge_vertices_count] =
                        (f + edge_vertices_count - i - 1) % triangle_vertices_count +
                        triangle_vertices_count;
                }
            }
        }
        break;

        case mesh::FEType::hexahedron:
        {
            constexpr uint8_t base_faces_count = 2;
            constexpr uint8_t end_faces_count = 4;
            constexpr uint8_t edges_count = 12;
            UNUSED(edges_count);

            res.faces_mask.resize(
                (base_faces_count + end_faces_count) * quadrilateral_vertices_count);
            uint8_t *faces_mask = res.faces_mask.data();

            uint8_t i;

            for(i = 0; i < quadrilateral_vertices_count; ++i) {
                faces_mask[i] = i;
                faces_mask[i + quadrilateral_vertices_count] = i + quadrilateral_vertices_count;
            }

            faces_mask += base_faces_count * quadrilateral_vertices_count;

            for (uint8_t f = 0;
                 f < end_faces_count;
                 ++f, faces_mask += quadrilateral_vertices_count) {
                for (i = 0; i < edge_vertices_count; ++i) {
                    faces_mask[i] = (f + i) % quadrilateral_vertices_count;
                    faces_mask[i + edge_vertices_count] =
                        (f + edge_vertices_count - i - 1) % quadrilateral_vertices_count +
                        quadrilateral_vertices_count;
                }
            }
        }
        break;

        default:
        break;
    }

    return res;
}


bool Mesh::checkRegionFEType(const mesh::Region &p_region,
                             mesh::FEType        p_fe_type,
                             uint8_t             p_edge_inner_nodes_count,
                             bool                p_is_inner_nodes) noexcept
{
    if(p_region.type != p_fe_type)
        return false;

    if(mesh::FETypeSignature signature; FETypeSignature(p_region, signature))
        return signature.edge_inner_nodes_count == p_edge_inner_nodes_count &&
               signature.is_inner_nodes == p_is_inner_nodes;

     return false;
}


Array<mesh::FETypeSignature> Mesh::FETypesSignatures() const
{
    if (empty())
        return {};

    Array<mesh::FETypeSignature> res(m_metric.regions_count);
    auto *signatures = res.data();

    for (uint8_t r = 0; r < m_metric.regions_count; ++r) {
        if (!FETypeSignature(m_data.regions[r], signatures[r])) {
            res.clear();
            break;
        }

        signatures[r].is_curvilinear = m_metric.sd_buffer_size;
    }

    return res;
}


void Mesh::clear()
{
    delete[] m_data.nodes_coords;

    delete[] m_data.subareas_ids;
    delete[] m_data.domains_subareas_ids;
    delete[] m_data.domains_regions_ids;
    delete[] m_data.regions;
    delete[] m_data.fes_num_limits;
    delete[] m_data.fes_nodes;

    delete[] m_data.boundary_domains_subareas_ids;
    delete[] m_data.boundary_domains_regions_ids;
    delete[] m_data.boundary_regions;
    delete[] m_data.bes_num_limits;
    delete[] m_data.bes_nodes;
    delete[] m_data.bes_fes;

    delete[] m_data.twice_boundary_subareas_ids;
    delete[] m_data.twice_boundary_subareas_regions_ids;
    delete[] m_data.twice_boundary_regions;
    delete[] m_data.twice_bes_num_limits;
    delete[] m_data.twice_bes_nodes;

    delete[] m_data.vertices_ids;
    delete[] m_data.vertices_nodes;

    delete[] m_data.sd_buffer;

//    delete[] m_fv_data.ifaces_area;
//    delete[] m_fv_data.ifaces_normal;
//    delete[] m_fv_data.ifaces_gauss_coords;
//    delete[] m_fv_data.ifaces_elements;
//    delete[] m_fv_data.ifaces_distance;
//    delete[] m_fv_data.bfaces_area;
//    delete[] m_fv_data.bfaces_normal;
//    delete[] m_fv_data.bfaces_gauss_coords;
//    delete[] m_fv_data.bfaces_elements;
//    delete[] m_fv_data.bfaces_elements2;
//    delete[] m_fv_data.bfaces_distance;
//    delete[] m_fv_data.volumes;
//    delete[] m_fv_data.volumes_center_coords;
//    delete[] m_fv_data.volumes_gauss_points;
//    delete[] m_fv_data.elements_distance;
//    delete[] m_fv_data.stencils;
//    delete[] m_fv_data.hard_wall_steps;

    m_fv_data = {};
    m_fv_metric = {};

    m_data = {};
    m_metric = {};
}


Apos Mesh::findNode(const std::array<Real, mesh::max_dimension> &p_point) const
{
    const Apos nodes_count = m_metric.nodes_count;
    const Real *nodes_coords = m_data.nodes_coords;
    const uint8_t dimension = m_metric.dimension;

    struct ThreadData
    {
        Real metric;

        Apos node;
    };

    int threads_count{ 1 };

    if(auto hardware_threads_count = std::thread::hardware_concurrency())
        threads_count = static_cast<int>(hardware_threads_count);
    else
        threads_count = omp_get_max_threads();

    const Real *coords = p_point.data();

    ThreadData *thread_data = new ThreadData[threads_count];

    for(int t = 0; t < threads_count; ++t)
        thread_data[t] = { REAL_MAX, APOS_MAX };

    Real metric;
    uint8_t d;

#   pragma omp parallel firstprivate(thread_data) private(metric, d) num_threads(threads_count)
    {
        thread_data += omp_get_thread_num();

#   pragma omp for
        for(Apos node = 0; node < nodes_count; ++node) {
            metric = 0;

            for(d = 0; d < dimension; ++d)
                metric += mat::sqr(nodes_coords[node * dimension + d] - coords[d]);

            if(thread_data->metric > metric) {
                thread_data->metric = metric;
                thread_data->node = node;
            }
        }
    }

    Apos res{ APOS_MAX };
    metric = REAL_MAX;

    for(int t = 0; t < threads_count; ++t) {
        if(metric > thread_data[t].metric) {
            metric = thread_data[t].metric;
            res = thread_data[t].node;
        }
    }

    return res;
}

// Реализация методов MeshModifier

bool MeshModifier::makeNodes(Apos p_nodes_count, uint8_t p_dimension)
{
    if (p_dimension < mesh::min_dimension || p_dimension > mesh::max_dimension)
        return false;

    if (p_nodes_count < mesh::min_nodes_count)
        return false;

    allocate(m_mesh->m_data.nodes_coords,
             m_mesh->m_metric.dimension * m_mesh->m_metric.nodes_count,
             p_dimension * p_nodes_count);

    m_mesh->m_metric.dimension = p_dimension;
    m_mesh->m_metric.nodes_count = p_nodes_count;

    return true;
}


bool MeshModifier::makeElements(ElementsSection            p_section,
                                const Array<Apos>         &p_regions_elements_count,
                                const Array<uint8_t>      &p_regions_elements_nodes_count,
                                const Array<mesh::FEType> &p_regions_elements_types,
                                uint16_t                   p_subareas_count,
                                Apos                       p_domains_count)
{
    Apos *elements_count;
    uint16_t *subareas_count;
    Apos *domains_count;
    uint8_t *regions_count;

    uint16_t **domains_subareas_ids;
    uint8_t **domains_regions_ids;
    mesh::Region **regions;
    Apos **elements_num_limits;
    Apos **elements_nodes;

    auto &metric = m_mesh->m_metric;
    auto &data = m_mesh->m_data;

    Apos twice_boundary_domains_count = metric.twice_boundary_subareas_count;

    switch (p_section)
    {
        case ElementsSection::finite_elements:
        {
            elements_count = &metric.fes_count;
            subareas_count = &metric.subareas_count;
            domains_count = &metric.domains_count;
            regions_count = &metric.regions_count;

            domains_subareas_ids = &data.domains_subareas_ids;
            domains_regions_ids = &data.domains_regions_ids;
            regions = &data.regions;
            elements_num_limits = &data.fes_num_limits;
            elements_nodes = &data.fes_nodes;
        }
        break;

        case ElementsSection::boundary_elements:
        {
            elements_count = &metric.bes_count;
            subareas_count = &metric.boundary_subareas_count;
            domains_count = &metric.boundary_domains_count;
            regions_count = &metric.boundary_regions_count;

            domains_subareas_ids = &data.boundary_domains_subareas_ids;
            domains_regions_ids = &data.boundary_domains_regions_ids;
            regions = &data.boundary_regions;
            elements_num_limits = &data.bes_num_limits;
            elements_nodes = &data.bes_nodes;
        }
        break;

        default:
        {
            elements_count = &metric.twice_bes_count;
            subareas_count = &metric.twice_boundary_subareas_count;
            domains_count = &twice_boundary_domains_count;
            regions_count = &metric.twice_boundary_regions_count;

            domains_subareas_ids = &data.twice_boundary_subareas_ids;
            domains_regions_ids = &data.twice_boundary_subareas_regions_ids;
            regions = &data.twice_boundary_regions;
            elements_num_limits = &data.twice_bes_num_limits;
            elements_nodes = &data.twice_bes_nodes;
        }
        break;
    }

    if (p_subareas_count > p_domains_count ||
        (p_section == ElementsSection::twice_boundary_elements &&
         p_subareas_count != p_domains_count) ||
        (!p_regions_elements_nodes_count.empty() &&
         (p_regions_elements_count.size() != p_regions_elements_nodes_count.size() ||
          p_regions_elements_types.size() != p_regions_elements_nodes_count.size())) ||
        p_regions_elements_count.size() > UINT8_MAX)
        return false;

    if (!p_regions_elements_nodes_count.empty() && p_subareas_count && p_domains_count) {
        const uint8_t new_regions_count = static_cast<uint8_t>(p_regions_elements_nodes_count.size());
        Exapos new_fes_nodes_array_elements_count{ 0 };

        for (uint8_t r = 0; r < new_regions_count; ++r) {
            if (p_regions_elements_count[r] == 0 || p_regions_elements_nodes_count[r] == 0)
                return false;

            new_fes_nodes_array_elements_count +=
                p_regions_elements_nodes_count[r] * p_regions_elements_count[r];
        }

        Exapos current_fes_nodes_array_elements_count{ 0 };

        mesh::Region *current_region;
        mesh::Region *next_region;

        for (uint8_t r = 0; r < *regions_count; ++r) {
            current_region = *regions + r;
            next_region = *regions + r + 1;

            current_fes_nodes_array_elements_count +=
                current_region->nodes_count * (next_region->first_fe -
                                               current_region->first_fe);
        }

        if(p_section == ElementsSection::finite_elements)
            allocate(data.subareas_ids, metric.subareas_count, p_subareas_count);

        allocate(*domains_subareas_ids, *domains_count, p_domains_count);

        allocate(*domains_regions_ids, *domains_count, p_domains_count);

        allocate(*regions, *regions_count + 1, new_regions_count + 1);

        allocate(*elements_num_limits, *domains_count + 1, p_domains_count + 1);

        allocate(*elements_nodes,
                 current_fes_nodes_array_elements_count,
                 new_fes_nodes_array_elements_count);

        for (uint8_t r = 0; r < new_regions_count; ++r) {
            current_region = *regions + r;
            next_region = *regions + r + 1;

            next_region->fes_nodes_offset = current_region->fes_nodes_offset +
                p_regions_elements_count[r] * p_regions_elements_nodes_count[r];

            current_region->nodes_count = p_regions_elements_nodes_count[r];
            current_region->type = p_regions_elements_types[r];

            next_region->first_fe = current_region->first_fe +
                p_regions_elements_count[r];
        }

        current_region = *regions + new_regions_count;

        if(p_section == ElementsSection::boundary_elements) {
            allocate(data.bes_fes,
                     2 * *elements_count,
                     2 * current_region->first_fe);
        }

        *elements_count = current_region->first_fe;
        *subareas_count = p_subareas_count;
        *domains_count = p_domains_count;
        *regions_count = new_regions_count;
    } else {
        if(p_section == ElementsSection::finite_elements){
            delete[] data.subareas_ids;
            data.subareas_ids = nullptr;
        }

        delete[] *domains_subareas_ids;
        *domains_subareas_ids = nullptr;

        delete[] *domains_regions_ids;
        *domains_regions_ids = nullptr;

        delete[] *regions;
        *regions = nullptr;

        delete[] *elements_num_limits;
        *elements_num_limits = nullptr;

        delete[] *elements_nodes;
        *elements_nodes = nullptr;

        if(p_section == ElementsSection::boundary_elements) {
            delete[] data.bes_fes;
            data.bes_fes = nullptr;
        }

        *elements_count = 0;
        *subareas_count = 0;
        *domains_count = 0;
        *regions_count = 0;
    }

    return true;
}


void MeshModifier::makeVertices(uint16_t p_vertices_count)
{
    if (p_vertices_count) {
        allocate(m_mesh->m_data.vertices_ids,
                 m_mesh->m_metric.vertices_count,
                 p_vertices_count);

        allocate(m_mesh->m_data.vertices_nodes,
                 m_mesh->m_metric.vertices_count,
                 p_vertices_count);
    } else {
        delete[] m_mesh->m_data.vertices_ids;
        m_mesh->m_data.vertices_ids = nullptr;

        delete[] m_mesh->m_data.vertices_nodes;
        m_mesh->m_data.vertices_nodes = nullptr;
    }

    m_mesh->m_metric.vertices_count = p_vertices_count;
}


void MeshModifier::makeSDBuffer(uint64_t p_size)
{
    if (p_size) {
        allocate(m_mesh->m_data.sd_buffer,
                 m_mesh->m_metric.sd_buffer_size,
                 p_size);
    } else {
        delete[] m_mesh->m_data.sd_buffer;
        m_mesh->m_data.sd_buffer = nullptr;
    }

    m_mesh->m_metric.sd_buffer_size = p_size;
}


void MeshModifier::removeUnusedNodes()
{
    if (empty())
        return;

    const Apos source_nodes_count = m_mesh->m_metric.nodes_count;
    Apos nodes_count{ 0 };
    uint8_t node;

    Apos *numeration = new Apos[source_nodes_count];
    memset(numeration, 0, source_nodes_count * sizeof(Apos));

    {
        const uint8_t regions_count = m_mesh->m_metric.regions_count;
        const mesh::Region *regions = m_mesh->m_data.regions;
        Apos *fes_nodes = m_mesh->m_data.fes_nodes;

        uint8_t fe_nodes_count;
        Apos fes_count;
        Apos fe;

        for (uint16_t r = 0; r < regions_count; ++r) {
            fes_count = regions[r + 1].first_fe - regions[r].first_fe;
            fe_nodes_count = regions[r].nodes_count;

            for (fe = 0; fe < fes_count; ++fe) {
                for (node = 0; node < fe_nodes_count; ++node) {
                    if (numeration[fes_nodes[fe * fe_nodes_count + node]] == 0) {
                        numeration[fes_nodes[fe * fe_nodes_count + node]] = nodes_count + 1;
                        ++nodes_count;
                    }
                }
            }

            fes_nodes += fes_count * fe_nodes_count;
        }

        if (nodes_count != source_nodes_count) {
            fes_nodes = m_mesh->m_data.fes_nodes;

            for (uint16_t r = 0; r < regions_count; ++r) {
                fes_count = regions[r + 1].first_fe - regions[r].first_fe;
                fe_nodes_count = regions[r].nodes_count;

                for (fe = 0; fe < fes_count; ++fe) {
                    for (node = 0; node < fe_nodes_count; ++node) {
                        fes_nodes[fe_nodes_count * fe + node] =
                            numeration[fes_nodes[fe_nodes_count * fe + node]] - 1;
                    }
                }

                fes_nodes += fes_count * fe_nodes_count;
            }
        }
    }

    if (nodes_count != source_nodes_count) {
        {
            const uint8_t boundary_regions_count = m_mesh->m_metric.boundary_regions_count;
            const mesh::Region *boundary_regions = m_mesh->m_data.boundary_regions;
            Apos *bes_nodes = m_mesh->m_data.bes_nodes;

            uint8_t be_nodes_count;
            Apos bes_count;
            Apos be;

            for (uint16_t r = 0; r < boundary_regions_count; ++r) {
                bes_count = boundary_regions[r + 1].first_fe - boundary_regions[r].first_fe;
                be_nodes_count = boundary_regions[r].nodes_count;

                for (be = 0; be < bes_count; ++be) {
                    for (node = 0; node < be_nodes_count; ++node) {
                        bes_nodes[be_nodes_count * be + node] =
                            numeration[bes_nodes[be_nodes_count * be + node]] - 1;
                    }
                }

                bes_nodes += bes_count * be_nodes_count;
            }
        }

        {
            const uint8_t twice_boundary_regions_count =
                m_mesh->m_metric.twice_boundary_regions_count;
            const mesh::Region *twice_boundary_regions = m_mesh->m_data.twice_boundary_regions;
            Apos *twice_bes_nodes = m_mesh->m_data.twice_bes_nodes;

            uint8_t twice_be_nodes_count;
            Apos twice_bes_count;
            Apos tbe;

            for (uint16_t r = 0; r < twice_boundary_regions_count; ++r) {
                twice_bes_count =
                    twice_boundary_regions[r + 1].first_fe - twice_boundary_regions[r].first_fe;
                twice_be_nodes_count = twice_boundary_regions[r].nodes_count;

                for (tbe = 0; tbe < twice_bes_count; ++tbe) {
                    for (node = 0; node < twice_be_nodes_count; ++node) {
                        twice_bes_nodes[twice_be_nodes_count * tbe + node] =
                            numeration[twice_bes_nodes[twice_be_nodes_count * tbe + node]] - 1;
                    }
                }

                twice_bes_nodes += twice_bes_count * twice_be_nodes_count;
            }
        }

        {
            const uint16_t vertices_count = m_mesh->m_metric.vertices_count;
            Apos *vertices_nodes = m_mesh->m_data.vertices_nodes;

            for (uint16_t vertex = 0; vertex < vertices_count; ++vertex)
                vertices_nodes[vertex] = numeration[vertices_nodes[vertex]] - 1;
        }

        const uint8_t dimension = m_mesh->m_metric.dimension;
        const Real *nodes_coords = m_mesh->m_data.nodes_coords;
        Real *new_nodes_data = new Real[dimension * nodes_count];

        for (Apos node = 0; node < source_nodes_count; ++node) {
            if (numeration[node]) {
                memcpy(new_nodes_data + (numeration[node] - 1) * dimension,
                       nodes_coords + node * dimension,
                       dimension * sizeof(Real));
            }
        }

        delete[] m_mesh->m_data.nodes_coords;
        m_mesh->m_data.nodes_coords = new_nodes_data;
        m_mesh->m_metric.nodes_count = nodes_count;
    }

    delete[] numeration;
}
