#ifndef MESH_STRUCTURES_H
#define MESH_STRUCTURES_H

#include <limits>
#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "Defs.h"
#include "Arrays.h"

/// Вспомогательные структуры сетки

namespace mesh
{
/// Геометрический тип конечного элемента

    enum FEType : uint8_t
    {
        /// Неизвестно
        unknown = UINT8_MAX,

        /// Отрезок
        edge = 0,

        /// Треугольник
        triangle,

        /// Четырехугольник
        quadrilateral,

        /// Тетраэдр
        tetrahedron,

        /// Четырехугольная пирамида
        quadrilateral_pyramid,

        /// Треугольная призма
        triangular_prism,

        /// Шестигранник
        hexahedron,

        last_fe_type = hexahedron
    };

/// Тип подобласти

    enum SubareaType : uint8_t
    {
        /// Объем
        volume,

        /// Поверхность
        surface,

        /// Кривая
        curve,

        /// Вершина
        vertex,

        last_subarea_type = vertex
    };

/// Сигнатура типа КЭ

    struct FETypeSignature
    {
        /// Число внутренних на каждом ребре конечного элемента
        uint8_t edge_inner_nodes_count{ 0 };

        /// Флаг наличия внутренних узлов
        bool is_inner_nodes{ false };

        /// Флаг криволинейности элемента
        bool is_curvilinear{ false };

        FETypeSignature() = default;

        inline FETypeSignature(uint8_t p_edge_inner_nodes_count,
                               bool    p_is_inner_nodes = false,
                               bool    p_is_curvilinear = false) noexcept:
            edge_inner_nodes_count(p_edge_inner_nodes_count),
            is_inner_nodes(p_is_inner_nodes),
            is_curvilinear(p_is_curvilinear)
        {}

        inline bool operator==(const FETypeSignature &p_signature) const noexcept
        {
            return edge_inner_nodes_count == p_signature.edge_inner_nodes_count &&
                   is_inner_nodes         == p_signature.is_inner_nodes &&
                   is_curvilinear         == p_signature.is_curvilinear;
        }

        inline bool operator!=(const FETypeSignature &p_signature) const noexcept
        {
            return !operator==(p_signature);
        }
    };

/// Получение кода сигнатуры

    constexpr uint32_t signatureCode(mesh::FEType                 p_fe_type,
                                     const mesh::FETypeSignature &p_signature = {}) noexcept
    {
        union {
            struct {
                FETypeSignature signature;
                FEType          fe_type;
            } data;

            uint32_t code;
        } data { { p_signature, p_fe_type } };

        return data.code;
    }

/// Вернуть тип подобласти конечного элемента

    constexpr SubareaType FETypeSubareaType(FEType p_type) noexcept
    {
        if (p_type >= FEType::tetrahedron)
            return SubareaType::volume;

        if (p_type >= FEType::triangle)
            return SubareaType::surface;

        return SubareaType::curve;
    }

/// Проверка типа КЭ на наличие ГЭ разных типов

    constexpr bool isMixedFEType(FEType p_type) noexcept
    {
        return p_type >= FEType::quadrilateral_pyramid &&
               p_type <= FEType::triangular_prism;
    }

/// Метрика типа конечного элемента

    struct FETypeMetric
    {
        /// Число вершин у КЭ
        uint8_t vertices_count{ 0 };

        /// Число граней в КЭ
        uint8_t faces_count{ 0 };

        /// Число ребер в КЭ
        uint8_t edges_count{ 0 };
    };

/// Метрика сетки

    struct Metric
    {
        /// Размер буффера специальных данных
        uint64_t sd_buffer_size{ 0 };

        /// Число узлов
        Apos nodes_count{ 0 };

        /// Число элементов
        Apos fes_count{ 0 };

        /// Число граничных элементов
        Apos bes_count{ 0 };

        /// Число дважды граничных элементов
        Apos twice_bes_count{ 0 };

        /// Число доменов сетки
        Apos domains_count{ 0 };

        /// Число граничных доменов
        Apos boundary_domains_count{ 0 };

        /// Число подобластей сетки
        uint16_t subareas_count{ 0 };

        /// Число граничных подобластей
        uint16_t boundary_subareas_count{ 0 };

        /// Число дважды граничных подобластей
        uint16_t twice_boundary_subareas_count{ 0 };

        /// Число вершин
        uint16_t vertices_count{ 0 };

        /// Число регионов сетки
        uint8_t regions_count{ 0 };

