#include <sys/stat.h>
#include <stdio.h>

#include "BMFFileLoader.h"
#include "BMFStructures.h"

using namespace bmf;
unsigned long long fileSize(const std::string &p_path)
{
#   if defined(OS_LINUX)
#        define STRUCT_STAT64 stat64
#        define STAT64 stat64
#   elif defined(OS_WIN)
#        define STRUCT_STAT64 __stat64
#       define STAT64 _stat64
#   else
#        error Platform is not supported
#   endif

    struct STRUCT_STAT64 sb;

    STAT64(p_path.c_str(), &sb);

    return sb.st_size;

#   undef STRUCT_STAT64
#   undef STAT64
}

// Чтение секции конечных элементов

bool readFEsSection(FILE *p_file, MeshModifier p_mesh_modifier)
{
    enum { elements_count = 3 };
    
    size_t reading_elements;

    uint8_t  regions_count;
    uint16_t subareas_count;
    Apos     domains_count;

    reading_elements = fread(&regions_count, sizeof(uint8_t), 1, p_file);
    reading_elements += fread(&subareas_count, sizeof(uint16_t), 1, p_file);
    reading_elements += fread(&domains_count, sizeof(Apos), 1, p_file);

    if (reading_elements != elements_count || regions_count == 0) {
        ERROR_MESSAGE("Некорректный блок метрики секции конечных элементов");
        return false;
    }

    Array<Apos>         regions_elements_count(regions_count);
    Array<uint8_t>      regions_elements_nodes_count(regions_count);
    Array<mesh::FEType> regions_elements_types(regions_count);

    reading_elements = fread(regions_elements_count.data(), sizeof(Apos), regions_count, p_file);
    reading_elements += fread(regions_elements_nodes_count.data(),
                              sizeof(uint8_t),
                              regions_count,
                              p_file);
    reading_elements += fread(regions_elements_types.data(),
                              sizeof(mesh::FEType),
                              regions_count,
                              p_file);

    if(reading_elements != 3 * regions_count) {
        ERROR_MESSAGE("Некорректный блок данных о регионах секции конечных элементов");
        return false;
    }

    if (!p_mesh_modifier.makeElements(MeshModifier::finite_elements,
                                      regions_elements_count,
                                      regions_elements_nodes_count,
                                      regions_elements_types,
                                      subareas_count,
                                      domains_count)) {
        ERROR_MESSAGE("Некорректный блок метрики секции конечных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().subareas_ids,
                             sizeof(uint16_t),
                             subareas_count,
                             p_file);

    if (subareas_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов подобластей секции конечных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().domains_subareas_ids,
                             sizeof(uint16_t),
                             domains_count,
                             p_file);

    if (domains_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов подобластей доменов секции конечных "
                      "элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().domains_regions_ids,
                             sizeof(uint8_t),
                             domains_count,
                             p_file);

    if (domains_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов регионов доменов секции конечных "
                      "элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().fes_num_limits,
                             sizeof(Apos),
                             domains_count + 1,
                             p_file);

    if (static_cast<size_t>(domains_count) + 1 != reading_elements) {
        ERROR_MESSAGE("Некорректный блок пределов изменения номеров конечных элементов в "
                      "доменах секции конечных элементов");
        return false;
    }

    const auto fes_nodes_size = p_mesh_modifier.data().regions[regions_count].fes_nodes_offset;

    reading_elements = fread(p_mesh_modifier.data().fes_nodes,
                             sizeof(Apos),
                             fes_nodes_size,
                             p_file);

    if (fes_nodes_size != reading_elements) {
        ERROR_MESSAGE("Некорректный блок конечных элементов");
        return false;
    }

    return true;    
}

// Чтение секции граничных элементов

bool readBEsSection(FILE *p_file, MeshModifier p_mesh_modifier)
{
    enum { elements_count = 3 };

    size_t reading_elements;

    uint8_t  regions_count;
    uint16_t subareas_count;
    Apos     domains_count;

    reading_elements = fread(&regions_count, sizeof(uint8_t), 1, p_file);
    reading_elements += fread(&subareas_count, sizeof(uint16_t), 1, p_file);
    reading_elements += fread(&domains_count, sizeof(Apos), 1, p_file);

    if (reading_elements != elements_count || regions_count == 0) {
        ERROR_MESSAGE("Некорректный блок метрики секции граничных элементов");
        return false;
    }

    Array<Apos>         regions_elements_count(regions_count);
    Array<uint8_t>      regions_elements_nodes_count(regions_count);
    Array<mesh::FEType> regions_elements_types(regions_count);

    reading_elements = fread(regions_elements_count.data(), sizeof(Apos), regions_count, p_file);
    reading_elements += fread(regions_elements_nodes_count.data(),
                              sizeof(uint8_t),
                              regions_count,
                              p_file);
    reading_elements += fread(regions_elements_types.data(),
                              sizeof(mesh::FEType),
                              regions_count,
                              p_file);

    if(reading_elements != 3 * regions_count) {
        ERROR_MESSAGE("Некорректный блок данных о регионах секции граничных элементов");
        return false;
    }

    if (!p_mesh_modifier.makeElements(MeshModifier::boundary_elements,
                                      regions_elements_count,
                                      regions_elements_nodes_count,
                                      regions_elements_types,
                                      subareas_count,
                                      domains_count)) {
        ERROR_MESSAGE("Некорректный блок метрики секции граничных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().boundary_domains_subareas_ids,
                             sizeof(uint16_t),
                             domains_count,
                             p_file);

    if (domains_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов подобластей доменов секции граничных "
                      "элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().boundary_domains_regions_ids,
                             sizeof(uint8_t),
                             domains_count,
                             p_file);

    if (domains_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов регионов доменов секции граничных "
                      "элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().bes_num_limits,
                             sizeof(Apos),
                             domains_count + 1,
                             p_file);

    if (static_cast<size_t>(domains_count) + 1 != reading_elements) {
        ERROR_MESSAGE("Некорректный блок пределов изменения номеров граничных элементов в "
                      "доменах секции граничных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().bes_fes,
                             2 * sizeof(Apos),
                             p_mesh_modifier.metric().bes_count,
                             p_file);

    if (p_mesh_modifier.metric().bes_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок пар конечного элемента, к которым принадлежит данный "
                      "граничный элемент секции граничных элементов");
        return false;
    }

    const auto bes_nodes_size =
        p_mesh_modifier.data().boundary_regions[regions_count].fes_nodes_offset;

    reading_elements = fread(p_mesh_modifier.data().bes_nodes,
                             sizeof(Apos),
                             bes_nodes_size,
                             p_file);

    if (bes_nodes_size != reading_elements) {
        ERROR_MESSAGE("Некорректный блок граничных элементов");
        return false;
    }

    return true;
}

// Чтение секции дважды граничных элементов

bool readTwiceBEsSection(FILE *p_file, MeshModifier p_mesh_modifier)
{
    enum { elements_count = 2 };

    size_t reading_elements;

    uint8_t  regions_count;
    uint16_t subareas_count;

    reading_elements = fread(&regions_count, sizeof(uint8_t), 1, p_file);
    reading_elements += fread(&subareas_count, sizeof(uint16_t), 1, p_file);

    if (reading_elements != elements_count || regions_count == 0) {
        ERROR_MESSAGE("Некорректный блок метрики секции дважды граничных элементов");
        return false;
    }

    Array<Apos>         regions_elements_count(regions_count);
    Array<uint8_t>      regions_elements_nodes_count(regions_count);
    Array<mesh::FEType> regions_elements_types(regions_count);

    reading_elements = fread(regions_elements_count.data(), sizeof(Apos), regions_count, p_file);
    reading_elements += fread(regions_elements_nodes_count.data(),
                              sizeof(uint8_t),
                              regions_count,
                              p_file);
    reading_elements += fread(regions_elements_types.data(),
                              sizeof(mesh::FEType),
                              regions_count,
                              p_file);

    if(reading_elements != 3 * regions_count) {
        ERROR_MESSAGE("Некорректный блок данных о регионах секции дважды граничных элементов");
        return false;
    }

    if (!p_mesh_modifier.makeElements(MeshModifier::twice_boundary_elements,
                                      regions_elements_count,
                                      regions_elements_nodes_count,
                                      regions_elements_types,
                                      subareas_count,
                                      subareas_count)) {
        ERROR_MESSAGE("Некорректный блок метрики секции дважды граничных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().twice_boundary_subareas_ids,
                             sizeof(uint16_t),
                             subareas_count,
                             p_file);

    if (subareas_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов подобластей секции дважды "
                      "граничных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().twice_boundary_subareas_regions_ids,
                             sizeof(uint8_t),
                             subareas_count,
                             p_file);

    if (subareas_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов регионов подобластей секции дважды "
                      "раничных элементов");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().twice_bes_num_limits,
                             sizeof(Apos),
                             subareas_count + 1,
                             p_file);

    if (static_cast<size_t>(subareas_count) + 1 != reading_elements) {
        ERROR_MESSAGE("Некорректный блок пределов изменения номеров дважды граничных элементов в "
                      "подобластях секции граничных элементов");
        return false;
    }

    const auto twice_bes_nodes_size =
        p_mesh_modifier.data().twice_boundary_regions[regions_count].fes_nodes_offset;

    reading_elements = fread(p_mesh_modifier.data().twice_bes_nodes,
                             sizeof(Apos),
                             twice_bes_nodes_size,
                             p_file);

    if (twice_bes_nodes_size != reading_elements) {
        ERROR_MESSAGE("Некорректный блок дважды граничных элементов");
        return false;
    }

    return true;
}

// Чтение секции вершин

bool readVerticesSection(FILE         *p_file,
                         MeshModifier  p_mesh_modifier)
{
    uint16_t vertices_count;
    
    enum { elements_count = 1 };

    size_t reading_elements;
        
    reading_elements = fread(&vertices_count, sizeof(uint16_t), 1, p_file);

    if (reading_elements != elements_count || vertices_count == 0) {
        ERROR_MESSAGE("Некорректный блок метрики секции вершин");
        return false;
    }

    p_mesh_modifier.makeVertices(vertices_count);

    reading_elements = fread(p_mesh_modifier.data().vertices_ids,
                             sizeof(uint16_t),
                             vertices_count,
                             p_file);

    if (vertices_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок идентификаторов вершин");
        return false;
    }

    reading_elements = fread(p_mesh_modifier.data().vertices_nodes,
                             sizeof(Apos),
                             vertices_count,
                             p_file);

    if (vertices_count != reading_elements) {
        ERROR_MESSAGE("Некорректный блок вершин");
        return false;
    }
    
    return true;
}

// Реализация методов BMFFileLoader

bool BMFFileLoader::load(MeshModifier p_mesh_modifier) const
{
    FILE *file = fopen(fileName().c_str(), "rb");

    if (file == nullptr) {
        ERROR_MESSAGE("Файл с именем " << fileName() << " не найден.");
        return false;
    }

    const auto size = fileSize(fileName());

    PROGRESS_STATUS(1, 100);

    // Число прочитанных элементов
    size_t reading_elements;

    bool res;

    p_mesh_modifier.clear();

    // Считывание секции размеров базовых типов
    {
        uint16_t apos_size, real_size;
        
        enum { elements_count = 2 };
        
        reading_elements = fread(&apos_size, sizeof(uint16_t), 1, file);
        reading_elements += fread(&real_size, sizeof(uint16_t), 1, file);
        
        if(reading_elements != elements_count) {
            fclose(file);

            ERROR_MESSAGE("Отсутствует секция размеров базовых типов");
            return false;
        }
        
        if(apos_size != sizeof(Apos) || real_size != sizeof(Real)) {
            fclose(file);

            ERROR_MESSAGE("Неподдерживаемая версия формата BMF");
            return false;
        }    
    }

    // Считывание секции узлов
    {
        uint8_t dimension;
        Apos nodes_count;

        enum { elements_count = 2 };

        reading_elements = fread(&dimension, sizeof(uint8_t), 1, file);
        reading_elements += fread(&nodes_count, sizeof(Apos), 1, file);

        if (reading_elements != elements_count) {
            fclose(file);

            ERROR_MESSAGE("Некорректный блок метрики узлов");
            return false;
        }

         // Выделение памяти под узлы
        res = p_mesh_modifier.makeNodes(nodes_count, dimension);

        if (!res) {
            fclose(file);
            ERROR_MESSAGE("Некорректный блок метрики узлов");
            return false;
        }

        reading_elements = fread(p_mesh_modifier.data().nodes_coords, 
                                 sizeof(Real) * dimension, 
                                 nodes_count, 
                                 file);

        if (nodes_count != reading_elements) {
            fclose(file);
            p_mesh_modifier.clear();

            ERROR_MESSAGE("Некорректная секция узлов");
            return false;
        }
    }

    PROGRESS_STATUS(ftell(file), size);

    uint8_t section_id;

    while(fread(&section_id, sizeof(uint8_t), 1, file) == 1) {
        switch(section_id) {
            case mesh::BMFSectionsIDs::finite_elements:
                res = readFEsSection(file, p_mesh_modifier);
            break;
            
            case mesh::BMFSectionsIDs::boundary_elements:
                res = readBEsSection(file, p_mesh_modifier);
            break;
            
            case mesh::BMFSectionsIDs::twice_boundary_elements:
                res = readTwiceBEsSection(file, p_mesh_modifier);
            break;

            case mesh::BMFSectionsIDs::vertices:
                res = readVerticesSection(file, p_mesh_modifier);
            break;
            
            case mesh::BMFSectionsIDs::special_data_buffer:{
                uint64_t sd_buffer_size;

                enum { elements_count = 1 };

                reading_elements = fread(&sd_buffer_size, sizeof(uint64_t), 1, file);

                if (reading_elements != elements_count || sd_buffer_size == 0) {
                    fclose(file);
                    p_mesh_modifier.clear();

                    ERROR_MESSAGE("Некорректный блок метрики секции буфера специальных данных");
                    return false;
                }

                p_mesh_modifier.makeSDBuffer(sd_buffer_size);

                reading_elements = fread(p_mesh_modifier.data().sd_buffer,
                                         sizeof(uint8_t),
                                         sd_buffer_size,
                                         file);

                if (sd_buffer_size != reading_elements) {
                    ERROR_MESSAGE("Некорректный блок специальных данных");
                    res = false;
                }
            }
            break;

            default: {
                ERROR_MESSAGE("Неизвестный тип блока");
                res = false;
            }
            break;
        }
        
        PROGRESS_STATUS(ftell(file), size);
        
        if(!res) {
            fclose(file);
            p_mesh_modifier.clear();
            
            return false;
        }
    }

//    if(p_mesh_modifier.metric().twice_bes_count == 0)
//        mesh::findTwiceBEs(p_mesh_modifier);

//    if (p_mesh_modifier.metric().vertices_count == 0)
//        mesh::findVertices(p_mesh_modifier);

    PROGRESS_STATUS(100, 100);

    fclose(file);

    return true;
}
