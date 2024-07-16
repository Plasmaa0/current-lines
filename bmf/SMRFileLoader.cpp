#include <sys/stat.h>
#include <stdio.h>

#include "SMRFileLoader.h"

extern unsigned long long fileSize(const std::string &p_path);


inline int fseek_(FILE *p_stream, unsigned long long p_offset, int p_origin)
{
#   if defined(OS_LINUX)

    return fseeko(p_stream, p_offset, p_origin);

#   elif defined(OS_WIN)

    return _fseeki64(p_stream, p_offset, p_origin);

#   else
#        error Platform is not supported
#   endif
}

using ElementalFieldsRef = AbstractResultsFileLoader::ElementalFieldsRef;

bool loadSMRFileHead(const std::string        &p_file_name,
                     FILE                     *p_file,
                     SMRFileLoader::Metric    &o_metric,
                     std::vector<std::string> *o_fields_ids = nullptr)
{
    if (p_file == nullptr) {
        ERROR_MESSAGE("Файл с именем " << p_file_name << " не существует.");
        o_metric = SMRFileLoader::Metric();
        return false;
    }

    // Число прочитанных элементов
    size_t reading_elements;

    // Загрузка шапки
    enum { elements_count = 6 };

    reading_elements = fread(&o_metric.nodes_count, sizeof(Apos), 1, p_file);
    reading_elements += fread(&o_metric.duplicated_nodes_count, sizeof(Apos), 1, p_file);

    reading_elements += fread(&o_metric.main_fields_count, sizeof(uint8_t), 1, p_file);
    reading_elements += fread(&o_metric.side_fields_count, sizeof(uint8_t), 1, p_file);
    reading_elements += fread(&o_metric.parameters_count, sizeof(uint8_t), 1, p_file);

    reading_elements += fread(&o_metric.fe_fields_data_size, sizeof(Exapos), 1, p_file);

    // Проверка корректности данных
    if (reading_elements != elements_count) {
        ERROR_MESSAGE("Некорректный блок метрики данных");
        return false;
    }

    // Чтение строки идентификаторов полей
    uint16_t fields_ids_string_size;

    reading_elements = fread(&fields_ids_string_size, sizeof(uint16_t), 1, p_file);

    if (reading_elements == 1) {
        if (o_fields_ids != nullptr) {
            o_fields_ids->clear();

            if (fields_ids_string_size > 0) {
                char* parameter_ids_string = new char[fields_ids_string_size + 1];

                reading_elements = fread(parameter_ids_string, 
                                         sizeof(char),
                                         fields_ids_string_size,
                                         p_file);

                if (fields_ids_string_size != reading_elements) {
                    delete[] parameter_ids_string;

                    ERROR_MESSAGE("Некорректный блок идентификаторов полей");
                    return false;
                }

                --fields_ids_string_size;

                std::string parameter_id;

                for (size_t pos = 0; pos < fields_ids_string_size;) {
                    parameter_id = std::string(parameter_ids_string + pos);
                    pos += parameter_id.size() + 1;

                    o_fields_ids->emplace_back(std::move(parameter_id));
                }

                delete[] parameter_ids_string;
            }

            uint16_t summary_fields_count =
                o_metric.main_fields_count + o_metric.side_fields_count;

            if (fields_ids_string_size == 0 ||
                o_fields_ids->size() != summary_fields_count) {
                for (uint16_t p = 0; p < summary_fields_count; ++p)
                    o_fields_ids->emplace_back("field" + std::to_string(p + 1));
            }
        } else {
            int rv = fseek_(p_file, sizeof(char) * fields_ids_string_size, SEEK_CUR);

            if (rv != 0) {
                ERROR_MESSAGE("Некорректный блок идентификаторов полей");
                return false;
            }
        }
    } else {
        ERROR_MESSAGE("Некорректный блок идентификаторов полей");
        return false;
    }

    o_metric.first_data_block_offset = static_cast<size_t>(ftell(p_file));
    auto file_data_blocks_size = fileSize(p_file_name) - o_metric.first_data_block_offset;

    o_metric.block_size = sizeof(Real) * (
        o_metric.parameters_count +
        o_metric.main_fields_count * o_metric.nodes_count +
        o_metric.side_fields_count * (o_metric.nodes_count + o_metric.duplicated_nodes_count) +
        o_metric.fe_fields_data_size);

    if (file_data_blocks_size % o_metric.block_size != 0 ||
        file_data_blocks_size / o_metric.block_size > UINT16_MAX) {
        ERROR_MESSAGE("Некорректный блок данных");
        return false;
    }

    o_metric.tasks_count = static_cast<uint16_t>(file_data_blocks_size / o_metric.block_size);

    return true;
}


