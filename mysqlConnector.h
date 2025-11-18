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
);

#endif // MYSQLCONNECTOR_H_INCLUDED
