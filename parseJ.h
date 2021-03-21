#ifndef PARSEJ_H
#define PARSEJ_H

/* 
 Copyright © Dougie Lawson, 2020, All rights reserved 
*/

#define FALSE 0
#define TRUE !(FALSE)

#include <mysql.h>
#include "json-c/json.h"
#include <libconfig.h>

json_object* decode_json(json_object* jObj);
json_object* json_parse_array(json_object* jObj, int key);
json_object* json_object_parse(json_object* jObj);
json_object* json_parse(json_object* jObj);

int lexer(const char *s);
float watts(int zValue);
float ctSums(int ip, int ep, int p1, int p2, int p3, int n1, int n2, int n3);
void sql_init();
void sql_insert();
int sql_terminate();
void sql_commit();
void curl2();

extern char* current_key;
extern char* curTime;
extern int minute;
extern int hour;
extern char* day_of_week;
extern int day_of_month;
extern int month;
extern int year;
extern int import;
extern int exported;
extern int genplus;
extern int genminus;
extern int zappiimp;
extern int zappidiv;
extern int voltage;
extern int frequency;
extern int pect_1;
extern int nect_1;
extern int pect_2;
extern int nect_2;
extern int pect_3;
extern int nect_3;
extern char* serno;
extern char* ectt_1;
extern char* ectt_2;
extern char* ectt_3;

extern char* ectt_4;
extern char* ectt_5;
extern char* ectt_6;
extern int cts[6];

extern const char* d_host;
extern const char* d_db;
extern const char* d_user;
extern const char* d_pwd;

extern MYSQL* con;
extern MYSQL_STMT *stmt;
extern int row_count;

#endif
