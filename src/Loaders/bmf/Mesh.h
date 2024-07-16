#ifndef MESH_H
#define MESH_H

#include <utility>
#include <memory>
#include <array>

#include "MeshStructures.h"


/// Сетка области
namespace bmf{
class Mesh final
{
    friend class MeshModifier;
    friend class MeshEditor;

    DISABLE_COPY(Mesh);

public:

/// Конструктор по умолчанию

    inline Mesh() noexcept:
        m_data()
    {}

/// Конструктор переноса
/// \param[in] p_mesh Сетка

    inline Mesh(Mesh &&p_mesh) noexcept:
        m_data()
    {
        operator=(std::move(p_mesh));
    }

/// Деструктор

    inline ~Mesh()
    {
        clear();
    }

/// Получение метрики сетки
/// \return Метрика сетки

    inline const mesh::Metric& metric() const noexcept
    {
        return m_metric;
    }

/// Получение метрики конечного элемента
/// \param[in] p_type Тип конечного элемента
/// \return Метрика конечного элемента

    static mesh::FETypeMetric FETypeMetric(mesh::FEType p_type) noexcept;

/// Получение масок конечного элемента
/// \param[in] p_type Тип конечного элемента
/// \return Маски конечного элемента

    static mesh::FETypeMasks FETypeMasks(mesh::FEType p_type);

/// Проверка типа КЭ региона
/// \param[in] p_region Данные региона
/// \param[in] p_fe_type Тип КЭ
/// \param[in] p_edge_inner_nodes_count Число внутренних на каждом ребре конечного элемента
/// \param[in] p_is_inner_nodes Флаг наличия внутренних узлов
/// \return Результат проверки

    static bool checkRegionFEType(const mesh::Region &p_region,
                                  mesh::FEType        p_fe_type,
                                  uint8_t             p_edge_inner_nodes_count = 0,
                                  bool                p_is_inner_nodes = false) noexcept;

/// Получение сигнатур конечных элементов
/// \return Сигнатуры конечных элементов

    Array<mesh::FETypeSignature> FETypesSignatures() const;

/// Получение данных сетки
/// \return Данные сетки

    inline const mesh::Data& data() const noexcept
    {
        return m_public_data;
    }

/// Получение представления конечно-объёмной сетки
/// \return Представление конечно-объёмной сетки

    inline const mesh::FVData& FVData() const noexcept
    {
        return m_fv_public_data;
    }

/// Возвращает конечно-объёмную метрику пользователю
/// \return Метрика сетки

    inline const mesh::FVMetric& FVMetric() const noexcept
    {
        return m_fv_metric;
    }


/// Проверка контейнера на пустоту
/// \return Результат проверки

    inline bool empty() const noexcept
    {
        return (m_metric.nodes_count == 0) ||
               ((m_metric.fes_count == 0) && (m_metric.bes_count == 0));
    }

/// Проверка геометрии на однородность
/// \param[in] p_type Тип подобластей геометрии
/// \return Результат проверки

    inline bool isHomogeneousGeometry(mesh::SubareaType p_type) const noexcept
    {
        for (uint8_t r = 0; r < m_metric.regions_count; ++r)
            if (mesh::FETypeSubareaType(m_data.regions[r].type) != p_type)
                return false;

        return true;
    }

/// Проверка наличия подобластей заданного типа в геометрии
/// \param[in] p_type Тип подобластей геометрии
/// \return Результат проверки

    inline bool isGeometryType(mesh::SubareaType p_type) const noexcept
    {
        for (uint8_t r = 0; r < m_metric.regions_count; ++r)
            if (mesh::FETypeSubareaType(m_data.regions[r].type) == p_type)
                return true;

        return false;
    }

/// Поиск узла по координатам
/// \param[in] p_point Координаты узла
/// \return Номер узла сетки

    Apos findNode(const std::array<Real, mesh::max_dimension> &p_point) const;

/// Отчистка контейнера

