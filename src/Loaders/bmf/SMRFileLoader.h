#ifndef SMR_FILE_LOADER_H
#define SMR_FILE_LOADER_H

#include "AbstractResultsFileLoader.h"

/// Загрузчик результата расчета из файла формата SMR
namespace bmf {
    class SMRFileLoader final : public AbstractResultsFileLoader
    {
    public:

        /// Метрика данных файла
        struct Metric
        {
            size_t first_data_block_offset{ 0 };

            size_t block_size{ 0 };

            uint16_t tasks_count{ 0 };

            uint8_t main_fields_count{ 0 };

            uint8_t side_fields_count{ 0 };

            uint8_t parameters_count{ 0 };

            Apos nodes_count{ 0 };

            Apos duplicated_nodes_count{ 0 };

            Exapos fe_fields_data_size{ 0 };
        };

        /// Получение числа доступных задач

        uint16_t tasksCount() const override;

        /// Получение числа задач в группе

        uint16_t tasksGroupSize() const override;

        /// Получение числа параметров для каждой задачи

        uint16_t parametersCount() const override;

        /// Определение параметров задач

        std::vector<Real> tasksParameters(uint8_t p_parameter_id = 0) const override;

        /// Определение идентификаторов полей

        std::vector<std::string> fieldsIDs() const override;

        /// Загрузка данных и вставка в контейнер без выделения памяти для данной задачи
        /// \param[out] o_nodal_fields Модификатор контейнера полей в узлах
        /// \param[out] o_parameters Параметры задачи
        /// \param[in] p_task_num Номер задачи
        /// \param[out] o_fe_fields Поля в элементах
        /// \return Успешность операции

        bool insert(NodalFieldsModifier                    o_nodal_fields,
                    const Parameters                      &o_parameters = {},
                    uint16_t                               p_task_num = 0,
                    const std::vector<ElementalFieldsRef> &o_fe_fields = {}) const override;

        /// Загрузка данных и вставка в контейнер с выделением памяти
        /// \param[in] p_mesh Cетка
        /// \param[out] o_nodal_fields Редактор контейнера полей в узлах
        /// \param[in] p_task_num Номер задачи
        /// \param[out] щ_parameters Параметры задачи
        /// \return Успешность операции

        bool replace(const Mesh        &p_mesh,
                     NodalFieldsEditor  o_nodal_fields,
                     uint16_t           p_task_num = 0,
                     Array<Real>       *o_parameters = nullptr) const override;

        /// Отчистка данных

        void clear() override;

    private:

        mutable Metric m_metric;
    };
}
#endif // SMR_FILE_LOADER_H
