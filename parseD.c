/* 
 Copyright © Dougie Lawson, 2020-2023, All rights reserved 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#define __USE_XOPEN
#include <time.h>
#include "parseJ.h"

#define COLUMN_COUNT 17

MYSQL* con;
MYSQL_STMT *stmt;
int row_count;

void exit_on_error(char* call, MYSQL *con)
{
	fprintf(stderr, "SQL error %s\n", call);
	if (con != NULL)
	{       
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
	}
	exit(-20);
}

void sql_init()
{

	my_bool trunc = 0;
   	char *sql;

	mysql_library_init(0, NULL, NULL);

	con = mysql_init(NULL);
	mysql_options(con, MYSQL_REPORT_DATA_TRUNCATION, &trunc);
	mysql_options(con, MYSQL_OPT_SSL_CA, "/etc/mysql/ssl-certs/ca-cert.pem");
	mysql_options(con, MYSQL_OPT_SSL_CERT, "/etc/mysql/ssl-certs/client-cert.pem");
	mysql_options(con, MYSQL_OPT_SSL_KEY, "/etc/mysql/ssl-certs/client-key.pem");
	mysql_options(con, MYSQL_OPT_TLS_VERSION, "TLSv1.2,TLSv1.3");

	if( con == NULL )
	{
		fprintf(stderr, "Can't run Mariadb");
		exit_on_error("Really bad", NULL);
	}

	if (mysql_real_connect(con, d_host, d_user, d_pwd, d_db, 0, NULL, 0) == NULL)
	{
		exit_on_error("Connect", con);
	}
 
	//printf("mariadb-connection-id: %ld\n" , mysql_thread_id(con));

	
	sql = "INSERT IGNORE INTO Zappi_test(datetime, imported, exported, genplus, genminus, zappiimp, zappidiv, voltage, frequency, pect1, nect1, pect2, nect2, pect3, nect3, ctsums, house) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	stmt = mysql_stmt_init(con);
	if (!stmt) 
	{
		exit_on_error("Stmt init", con);
	}

	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		exit_on_error("Stmt prepare", con);
	}
	row_count = 0;

}

void sql_insert()
{
	int rc;
	int param_count, column_count;
	MYSQL_BIND bind[COLUMN_COUNT];
	MYSQL_TIME ts;
	my_bool error[COLUMN_COUNT];
	my_bool* is_null[COLUMN_COUNT];
	float w_imported, w_exported, w_genplus, w_genminus, w_zappiimp, w_zappidiv, f_voltage, f_freq, w_pect_1, w_nect_1, w_pect_2, w_nect_2, w_pect_3, w_nect_3, w_ctsums, w_house;
	int count = 0;

	param_count = mysql_stmt_param_count(stmt);
	if (param_count != COLUMN_COUNT)
	{
		exit_on_error("Invalid param count", con);
	} 

	memset(bind, 0, sizeof(bind));

	struct tm tm;
	ts.year = year;
	ts.month = month;
	ts.day = day_of_month;
	ts.hour = hour;
	ts.minute = minute;
	ts.second = 0;
	ts.second_part = 0;

	bind[0].buffer_type = MYSQL_TYPE_DATETIME;
	bind[0].buffer = &ts;
	is_null[0] = (my_bool)0;
	bind[0].is_null = is_null[0];
	bind[0].error = &error[0];

	w_imported = watts(import);
	bind[1].buffer_type = MYSQL_TYPE_FLOAT;
	bind[1].buffer = &w_imported;
	is_null[1] = (my_bool)0;
	bind[1].is_null = is_null[1];
	bind[1].error = &error[1];

	w_exported = watts(exported);
	bind[2].buffer_type = MYSQL_TYPE_FLOAT;
	bind[2].buffer = &w_exported;
	is_null[2] = (my_bool)0;
	bind[2].is_null = is_null[2];
	bind[2].error = &error[2];

	w_genplus = watts(genplus);
	bind[3].buffer_type = MYSQL_TYPE_FLOAT;
	bind[3].buffer = &w_genplus;
	is_null[3] = (my_bool)0;
	bind[3].is_null = is_null[3];
	bind[3].error = &error[3];

	w_genminus = watts(genminus);
	bind[4].buffer_type = MYSQL_TYPE_FLOAT;
	bind[4].buffer = &w_genminus; 
	is_null[4] = (my_bool)0;
	bind[4].is_null = is_null[4];
	bind[4].error = &error[4];

	w_zappiimp = watts(zappiimp);
	bind[5].buffer_type = MYSQL_TYPE_FLOAT;
	bind[5].buffer = &w_zappiimp;
	is_null[5] = (my_bool)0;
	bind[5].is_null = is_null[5];
	bind[5].error = &error[5];

	w_zappidiv = watts(zappidiv);
	bind[6].buffer_type = MYSQL_TYPE_FLOAT;
	bind[6].buffer = &w_zappidiv;
	is_null[6] = (my_bool)0;
	bind[6].is_null = is_null[6];
	bind[6].error = &error[6];

	f_voltage = (float)voltage/10.0;
	bind[7].buffer_type = MYSQL_TYPE_FLOAT;
	bind[7].buffer = &f_voltage;
	is_null[7] = (my_bool)0;
	bind[7].is_null = is_null[7];
	bind[7].error = &error[7];

	f_freq = (float)frequency/100.0;
	bind[8].buffer_type = MYSQL_TYPE_FLOAT;
	bind[8].buffer = &f_freq;
	is_null[8] = (my_bool)0;
	bind[8].is_null = is_null[8];
	bind[8].error = &error[8];

	w_pect_1 = watts(pect_1);
	bind[9].buffer_type = MYSQL_TYPE_FLOAT;
	bind[9].buffer = &w_pect_1;
	is_null[9] = (my_bool)0;
	bind[9].is_null = is_null[9];
	bind[9].error = &error[9];

	w_nect_1 = watts(nect_1);
	bind[10].buffer_type = MYSQL_TYPE_FLOAT;
	bind[10].buffer = &w_nect_1;
	is_null[10] = (my_bool)0;
	bind[10].is_null = is_null[10];
	bind[10].error = &error[10];

	w_pect_2 = watts(pect_2);
	bind[11].buffer_type = MYSQL_TYPE_FLOAT;
	bind[11].buffer = &w_pect_2;
	is_null[11] = (my_bool)0;
	bind[11].is_null = is_null[11];
	bind[11].error = &error[11];

	w_nect_2 = watts(nect_2);
	bind[12].buffer_type = MYSQL_TYPE_FLOAT;
	bind[12].buffer = &w_nect_2;
	is_null[12] = (my_bool)0;
	bind[12].is_null = is_null[12];
	bind[12].error = &error[12];

	if (cts[3] != FALSE)
	{
		w_pect_3 = watts(pect_3);
		bind[13].buffer_type = MYSQL_TYPE_FLOAT;
		bind[13].buffer = &w_pect_3;
		is_null[13] = (my_bool)0;
		bind[13].is_null = is_null[13];
		bind[13].error = &error[13];
	
		w_nect_3 = watts(nect_3);
		bind[14].buffer_type = MYSQL_TYPE_FLOAT;
		bind[14].buffer = &w_nect_3;
		is_null[14] = (my_bool)0;
		bind[14].is_null = is_null[14];
		bind[14].error = &error[14];
	}
	else
	{
		bind[13].buffer_type = MYSQL_TYPE_NULL;
		bind[13].buffer = NULL;
		is_null[13] = (my_bool*)1;
		bind[13].is_null = is_null[13];
		bind[13].error = &error[13];
	
		bind[14].buffer_type = MYSQL_TYPE_NULL;
		bind[14].buffer = NULL;
		is_null[14] = (my_bool*)1;
		bind[14].is_null = is_null[14];
		bind[14].error = &error[14];

	}

	w_ctsums = ctSums(import, exported, pect_1, pect_2, pect_3, nect_1, nect_2, nect_3);
	bind[15].buffer_type = MYSQL_TYPE_FLOAT;
	bind[15].buffer = &w_ctsums;
	is_null[15] = (my_bool)0;
	bind[15].is_null = is_null[15];
	bind[15].error = &error[15];

	w_house = w_imported - (w_zappidiv + w_zappiimp);
	bind[16].buffer_type = MYSQL_TYPE_FLOAT;
	bind[16].buffer = &w_house;
	is_null[16] = (my_bool)0;
	bind[16].is_null = is_null[16];
	bind[16].error = &error[16];


	if (mysql_stmt_bind_param(stmt, bind))
	{
		exit_on_error("Stmt bind param", con);
	}
	if (year>0) 
	{
		rc = mysql_stmt_execute(stmt);

		if (rc)
		{
			exit_on_error("Stmt execute", con);
		}
		else row_count++;
		if (row_count > 59) sql_commit();
	}

}
void sql_commit()
{
	//printf("Commit done\n");
	mysql_commit(con);
	row_count = 0;
}

int sql_terminate()
{
   	if (mysql_stmt_close(stmt))
   	{
        	exit_on_error("Stmt close", con);
   	} 

	mysql_close(con);
	mysql_library_end();
  
	return 0;
}
