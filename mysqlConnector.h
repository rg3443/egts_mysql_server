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

/* ----------------------------------------------------------------------
 *  All Call Procedures
 * ---------------------------------------------------------------------- */

int InsertAnalogSensor(MYSQL *conn,
                       const char *reg_num,               // VARCHAR(8)
                       const char *time_stamp,            // DATETIME
                       uint8_t sensor_number,             // TINYINT U
                       uint32_t sensor_value);            // INT U

int InsertCounter(MYSQL *conn,
                  const char *reg_num,
                  const char *time_stamp,
                  uint8_t counter_number,
                  uint32_t counter_value);

int InsertDigitalSensor(MYSQL *conn,
                        const char *reg_num,
                        const char *time_stamp,
                        uint8_t sensor_number,
                        uint8_t sensor_value);

int InsertMachine(MYSQL *conn,
                  const char *reg_num,
                  int32_t software_ver,
                  int32_t landing_gear_num,
                  int32_t status,
                  uint8_t work_mode,
                  const char *config_name);

int InsertPosition(MYSQL *conn,
                   const char *reg_num,
                   const char *time_stamp,
                   double latitude,
                   double longitude,
                   int32_t *altitude,          // NULLable
                   int16_t *speed,            // NULLable
                   uint16_t *direction,        // NULLable
                   uint32_t *odometer,         // NULLable
                   uint8_t *digital_inputs,    // NULLable
                   uint8_t *source);           // NULLable

int InsertWorkSession(MYSQL *conn,
                      const char *reg_num,
                      uint8_t work_mode,
                      const char *start_time_stamp,   /* DATETIME NULLable */
                      const char *end_time_stamp);    /* DATETIME NULLable */

int DeleteAnalogSensor(MYSQL *conn, uint64_t id);

int DeleteCounter(MYSQL *conn, uint64_t id);

int DeleteDigitalSensor(MYSQL *conn, uint64_t id);

int DeleteMachine(MYSQL *conn, const char *reg_num);

int DeletePosition(MYSQL *conn, uint64_t id);

int DeleteWorkSession(MYSQL *conn, uint32_t id);

int SelectAnalogSensor(MYSQL *conn,
                       const char *reg_num,
                       const char *time_stamp,
                       uint64_t *out_id,
                       char out_reg_num[9],
                       char out_time_stamp[20],
                       uint8_t *out_sensor_number,
                       uint32_t *out_sensor_value);

int SelectCounter(MYSQL *conn,
                  const char *reg_num,
                  const char *time_stamp,
                  uint64_t *out_id,
                  char out_reg_num[9],
                  char out_time_stamp[20],
                  uint8_t *out_counter_number,
                  uint32_t *out_counter_value);

int SelectDigitalSensor(MYSQL *conn,
                        const char *reg_num,
                        const char *time_stamp,
                        uint64_t *out_id,
                        char out_reg_num[9],
                        char out_time_stamp[20],
                        uint8_t *out_sensor_number,
                        uint8_t *out_sensor_value);

int SelectMachine(MYSQL *conn,
                  const char *reg_num,
                  uint32_t *out_id,
                  char out_reg_num[9],
                  int32_t *out_software_ver,
                  int32_t *out_landing_gear_num,
                  int32_t *out_status,
                  uint8_t *out_work_mode,
                  char **out_config_name);   /* caller must free() */

int SelectPosition(MYSQL *conn,
                   const char *reg_num,
                   const char *time_stamp,
                   uint64_t *out_id,
                   char out_reg_num[9],
                   char out_time_stamp[20],
                   double *out_latitude,
                   double *out_longitude,
                   int32_t *out_altitude,        /* NULLable */
                   uint16_t *out_speed,          /* NULLable */
                   uint16_t *out_direction,      /* NULLable */
                   uint32_t *out_odometer,       /* NULLable */
                   uint8_t *out_digital_inputs,  /* NULLable */
                   uint8_t *out_source);         /* NULLable */

int SelectWorkSession(MYSQL *conn,
                      const char *reg_num,
                      uint32_t *out_id,
                      char out_reg_num[9],
                      uint8_t *out_work_mode,
                      char out_start[20],
                      char out_end[20]);

int UpdateAnalogSensor(MYSQL *conn,
                       uint64_t id,
                       const char *reg_num,
                       const char *time_stamp,
                       uint8_t sensor_number,
                       uint32_t sensor_value);

int UpdateCounter(MYSQL *conn,
                  uint64_t id,
                  const char *reg_num,
                  const char *time_stamp,
                  uint8_t counter_number,
                  uint32_t counter_value);

int UpdateDigitalSensor(MYSQL *conn,
                        uint64_t id,
                        const char *reg_num,
                        const char *time_stamp,
                        uint8_t sensor_number,
                        uint8_t sensor_value);

int UpdateMachine(MYSQL *conn,
                  uint32_t id,
                  const char *reg_num,
                  int32_t software_ver,
                  int32_t landing_gear_num,
                  int32_t status,
                  uint8_t work_mode,
                  const char *config_name);

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
                   const uint8_t *source);

int UpdateWorkSession(MYSQL *conn,
                      uint32_t id,
                      const char *reg_num,
                      uint8_t work_mode,
                      const char *start_time_stamp,
                      const char *end_time_stamp);

// for testing
int CallAllProceduresDemo(MYSQL *conn);

#endif // MYSQLCONNECTOR_H_INCLUDED