        /// Число граничных регионов
        uint8_t boundary_regions_count{ 0 };

        /// Число дважды граничных регионов
        uint8_t twice_boundary_regions_count{ 0 };

        /// Размерность сетки
        uint8_t dimension{ 0 };
    };

/// Маски конечного элемента

    struct FETypeMasks
    {
        /// Маска граней (только для поверхностных и объемных КЭ)
        Array<uint8_t> faces_mask;

        /// Маска ребер
        Array<uint8_t> edges_mask;
    };

/// Данные региона сетки

    struct Region
    {
        /// Сдвиг группы КЭ отнесенных к данному региону
        Exapos fes_nodes_offset{ 0 };

        /// Первый КЭ региона
        Apos first_fe{ 0 };

        /// Число узлов КЭ
        uint8_t nodes_count{ 0 };

        /// Тип КЭ
        FEType type{ edge };
    };

/// Данные сетки

    struct DataPrivate
    {
        /// Координаты узлов
        Real *nodes_coords{ nullptr };

        /// Идентификаторы подобластей сетки
        uint16_t *subareas_ids;

        /// Локальные идентификаторы подобластей доменов
        uint16_t *domains_subareas_ids{ nullptr };

        /// Идентификаторы регионов доменов
        uint8_t *domains_regions_ids{ nullptr };

        /// Данные о регионах сетки
        Region *regions{ nullptr };

        /// Пределы изменения номеров КЭ в доменах
        Apos *fes_num_limits{ nullptr };

        /// Узлы элементов
        Apos *fes_nodes{ nullptr };

        /// Идентификаторы граничных подобластей граничных доменов
        uint16_t *boundary_domains_subareas_ids{ nullptr };

        /// Идентификаторы регионов граничных доменов
        uint8_t *boundary_domains_regions_ids{ nullptr };

        /// Данные о граничных регионах сетки
        Region *boundary_regions{ nullptr };

        /// Пределы изменения номеров КЭ в граничных доменах
        Apos *bes_num_limits{ nullptr };

        /// Номер первого и второго (увеличенный на единицу или 0 если такого элемента нет)
        /// КЭ к которому принадлежит данный ГЭ
        Apos *bes_fes{ nullptr };

        /// Узлы граничных элементов
        Apos *bes_nodes{ nullptr };

        /// Идентификаторы дважды граничных подобластей
        uint16_t *twice_boundary_subareas_ids{ nullptr };

        /// Идентификаторы регионов дважды граничных подобластей
        uint8_t *twice_boundary_subareas_regions_ids{ nullptr };

        /// Данные о дважды граничных регионах сетки
        Region *twice_boundary_regions{ nullptr };

        /// Пределы изменения номеров дважды ГЭ в дважды граничных подобластях
        Apos *twice_bes_num_limits{ nullptr };

        /// Узлы дважды граничных элементов
        Apos *twice_bes_nodes{ nullptr };

        /// Идентификаторы вершин
        uint16_t *vertices_ids{ nullptr };

        /// Узлы вершин
        Apos *vertices_nodes{ nullptr };

        /// Буфер специальных данных
        uint8_t *sd_buffer{ nullptr };
    };

/// Константные данные сетки

    struct Data
    {
        /// Координаты узлов
        const Real *nodes_coords{ nullptr };

        /// Локальные идентификаторы подобластей сетки
        const uint16_t *subareas_ids;

        /// Локальные идентификаторы подобластей доменов
        const uint16_t *domains_subareas_ids{ nullptr };

        /// Идентификаторы регионов доменов
        const uint8_t *domains_regions_ids{ nullptr };

        /// Данные о регионах сетки
        const Region *regions{ nullptr };

        /// Пределы изменения номеров КЭ в доменах
        const Apos *fes_num_limits{ nullptr };

        /// Узлы элементов
        const Apos *fes_nodes{ nullptr };

        /// Идентификаторы граничных подобластей граничных доменов
        const uint16_t *boundary_domains_subareas_ids{ nullptr };

        /// Идентификаторы регионов граничных доменов
        const uint8_t *boundary_domains_regions_ids{ nullptr };

        /// Данные о граничных регионах сетки
        const Region *boundary_regions{ nullptr };

        /// Пределы изменения номеров КЭ в граничных доменах
        const Apos *bes_num_limits{ nullptr };

        /// Номер первого и второго (увеличенный на единицу или 0 если такого элемента нет)
        /// КЭ к которому принадлежит данный ГЭ
        const Apos *bes_fes{ nullptr };

