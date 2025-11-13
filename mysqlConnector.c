#include "mysqlConnector.h"

//conn = 0;
MYSQL * conn_ = NULL;
bool ConnectMYSQL(const char* server,
                  const char* user,
                  const char* password,
                  const char* db)
{
    // Инициилизация обьекта подключения
    if(conn_ != NULL) {
        printf("MYSQL allready inited\n");
    } else {
        conn_ = mysql_init(NULL);
        if (conn_ == NULL) {
            fprintf(stderr, "mysql_init() failed\n");
            return false;
        }
    }
    // Подключение (real)
    if (mysql_real_connect(conn_, server, user, password, db, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn_));
        mysql_close(conn_);
        return false;
    }

    printf("Подключено к MySQL!\n");
    return true;
}

bool QuerryMYSQL(const char* querryStr, MYSQL_RES * res)
{
    MYSQL_ROW row;
    // Проверка подключения
    if(!mysql_ping(conn_)) {
        fprintf(stderr, "mysql_ping() failed: %s\n", mysql_error(conn_));
        mysql_close(conn_);
        return false;
    }
    // Запрос
    if(!mysql_query(conn_,querryStr)) {
        fprintf(stderr, "mysql_querry() failed: %s\n", mysql_error(conn_));
        return false;
    } else {
        // Cохранение результата запроса (удачно\неудачно)
        res = mysql_store_result(conn_);
        if (res == NULL) {
            fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn_));
            return false;
        } else {
            // Вывод данных
            int num_fields = mysql_num_fields(res);
            printf("Найдено строк: %lu\n", (unsigned long)mysql_num_rows(res));

            while ((row = mysql_fetch_row(res))) {
                for (int i = 0; i < num_fields; i++) {
                    printf("%s ", row[i] ? row[i] : "NULL");
                }
                printf("\n");
            }
            // Освобождение результата
            mysql_free_result(res);
        }
    }
    return true;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------------
 *  Helper: bind parameter (IN)
 * ---------------------------------------------------------------------- */
static inline void bind_param(MYSQL_BIND *b, enum enum_field_types type,
                              const void *buf, unsigned long len,
                              bool is_null)
{
    memset(b, 0, sizeof(*b));
    b->buffer_type   = type;
    b->buffer        = (char *)buf;
    b->buffer_length = len;
    b->length        = 0;
    b->is_null_value = is_null;
    b->is_null       = &b->is_null_value;
}

/* ----------------------------------------------------------------------
 *  Helper: execute CALL with parameters
 * ---------------------------------------------------------------------- */
static int exec_call(MYSQL *conn, const char *sql,
                     MYSQL_BIND *params, unsigned count,
                     MYSQL_STMT **out_stmt)
{
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) return -1;

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        mysql_stmt_close(stmt);
        return -1;
    }

    if (count && mysql_stmt_bind_param(stmt, params)) {
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }

    if (out_stmt) *out_stmt = stmt;
    else mysql_stmt_close(stmt);
    return 0;
}

/* ======================================================================
   INSERT PROCEDURES
   ====================================================================== */

int InsertAnalogSensor(MYSQL *conn,
                       const char *reg_num,
                       const char *time_stamp,
                       uint8_t sensor_number,
                       uint32_t sensor_value)
{
    const char *sql = "CALL InsertAnalogSensor(?,?,?,?)";
    MYSQL_BIND b[4] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[2], MYSQL_TYPE_TINY, &sensor_number, sizeof(sensor_number), 0);
    bind_param(&b[3], MYSQL_TYPE_LONG, &sensor_value, sizeof(sensor_value), 0);

    return exec_call(conn, sql, b, 4, NULL);
}

int InsertCounter(MYSQL *conn,
                  const char *reg_num,
                  const char *time_stamp,
                  uint8_t counter_number,
                  uint32_t counter_value)
{
    const char *sql = "CALL InsertCounter(?,?,?,?)";
    MYSQL_BIND b[4] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[2], MYSQL_TYPE_TINY, &counter_number, sizeof(counter_number), 0);
    bind_param(&b[3], MYSQL_TYPE_LONG, &counter_value, sizeof(counter_value), 0);

    return exec_call(conn, sql, b, 4, NULL);
}