    void clear();

/// Оператор переноса
/// \param[in] p_mesh Сетка

    void operator=(Mesh &&p_mesh) noexcept;

private:

    // Метрика сетки
    mesh::Metric m_metric;

    // Данные сетки
    union
    {
        mesh::DataPrivate m_data;
        mesh::Data m_public_data;
    };

    // Метрика конечно-объёмной сетки
    mesh::FVMetric m_fv_metric;

    // Конечно-объёмная сетка
    union
    {
        mesh::FVDataPrivate m_fv_data;
        mesh::FVData m_fv_public_data;
    };
}; // class Mesh

/// Модификатор сетки с ограниченным доступом

class MeshModifier final
{
public:

    /// Секции конечных элементов сетки
    enum ElementsSection: uint8_t
    {
        /// Секция конечных элементов
        finite_elements,

        /// Граничный элемент
        boundary_elements,

        /// Секция дважды граничный элемент
        twice_boundary_elements
    };

public:

/// Конструктор
/// \param[in] p_mesh Сетка

    inline MeshModifier(Mesh &p_mesh) noexcept:
        m_mesh(std::addressof(p_mesh)){}

/// Проверка контейнера на пустоту
/// \return Результат проверки

    inline bool empty() const noexcept
    {
        return m_mesh->empty();
    }

/// Проверка геометрии на однородность
/// \param[in] p_type Тип подобластей геометрии
/// \return Результат проверки

    inline bool isHomogeneousGeometry(mesh::SubareaType p_type) const noexcept
    {
        return m_mesh->isHomogeneousGeometry(p_type);
    }

/// Получение данных сетки с доступом на изменение
/// \return Данные сетки

    inline const mesh::DataPrivate& data() noexcept
    {
        return m_mesh->m_data;
    }

/// Получение данных сетки
/// \return Данные сетки

    inline const mesh::Data& data() const noexcept
    {
        return m_mesh->data();
    }

/// Получение метрики сетки
/// \return Метрика сетки

    inline const mesh::Metric& metric() const noexcept
    {
        return m_mesh->metric();
    }

/// Оператор приведения к контейнру сетки
/// \return Контейнер сетки

    inline operator const Mesh&() const noexcept
    {
        return *m_mesh;
    }

/// Получение контейнра сетки
/// \return Контейнер сетки

    inline const Mesh& mesh() const noexcept
    {
        return *m_mesh;
    }

/// Возвращает данные о конечно-объёмной сетке
/// \return Данные сетки

    inline mesh::FVDataPrivate& FVData() noexcept
    {
        return m_mesh->m_fv_data;
    }

/// Возвращает данные о конечно-объёмной сетке
/// \return Данные сетки

    inline const mesh::FVData& FVData() const noexcept
    {
        return m_mesh->FVData();
    }

/// Возвращает конечно-объёмную метрику пользователю
/// \return Метрика сетки

    inline mesh::FVMetric& FVMetric() noexcept
    {
        return m_mesh->m_fv_metric;
    }

/// Возвращает конечно-объёмную метрику пользователю
/// \return Метрика сетки

    inline const mesh::FVMetric& FVMetric() const noexcept
    {
        return m_mesh->m_fv_metric;
    }

/// Выделение памяти под секцию узлов
/// \param[in] p_nodes_count Число узлов
/// \param[in] p_dimension Размерность пространства
/// \return Успешность операции

    bool makeNodes(Apos p_nodes_count, uint8_t p_dimension);

/// Выделение памяти под секцию элементов
/// \param[in] p_section Секция элементов
/// \param[in] p_regions_elements_count Число элементов регионов сетки
/// \param[in] p_regions_elements_count Число элементов регионов сетки
/// \param[in] p_regions_elements_nodes_count Число узлов в каждом КЭ региона сетки
/// \param[in] p_regions_elements_types Типы элеметов регионов
/// \param[in] p_subareas_count Число подобластей
/// \param[in] p_domains_count Число доменов
/// \return Успешность операции

