#include "mysqlConnector.h"

//conn = 0;
MYSQL * conn_ = NULL;

const char* server_ = "172.27.160.1";
const char* user_  = "remote_user";
const char* password_ = "[eqdgbplt1337A";
const char* db_ = "egts_telemetry";

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


int InsertTerminal(
	MYSQL *conn,
	uint32_t terminalId,
	const char* expiredAt,
	uint16_t hdid,
	uint16_t bs
	//char imei[15],
	//char imsi[16],
	//char lngc[3],
	//uint8_t nid[3],
	//char msisdn[15]
)
{
    const char* sqlQuerry = "CALL upsert_terminal_adv(?,'.',NULL,?,NULL,NULL,NULL,NULL,NULL,NULL);";

	MYSQL_BIND b[2] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(uint32_t),0);
	bind_param(&b[1], MYSQL_TYPE_SHORT, hdid, sizeof(uint16_t),0);
	//todo: other params


    return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertPos(
    MYSQL *conn,
    uint32_t terminalId,
    uint32_t ntm, 	// navigation time
    uint32_t lat, 	// latitude
    uint32_t longg,	// longitude
    unsigned lohs, 	// 1-west 0-east
    unsigned lahs,	// 1-south 0-north
    unsigned mv, 	// is moving
    unsigned bb, 	// is blackbox
    unsigned fix,	// is 3d
    unsigned cs,    //  0-WGS-84, 1-ПЗ-90.02
    unsigned vld,   // is valid
    int16_t spd,   	// speed
    unsigned alte, 	// is altitude here
    int16_t dir, 	// direction
    int8_t odm[3], 	// odometr
    unsigned din, 	// digital input
    unsigned src 		// source
)
{
    const char* sqlQuerry = "CALL insert_position(?,NOW(6),?,?,?,0,?,?,?,?,?,?,?,?,0,?,?,?,0,0);";
    MYSQL_BIND b[17] = {0};
    bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, 	sizeof(uint32_t),0);
   // bind_param(&b[1], MYSQL_TYPE_DATETIME, timeStamp, sizeof(timeStamp),0);
    bind_param(&b[1],  MYSQL_TYPE_LONG, &ntm,			sizeof(uint32_t), 0);
    bind_param(&b[2],  MYSQL_TYPE_LONG, &lat,		 	sizeof(uint32_t), 0);
    bind_param(&b[3],  MYSQL_TYPE_LONG, &longg, 		sizeof(uint32_t), 0);
    bind_param(&b[4],  MYSQL_TYPE_TINY, &lohs, 			sizeof(unsigned), 0);
    bind_param(&b[5],  MYSQL_TYPE_TINY, &lahs, 			sizeof(unsigned), 0);
    bind_param(&b[6],  MYSQL_TYPE_TINY, &mv, 			sizeof(unsigned), 0);
    bind_param(&b[7],  MYSQL_TYPE_TINY, &bb, 			sizeof(unsigned), 0);
    bind_param(&b[8],  MYSQL_TYPE_TINY, &fix, 			sizeof(unsigned), 0);
    bind_param(&b[9],  MYSQL_TYPE_TINY, &cs, 			sizeof(unsigned), 0);
    bind_param(&b[10], MYSQL_TYPE_TINY, &vld, 			sizeof(unsigned), 0);
    bind_param(&b[11], MYSQL_TYPE_LONG, &spd, 			sizeof(uint16_t), 0);
    bind_param(&b[12], MYSQL_TYPE_TINY, &alte, 			sizeof(unsigned), 0);
    bind_param(&b[13], MYSQL_TYPE_SHORT,&dir, 			sizeof(uint16_t), 0);
    //todo: combine odm
    uint32_t odm_ = 0;
    bind_param(&b[14], MYSQL_TYPE_LONG, &odm_, 			sizeof(uint32_t), 0);
    bind_param(&b[15], MYSQL_TYPE_LONG, &din,           sizeof(unsigned),  0);
    bind_param(&b[16], MYSQL_TYPE_LONG, &src, 		    sizeof(unsigned),  0);

    return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertState(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t st,		// state
	uint8_t mpsv,	// main power source voltage
	uint8_t bbv,	// backup battery votlage
	uint8_t ibv,	// interal battery voltage
	unsigned nms, 	// navigation module time
	unsigned ibu,	// internal battery use
	unsigned bbu	// backup battery use
)
{
	const char* sqlQuerry = "CALL upsert_state(?,?,?,?,?,?,?,?);";

	MYSQL_BIND b[8] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId,  sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_TINY, st,             	sizeof(uint8_t),0);
	bind_param(&b[2], MYSQL_TYPE_TINY, mpsv,            sizeof(uint8_t),0);
	bind_param(&b[3], MYSQL_TYPE_TINY, bbv,             sizeof(uint8_t),0);
	bind_param(&b[4], MYSQL_TYPE_TINY, ibv,             sizeof(uint8_t),0);
	bind_param(&b[5], MYSQL_TYPE_TINY, nms,        	 	sizeof(unsigned),0);
	bind_param(&b[6], MYSQL_TYPE_TINY, ibu,         	sizeof(unsigned),0);
	bind_param(&b[7], MYSQL_TYPE_TINY, bbu,         	sizeof(unsigned),0);

	return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertDin(
	MYSQL *conn,
	uint32_t terminalId,
	uint32_t dinId,
	uint16_t dinVal
)
{
	const char* sqlQuerry = "CALL upsert_din_sens_data(?,?,?);";

	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_SHORT, dinId, sizeof(uint32_t),0);
	bind_param(&b[2], MYSQL_TYPE_SHORT, dinVal, sizeof(uint16_t),0);

	return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertAin(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t ainId,
	uint32_t ainVal
)
{
	const char* sqlQuerry = "CALL upsert_an_sens_data(?,?,?);";

	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_TINY, ainId, sizeof(uint8_t),0);
	bind_param(&b[2], MYSQL_TYPE_LONG, ainVal, sizeof(uint32_t),0);

	return exec_call(conn,sqlQuerry,b,4,NULL);
}

int InsertCntr(
	MYSQL * conn,
	uint32_t terminalId,
	uint8_t cn, 	// Counter Number
	uint32_t cnv 	// Coutner Value
)
{
	const char* sqlQuerry = "CALL upsert_cntr_data(?,?,?);";
	
	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_TINY, 	   cn, 		   sizeof(uint8_t),0);
	bind_param(&b[0], MYSQL_TYPE_SHORT,     cnv, 	   sizeof(uint32_t),0);
	
}
