#ifndef NODAL_FIELDS_H
#define NODAL_FIELDS_H

#include <memory>
#include <vector>

#include "Defs.h"

/// Контейнер полей, заданных узловыми значениями

class NodalFields final
{
    friend class NodalFieldsModifier;
    friend class NodalFieldsVectorModifier;
    friend class NodalFieldsEditor;
    
public:

/// Тип блока данных

    enum Block
    {
        main,
        side
    };

/// Конструктор по умолчанию

    NodalFields() = default;

/// Конструктор копирования

    NodalFields(const NodalFields& p_fields);

/// Конструктор переноса

    inline NodalFields(NodalFields &&p_fields) noexcept
    {
        swap(p_fields);
    }

/// Оператор копирования

    NodalFields& operator=(const NodalFields& p_fields);

/// Оператор переноса

    inline NodalFields& operator=(NodalFields &&p_fields) noexcept
    {
        if (this != &p_fields) {
            delete[] m_main_data;
            delete[] m_side_data;
            swap(p_fields);
        }

        return *this;
    }

/// Деструктор

    ~NodalFields();

/// Вернуть указатель на основной блок

    inline const Real* mainData() const noexcept
    {
        return m_main_data;
    }

/// Вернуть указатель на дополнительный блок

    inline const Real* sideData() const noexcept
    {
        return m_side_data;
    }

/// Вернуть блок данных в соотвествии с 
/// идентификтором поля

    inline const Real* data(uint8_t  p_field_id,
                            uint8_t &o_offset,
                            uint8_t &o_fields_count) const noexcept
    {
        const Real *res;
        
        if (p_field_id < m_main_fields_count) {
            o_offset = 0;
            o_fields_count = m_main_fields_count;
            res = m_main_data;
        } else if (p_field_id < (m_main_fields_count + m_side_fields_count)) {
            o_offset = m_main_fields_count;
            o_fields_count = m_side_fields_count;
            res = m_side_data;
        } else {
            o_offset = 0;
            o_fields_count = 0;
            res = nullptr;
        }

        return res;
    }

/// Вернуть число полей в основном блоке
    
    inline uint8_t mainFieldsCount() const noexcept
    {
        return m_main_fields_count;
    }

/// Вернуть число полей в дополнительном блоке

    inline uint8_t sideFieldsCount() const noexcept
    {
        return m_side_fields_count;
    }

/// Вернуть общее число полей

    inline uint8_t fieldsCount() const noexcept
    {
        return m_main_fields_count + m_side_fields_count;
    }

/// Вернуть число узлов

    inline Apos nodesCount() const noexcept
    {
        return m_nodes_count;
    }
    
/// Вернуть число дублируемых узлов
/// (для разрывных полей)

    inline Apos duplicatedNodesCount() const noexcept
    {
        return m_duplicated_nodes_count;
    }

/// Вернуть общее число узлов

    inline Apos totalNodesCount() const noexcept
    {
        return m_nodes_count + m_duplicated_nodes_count;
    }

/// Вернуть размер данных в контейнере

    inline Apos size() const noexcept
    {
        return m_main_fields_count * m_nodes_count + 
               m_side_fields_count * (m_nodes_count + m_duplicated_nodes_count);
    }

/// Проверка блока основных полей

    inline bool checkMainData(uint8_t p_fields_count,
                              Apos    p_nodes_nount) const noexcept
    {
        return m_main_fields_count == p_fields_count &&
            m_nodes_count == p_nodes_nount;
    }

/// Проверка блока дополнительных полей

    inline bool checkSideData(uint8_t p_side_fields_threshold,
                              Apos    p_nodes_nount,
                              Apos    p_duplicated_nodes_count = 0) const noexcept
    {
        return m_side_fields_count >= p_side_fields_threshold &&
               m_nodes_count == p_nodes_nount &&
               m_duplicated_nodes_count == p_duplicated_nodes_count;
    }

/// Пуста ли секция основных данных

    inline bool emptyMainData() const noexcept
    {
        return m_main_data == nullptr;
    }

/// Пуста ли секция дополнительных данных

    inline bool emptySideData() const noexcept
    {
        return m_side_data == nullptr;
    }

/// Пуст ли контейнер
    
    inline bool empty() const noexcept
    {
        return m_main_data == nullptr &&
               m_side_data == nullptr;
    }

/// Обмен данных контейнеров

    inline void swap(NodalFields &p_fields) noexcept
    {
        std::swap(m_main_fields_count, p_fields.m_main_fields_count);
        std::swap(m_side_fields_count, p_fields.m_side_fields_count);
        std::swap(m_nodes_count, p_fields.m_nodes_count);
        std::swap(m_duplicated_nodes_count, p_fields.m_duplicated_nodes_count);
        
        std::swap(m_main_data, p_fields.m_main_data);
        std::swap(m_side_data, p_fields.m_side_data);
    }

private:

    // Основной и дополнительный блоки данных
    Real *m_main_data{ nullptr };
    Real *m_side_data{ nullptr };

    // Число узлов
    Apos m_nodes_count{ 0 };