int InsertDigitalSensor(MYSQL *conn,
                        const char *reg_num,
                        const char *time_stamp,
                        uint8_t sensor_number,
                        uint8_t sensor_value)
{
    const char *sql = "CALL InsertDigitalSensor(?,?,?,?)";
    MYSQL_BIND b[4] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[2], MYSQL_TYPE_TINY, &sensor_number, sizeof(sensor_number), 0);
    bind_param(&b[3], MYSQL_TYPE_TINY, &sensor_value, sizeof(sensor_value), 0);

    return exec_call(conn, sql, b, 4, NULL);
}

int InsertMachine(MYSQL *conn,
                  const char *reg_num,
                  int32_t software_ver,
                  int32_t landing_gear_num,
                  int32_t status,
                  uint8_t work_mode,
                  const char *config_name)
{
    const char *sql = "CALL InsertMachine(?,?,?,?,?,?)";
    MYSQL_BIND b[6] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_LONG, &software_ver, sizeof(software_ver), 0);
    bind_param(&b[2], MYSQL_TYPE_LONG, &landing_gear_num, sizeof(landing_gear_num), 0);
    bind_param(&b[3], MYSQL_TYPE_LONG, &status, sizeof(status), 0);
    bind_param(&b[4], MYSQL_TYPE_TINY, &work_mode, sizeof(work_mode), 0);
    bind_param(&b[5], MYSQL_TYPE_STRING, config_name, strlen(config_name), 0);

    return exec_call(conn, sql, b, 6, NULL);
}

int InsertPosition(MYSQL *conn,
                   const char *reg_num,
                   const char *time_stamp,
                   double latitude,
                   double longitude,
                   int32_t *altitude,
                   int16_t *speed,
                   uint16_t *direction,
                   uint32_t *odometer,
                   uint8_t *digital_inputs,
                   uint8_t *source)
{
    const char *sql = "CALL InsertPosition(?,?,?,?,?,?,?,?,?,?,?)";
    MYSQL_BIND b[11] = {0};
    bool nulls[11] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[2], MYSQL_TYPE_DOUBLE, &latitude, sizeof(latitude), 0);
    bind_param(&b[3], MYSQL_TYPE_DOUBLE, &longitude, sizeof(longitude), 0);

    nulls[4] = (altitude == NULL);
    bind_param(&b[4], MYSQL_TYPE_LONG, altitude, sizeof(int32_t), nulls[4]);

    nulls[5] = (speed == NULL);
    bind_param(&b[5], MYSQL_TYPE_SHORT, speed, sizeof(uint16_t), nulls[5]);

    nulls[6] = (direction == NULL);
    bind_param(&b[6], MYSQL_TYPE_SHORT, direction, sizeof(uint16_t), nulls[6]);

    nulls[7] = (odometer == NULL);
    bind_param(&b[7], MYSQL_TYPE_LONG, odometer, sizeof(uint32_t), nulls[7]);

    nulls[8] = (digital_inputs == NULL);
    bind_param(&b[8], MYSQL_TYPE_TINY, digital_inputs, sizeof(uint8_t), nulls[8]);

    nulls[9] = (source == NULL);
    bind_param(&b[9], MYSQL_TYPE_TINY, source, sizeof(uint8_t), nulls[9]);

    return exec_call(conn, sql, b, 11, NULL);
}

int InsertWorkSession(MYSQL *conn,
                      const char *reg_num,
                      uint8_t work_mode,
                      const char *start_time_stamp,
                      const char *end_time_stamp)
{
    const char *sql = "CALL InsertWorkSession(?,?,?,?)";
    MYSQL_BIND b[4] = {0};
    bool nulls[4] = {0};

    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[1], MYSQL_TYPE_TINY, &work_mode, sizeof(work_mode), 0);

    nulls[2] = (start_time_stamp == NULL);
    bind_param(&b[2], MYSQL_TYPE_STRING, start_time_stamp,
               start_time_stamp ? strlen(start_time_stamp) : 0, nulls[2]);

    nulls[3] = (end_time_stamp == NULL);
    bind_param(&b[3], MYSQL_TYPE_STRING, end_time_stamp,
               end_time_stamp ? strlen(end_time_stamp) : 0, nulls[3]);

    return exec_call(conn, sql, b, 4, NULL);
}

