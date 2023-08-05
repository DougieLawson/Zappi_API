/* 
 Copyright © Dougie Lawson, 2020-2023, All rights reserved 
*/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>     
#include <curl/curl.h>
#include "parseJ.h"
#define CONFIG_FILE "/home/pi_d/.zappi.cfg"

char nameBuff1[24];
char nameBuff2[24];
FILE* outfile1;
FILE* outfile2;

const char* d_host;
const char* d_db;
const char* d_user;
const char* d_pwd;

const char* username;
const char* password;
const char* asn;

time_t t;
struct tm tm;
enum URLtype{curlJstatus, curlJday};

json_object* statusz;
int no_delay = FALSE;

char* makeURL (enum URLtype urlT)
{
	char* url = malloc(65);
	char* s;
	time_t rawtime;
	struct tm* ctm;
	int dur;
	time(&rawtime);
	ctm = gmtime(&rawtime);
	if (no_delay == FALSE)
	{
		if (ctm->tm_hour > 2)
		{
			ctm->tm_hour -= 2;
		}
		dur = (24 - ctm->tm_hour) * 60;
	}
	else {
		ctm->tm_hour = 0;
		ctm->tm_min = 0;
		dur = 1440;
	}
	switch(urlT)
	{
		case curlJstatus:
	  		sprintf(url, "https://%s/cgi-jstatus-*", asn);
			break;
		case curlJday:
	  		sprintf(url, "https://%s/cgi-jday-Z%s-%d-%d-%d-%d-%d-%d", asn, serno, tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday, ctm->tm_hour, ctm->tm_min, dur);
			break;
	}
	printf("%s\n", url);
	return url;
}

static size_t curl_print(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written =  fwrite(ptr, size, nmemb, (FILE*)stream);
	return written; 
}

void curl2()
{
	CURL* curl;
	CURLcode res;
	strncpy(nameBuff2, "/tmp/parseZFil2-XXXXXX",23);
	int tempnam2 = mkstemp(nameBuff2);
	outfile2 = fopen(nameBuff2, "wb");

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
		curl_easy_setopt(curl, CURLOPT_URL, makeURL(curlJday));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_print);

		if (outfile2)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile2);

    			res = curl_easy_perform(curl);
   		 	if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			fclose(outfile2);
			statusz = json_object_from_file(nameBuff2);
			json_parse(statusz);
		}
	curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
	unlink(nameBuff2);
}

int main(int argc, char **argv)
{
	int c;
	char *s;
	t = time(NULL);
	s = (char*)TRUE;
	tm = *gmtime(&t);
 	while (1)
	{
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"iso", required_argument, 0,  0 },
			{"eur", required_argument, 0,  0 },
			{"usa", required_argument, 0,  0 },
			{"ymd", required_argument, 0,  0 },
			{"dmy", required_argument, 0,  0 },
			{"mdy", required_argument, 0,  0 },
			{"nodelay", no_argument, 0, 0 },
			{0,          0,                 0,  0 }
		};
		c = getopt_long(argc, argv, "i:e:a:n", long_options, &option_index);
		if (c == -1) break;
		switch (c)
		{
			case 0:
				if (option_index == 0 || option_index == 3)
				{
					s = strptime(optarg, "%Y/%m/%d", &tm);
				}
				else if (option_index == 1 || option_index == 4)
				{
					s = strptime(optarg, "%d/%m/%Y", &tm);
				}
				else if (option_index == 2 || option_index == 5)
				{
					s = strptime(optarg, "%m/%d/%Y", &tm);
				}
				else if (option_index == 6)
				{
					no_delay = TRUE;
				}
				else
				{
					printf("idx: %d opt: %s %s ",option_index, long_options[option_index].name, optarg);
					s = NULL;
				}
				printf("\n");
				break;
			case 'i':
				s = strptime(optarg, "%Y/%m/%d", &tm);
				break;
			case 'e':
				s = strptime(optarg, "%d/%m/%Y", &tm);
				break;
			case 'a':
				s = strptime(optarg, "%m/%d/%Y", &tm);
				break;
			case 'n':
				no_delay = TRUE;
				break;
			default:
				break;
		}

		if (s == NULL) {
			printf("Cannot parse date\n"); 
			return 1;
		}
	}

	config_t cfg;
	config_setting_t *root, *setting;
	config_setting_t *zappi_g, *z_user, *z_pwd, *z_asn;
	config_setting_t *database_g, *db_host, *db_db, *db_pwd, *db_user;

	CURL* curl;
	CURLcode res;

	memset(nameBuff1,0,sizeof(nameBuff1));
	strncpy(nameBuff1, "/tmp/parseZFil1-XXXXXX",23);
	int tempname1 = mkstemp(nameBuff1);
	outfile1 = fopen(nameBuff1, "wb");
 
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
 
	config_init(&cfg);
	if(! config_read_file(&cfg, CONFIG_FILE))
	{
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}

	root = config_root_setting(&cfg);

	zappi_g = config_setting_get_member(root, "Zappi");

	z_user = config_setting_get_member(zappi_g, "username");
	username = config_setting_get_string(z_user);
	z_pwd = config_setting_get_member(zappi_g, "password");
	password = config_setting_get_string(z_pwd);
	z_asn = config_setting_get_member(zappi_g, "asn");
	asn = config_setting_get_string(z_asn);

	database_g = config_setting_get_member(root, "Database");

	db_host = config_setting_get_member(database_g, "sqlhost");
	d_host = config_setting_get_string(db_host);
	db_db = config_setting_get_member(database_g, "sqldbase");
	d_db = config_setting_get_string(db_db);
	db_user = config_setting_get_member(database_g, "sqluser");
	d_user = config_setting_get_string(db_user); 
	db_pwd = config_setting_get_member(database_g, "sqlpwd");
	d_pwd = config_setting_get_string(db_pwd);

	sql_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
		curl_easy_setopt(curl, CURLOPT_URL, makeURL(curlJstatus));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_print);

		if (outfile1)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile1);

    			res = curl_easy_perform(curl);
   		 	if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

			fclose(outfile1);

			// this is an ugly hack
			if (no_delay == FALSE) 
			{
				sleep(150); // give the server 2.5 minutes so we get the full hour
			}

			statusz = json_object_from_file(nameBuff1);
			json_parse(statusz);
		}
	}
	unlink(nameBuff1);

	config_destroy(&cfg);
	sql_terminate(con);
	return 0;
}