        /// Узлы граничных элементов
        const Apos *bes_nodes{ nullptr };

        /// Идентификаторы дважды граничных подобластей
        const uint16_t *twice_boundary_subareas_ids{ nullptr };

        /// Идентификаторы регионов дважды граничных подобластей
        const uint8_t *twice_boundary_subareas_regions_ids{ nullptr };

        /// Данные о дважды граничных регионах сетки
        const Region *twice_boundary_regions{ nullptr };

        /// Пределы изменения номеров дважды ГЭ в дважды граничных подобластях
        const Apos *twice_bes_num_limits{ nullptr };

        /// Узлы дважды граничных элементов
        const Apos *twice_bes_nodes{ nullptr };

        /// Идентификаторы вершин
        const uint16_t *vertices_ids{ nullptr };

        /// Узлы вершин
        const Apos *vertices_nodes{ nullptr };

        /// Буфер специальных данных
        const uint8_t *sd_buffer{ nullptr };
    };

/// Домен сетки с заданным типом подобласти

    struct SubareaTypeDomain
    {
        /// Узлы элементов региона
        const Apos *elements_nodes;

        /// Номера конечных элементов домена которые разделяет данный ГЭ
        /// (в случае граничного домена, иначе nullptr)
        const Apos *bes_fes;

        /// Число КЭ домена
        Apos elements_count;

        /// Идентификатор подобласти
        uint16_t subarea_id;

        /// Число узлов КЭ
        uint8_t nodes_count{ 0 };

        /// Тип КЭ
        FEType type{ edge };

        /// Конустрктор
        inline SubareaTypeDomain(const Apos *p_element_nodes,
                                 const Apos *p_bes_fes,
                                 Apos        p_elements_count,
                                 uint16_t    p_subarea_id,
                                 uint8_t     p_nodes_count,
                                 FEType      p_type):
            elements_nodes(p_element_nodes),
            bes_fes(p_bes_fes),
            elements_count(p_elements_count),
            subarea_id(p_subarea_id),
            nodes_count(p_nodes_count),
            type(p_type)
        {}

        /// Оператор сравнения
        inline bool operator <(const SubareaTypeDomain &p_domain) const noexcept
        {
            return subarea_id < p_domain.subarea_id;
        }
    };

/// Данные для описания разрывных при переходе через границу подобластей полей

    struct DiscontinuityData
    {
        /// Дублируемые узлы
        std::vector<Apos> duplicated_nodes;

        /// Данные о КЭ
        Array<Apos> fes_nodes;
    };

/// Ограничения на данные сетки

    enum Restrictions : uint8_t
    {
        min_dimension = 2,
        max_dimension = 3,

        min_fe_nodes_count = 2,

        min_be_nodes_count = 2,

        min_twice_be_nodes_count = 2,

        min_nodes_count = 3,
    };

/// Внутренняя грань конечно-объёмного элемента

    struct ShellPseudoVolumeNode
    {
        /// Нормальная координата
        Real normal_coord{ 0 };

        /// Номер узла в базовой сетке
        Apos base_node{ 0 };

        /// Номер слоя
        uint16_t layer_id{ 0 };
    };

    struct FaceRegion
    {
        /// Первая поверхность региона
        Apos first_face{ 0 };

        /// Число узлов интегрирования Гаусса
        uint8_t gauss_points_count{ 0 };

        /// Весовые коэффициенты Гаусса для данного типа грани,
        /// коэффициенты нормированны на множитель у площади,
        /// для треугольника коэффициент нормирования == 1,
        /// для четырёхугольника == 4
        Real gauss_weights[9] = { 0.0 };

        /// Сдвиг в массиве координат точек Гаусса
        Apos gauss_points_offset { 0 };

        /// Количество нормалей на грани
        uint8_t normals_count{ 0 };

        /// Сдвиг в массиве координат нормалей
        Apos normal_offset { 0 };

        /// Сдвиг при переходе от одной нормали к другой для заданной поверхности
        enum FaceNormalOffset: uint8_t {
            zero = 0, // для треугольника
            dimension = 3 // для четырёхугольника
        };
        FaceNormalOffset face_normal_offset { zero };
    };

/// Представление конечно-объёмной сетки для редактирования данных

    struct FVDataPrivate
    {
        /// Регионы для внутренних граней элементов
        FaceRegion iface_regions[3];

        /// Регионы для внешних (граничных) граней элементов
        FaceRegion bface_regions[3];

