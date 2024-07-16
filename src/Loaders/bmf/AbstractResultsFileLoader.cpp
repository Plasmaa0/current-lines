#include "AbstractResultsFileLoader.h"

// Реализация методов AbstractResultsFileLoader::Parameters

using namespace bmf;
size_t AbstractResultsFileLoader::Parameters::size() const noexcept
{
    size_t res;
    
    if (m_is_array)
        res = m_array->size();
    else
        res = m_value == nullptr ? 0 : 1;
        
    return res;
}


Real* AbstractResultsFileLoader::Parameters::data() const noexcept
{
    if (m_is_array)
        return m_array->data();

    return m_value;
}

// Реализация методов AbstractResultsFileLoader

bool AbstractResultsFileLoader::checkBaseData(
    const NodalFields                     &p_nodal_fields,
    const std::vector<ElementalFieldsRef> &p_fe_fields,
    const Parameters                      &p_parameters,
    Apos                                   p_nodes_count,
    Apos                                   p_duplicated_nodes_count,
    uint8_t                                p_main_fields_count,
    uint8_t                                p_side_fields_count,
    uint8_t                                p_parameters_count,
    Exapos                                 p_fe_fields_data_size) const noexcept
{
    if (p_nodal_fields.nodesCount() != p_nodes_count) {
        ERROR_MESSAGE("Число узлов в файле результатов и контейнере полей не совпадает");
        return false;
    }

    if (p_side_fields_count > 0 &&
        p_nodal_fields.duplicatedNodesCount() != p_duplicated_nodes_count) {
        ERROR_MESSAGE("Число дублируемых узлов в файле результатов и контейнере полей "
                      "не совпадает");
        return false;
    }

    if (p_main_fields_count > 0 &&
        p_nodal_fields.mainFieldsCount() != p_main_fields_count) {
        ERROR_MESSAGE("Число полей главной секции в файле результатов и контейнере полей "
                      "не совпадает");
        return false;
    }

    if (p_side_fields_count > 0 &&
        p_nodal_fields.sideFieldsCount() != p_side_fields_count) {
        ERROR_MESSAGE("Число полей дополнительной секции в файле результатов и контейнере "
                      "полей не совпадает");
        return false;
    }

    if (p_parameters.size() && p_parameters.size() != p_parameters_count) {
        ERROR_MESSAGE("Число параметров в файле результатов и контейнере параметров "
                      "не совпадает");
        return false;
    }

    if (p_fe_fields_data_size > 0) {
        Exapos fe_fields_data_size{ 0 };

        // Проверка числа функций
        for (const ElementalFields &fields : p_fe_fields)
            fe_fields_data_size += fields.dataSize();

        if (fe_fields_data_size != p_fe_fields_data_size) {
            ERROR_MESSAGE("Некорректный блок данных полей в элементах");
            return false;
        }
    }

    return true;
}


bool AbstractResultsFileLoader::checkMesh(const Mesh &p_mesh,
                                          Apos        p_nodes_count) const noexcept
{
    if (p_mesh.empty()) {
        ERROR_MESSAGE("Отсутствует сетка");
        return false;
    }

    if (p_mesh.metric().nodes_count != p_nodes_count) {
        ERROR_MESSAGE("Число узлов в файле результатов не сопадает с "
                      "числом узлов в контейнере сетки");
        return false;
    }

    return true;
}
