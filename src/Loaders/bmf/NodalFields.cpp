// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <memory>
#include <string.h>

#include "NodalFields.h"

// Реализация методов NodalFields

NodalFields::NodalFields(const NodalFields &p_fields)
{
    NodalFieldsEditor editor(*this);
    editor.copy(p_fields, NodalFields::main);
    editor.copy(p_fields, NodalFields::side);
}


NodalFields& NodalFields::operator=(const NodalFields &p_fields)
{
    NodalFieldsEditor editor(*this);
    editor.copy(p_fields, NodalFields::main, false);
    editor.copy(p_fields, NodalFields::side, false);
    return *this;
}


NodalFields::~NodalFields()
{
    delete[] m_main_data;
    delete[] m_side_data;
}

// Реализация методов NodalFieldsModifier

void NodalFieldsModifier::nullifyData(NodalFields::Block p_block) noexcept
{
    Apos nodes_count;
    
    uint8_t fields_count;
    
    Real *data;
    
    if(p_block == NodalFields::main) {
        nodes_count = m_fields->m_nodes_count;
        fields_count = m_fields->m_main_fields_count;
        data = m_fields->m_main_data;
    } else {
        nodes_count = m_fields->m_nodes_count + m_fields->m_duplicated_nodes_count;
        fields_count = m_fields->m_side_fields_count;
        data = m_fields->m_side_data;
    }
    
#   pragma omp parallel for
    for (Apos nd = 0; nd < nodes_count; ++nd)
        memset(data + nd * fields_count, 0, fields_count * sizeof(Real));
}


// Реализация методов NodalFieldsEditor

void NodalFieldsEditor::makeMainData(uint8_t p_main_fields_count,
                                     Apos    p_nodes_count,
                                     Apos    p_spurious_nodes_count)
{
    if (m_fields->m_nodes_count != p_nodes_count) {
        delete[] m_fields->m_side_data;
        m_fields->m_side_data = nullptr;
        
        m_fields->m_side_fields_count = 0;
        m_fields->m_duplicated_nodes_count = 0;
        
        if(p_main_fields_count != 0)
            m_fields->m_nodes_count = p_nodes_count;
        else
            m_fields->m_nodes_count = 0;
    }
    
    if (m_fields->m_nodes_count != p_nodes_count ||
        m_fields->m_main_fields_count != p_main_fields_count) {
        delete[] m_fields->m_main_data;
        
        if (p_nodes_count != 0 && p_main_fields_count != 0)    {
            m_fields->m_main_data = 
                new Real[p_main_fields_count * (static_cast<size_t>(p_nodes_count) +
                                                p_spurious_nodes_count)];

            m_fields->m_main_fields_count = p_main_fields_count;
        } else {
            m_fields->m_main_data = nullptr;
            m_fields->m_main_fields_count = 0;
        }
    }
}


void NodalFieldsEditor::makeSideData(uint8_t p_side_fields_count,
                                     Apos    p_nodes_count,
                                     Apos    p_duplicated_nodes_count)
{
    if (m_fields->m_nodes_count != p_nodes_count) {
        delete[] m_fields->m_main_data;
        m_fields->m_main_data = nullptr;
        m_fields->m_main_fields_count = 0;
        
        if(p_side_fields_count != 0)
            m_fields->m_nodes_count = p_nodes_count;
        else
            m_fields->m_nodes_count = 0;
    }
    
    if (m_fields->m_nodes_count != p_nodes_count ||
        m_fields->m_duplicated_nodes_count != p_nodes_count ||
        m_fields->m_side_fields_count != p_side_fields_count) {
        delete[] m_fields->m_side_data;

        if (p_nodes_count != 0 && p_side_fields_count != 0) {
            const size_t total_nodes_count = static_cast<size_t>(p_nodes_count) + 
                static_cast<size_t>(p_duplicated_nodes_count);

            auto side_data_size = p_side_fields_count * total_nodes_count;
            m_fields->m_side_data = new Real[side_data_size];
            memset(m_fields->m_side_data, 0, sizeof(Real) * side_data_size);
                
            m_fields->m_side_fields_count = p_side_fields_count;
            m_fields->m_duplicated_nodes_count = p_duplicated_nodes_count;
        } else {
            m_fields->m_side_data = nullptr;
            
            m_fields->m_side_fields_count = 0;
            m_fields->m_duplicated_nodes_count = p_duplicated_nodes_count;
        }
    }
}