    bool makeElements(ElementsSection            p_section,
                      const Array<Apos>         &p_regions_elements_count,
                      const Array<uint8_t>      &p_regions_elements_nodes_count,
                      const Array<mesh::FEType> &p_regions_elements_types,
                      uint16_t                   p_subareas_count,
                      Apos                       p_domains_count);

/// Выделение памяти под секцию вершин
/// \param[in] p_vertices_count Число вершин

    void makeVertices(uint16_t p_vertices_count);

/// Выделение памяти под буфер специальных данных
/// \param[in] p_size Размер буфера

    void makeSDBuffer(uint64_t p_size);

/// Удаление узлов не относящихся ни к одному конечному элементу

    void removeUnusedNodes();

/// Устанавливает представление конечно-объёмной сетки
/// \param[in] p_fvm_mesh Конечно-объёмная сетка

    inline void setFVMData(mesh::FVDataPrivate &&p_fv_data)
    {
        m_mesh->m_fv_data = std::move(p_fv_data);
    }

/// Отчистка контейнера

    inline void clear()
    {
        m_mesh->clear();
    }

private:

    Mesh *m_mesh;
}; // class MeshModifier

/// Модификатор сетки c полным доступом

class MeshEditor final
{
public:

/// Конструктор
/// \param[in] p_mesh Сетка

    MeshEditor(Mesh &p_mesh) noexcept:
        m_mesh(std::addressof(p_mesh)) {}

/// Проверка контейнера на пустоту
/// \return Результат проверки

    inline bool empty() const noexcept
    {
        return m_mesh->empty();
    }

/// Проверка геометрии на однородность
/// \param[in] p_type Тип подобластей геометрии
/// \return Результат проверки

    inline bool isHomogeneousGeometry(mesh::SubareaType p_type) const noexcept
    {
        return m_mesh->isHomogeneousGeometry(p_type);
    }

/// Оператор приведения к контейнру сетки
/// \return Контейнер сетки

    inline operator const Mesh&() const noexcept
    {
        return *m_mesh;
    }

/// Оператор приведения к контейнру сетки
/// \return Контейнер сетки

    inline operator Mesh&() noexcept
    {
        return *m_mesh;
    }

/// Получение контейнра сетки
/// \return Контейнер сетки

    inline Mesh& mesh() noexcept
    {
        return *m_mesh;
    }

/// Получение контейнра сетки
/// \return Контейнер сетки

    inline const Mesh& mesh() const noexcept
    {
        return *m_mesh;
    }

/// Получение данных сетки с полным доступом на изменение
/// \return Данные сетки

    inline mesh::DataPrivate& data() noexcept
    {
        return m_mesh->m_data;
    }

/// Получение данных сетки
/// \return Данные сетки

    inline const mesh::Data& data() const noexcept
    {
        return m_mesh->data();
    }

/// Возвращает данные о конечно-объёмной сетке с полным доступом на изменение
/// \return Данные сетки

    inline const mesh::FVDataPrivate& FVData() noexcept
    {
        return m_mesh->m_fv_data;
    }

/// Получение метрики сетки с доступом на изменение
/// \return Метрика сетки

    inline mesh::Metric& metric() noexcept
    {
        return m_mesh->m_metric;
    }

/// Получение метрики сетки
/// \return Метрика сетки

    inline const mesh::Metric& metric() const noexcept
    {
        return m_mesh->metric();
    }

/// Возвращает конечно-объёмную метрику с полным доступом на изменение
/// \return Метрика сетки

    inline mesh::FVMetric& FVMMetric() noexcept
    {
        return m_mesh->m_fv_metric;
    }

/// Возвращает конечно-объёмную метрику пользователю
/// \return Метрика сетки

    inline const mesh::FVMetric& FVMetric() const noexcept
    {
        return m_mesh->m_fv_metric;
    }

private:

    Mesh *m_mesh;
}; // class MeshEditor
}
#endif // MESH_H
