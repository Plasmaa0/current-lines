#ifndef BMF_FILE_LOADER_H
#define BMF_FILE_LOADER_H

#include "AbstractFEMMeshFileLoader.h"

/// Загрузчик сетки из файла формата BMF
namespace bmf {
    class BMFFileLoader final : public AbstractMeshFileLoader
    {
    public:

        /// Загрузка данных из файла
        /// \param[in] p_mesh_modifier Модификатор сетки
        /// \return Успешность операции

        bool load(MeshModifier p_mesh_modifier) const override;
    };
}
#endif // BMF_FILE_LOADER_H
