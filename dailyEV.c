/* 
 Copyright © Dougie Lawson, 2020, All rights reserved 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <libconfig.h>
#include <mysql.h>
#include <libgen.h>
#define CONFIG_FILE "/usr/local/etc/zappi.cfg"
#define COLUMNS 6

char *comma = " ";

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

int main(int argc, char* argv[])
{
	int rc;
	int param_count, column_count;
	MYSQL_STMT *stmtTZ, *stmt;
	MYSQL_BIND bind[COLUMNS];
	MYSQL_RES *prepare_meta_result;
	MYSQL_TIME ts;
	float full_usage, zappidiv, zappiimp, charging, house, exported;
	unsigned long length[COLUMNS];
	my_bool is_null[COLUMNS];
	my_bool error[COLUMNS];
	my_bool trunc = 0;
	char *sqlTZ, *sql;

	const char* d_host;
	const char* d_db;
	const char* d_user;
	const char* d_pwd;

	config_t cfg;
	config_setting_t *root, *setting;
	config_setting_t *database_g, *db_host, *db_db, *db_pwd, *db_user;

	printf("Content-Type: application/json\r\n");
	setenv("TZ", "Europe/London", 1);

	MYSQL *con = mysql_init(NULL);
	mysql_options(con, MYSQL_REPORT_DATA_TRUNCATION, &trunc);

	if( con == NULL )
	{
		fprintf(stderr, "Can't run Mariadb");
		exit_on_error("Really bad", NULL);
	}

	config_init(&cfg);
	if(! config_read_file(&cfg, CONFIG_FILE))
	{
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}

	root = config_root_setting(&cfg);

	database_g = config_setting_get_member(root, "Database");

	db_host = config_setting_get_member(database_g, "sqlhost");
	db_user = config_setting_get_member(database_g, "sqluser");
	db_pwd = config_setting_get_member(database_g, "sqlpwd");
	db_db = config_setting_get_member(database_g, "sqldbase");

	d_host = config_setting_get_string(db_host);
	d_user = config_setting_get_string(db_user); 
	d_pwd = config_setting_get_string(db_pwd);
	d_db = config_setting_get_string(db_db);

	if (mysql_real_connect(con, d_host, d_user, d_pwd, d_db, 0, NULL, 0) == NULL)
	{
		exit_on_error("Connect", con);
	}

	printf("X-cgi-name: %s\r\n", basename(getenv("REQUEST_URI")));	
	printf("X-mariadb-connection-id: %ld\r\n\r\n[\n" , mysql_thread_id(con));

	sqlTZ = "SET time_zone = 'Europe/London';";

	stmtTZ = mysql_stmt_init(con);

	if (!stmtTZ) 
	{
		exit_on_error("Stmt_init", con);
	}

	if (mysql_stmt_prepare(stmtTZ, sqlTZ, strlen(sqlTZ)))
	{
		exit_on_error("Stmt prepare", con);
	}

	param_count = mysql_stmt_param_count(stmtTZ);
	if (param_count != 0)
	{
		exit_on_error("Invalid param_count", con);
	}

	prepare_meta_result = mysql_stmt_result_metadata(stmtTZ);

	rc = mysql_stmt_execute(stmtTZ);
	if (rc)
	{
		exit_on_error("Stmt execute", con);
	}

	if (mysql_stmt_close(stmtTZ))
	{
		exit_on_error("Stmt close", con);
	}

	stmt = mysql_stmt_init(con);
	if (!stmt) 
	{
		exit_on_error("Stmt init", con);
	}

	if (!strcmp(basename(getenv("REQUEST_URI")), "weeklyEV"))
	{
		sql = "SELECT datetime, imported, zappidiv, zappiimp, house, exported FROM Zappi_test where datetime between date_sub(now(), interval 7 day) and now() order by datetime;";
	}
	else
	{
		sql = "SELECT datetime, imported, zappidiv, zappiimp, house, exported FROM Zappi_test where datetime between date_sub(now(), interval 1 day) and now() order by datetime;";
	}
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		exit_on_error("Stmt prepare", con);
	}

	param_count = mysql_stmt_param_count(stmt);
	if (param_count != 0)
	{
		exit_on_error("Invalid param count", con);
	}

	prepare_meta_result = mysql_stmt_result_metadata(stmt);
	if (!prepare_meta_result)
	{
		exit_on_error("Stmt result metadata", con);
	}

	rc = mysql_stmt_execute(stmt);
	if (rc)
	{
		exit_on_error("Stmt execute", con);
	}

	column_count = mysql_num_fields(prepare_meta_result);
	if (column_count != COLUMNS)
	{
		exit_on_error("Invalid column count", con);
	}

	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_DATETIME;
	bind[0].buffer = (char *)&ts;
	bind[0].is_null = &is_null[0];
	bind[0].length = &length[0];
	bind[0].error = &error[0];

	bind[1].buffer_type = MYSQL_TYPE_FLOAT;
	bind[1].buffer = (float *)&full_usage;
	bind[1].is_null = &is_null[1];
	bind[1].length = &length[1];
	bind[1].error = &error[1];

	bind[2].buffer_type = MYSQL_TYPE_FLOAT;
	bind[2].buffer = (float *)&zappiimp;
	bind[2].is_null = &is_null[2];
	bind[2].length = &length[2];
	bind[2].error = &error[2];

	bind[3].buffer_type = MYSQL_TYPE_FLOAT;
	bind[3].buffer = (float *)&zappidiv;
	bind[3].is_null = &is_null[3];
	bind[3].length = &length[3];
	bind[3].error = &error[3];

	bind[4].buffer_type = MYSQL_TYPE_FLOAT;
	bind[4].buffer = (float *)&house;
	bind[4].is_null = &is_null[4];
	bind[4].length = &length[4];
	bind[4].error = &error[4];

	bind[5].buffer_type = MYSQL_TYPE_FLOAT;
	bind[5].buffer = (float *)&exported;
	bind[5].is_null = &is_null[5];
	bind[5].length = &length[5];
	bind[5].error = &error[5];


	if (mysql_stmt_bind_result(stmt, bind))
	{
		exit_on_error("Stmt bind result", con);
	}

	rc = mysql_stmt_fetch(stmt);
	while(!rc)
	{
		charging = zappiimp + zappidiv;
		printf("%s{",comma);
		comma = ",";
		printf("\"date_time\":");
		printf("\"%04d-%02d-%02d %02d:%02d\"", ts.year, ts.month, ts.day, ts.hour, ts.minute);
		printf(",\"Full_usage\":");
		printf("%2.2f", full_usage);
		printf(",\"Charging\":");
		printf("%2.2f",charging);
		printf(",\"House\":");
		printf("%2.2f",house);
		printf(",\"Export\":");
		printf("%2.2f",exported);
		printf("}\n");
		rc = mysql_stmt_fetch(stmt);
	}

	mysql_free_result(prepare_meta_result);
	if (mysql_stmt_close(stmt))
	{
		exit_on_error("Stmt close", con);
	}

	mysql_close(con);
	mysql_library_end();

	printf("]\n");
	return 0;
}
