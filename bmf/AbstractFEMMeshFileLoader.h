#ifndef MESH_FILE_LOADER_H
#define MESH_FILE_LOADER_H

#include <string>

#include "Mesh.h"


/// Интерфейс загрузчика сетки из файла

class AbstractMeshFileLoader
{
    DISABLE_COPY(AbstractMeshFileLoader);

public:

    AbstractMeshFileLoader() = default;

    inline bool setFileName(const std::string &p_file_name)
    {
        m_file_name = p_file_name;

        return true;
    }

/// Получение имени файла
/// \return Имя файла сетки

    inline const std::string& fileName() const noexcept
    {
        return m_file_name;
    }

/// Загрузка данных из файла
/// \param[in] p_mesh_modifier Модификатор сетки
/// \return Успешность операции

    virtual bool load(MeshModifier p_mesh_modifier) const = 0;

private:

    std::string m_file_name;
};
#endif // MESH_FILE_LOADER_H
