#ifndef ELEMENTAL_FILEDS_H
#define ELEMENTAL_FILEDS_H

#include <string.h>
#include <math.h>
#include <type_traits>

//#include <MemoryTools>
#include "Arrays.h"
#include "Defs.h"

/// Сдвиг в массиве данных для контейнера полей к элементах

#   define FE_FIELDS_OFFSET(fe_num, term_num, fe_function_num, fe_functions_count, terms_count) \
        (fe_functions_count) * ((terms_count) * (fe_num) + (term_num)) + (fe_function_num)

class ElementalFields;

/// Контейнер полей в элементах для региона сетки

class RegionElementalFields final
{
    friend ElementalFields;

//    DISABLE_COPY(RegionElementalFields);

public:

    /// Метрика данных контейнера для региона сетки
    struct Metric
    {
        /// Число КЭ региона
        Apos fes_count{ 0 };

        /// Число полей в каждом КЭ
        uint8_t fields_count{ 0 };

        /// Число точек в КЭ для однозначного представления поля
        uint8_t terms_count{ 0 };
    };

public:

/// Конструктор

    RegionElementalFields() = default;

/// Получение представления в виде массива значений в узлах КЭ
/// \return Представление в виде массива значений в узлах КЭ

    inline Real* data() noexcept
    {
        return m_data;
    }

/// Получение представления в виде массива значений в узлах КЭ
/// \return Представление в виде массива значений в узлах КЭ

    inline const Real* data() const noexcept
    {
        return m_data;
    }

/// Получение представления в виде массива значений в узлах КЭ
/// \param[in] p_region_fe Номер КЭ региона
/// \return Представление в виде массива значений в узлах КЭ

    inline const Real* data(Apos p_region_fe) const noexcept
    {
        return m_data + p_region_fe * m_terms_count * m_fields_count;
    }

/// Зануление полей контейнера

    inline void nullify()
    {
        memset(m_data,
               0,
               m_terms_count * m_fields_count * m_fes_count * sizeof(Real));
    }

/// Получение числа полей в КЭ
/// \return Число полей в КЭ

    inline uint8_t fieldsCount() const noexcept
    {
        return m_fields_count;
    }

/// Получение числа КЭ
/// \return Число КЭ

    inline Apos FEsCount() const noexcept
    {
        return m_fes_count;
    }

/// Получение числа точек в КЭ для однозначного представления каждого поля
/// \return Число точек в КЭ для однозначного представления каждого поля

    inline uint8_t termsCount() const noexcept
    {
        return m_terms_count;
    }

/// Проверка совместимости двух контейнеров
/// \param[in] p_fields Второй контейнер для проверки
/// \param[in] p_current_functions_count Число КЭ в текущем контейнере
/// \param[in] p_passed_functions_count Число КЭ в переданном контейнере контейнере
/// \return Результат проверки

    inline bool checkCompatibility(
        const RegionElementalFields &p_fields,
        uint8_t                      p_current_functions_count,
        uint8_t                      p_passed_functions_count) const noexcept
    {
        return p_fields.m_terms_count == m_terms_count &&
               p_fields.m_fes_count >= m_fes_count &&
               m_fields_count == p_current_functions_count &&
               p_fields.m_fields_count == p_passed_functions_count;
    }

/// Проверка пределов изменения номеров КЭ
/// \param[in] p_start_fe Первый КЭ
/// \param[in] p_end_fe Последний КЭ
/// \return Результат проверки

    inline bool checkFEsLimits(Apos p_start_fe, Apos p_end_fe) const noexcept
    {
        return (p_end_fe - p_start_fe) <= m_fes_count && p_start_fe < p_end_fe;
    }

/// Проверка контейнера
/// \param[in] p_fes_count Число КЭ
/// \param[in] p_terms_count Число точек в КЭ для однозначного представления поля
/// \return Результат проверки

    inline bool check(Apos p_fes_count, uint8_t p_terms_count) const noexcept
    {
        return m_fes_count == p_fes_count && m_terms_count == p_terms_count;
    }

private:

    Real    *m_data{ nullptr };  //< Значения полей в узлах КЭ
    Apos     m_fes_count{ 0 };   //< Число КЭ региона
    uint8_t  m_fields_count{ 0 };//< Число полей в каждом КЭ
    uint8_t  m_terms_count{ 0 }; //< Число точек в КЭ для однозначного представления поля

}; // class FETypeElementalFields

/// Контейнер полей в элементах

class ElementalFields final
{
public:

    /// Метрика данных контейнера для одного региона
    using RegionMetric = RegionElementalFields::Metric;

public:

/// Конструктор
/// \param[in] p_fe_types_metrics Метрики данных контейнера для всех регионов

