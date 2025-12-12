#include "mysqlConnector.h"
static uint32_t g_mysql_params[20] = {0};
//conn = 0;
MYSQL * conn_ = NULL;
bool mysqlConnected = false;

const char* server_ = "185.105.109.237";
const char* user_  = "vkm";
const char* password_ = "StrongVkm1337!";
const char* db_ = "vkm_telemetry";

uint32_t odm_ = 0;

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
	uint32_t terminalId//,
	//uint16_t hdid,
	//uint16_t bs
	//char imei[15],
	//char imsi[16],
	//char lngc[3],
	//uint8_t nid[3],
	//char msisdn[15]
)
{
    //const char* sqlQuerry = "CALL usp_terminal_keys_upsert(?,'.',NULL,11,NULL,NULL,NULL,NULL,NULL,NULL);";

	//MYSQL_BIND b[1] = {0};
    //bind_param(&b[0], MYSQL_TYPE_LONGLONG, &terminalId, sizeof(terminalId),0);
	//bind_param(&b[1], MYSQL_TYPE_SHORT, hdid, sizeof(uint16_t),0);
	//todo: other params


    //return exec_call(conn,sqlQuerry,b,1,NULL);
	return 0;
}

int InsertAin(
	MYSQL *conn,
	uint32_t terminalId,
	uint16_t ainId,
	uint16_t ainVal
)
{
	const char* sqlQuerry = "CALL upsert_an_sens_data(?,?,?);";

	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_SHORT, ainId, sizeof(uint16_t),0);
	bind_param(&b[2], MYSQL_TYPE_SHORT, ainVal, sizeof(uint16_t),0);

	return exec_call(conn,sqlQuerry,b,3,NULL);
}

int InsertDin(
	MYSQL *conn,
	uint32_t terminalId,
	uint16_t dinId,
	uint16_t dinVal
)
{
	const char* sqlQuerry = "CALL upsert_din_sens_data(?,?,?);";

	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId),0);
	bind_param(&b[1], MYSQL_TYPE_SHORT, dinId, sizeof(uint16_t),0);
	bind_param(&b[2], MYSQL_TYPE_SHORT, dinVal, sizeof(uint16_t),0);

	return exec_call(conn,sqlQuerry,b,3,NULL);
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
	bind_param(&b[2], MYSQL_TYPE_SHORT,     cnv, 	   sizeof(uint32_t),0);

	return exec_call(conn,sqlQuerry,b,3,NULL);
}

int InsertLoopin(
	MYSQL * conn,
	uint32_t terminalId,
	uint16_t lin, 	// Loop In Number
	uint8_t lis		// Loop In State
)
{
	const char* sqlQuerry = "CALL upsert_loopin(?,?,?);";

	MYSQL_BIND b[3] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId), 0);
	bind_param(&b[1], MYSQL_TYPE_SHORT, lin, sizeof(uint16_t), 0);
	bind_param(&b[2], MYSQL_TYPE_TINY, lis, sizeof(uint8_t), 0);

	return exec_call(conn,sqlQuerry,b,3,NULL);
}

int InsertLiquidLevel(
	MYSQL * conn,
	uint32_t terminalId,
	unsigned llsef, // Liquid Level Sensor Error Flag
	uint8_t llsvu,  // Liquid level Sensor Value Unit
	unsigned rdf,   // Raw Data Flag
	uint8_t llsn,   // Liquid Level Sensor Number
	uint16_t maddr, // Module Address
	uint32_t llsd   // Liquid Level Sensor (Data?)
)
{
	const char* sqlQuerry = "CALL upsert_liquid_level(?,?,?,?,?,?,?);";

	MYSQL_BIND b[7] = {0};
	bind_param(&b[0], MYSQL_TYPE_LONGLONG, terminalId, sizeof(terminalId), 0);
	bind_param(&b[1], MYSQL_TYPE_TINY, llsef, sizeof(unsigned), 0);
	bind_param(&b[2], MYSQL_TYPE_TINY, llsvu, sizeof(uint8_t), 0);
	bind_param(&b[3], MYSQL_TYPE_TINY, rdf, sizeof(unsigned), 0);
	bind_param(&b[4], MYSQL_TYPE_TINY, llsn, sizeof(uint8_t), 0);
	bind_param(&b[5], MYSQL_TYPE_SHORT, maddr, sizeof(uint16_t), 0);
	bind_param(&b[6], MYSQL_TYPE_LONG, llsd, sizeof(uint32_t), 0);

	return exec_call(conn,sqlQuerry,b,7,NULL);
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

	return exec_call(conn,sqlQuerry,b,8,NULL);
}

int InsertPos(
    MYSQL *conn,
    uint32_t terminalId,
    uint32_t ntm, 	// navigation time
    uint32_t lat, 	// latitude
    uint32_t longg,	// longitude
    unsigned alte, 	// is altitude here
    unsigned lohs, 	// 1-west 0-east
    unsigned lahs,	// 1-south 0-north
    unsigned mv, 	// is moving
    unsigned bb, 	// is blackbox
    unsigned fix,	// is 3d
    unsigned cs,    //  0-WGS-84, 1-ПЗ-90.02
    unsigned vld,   // is valid
    int16_t spd,   	// speed
    unsigned alts, 	// altitude
    int16_t dir, 	// direction
    int8_t odm[3], 	// odometr
    unsigned din, 	// digital input
    unsigned src 		// source
)
{
// Собираем одометр правильно
    uint32_t odometer = (uint32_t)(unsigned char)odm[0]       |
                        (uint32_t)(unsigned char)odm[1] << 8  |
                        (uint32_t)(unsigned char)odm[2] << 16;

    // Заполняем глобальный буфер
    g_mysql_params[0]  = terminalId;
    g_mysql_params[1]  = ntm;
    g_mysql_params[2]  = lat;
    g_mysql_params[3]  = longg;
    g_mysql_params[4]  = alte;
    g_mysql_params[5]  = lohs;
    g_mysql_params[6]  = lahs;
    g_mysql_params[7]  = mv;
    g_mysql_params[8]  = bb;
    g_mysql_params[9]  = fix;
    g_mysql_params[10] = cs;
    g_mysql_params[11] = vld;
    g_mysql_params[12] = (uint32_t)spd;
    g_mysql_params[13] = alts;
    g_mysql_params[14] = (uint32_t)dir;
    g_mysql_params[15] = odometer;
    g_mysql_params[16] = din;
    g_mysql_params[17] = src;
    g_mysql_params[18] = 0;  // altitude (или заполняй, если ALTE=1)
    g_mysql_params[19] = 0;  // srcd

    const char* sql = "CALL upsert_sr_pos_data(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
    MYSQL_BIND b[20] = {0};

    for (int i = 0; i < 20; i++) {
        b[i].buffer_type = (i == 0) ? MYSQL_TYPE_LONGLONG : MYSQL_TYPE_LONG;
        b[i].buffer = &g_mysql_params[i];
        b[i].buffer_length = sizeof(uint32_t);
        b[i].is_null_value = 0;
    }

    int ret = exec_call(conn, sql, b, 20, NULL);
    if (ret != 0) {
        print_msg(1, "InsertPos failed: %s\n", mysql_error(conn));
    }
}