        /// Площадь внутренней грани
        Real* ifaces_area { nullptr };

        /// Компоненты нормали для внутренней грани
        Real* ifaces_normal { nullptr };

        /// Координаты точек Гаусса на внутренней грани для интегрирования по поверхности
        Real* ifaces_gauss_coords { nullptr };

        /// Элементы, которым принадлежит внутренняя грань: для element1 нормаль внешняя,
        /// для element2 - внутренняя
        Apos* ifaces_elements { nullptr };

        /// Расстояние от точки Гаусса внутренней грани до твёрдой стенки
        Real* ifaces_distance { nullptr };

        /// Площадь граничной грани
        Real* bfaces_area { nullptr };

        /// Компоненты внешней нормали для граничной грани
        Real* bfaces_normal { nullptr };

        /// Координаты точек Гаусса на граничной грани для интегрирования по поверхности
        Real* bfaces_gauss_coords { nullptr };

        /// Элемент, которыму принадлежит граничная грань
        Apos* bfaces_elements { nullptr };

//        /// Второй внутренний элемент от граничной грани
//        Apos* bfaces_elements2 { nullptr };

        /// Расстояние от точки Гаусса граничной грани до твёрдой стенки
        Real* bfaces_distance { nullptr };

        /// Объём конечно-объёмного элемента
        Real* volumes { nullptr };

        /// Координаты центра конечно-объёмного элемента
        Real* volumes_center_coords { nullptr };

        /// Точки интегрирования Гаусса в объёме
        Real* volumes_gauss_points { nullptr };

        /// Расстояние от центра элемента до твёрдой стенки
        Real* elements_distance { nullptr };

        /// Ближайшие соседи, образующие шаблон схемы
        Apos* stencils { nullptr };

        /// Сдвиг в массиве stencils
        Apos* stencils_offset { nullptr };

        /// Пристеночный шаг (для турбулентности)
        Real* hard_wall_steps { nullptr };
    };

/// Константное представление конечно-объёмной сетки

    struct FVData
    {
        /// Регионы для внутренних граней элементов
        const FaceRegion iface_regions[3];

        /// Регионы для внешних (граничных) граней элементов
        const FaceRegion bface_regions[3];

        /// Площадь внутренней грани
        const Real* ifaces_area { nullptr };

        /// Компоненты нормали для внутренней грани
        const Real* ifaces_normal { nullptr };

        /// Координаты точек Гаусса на внутренней грани для интегрирования по поверхности
        const Real* ifaces_gauss_coords { nullptr };

        /// Элементы, которым принадлежит внутренняя грань: для element1 нормаль внешняя,
        /// для element2 - внутренняя
        const Apos* ifaces_elements { nullptr };

        /// Расстояние от точки Гаусса внутренней грани до твёрдой стенки
        const Real* ifaces_distance { nullptr };

        /// Площадь граничной грани
        const Real* bfaces_area { nullptr };

        /// Компоненты внешней нормали для граничной грани
        const Real* bfaces_normal { nullptr };

        /// Координаты точек Гаусса на граничной грани для интегрирования по поверхности
        const Real* bfaces_gauss_coords { nullptr };

        /// Элемент, которыму принадлежит граничная грань
        const Apos* bfaces_elements { nullptr };

//        /// Второй внутренний элемент от граничной грани
//        const Apos* bfaces_elements2 { nullptr };

        /// Расстояние от точки Гаусса граничной грани до твёрдой стенки
        const Real* bfaces_distance { nullptr };

        /// Объём конечно-объёмного элемента
        const Real* volumes { nullptr };

        /// Координаты центра конечно-объёмного элемента
        const Real* volumes_center_coords { nullptr };

        /// Точки интегрирования Гаусса в объёме
        const Real* volumes_gauss_points { nullptr };

        /// Расстояние от центра элемента до твёрдой стенки
        const Real* elements_distance { nullptr };

        /// Ближайшие соседи, образующие шаблон схемы
        const Apos* stencils { nullptr };

        /// Сдвиг в массиве stencils
        const Apos* stencils_offset { nullptr };

        /// Пристеночный шаг (для турбулентности)
        const Real* hard_wall_steps { nullptr };
    };

    struct FVMetric
    {
        /// Количество регионов для внутренних граней
        uint8_t iface_regions_count = 0;

        /// Количество регионов для внешних (граничных) граней
        uint8_t bface_regions_count = 0;
    };
}

#endif