    inline explicit ElementalFields(const Array<RegionMetric> &p_regions_metrics = {})
    {
        resize(p_regions_metrics);
    }

/// Контруктор переноса
/// \param[in] p_fields Контейнер полей

    inline ElementalFields(ElementalFields &&p_fields) noexcept
    {
        operator=(std::move(p_fields));
    }

/// Изменение размера
/// \param[in] p_regions_metrics Метрики данных контейнера для всех регионов

    inline void resize(const Array<RegionMetric> &p_regions_metrics = {})
    {
        size_t buffer_size{ 0 };

        for (const auto &metric : p_regions_metrics) {
            buffer_size += metric.terms_count *
                           static_cast<size_t>(metric.fields_count) *
                           metric.fes_count;
        }

        if(buffer_size == 0) {
            clear();
            return;
        }

        m_data = new Real[buffer_size];

        m_fields.resize(p_regions_metrics.size());

        buffer_size = 0;

        uint8_t region_id{ 0 };

        for (const auto &metric : p_regions_metrics) {
            auto &fields = m_fields[region_id++];
            fields.m_data = m_data + buffer_size;
            fields.m_fes_count = metric.fes_count;
            fields.m_fields_count = metric.fields_count;
            fields.m_terms_count = metric.terms_count;
            buffer_size += metric.terms_count *
                           static_cast<size_t>(metric.fields_count) *
                           metric.fes_count;
        }

        nullify();
    }

/// Деструктор

    ~ElementalFields()
    {
        clear();
    }

/// Заполнение нулями массива функций

    inline void nullify()
    {
        size_t buffer_size{ 0 };

        for (const auto &fields : m_fields) {
            buffer_size += fields.m_terms_count *
                           static_cast<size_t>(fields.m_fields_count) *
                           fields.m_fes_count;
        }

        memset(m_data, 0, buffer_size * sizeof(Real));
    }

/// Отчистка памяти

    inline void clear()
    {
        m_fields.clear();
        delete[] m_data;
        m_data = nullptr;
    }

/// Оператор переноса
/// \param[in] p_fields Контейнер полей

    inline void operator=(ElementalFields &&p_fields) noexcept
    {
        clear();

        m_fields = std::move(p_fields.m_fields);
        m_data = p_fields.m_data;

        p_fields.m_data = nullptr;
    }

/// Оператор доступа к контейнеру полей для данного регоина
/// \param[in] p_region_id Номер регоина
/// \return Контейнер полей для данного регоина

    inline const RegionElementalFields& operator[](uint16_t p_region_id) const noexcept
    {
        return m_fields[p_region_id];
    }

/// Оператор доступа к контейнеру полей для данного регоина
/// \param[in] p_region_id Номер регоина
/// \return Контейнер полей для данного регоина

    inline RegionElementalFields& operator[](uint16_t p_region_id) noexcept
    {
        return m_fields[p_region_id];
    }

/// Получение представления в виде массива значений в узлах КЭ
/// \return Представление в виде массива значений в узлах КЭ

    inline Real* data() noexcept
    {
        return m_data;
    }

/// Получение представления в виде массива значений в узлах КЭ
/// \return Представление в виде массива значений в узлах КЭ

    inline const Real* data() const noexcept
    {
        return m_data;
    }

/// Получение объема данных для представления в виде массива значений в узлах КЭ
/// \return Объем данных для внутреннего представления

    inline size_t dataSize() const noexcept
    {
        size_t res{ 0 };

        for(const auto &region_fields : m_fields) {
            res += region_fields.m_fes_count *
                   region_fields.m_fields_count *
                   region_fields.m_terms_count;
        }

        return res;
    }

/// Получение числа полей (либо UINT8_MAX если число полей различно в регионах)
/// \return Число полей

    inline uint8_t fieldsCount() const noexcept
    {
        if(m_fields.empty())
            return 0;

        for(const auto &region_fields : m_fields) {
            if(region_fields.m_fields_count != m_fields.front().m_fields_count)
                return UINT8_MAX;
        }

        return m_fields.front().m_fields_count;
    }

/// Получение числа регионов
/// \return Число регионов

    inline uint8_t regionsCount() const noexcept
    {
        return static_cast<uint8_t>(m_fields.size());
    }

/// Проверка на пустоту
/// \return Состояние проверки

    inline bool empty() const noexcept
    {
        return m_fields.empty();
    }

private:

    // Поля для регионов
    Array<RegionElementalFields> m_fields;

    // Представление массива функций в виде значений в узлах КЭ
    Real *m_data{ nullptr };
};
#endif // ELEMENTAL_FILEDS_H