bool loadSMRFileData(FILE                                  *p_file,
                     bool                                   p_is_first_reading,
                     const SMRFileLoader::Metric           &p_metric,
                     uint16_t                               p_task_num,
                     Real                                  *o_parameters,
                     Real                                  *o_main_data,
                     Real                                  *o_side_data,
                     const std::vector<ElementalFieldsRef> &p_fe_fields = {})
{
    if (p_task_num >= p_metric.tasks_count)
        p_task_num = p_metric.tasks_count - 1;

    PROGRESS_STATUS(1, 100);
    
    size_t offset = p_is_first_reading ? 0 : p_metric.first_data_block_offset;
        
    if (fseek_(p_file, p_task_num * p_metric.block_size + offset, SEEK_CUR)) {
        ERROR_MESSAGE("Некорректный SMR файл");
        return false;
    }

    size_t reading_elements{ 0 };

    if (p_metric.parameters_count) {
        if(o_parameters) {
            // Чтение секции параметров
            reading_elements = fread(o_parameters,
                                     sizeof(Real),
                                     p_metric.parameters_count,
                                     p_file);
        } else if(fseek_(p_file, p_metric.parameters_count * sizeof(Real), SEEK_CUR))
            reading_elements = 0;
        else
            reading_elements = p_metric.parameters_count;

        if (reading_elements != p_metric.parameters_count) {
            ERROR_MESSAGE("Некорректный блок параметров");
            return false;
        }

        PROGRESS_STATUS(p_metric.parameters_count * sizeof(Real), p_metric.block_size);
    }

    if (p_metric.main_fields_count) {
        // Чтение секции основных данных в узлах
        reading_elements = fread(o_main_data,
                                 sizeof(Real) * p_metric.main_fields_count, 
                                 p_metric.nodes_count, 
                                 p_file);

        if (reading_elements != p_metric.nodes_count) {
            ERROR_MESSAGE("Некорректный основной блок полей");
            return false;
        }
        
        PROGRESS_STATUS((p_metric.parameters_count +
                         p_metric.main_fields_count * p_metric.nodes_count) * sizeof(Real), 
                        p_metric.block_size);
    }

    if (p_metric.side_fields_count) {
        const Apos total_nodes_count = 
            p_metric.nodes_count + p_metric.duplicated_nodes_count;

            // Чтение секции дополнительных данных в узлах
            reading_elements = fread(o_side_data,
                                     sizeof(Real) * p_metric.side_fields_count, 
                                     total_nodes_count, 
                                     p_file);

            if (reading_elements != total_nodes_count) {
                ERROR_MESSAGE("Некорректный дополнительный блок полей");
                return false;
            }
            
            PROGRESS_STATUS((p_metric.parameters_count +
                             p_metric.main_fields_count * p_metric.nodes_count + 
                             p_metric.side_fields_count * total_nodes_count) * sizeof(Real),
                            p_metric.block_size);
    }

    size_t fe_fields_data_size;

    // Чтение секции полей в элементах
    for (ElementalFields &fields : p_fe_fields) {
        fe_fields_data_size = fields.dataSize();
        reading_elements = fread(fields.data(), sizeof(Real), fe_fields_data_size, p_file);

        if (reading_elements != fe_fields_data_size) {
            ERROR_MESSAGE("Некорректный блок данных о полях в элементах");
            return false;
        }
    }

    PROGRESS_STATUS(100, 100);

    return true;
}

// Реализация методов SMRFileLoader

uint16_t SMRFileLoader::tasksCount() const
{
    if (m_metric.tasks_count == 0) {
        FILE* file = fopen(fileName().c_str(), "rb");

        loadSMRFileHead(fileName(), file, m_metric);

        if (file)
            fclose(file);
    }

    return m_metric.tasks_count;
}


uint16_t SMRFileLoader::tasksGroupSize() const
{
    if (empty()) {
        ERROR_MESSAGE("Отсуствует имя файла");
        return 0.0;
    }

    const bool is_first_reading = m_metric.tasks_count == 0;

    if (!is_first_reading && m_metric.parameters_count == 0)
        return 1;

    FILE* file = fopen(fileName().c_str(), "rb");

    if (is_first_reading && !loadSMRFileHead(fileName(), file, m_metric)) {
        fclose(file);
        return 1;
    }

    const uint8_t parameter_id =
        m_metric.parameters_count > 1 ? (m_metric.parameters_count - 2) : 0;

    const auto local_offset = parameter_id * sizeof(Real) +
                              (is_first_reading ? size_t(0) : m_metric.first_data_block_offset);
    const auto block_offset = m_metric.block_size - sizeof(Real);

    int rv;

    Real last_parameter_value{ 0. };
    Real current_parameter_value;
    uint16_t res{ 0 };

    for (uint16_t t = 0; t < m_metric.tasks_count; ++t) {
        rv = fseek_(file, t == 0 ? local_offset : block_offset, SEEK_CUR);

        if (rv || fread(&current_parameter_value, sizeof(Real), 1, file) != 1) {
            fclose(file);
            ERROR_MESSAGE("Некорректный SMR файл");
            return 0.0;
        }

        if(t == 0 || std::fpclassify(current_parameter_value - last_parameter_value) == FP_ZERO) {
           ++res;
           last_parameter_value = current_parameter_value;
        } else
            break;
    }

    fclose(file);
    return res;
}


