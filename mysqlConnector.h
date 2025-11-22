#ifndef MYSQLCONNECTOR_H_INCLUDED
#define MYSQLCONNECTOR_H_INCLUDED

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern MYSQL *conn_;           // Соединение

bool ConnectMYSQL(const char* server,
                  const char* user,
                  const char* password,
                  const char* db);

bool QuerryMYSQL(const char* querryStr, MYSQL_RES * res);

int InsertTerminal(MYSQL *conn, uint32_t terminalId);

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
);

#endif // MYSQLCONNECTOR_H_INCLUDED
