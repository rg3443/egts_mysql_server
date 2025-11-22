#include "mysqlConnector.h"

//conn = 0;
MYSQL * conn_ = NULL;
bool ConnectMYSQL(const char* server,
                  const char* user,
                  const char* password,
                  const char* db)
{
    // Если соединение уже есть — проверяем, живо ли онo
    if (conn_ != NULL) {
        if (mysql_ping(conn_) == 0) {
            // Соединение живо — ничего не делаем
            printf("MySQL: соединение уже активно\n");
            return true;
        } else {
            // Соединение мертво — закрываем и переподключаемся
            printf("MySQL: старое соединение разорвано, переподключаемся...\n");
            mysql_close(conn_);
            conn_ = NULL;
        }
    }

    // Инициализация нового объекта
    conn_ = mysql_init(NULL);
    if (conn_ == NULL) {
        fprintf(stderr, "mysql_init() failed — нехватка памяти\n");
        return false;
    }

    // Реальное подключение
    if (mysql_real_connect(conn_, server, user, password, db, 3306, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn_));
        mysql_close(conn_);
        conn_ = NULL;
        return false;
    }

    printf("Подключено к MySQL!\n");
    return true;
}

bool QuerryMYSQL(const char* querryStr, MYSQL_RES * res)
{
    MYSQL_ROW row;
    // Проверка подключения
    if(mysql_ping(conn_) != 0) {
        fprintf(stderr, "mysql_ping() failed: %s\n", mysql_error(conn_));
        mysql_close(conn_);
        return false;
    }
    // Запрос
    if(mysql_query(conn_,querryStr) != 0) {
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


int InsertTerminal(MYSQL *conn, uint32_t terminalId)
{
    const char* sqlQuerry = "CALL upsert_terminal_adv(?,'.','active',NULL);";
    MYSQL_BIND b[1] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);

    return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertPos(
    MYSQL *conn,
    uint32_t terminalId,
    uint32_t navigation_time, //ntm
    uint32_t latitude, // lat
    uint32_t longitude,// long
    unsigned lohs,
    unsigned lahs,
    unsigned is_moving, //mv
    unsigned is_blackbox, //bb
    unsigned is_3d, // fix
    unsigned cs, //  0-WGS-84, 1-ПЗ-90.02
    unsigned is_valid, // vld
    int16_t speed,
    unsigned alte,
    int8_t direction,
    int8_t odometr[3],
    uint8_t digital_input, //din
    int8_t source
)
{
    const char* sqlQuerry = "CALL insert_position(?,NOW(6),?,?,?,0,?,?,?,?,?,?,?,?,0,?,?,?,0,0);";
    MYSQL_BIND b[17] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
   // bind_param(&b[1], MYSQL_TYPE_DATETIME, timeStamp, sizeof(timeStamp),0);
    bind_param(&b[1], MYSQL_TYPE_LONG, &navigation_time, sizeof(uint32_t),0);
    bind_param(&b[2], MYSQL_TYPE_LONG, &latitude, sizeof(uint32_t),0);
    bind_param(&b[3], MYSQL_TYPE_LONG, &longitude, sizeof(uint32_t),0);
    bind_param(&b[4], MYSQL_TYPE_LONG, &lohs, sizeof(unsigned),0);
    bind_param(&b[5], MYSQL_TYPE_LONG,&lahs, sizeof(unsigned),0);
    bind_param(&b[6], MYSQL_TYPE_LONG,&is_moving, sizeof(unsigned),0);
    bind_param(&b[7], MYSQL_TYPE_LONG,&is_blackbox, sizeof(unsigned),0);
    bind_param(&b[8], MYSQL_TYPE_LONG,&is_3d, sizeof(unsigned),0);
    bind_param(&b[9], MYSQL_TYPE_LONG,&cs, sizeof(unsigned),0);
    bind_param(&b[10], MYSQL_TYPE_LONG,&is_valid, sizeof(unsigned),0);
    bind_param(&b[11], MYSQL_TYPE_LONG,&speed, sizeof(uint16_t),0);
    bind_param(&b[12], MYSQL_TYPE_LONG,&alte, sizeof(unsigned),0);
    bind_param(&b[13], MYSQL_TYPE_LONG,&direction, sizeof(uint16_t),0);
    //todo: combine odm
    uint32_t odm = 0;
    bind_param(&b[14], MYSQL_TYPE_LONG,&odm, sizeof(uint32_t),0);
    bind_param(&b[15], MYSQL_TYPE_LONG,&digital_input, sizeof(uint8_t),0);
    bind_param(&b[16], MYSQL_TYPE_LONG,&source, sizeof(uint8_t),0);

    return exec_call(conn,sqlQuerry,b,4,NULL);
}
