#ifndef BMF_STRUCTURES_H
#define BMF_STRUCTURES_H

namespace mesh
{
/// Идентификаторы секций формата BMF

    enum BMFSectionsIDs : uint8_t
    {
        /// Конечные элементы
        finite_elements = 1,

        /// Граничные элементы
        boundary_elements = 2,

        /// Дважды граничные элементы
        twice_boundary_elements = 3,

        /// Буфер специальных данных
        special_data_buffer = 4,

        /// Вершины
        vertices = 5,

        /// Конечно-объёмное представление сетки
        fvm_mesh = 6
    };
}

#endif // BMF_STRUCTURES_H
