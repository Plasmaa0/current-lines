#ifndef RESULTS_FILE_LOADER_H
#define RESULTS_FILE_LOADER_H

#include <vector>
#include <string>

#include "Mesh.h"
#include "NodalFields.h"
#include "ElementalFields.h"
//#include <FileTools>

/// Интерфейс средства загрузки результатов расчета из файла

namespace bmf {
    class AbstractResultsFileLoader
    {
        DISABLE_COPY(AbstractResultsFileLoader);

    public:

        /// Контейнер параметров
        class Parameters final
        {
        public:

            inline Parameters() noexcept:
                m_value(nullptr),
                m_is_array(false)
            {}


            inline Parameters(FixedArray<Real> &p_array) noexcept:
                m_array(&p_array),
                m_is_array(true)
            {}


            inline Parameters(Real &p_value) noexcept:
                m_value(&p_value),
                m_is_array(false)
            {}

            size_t size() const noexcept;

            Real *data() const noexcept;

        private:

            union
            {
                FixedArray<Real> *m_array;
                Real             *m_value;
            };

            bool m_is_array;
        };

        /// Ссылка на контейнер полей
        using ElementalFieldsRef = std::reference_wrapper<ElementalFields>;

        /// Проверка данных для файла и контейнеров

        bool checkBaseData(
            const NodalFields                     &p_nodal_fields,
            const std::vector<ElementalFieldsRef> &p_fe_fields,
            const Parameters                      &p_parameters,
            Apos                                   p_nodes_count,
            Apos                                   p_duplicated_nodes_count,
            uint8_t                                p_main_fields_count,
            uint8_t                                p_side_fields_count,
            uint8_t                                p_parameters_count,
            Exapos                                 p_fe_fields_data_size) const noexcept;

        /// Проверка соотвествия параметров полей для файла и контейнера

        bool checkMesh(const Mesh &p_mesh, Apos p_nodes_count) const noexcept;

    public:

        AbstractResultsFileLoader() = default;

        /// Деструктор

        virtual ~AbstractResultsFileLoader() {}

        /// Проверка на заданность имен файлов

        inline bool empty() const noexcept
        {
            return m_file_names.empty();
        }

        /// Получение имени файла

        inline const std::string& fileName() const noexcept
        {
            return m_file_names[m_file_id];
        }

        /// Получение числа файлов

        inline uint8_t filesCount() const noexcept
        {
            return static_cast<uint8_t>(m_file_names.size());
        }

        /// Задание имени файла для загрузки

        inline void setFileName(const std::string &p_file_name)
        {
            clear();
            m_file_names = { p_file_name };
            m_file_id = 0;
        }

        /// Задание имен файлов для загрузки

        inline void setFileNames(const std::vector<std::string> &p_file_names)
        {
            clear();
            m_file_names = p_file_names;

            if (m_file_id >= p_file_names.size())
                m_file_id = 0;
        }

        /// Задание идентификатора имени файла для загрузки

        inline void setFileID(uint8_t p_file_id)
        {
            if (p_file_id < m_file_names.size()) {
                clear();
                m_file_id = p_file_id;
            }
        }

        /// Получение числа доступных задач

        virtual uint16_t tasksCount() const = 0;

        /// Получение числа задач в группе

        virtual uint16_t tasksGroupSize() const = 0;

        /// Получение числа параметров для каждой задачи

        virtual uint16_t parametersCount() const = 0;

        /// Определение параметров задач

        virtual std::vector<Real> tasksParameters(uint8_t p_parameter_id = 0) const = 0;

        /// Определение идентификаторов полей

        virtual std::vector<std::string> fieldsIDs() const = 0;

        /// Загрузка данных и вставка в контейнер без выделения памяти для данной задачи
        /// \param[out] o_nodal_fields Модификатор контейнера полей в узлах
        /// \param[out] o_parameters Параметры задачи
        /// \param[in] p_task_num Номер задачи
        /// \param[out] o_fe_fields Поля в элементах
        /// \return Успешность операции

        virtual bool insert(NodalFieldsModifier                    o_nodal_fields,
                            const Parameters                      &o_parameters = {},
                            uint16_t                               p_task_num = 0,
                            const std::vector<ElementalFieldsRef> &o_fe_fields = {}) const = 0;

        /// Загрузка данных и вставка в контейнер с выделением памяти
        /// \param[in] p_mesh Cетка
        /// \param[out] o_nodal_fields Редактор контейнера полей в узлах
        /// \param[in] p_task_num Номер задачи
        /// \param[out] щ_parameters Параметры задачи
        /// \return Успешность операции

        virtual bool replace(const Mesh        &p_mesh,
                             NodalFieldsEditor  o_nodal_fields,
                             uint16_t           p_task_num = 0,
                             Array<Real>       *o_parameters = nullptr) const = 0;


        /// Отчистка данных

        virtual void clear() = 0;

    private:

        std::vector<std::string> m_file_names;
        uint8_t                  m_file_id{ 0 };
    };
}
#endif // RESULTS_FILE_LOADER_H