uint16_t SMRFileLoader::parametersCount() const
{
    if (m_metric.tasks_count == 0) {
        FILE* file = fopen(fileName().c_str(), "rb");

        loadSMRFileHead(fileName(), file, m_metric);

        if (file)
            fclose(file);
    }

    return m_metric.parameters_count;
}


std::vector<Real> SMRFileLoader::tasksParameters(uint8_t p_parameter_id) const
{
    if (empty()) {
        ERROR_MESSAGE("Отсуствует имя файла");
        return {};
    }

    const bool is_first_reading = m_metric.tasks_count == 0;

    if (!is_first_reading && m_metric.parameters_count == 0)
        return {};

    FILE* file = fopen(fileName().c_str(), "rb");

    if (is_first_reading && !loadSMRFileHead(fileName(), file, m_metric)) {
        fclose(file);
        return {};
    }

    if(p_parameter_id >= m_metric.parameters_count)
        p_parameter_id = m_metric.parameters_count - 1;

    const auto local_offset = p_parameter_id * sizeof(Real) +
                              (is_first_reading ? size_t(0) : m_metric.first_data_block_offset);
    const auto block_offset = m_metric.block_size - sizeof(Real);

    int rv;

    std::vector<Real> res;
    res.reserve(m_metric.tasks_count);

    Real parameter_value;

    for (uint16_t t = 0; t < m_metric.tasks_count; ++t) {
        rv = fseek_(file, t == 0 ? local_offset : block_offset, SEEK_CUR);

        if (rv || fread(&parameter_value, sizeof(Real), 1, file) != 1) {
            fclose(file);
            ERROR_MESSAGE("Некорректный SMR файл");
            return {};
        }

        res.emplace_back(parameter_value);
    }

    fclose(file);
    return res;
}


std::vector<std::string> SMRFileLoader::fieldsIDs() const
{
    std::vector<std::string> res;

    FILE* file = fopen(fileName().c_str(), "rb");

    loadSMRFileHead(fileName(), file, m_metric, &res);

    if (file)
        fclose(file);
    
    return res;
}


bool SMRFileLoader::insert(NodalFieldsModifier                    o_nodal_fields,
                           const Parameters                      &o_parameters,
                           uint16_t                               p_task_num,
                           const std::vector<ElementalFieldsRef> &o_fe_fields) const
{
    if (empty()) {
        ERROR_MESSAGE("Отсуствует имя файла");
        return false;
    }
    
    FILE* file = fopen(fileName().c_str(), "rb");

    const bool is_first_reading = m_metric.tasks_count == 0;

    bool res;
    
    if (is_first_reading) {
        res = loadSMRFileHead(fileName(), file, m_metric);

        if (!res) {
            if (file)
                fclose(file);

            return false;
        }
    }

    if (!checkBaseData(o_nodal_fields.fields(),
                       o_fe_fields,
                       o_parameters,
                       m_metric.nodes_count,
                       m_metric.duplicated_nodes_count,
                       m_metric.main_fields_count,
                       m_metric.side_fields_count,
                       m_metric.parameters_count,
                       m_metric.fe_fields_data_size)) {
        fclose(file);
        return false;
    }

    res = loadSMRFileData(file,
                          is_first_reading,
                          m_metric,
                          p_task_num,
                          o_parameters.data(),
                          o_nodal_fields.mainData(),
                          o_nodal_fields.sideData(),
                          o_fe_fields);

    fclose(file);

    return res;
}


bool SMRFileLoader::replace(const Mesh        &p_mesh,
                            NodalFieldsEditor  o_nodal_fields,
                            uint16_t           p_task_num,
                            Array<Real>       *o_parameters) const
{
    UNUSED(p_mesh);
    if (empty()) {
        ERROR_MESSAGE("Отсуствует имя файла");
        return false;
    }
    
    FILE* file = fopen(fileName().c_str(), "rb");

    const bool is_first_reading = m_metric.tasks_count == 0;

    bool res{ true };

    if (is_first_reading)
        res = loadSMRFileHead(fileName(), file, m_metric);

//    if (res)
//        res = checkMesh(p_mesh, m_metric.nodes_count);

    if (!res) {
        if (file)
            fclose(file);

        return false;
    }

    o_nodal_fields.makeData(m_metric.main_fields_count,
                            m_metric.side_fields_count,
                            m_metric.nodes_count,
                            m_metric.duplicated_nodes_count);

    Real *parameters{ nullptr };

    if(o_parameters) {
        auto parameters_offset = o_parameters->size();
        o_parameters->resize(parameters_offset + m_metric.parameters_count);
        parameters = o_parameters->data() + parameters_offset;
    }

    res = loadSMRFileData(file,
                          is_first_reading,
                          m_metric,
                          p_task_num,
                          parameters,
                          o_nodal_fields.mainData(),
                          o_nodal_fields.sideData());


    fclose(file);

    return res;
}


void SMRFileLoader::clear()
{
    m_metric = Metric();
}
