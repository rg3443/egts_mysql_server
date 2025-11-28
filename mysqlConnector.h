#ifndef MYSQLCONNECTOR_H_INCLUDED
#define MYSQLCONNECTOR_H_INCLUDED

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern MYSQL *conn_;           // Соединение


extern const char* server_;
extern const char* user_;
extern const char* password_;
extern const char* db_;

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
	uint16_t bs
	//char imei[15] 	= {0},
	//char imsi[16] 	= {0},
	//char lngc[3] 	= {0},
	//uint8_t nid[3] 	= {0},
	//char msisdn[15] = {0}
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
    unsigned alte, 	// is altitude here
    int16_t dir, 	// direction
    int8_t odm[3], 	// odometr
    unsigned din, 	// digital input
    unsigned src 		// source
);

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
);

int InsertDin(
	MYSQL *conn,
	uint32_t terminalId,
	uint16_t dinId,
	uint16_t dinVal
);

int InsertAin(
	MYSQL *conn,
	uint32_t terminalId,
	uint8_t ainId,
	uint32_t ainVal
);

int InsertCntr(
	MYSQL * conn,
	uint32_t terminalId,
	uint8_t cn, 	// Counter Number
	uint32_t cnv 	// Coutner Value
);

int InsertLoopin(
	MYSQL * conn,
	uint32_t terminalId,
	uint16_t lin, 	// Loop In Number
	uint8_t lis		// Loop In State
);

int InsertLiquidLevel(
	MYSQL * conn,
	uint32_t terminalId,
	unsigned llsef, // Liquid Level Sensor Error Flag  
	uint8_t llsvu,  // Liquid level Sensor Value Unit
	unsigned rdf,   // Raw Data Flag
	uint8_t llsn,   // Liquid Level Sensor Number
	uint16_t maddr, // Module Address 
	uint32_t llsd   // Liquid Level Sensor (Data?)
);

#endif // MYSQLCONNECTOR_H_INCLUDED