    // Число дублируемых узлов
    // (для разрывных полей в подобластях)
    Apos m_duplicated_nodes_count{ 0 };

    // Число полей в основном блоке
    uint8_t m_main_fields_count{ 0 };

    // Число полей в дополнительном блоке
    // (в общем случае разрывных)
    uint8_t m_side_fields_count{ 0 };
}; // class NodalFields

/// Модификатор для контейнера NodalFields

class NodalFieldsModifier final
{
public:

/// Конструктор

    inline NodalFieldsModifier(NodalFields &p_fields) :
        m_fields(std::addressof(p_fields))
    {}

/// Вернуть ссылку на контейнер

    inline const NodalFields& fields() const noexcept
    {
        return *m_fields;
    }

/// Вернуть указатель на основной блок

    inline Real* mainData() noexcept
    {
        return m_fields->m_main_data;
    }

/// Вернуть указатель на дополнительный блок

    inline Real* sideData() noexcept
    {
        return m_fields->m_side_data;
    }

/// Зануление блока данных

    void nullifyData(NodalFields::Block p_block) noexcept;

private:
    NodalFields *m_fields;
}; // class NodalFieldsModifier

/// Модификатор для вектора контейнеров NodalFields

class NodalFieldsVectorModifier final
{
public:

/// Конструктор

    inline NodalFieldsVectorModifier(std::vector<NodalFields> &p_fields)
    {
        m_fields.reserve(p_fields.size());

        for (auto &fields : p_fields)
            m_fields.emplace_back(std::addressof(fields));
    }

/// Вернуть число элементов

    inline size_t size() const noexcept
    {
        return m_fields.size();
    }

/// Вернуть первый элемент

    inline const NodalFields& front() const noexcept
    {
        return *m_fields.front();
    }

/// Вернуть последний элемент

    inline const NodalFields& back() const noexcept
    {
        return *m_fields.back();
    }

/// Вернуть ссылку на контейнер

    inline const NodalFields& operator[](size_t p_id) const
    {
        return *m_fields[p_id];
    }

/// Вернуть указатель на основной блок

    inline Real* mainData(size_t p_id) noexcept
    {
        if (p_id >= m_fields.size())
            return nullptr;
        
        return m_fields[p_id]->m_main_data;
    }

/// Вернуть указатель на дополнительный блок

    inline Real* sideData(size_t p_id) noexcept
    {
        if (p_id >= m_fields.size())
            return nullptr;

        return m_fields[p_id]->m_side_data;
    }

private:
    std::vector<NodalFields*> m_fields;
}; // class NodalFieldsVectorModifier

/// Модификатор для контейнера
/// NodalFields c полным доступом

class NodalFieldsEditor final
{
public:

/// Конструктор

    inline NodalFieldsEditor(NodalFields &p_fields) :
        m_fields(std::addressof(p_fields))
    {}

/// Вернуть ссылку на контейнер

    inline const NodalFields& fields() const noexcept
    {
        return *m_fields;
    }

    inline NodalFields& fields() noexcept
    {
        return *m_fields;
    }

/// Вернуть модификатор

    inline NodalFieldsModifier modifier() const noexcept
    {
        return NodalFieldsModifier(*m_fields);
    }

/// Вернуть указатель на основной блок

    inline Real* mainData() noexcept
    {
        return m_fields->m_main_data;
    }

/// Вернуть указатель на дополнительный блок

    inline Real* sideData() noexcept
    {
        return m_fields->m_side_data;
    }

/// Выделение памяти под основную секцию данных

    void makeMainData(uint8_t p_main_fields_count,
                      Apos    p_nodes_count,
                      Apos    p_spurious_nodes_count = 0);

/// Выделение памяти под дополнительную секцию данных

    void makeSideData(uint8_t p_side_fields_count,
                      Apos    p_nodes_nount,
                      Apos    p_duplicated_nodes_count = 0);

/// Выделение памяти под поля в узлах

    inline void makeData(uint8_t p_main_fields_count,
                         uint8_t p_side_fields_count,
                         Apos    p_nodes_count,
                         Apos    p_duplicated_nodes_count = 0)
    {
        makeMainData(p_main_fields_count, p_nodes_count);
        makeSideData(p_side_fields_count, p_nodes_count, p_duplicated_nodes_count);
    }

/// Отчистка памяти секции данных

    void clear(NodalFields::Block p_block);

/// Отчистка контейнера

    inline void clear()
    {
        clear(NodalFields::main);
        clear(NodalFields::side);
    }

/// Обмен секций данных контейтеров

    void swap(NodalFields &p_fields, NodalFields::Block p_block) noexcept;

/// Обмен секций контейнера

    void swapSections() noexcept;

/// Копирование секций данных контейнеров

    void copy(const NodalFields &p_source_fields,
              NodalFields::Block p_block,
              bool               p_use_minimal_nodes_count = true);

/// Зануление основного блока данных

    inline void nullifyData(NodalFields::Block p_block) noexcept
    {
        NodalFieldsModifier(*m_fields).nullifyData(p_block);
    }

private:
    NodalFields *m_fields;
}; // class NodalFieldsEditor

#endif // NODAL_FIELDS_H