/* ======================================================================
   DELETE PROCEDURES
   ====================================================================== */

int DeleteAnalogSensor(MYSQL *conn, uint64_t id)
{
    const char *sql = "CALL DeleteAnalogSensor(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

int DeleteCounter(MYSQL *conn, uint64_t id)
{
    const char *sql = "CALL DeleteCounter(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

int DeleteDigitalSensor(MYSQL *conn, uint64_t id)
{
    const char *sql = "CALL DeleteDigitalSensor(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

int DeleteMachine(MYSQL *conn, const char *reg_num)
{
    const char *sql = "CALL DeleteMachine(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

int DeletePosition(MYSQL *conn, uint64_t id)
{
    const char *sql = "CALL DeletePosition(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

int DeleteWorkSession(MYSQL *conn, uint32_t id)
{
    const char *sql = "CALL DeleteWorkSession(?)";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONG, &id, sizeof(id), 0);
    return exec_call(conn, sql, b, 1, NULL);
}

/* ======================================================================
   SELECT PROCEDURES
   ====================================================================== */

int SelectAnalogSensor(MYSQL *conn,
                       const char *reg_num,
                       const char *time_stamp,
                       uint64_t *out_id,
                       char out_reg_num[9],
                       char out_time_stamp[20],
                       uint8_t *out_sensor_number,
                       uint32_t *out_sensor_value)
{
    const char *sql = "CALL SelectAnalogSensor(?,?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[2] = {0}, rb[5] = {0};
    bool is_null[5] = {0};
    unsigned long len[5] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&pb[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);

    if (exec_call(conn, sql, pb, 2, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONGLONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_STRING, out_time_stamp, 19, is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_TINY, out_sensor_number, sizeof(*out_sensor_number), is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_LONG, out_sensor_value, sizeof(*out_sensor_value), is_null[4]);

    rb[1].length = &len[1];
    rb[2].length = &len[2];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    out_reg_num[len[1]] = '\0';
    out_time_stamp[len[2]] = '\0';
    mysql_stmt_close(stmt);
    return 0;
}

int SelectCounter(MYSQL *conn,
                  const char *reg_num,
                  const char *time_stamp,
                  uint64_t *out_id,
                  char out_reg_num[9],
                  char out_time_stamp[20],
                  uint8_t *out_counter_number,
                  uint32_t *out_counter_value)
{
    const char *sql = "CALL SelectCounter(?,?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[2] = {0}, rb[5] = {0};
    bool is_null[5] = {0};
    unsigned long len[5] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&pb[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);

    if (exec_call(conn, sql, pb, 2, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONGLONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_STRING, out_time_stamp, 19, is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_TINY, out_counter_number, sizeof(*out_counter_number), is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_LONG, out_counter_value, sizeof(*out_counter_value), is_null[4]);

    rb[1].length = &len[1];
    rb[2].length = &len[2];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    out_reg_num[len[1]] = '\0';
    out_time_stamp[len[2]] = '\0';
    mysql_stmt_close(stmt);
    return 0;
}

int SelectDigitalSensor(MYSQL *conn,
                        const char *reg_num,
                        const char *time_stamp,
                        uint64_t *out_id,
                        char out_reg_num[9],
                        char out_time_stamp[20],
                        uint8_t *out_sensor_number,
                        uint8_t *out_sensor_value)
{
    const char *sql = "CALL SelectDigitalSensor(?,?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[2] = {0}, rb[5] = {0};
    bool is_null[5] = {0};
    unsigned long len[5] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&pb[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);

    if (exec_call(conn, sql, pb, 2, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONGLONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_STRING, out_time_stamp, 19, is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_TINY, out_sensor_number, sizeof(*out_sensor_number), is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_TINY, out_sensor_value, sizeof(*out_sensor_value), is_null[4]);

    rb[1].length = &len[1];
    rb[2].length = &len[2];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    out_reg_num[len[1]] = '\0';
    out_time_stamp[len[2]] = '\0';
    mysql_stmt_close(stmt);
    return 0;
}

int SelectMachine(MYSQL *conn,
                  const char *reg_num,
                  uint32_t *out_id,
                  char out_reg_num[9],
                  int32_t *out_software_ver,
                  int32_t *out_landing_gear_num,
                  int32_t *out_status,
                  uint8_t *out_work_mode,
                  char **out_config_name)
{
    const char *sql = "CALL SelectMachine(?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[1] = {0}, rb[7] = {0};
    bool is_null[7] = {0};
    unsigned long len[7] = {0};
    char config_buf[1024] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    if (exec_call(conn, sql, pb, 1, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_LONG, out_software_ver, sizeof(*out_software_ver), is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_LONG, out_landing_gear_num, sizeof(*out_landing_gear_num), is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_LONG, out_status, sizeof(*out_status), is_null[4]);
    bind_param(&rb[5], MYSQL_TYPE_TINY, out_work_mode, sizeof(*out_work_mode), is_null[5]);
    bind_param(&rb[6], MYSQL_TYPE_STRING, config_buf, sizeof(config_buf)-1, is_null[6]);

    rb[1].length = &len[1];
    rb[6].length = &len[6];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }

    out_reg_num[len[1]] = '\0';
    config_buf[len[6]] = '\0';
    *out_config_name = strdup(config_buf);
    mysql_stmt_close(stmt);
    return 0;
}

int SelectPosition(MYSQL *conn,
                   const char *reg_num,
                   const char *time_stamp,
                   uint64_t *out_id,
                   char out_reg_num[9],
                   char out_time_stamp[20],
                   double *out_latitude,
                   double *out_longitude,
                   int32_t *out_altitude,
                   uint16_t *out_speed,
                   uint16_t *out_direction,
                   uint32_t *out_odometer,
                   uint8_t *out_digital_inputs,
                   uint8_t *out_source)
{
    const char *sql = "CALL SelectPosition(?,?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[2] = {0}, rb[11] = {0};
    bool is_null[11] = {0};
    unsigned long len[11] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&pb[1], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);

    if (exec_call(conn, sql, pb, 2, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONGLONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_STRING, out_time_stamp, 19, is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_DOUBLE, out_latitude, sizeof(*out_latitude), is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_DOUBLE, out_longitude, sizeof(*out_longitude), is_null[4]);
    bind_param(&rb[5], MYSQL_TYPE_LONG, out_altitude, sizeof(*out_altitude), is_null[5]);
    bind_param(&rb[6], MYSQL_TYPE_SHORT, out_speed, sizeof(*out_speed), is_null[6]);
    bind_param(&rb[7], MYSQL_TYPE_SHORT, out_direction, sizeof(*out_direction), is_null[7]);
    bind_param(&rb[8], MYSQL_TYPE_LONG, out_odometer, sizeof(*out_odometer), is_null[8]);
    bind_param(&rb[9], MYSQL_TYPE_TINY, out_digital_inputs, sizeof(*out_digital_inputs), is_null[9]);
    bind_param(&rb[10], MYSQL_TYPE_TINY, out_source, sizeof(*out_source), is_null[10]);

    rb[1].length = &len[1];
    rb[2].length = &len[2];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    out_reg_num[len[1]] = '\0';
    out_time_stamp[len[2]] = '\0';
    mysql_stmt_close(stmt);
    return 0;
}

int SelectWorkSession(MYSQL *conn,
                      const char *reg_num,
                      uint32_t *out_id,
                      char out_reg_num[9],
                      uint8_t *out_work_mode,
                      char out_start[20],
                      char out_end[20])
{
    const char *sql = "CALL SelectWorkSession(?)";
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND pb[1] = {0}, rb[5] = {0};
    bool is_null[5] = {0};
    unsigned long len[5] = {0};

    bind_param(&pb[0], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    if (exec_call(conn, sql, pb, 1, &stmt)) return -1;

    bind_param(&rb[0], MYSQL_TYPE_LONG, out_id, sizeof(*out_id), is_null[0]);
    bind_param(&rb[1], MYSQL_TYPE_STRING, out_reg_num, 8, is_null[1]);
    bind_param(&rb[2], MYSQL_TYPE_TINY, out_work_mode, sizeof(*out_work_mode), is_null[2]);
    bind_param(&rb[3], MYSQL_TYPE_STRING, out_start, 19, is_null[3]);
    bind_param(&rb[4], MYSQL_TYPE_STRING, out_end, 19, is_null[4]);

    rb[1].length = &len[1];
    rb[3].length = &len[3];
    rb[4].length = &len[4];

    if (mysql_stmt_bind_result(stmt, rb) || mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return -1;
    }
    out_reg_num[len[1]] = '\0';
    out_start[len[3]] = '\0';
    out_end[len[4]] = '\0';
    mysql_stmt_close(stmt);
    return 0;
}

/* ======================================================================
   UPDATE PROCEDURES
   ====================================================================== */

int UpdateAnalogSensor(MYSQL *conn,
                       uint64_t id,
                       const char *reg_num,
                       const char *time_stamp,
                       uint8_t sensor_number,
                       uint32_t sensor_value)
{
    const char *sql = "CALL UpdateAnalogSensor(?,?,?,?,?)";
    MYSQL_BIND b[5] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[3], MYSQL_TYPE_TINY, &sensor_number, sizeof(sensor_number), 0);
    bind_param(&b[4], MYSQL_TYPE_LONG, &sensor_value, sizeof(sensor_value), 0);

    return exec_call(conn, sql, b, 5, NULL);
}

int UpdateCounter(MYSQL *conn,
                  uint64_t id,
                  const char *reg_num,
                  const char *time_stamp,
                  uint8_t counter_number,
                  uint32_t counter_value)
{
    const char *sql = "CALL UpdateCounter(?,?,?,?,?)";
    MYSQL_BIND b[5] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[3], MYSQL_TYPE_TINY, &counter_number, sizeof(counter_number), 0);
    bind_param(&b[4], MYSQL_TYPE_LONG, &counter_value, sizeof(counter_value), 0);

    return exec_call(conn, sql, b, 5, NULL);
}

int UpdateDigitalSensor(MYSQL *conn,
                        uint64_t id,
                        const char *reg_num,
                        const char *time_stamp,
                        uint8_t sensor_number,
                        uint8_t sensor_value)
{
    const char *sql = "CALL UpdateDigitalSensor(?,?,?,?,?)";
    MYSQL_BIND b[5] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[3], MYSQL_TYPE_TINY, &sensor_number, sizeof(sensor_number), 0);
    bind_param(&b[4], MYSQL_TYPE_TINY, &sensor_value, sizeof(sensor_value), 0);

    return exec_call(conn, sql, b, 5, NULL);
}

int UpdateMachine(MYSQL *conn,
                  uint32_t id,
                  const char *reg_num,
                  int32_t software_ver,
                  int32_t landing_gear_num,
                  int32_t status,
                  uint8_t work_mode,
                  const char *config_name)
{
    const char *sql = "CALL UpdateMachine(?,?,?,?,?,?,?)";
    MYSQL_BIND b[7] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_LONG, &software_ver, sizeof(software_ver), 0);
    bind_param(&b[3], MYSQL_TYPE_LONG, &landing_gear_num, sizeof(landing_gear_num), 0);
    bind_param(&b[4], MYSQL_TYPE_LONG, &status, sizeof(status), 0);
    bind_param(&b[5], MYSQL_TYPE_TINY, &work_mode, sizeof(work_mode), 0);
    bind_param(&b[6], MYSQL_TYPE_STRING, config_name, strlen(config_name), 0);

    return exec_call(conn, sql, b, 7, NULL);
}

int UpdatePosition(MYSQL *conn,
                   uint64_t id,
                   const char *reg_num,
                   const char *time_stamp,
                   double latitude,
                   double longitude,
                   const int32_t *altitude,
                   const uint16_t *speed,
                   const uint16_t *direction,
                   const uint32_t *odometer,
                   const uint8_t *digital_inputs,
                   const uint8_t *source)
{
    const char *sql = "CALL UpdatePosition(?,?,?,?,?,?,?,?,?,?,?)";
    MYSQL_BIND b[11] = {0};
    bool nulls[11] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONGLONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_STRING, time_stamp, strlen(time_stamp), 0);
    bind_param(&b[3], MYSQL_TYPE_DOUBLE, &latitude, sizeof(latitude), 0);
    bind_param(&b[4], MYSQL_TYPE_DOUBLE, &longitude, sizeof(longitude), 0);

    nulls[5] = (altitude == NULL);
    bind_param(&b[5], MYSQL_TYPE_LONG, altitude, sizeof(int32_t), nulls[5]);

    nulls[6] = (speed == NULL);
    bind_param(&b[6], MYSQL_TYPE_SHORT, speed, sizeof(uint16_t), nulls[6]);

    nulls[7] = (direction == NULL);
    bind_param(&b[7], MYSQL_TYPE_SHORT, direction, sizeof(uint16_t), nulls[7]);

    nulls[8] = (odometer == NULL);
    bind_param(&b[8], MYSQL_TYPE_LONG, odometer, sizeof(uint32_t), nulls[8]);

    nulls[9] = (digital_inputs == NULL);
    bind_param(&b[9], MYSQL_TYPE_TINY, digital_inputs, sizeof(uint8_t), nulls[9]);

    nulls[10] = (source == NULL);
    bind_param(&b[10], MYSQL_TYPE_TINY, source, sizeof(uint8_t), nulls[10]);

    return exec_call(conn, sql, b, 11, NULL);
}

int UpdateWorkSession(MYSQL *conn,
                      uint32_t id,
                      const char *reg_num,
                      uint8_t work_mode,
                      const char *start_time_stamp,
                      const char *end_time_stamp)
{
    const char *sql = "CALL UpdateWorkSession(?,?,?,?,?)";
    MYSQL_BIND b[5] = {0};
    bool nulls[5] = {0};

    bind_param(&b[0], MYSQL_TYPE_LONG, &id, sizeof(id), 0);
    bind_param(&b[1], MYSQL_TYPE_STRING, reg_num, strlen(reg_num), 0);
    bind_param(&b[2], MYSQL_TYPE_TINY, &work_mode, sizeof(work_mode), 0);

    nulls[3] = (start_time_stamp == NULL);
    bind_param(&b[3], MYSQL_TYPE_STRING, start_time_stamp,
               start_time_stamp ? strlen(start_time_stamp) : 0, nulls[3]);

    nulls[4] = (end_time_stamp == NULL);
    bind_param(&b[4], MYSQL_TYPE_STRING, end_time_stamp,
               end_time_stamp ? strlen(end_time_stamp) : 0, nulls[4]);

    return exec_call(conn, sql, b, 5, NULL);
}

/* ======================================================================
   DEMO: Call all procedures
   ====================================================================== */

int CallAllProceduresDemo(MYSQL *conn)
{
    // INSERT
    InsertAnalogSensor(conn, "X9999999", "2025-01-01 10:00:00", 1, 100);
    InsertCounter(conn, "X9999999", "2025-01-01 10:00:00", 2, 200);
    InsertDigitalSensor(conn, "X9999999", "2025-01-01 10:00:00", 3, 1);
    InsertMachine(conn, "X9999999", 5, 6, 7, 1, "demo_cfg");
    InsertPosition(conn, "X9999999", "2025-01-01 10:00:00", 50.0, 30.0, NULL, NULL, NULL, NULL, NULL, NULL);
    InsertWorkSession(conn, "X9999999", 1, "2025-01-01 08:00:00", NULL);

    // SELECT
    uint64_t id64; uint32_t id32; char buf[32]; uint8_t u8; uint32_t u32; double d;
    SelectAnalogSensor(conn, "X9999999", "2025-01-01 10:00:00", &id64, buf, buf, &u8, &u32);
    SelectMachine(conn, "X9999999", &id32, buf, (int32_t*)&u32, (int32_t*)&u32, (int32_t*)&u32, &u8, NULL);

    // UPDATE
    UpdateAnalogSensor(conn, id64, "X9999999", "2025-01-02 11:00:00", 9, 999);
    UpdateMachine(conn, id32, "X9999999", 99, 88, 77, 2, "updated_cfg");

    // DELETE
    DeleteAnalogSensor(conn, id64);
    DeleteMachine(conn, "X9999999");

    return 0;
}
