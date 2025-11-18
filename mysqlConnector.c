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

int InsertPos(
    MYSQL *conn,
    int64_t terminalId,
    const char* timeStamp,
    double latitude,
    double longitude,
    int32_t atitude,
    int16_t speed,
    int16_t direction,
    int32_t odometr,
    int8_t digitalInputs,
    int8_t source
)
{
    const char* sqlQuerry = "CALL insert_position(?,?,?,?,?,?,?,?,?,?);";
    MYSQL_BIND b[10] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
    bind_param(&b[1], MYSQL_TYPE_DATETIME, timeStamp, sizeof(timeStamp),0);
    bind_param(&b[2], MYSQL_TYPE_DOUBLE, &latitude, sizeof(latitude),0);
    bind_param(&b[3], MYSQL_TYPE_DOUBLE, &longitude, sizeof(longitude),0);
    bind_param(&b[4], MYSQL_TYPE_LONG, atitude, sizeof(atitude),0);
    bind_param(&b[5], MYSQL_TYPE_SHORT, speed, sizeof(speed),0);
    bind_param(&b[6], MYSQL_TYPE_SHORT, direction, sizeof(direction),0);
    bind_param(&b[7], MYSQL_TYPE_LONG, odometr, sizeof(odometr),0);
    bind_param(&b[8], MYSQL_TYPE_TINY, digitalInputs, sizeof(digitalInputs),0);
    bind_param(&b[9], MYSQL_TYPE_TINY, source, sizeof(source),0);

    return exec_call(conn,sqlQuerry,b,4,NULL);
}