void NodalFieldsEditor::clear(NodalFields::Block p_block)
{
    if (p_block == NodalFields::main) {
        delete[] m_fields->m_main_data;
        m_fields->m_main_data = nullptr;

        m_fields->m_main_fields_count = 0;

        if (m_fields->emptySideData())
            m_fields->m_nodes_count = 0;
    } else {
        delete[] m_fields->m_side_data;
        m_fields->m_side_data = nullptr;

        m_fields->m_side_fields_count = 0;
        m_fields->m_duplicated_nodes_count = 0;

        if (m_fields->emptyMainData())
            m_fields->m_nodes_count = 0;
    }
}


void NodalFieldsEditor::swap(NodalFields        &p_fields,
                             NodalFields::Block  p_block) noexcept
{
    if (m_fields->m_nodes_count != 0 && p_fields.m_nodes_count != 0 &&
        m_fields->m_nodes_count != p_fields.m_nodes_count)
        return;

    const Apos nodes_count =
        m_fields->m_nodes_count != 0 ? m_fields->m_nodes_count : p_fields.m_nodes_count;
    
    if(p_block == NodalFields::main) {
        std::swap(m_fields->m_main_data, p_fields.m_main_data);
        std::swap(m_fields->m_main_fields_count, p_fields.m_main_fields_count);

        if (!m_fields->emptySideData() || !p_fields.emptySideData()) {
            m_fields->m_nodes_count = nodes_count;
            p_fields.m_nodes_count = nodes_count;
        } else
            std::swap(m_fields->m_nodes_count, p_fields.m_nodes_count);
    } else {
        std::swap(m_fields->m_side_data, p_fields.m_side_data);
        std::swap(m_fields->m_side_fields_count, p_fields.m_side_fields_count);

        if (!m_fields->emptyMainData() || !p_fields.emptyMainData()) {
            m_fields->m_nodes_count = nodes_count;
            p_fields.m_nodes_count = nodes_count;
        } else
            std::swap(m_fields->m_nodes_count, p_fields.m_nodes_count);
    }
}


void NodalFieldsEditor::copy(const NodalFields &p_source_fields,
                             NodalFields::Block p_block,
                             bool               p_use_minimal_nodes_count)
{
    if ((p_block == NodalFields::main && p_source_fields.emptyMainData()) ||
        (p_block == NodalFields::side && p_source_fields.emptySideData()))
        return;

    Apos nodes_count;
    uint8_t fields_count;

    const Real *src_data;
    Real *dest_data;

    if(p_use_minimal_nodes_count)
        nodes_count = mat::min(p_source_fields.nodesCount(), m_fields->nodesCount());
    else
        nodes_count = p_source_fields.nodesCount();

    if (p_block == NodalFields::main) {
        makeMainData(p_source_fields.mainFieldsCount(), nodes_count);
        fields_count = p_source_fields.mainFieldsCount();

        src_data = p_source_fields.mainData();
        dest_data = m_fields->m_main_data;
    } else {
        const Apos duplicated_nodes_count = mat::min(p_source_fields.duplicatedNodesCount(),
                                                     m_fields->duplicatedNodesCount());

        makeSideData(p_source_fields.sideFieldsCount(), nodes_count, duplicated_nodes_count);

        nodes_count += duplicated_nodes_count;
        fields_count = p_source_fields.sideFieldsCount();

        src_data = p_source_fields.sideData();
        dest_data = m_fields->m_side_data;
    }

    if(dest_data) {
#   pragma omp parallel for
        for (Apos nd = 0; nd < nodes_count; ++nd) {
            memcpy(dest_data + nd * fields_count,
                   src_data + nd * fields_count,
                   fields_count * sizeof(Real));
        }
    }
}


void NodalFieldsEditor::swapSections() noexcept
{
    if (m_fields->duplicatedNodesCount() > 0)
        return;

    std::swap(m_fields->m_main_data, m_fields->m_side_data);
    std::swap(m_fields->m_main_fields_count, m_fields->m_side_fields_count);
}
