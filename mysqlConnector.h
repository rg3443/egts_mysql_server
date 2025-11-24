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

int InsertTerminal(
	MYSQL *conn, 
	uint32_t terminalId,
	const char* expiredAt,
	uint16_t hdid,
	uint16_t bs,
	char imei[15] 	= {0},
	char imsi[16] 	= {0},
	char lngc[3] 	= {0},
	uint8_t nid[3] 	= {0},
	char msisdn[15] = {0}
);

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
    unsigned alte, 	//is altitude here
    int8_t dir, 	// direction
    int8_t odm[3], 	// odometr
    uint8_t din, 	// digital input
    int8_t src 		// source
);

int InsertTerminalState(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t st,		// state
	uint32_t mpsv,	// main power source voltage
	uint32_t bbv,	// backup battery votlage
	uint32_t ibv,	// interal battery voltage
	unsigned nms, 	// navigation module time
	unsigned ibu,	// internal battery use
	unsigned bbu	// backup battery use
);

int InsertDin(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t dinId,
	uint32_t dinVal
);

int InsertAin(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t ainId,
	uint32_t ainVal
);

#endif // MYSQLCONNECTOR_H_INCLUDED
