/* 
 Copyright © Dougie Lawson, 2020-2024, All rights reserved 
*/

#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <string.h>
#include <getopt.h>
#include <libconfig.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <pcre.h>

#define CONFIG_FILE "/home/pi_d/.zappi.cfg.new"
#define MYENERGI_URL "https://director.myenergi.net"

#define FALSE 0
#define TRUE !(FALSE)

const char* matchedString;
int offsetVector[30];

int findMy(void* testString)
{
	char* regexPattern;
	pcre* regexComp;
	pcre_extra* regExtra;
	int regexErrorCode;
	int regexErrorOffset;
	const char* regexErrorStr;
	int j;

	regexPattern = "x_myenergi-asn: (.*.myenergi.net)";
	regexComp = pcre_compile(regexPattern, PCRE_CASELESS, &regexErrorStr, &regexErrorOffset, NULL);
	if (regexComp == NULL) 
	{
		printf("Error: %s %s\n", regexPattern, regexErrorStr);
		exit(1);
	}

	regExtra = pcre_study(regexComp, 0, &regexErrorStr);
	if (regexErrorStr != NULL)
	{
		printf("Study: %s %s\n", regexPattern, regexErrorStr);
		exit(1);
	}
	regexErrorCode = pcre_exec(regexComp, regExtra, testString,
		strlen(testString), 0, 0, offsetVector, 30);
	if (regexErrorCode > 0)
	{
		pcre_get_substring(testString, offsetVector, regexErrorCode, 1, &(matchedString));
	}
	else if (regexErrorCode < 0)
	{
		switch(regexErrorCode)
		{
			case PCRE_ERROR_NOMATCH : 
				//printf("No match\n"); 
				break;
			case PCRE_ERROR_NULL : 
				printf("Null\n"); 
				break;
			case PCRE_ERROR_BADOPTION : 
				printf("Bad option\n"); 
				break;
			case PCRE_ERROR_BADMAGIC : 
				printf("Voodoo\n"); 
				break;
			case PCRE_ERROR_UNKNOWN_NODE : 
				printf("Something kooky\n"); 
				break;
			case PCRE_ERROR_NOMEMORY : 
				printf("Oom\n"); 
				break;
			default : printf("Very bad\n"); break;
		}
	}
	else
	{
		printf("No error, but no match");
	}
	pcre_free(regexComp);
	if (regExtra != NULL)
	{
		pcre_free_study(regExtra);
	}
	return 0;
}

static size_t curl_print(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written =  fwrite(ptr, size, nmemb, (FILE*)stream);
	//size_t realsize = size * nmemb;
	return written; 
}

static size_t curl_header(void* ptr, size_t size, size_t nitems, void* userdata)
{
	findMy(ptr);
	return nitems * size;
}

void getASN(config_setting_t* z_user, config_setting_t* z_cred)
{
	CURL* curl;
	CURLcode res;
	static const char *outfilenm = "/tmp/curl.out";
	const char* username;
	const char* credential;
	FILE* outfile;

	username = config_setting_get_string(z_user);
	credential = config_setting_get_string(z_cred);

	curl_global_init(CURL_GLOBAL_ALL);
 
	curl = curl_easy_init();
	if(curl) {

		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, credential);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header);
		curl_easy_setopt(curl, CURLOPT_URL, MYENERGI_URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_print);

		outfile = fopen(outfilenm, "wb");
		if (outfile) {
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);

    			res = curl_easy_perform(curl);
   		 	if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			fclose(outfile);
		}
	curl_easy_cleanup(curl);
	}
 
	curl_global_cleanup();
 
	printf("Match: (%2d, %2d): %s\n", offsetVector[2], offsetVector[3], matchedString);
}

int main(int argc, char **argv)
{
	int c;
	int digit_optind = 0;
	static const char *output_file = CONFIG_FILE;
	mode_t mode;
	config_t cfg;
	config_setting_t *root, *setting;
	config_setting_t *zappi, *z_user, *z_pwd, *z_asn, *z_serial, *z_api;
	config_setting_t *database, *d_host, *d_db, *d_pwd, *d_user;
	config_init(&cfg);
	root = config_root_setting(&cfg);
	_Bool u_OK, p_OK, h_OK, d_OK, q_OK, s_OK, z_OK, a_OK;
	u_OK = FALSE;
	p_OK = FALSE;
	h_OK = FALSE;
	d_OK = FALSE;
	q_OK = FALSE;
	s_OK = FALSE;
	z_OK = FALSE;
	a_OK = FALSE;

	zappi = config_setting_add(root, "Zappi", CONFIG_TYPE_GROUP);
	database = config_setting_add(root, "Database", CONFIG_TYPE_GROUP);

	while (1)
	{
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"username", required_argument, 0,  0 },
//			{"password", required_argument, 0,  0 },
			{"sqluser", required_argument, 0,  0 },
			{"sqlpwd", required_argument, 0,  0 },
			{"sqlhost", required_argument, 0,  0 },
			{"sqldbase", required_argument, 0,  0 },
			{"zappi", required_argument, 0,  0 },
			{"api", required_argument, 0, 0},
			{0,          0,                 0,  0 }
		};
//		c = getopt_long(argc, argv, "u:p:h:d:q:s:z:a:", long_options, &option_index);
		c = getopt_long(argc, argv, "u:h:d:q:s:z:a:", long_options, &option_index);
		if (c == -1) break;
		switch (c)
		{
			case 0:
				printf("option --%s", long_options[option_index].name);

				if (!strcmp(long_options[option_index].name, "username")) u_OK = TRUE;
//				else if (!strcmp(long_options[option_index].name, "password")) p_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqluser")) s_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqlhost")) h_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqldbase")) d_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqlpwd")) q_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "zappi")) z_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "api")) a_OK = TRUE;

				if (optarg) printf(" with value %s", optarg);
				if (option_index <= 1 || option_index >= 6)
				{
					setting = config_setting_add(zappi, long_options[option_index].name, CONFIG_TYPE_STRING);
				}
				else
				{
					setting = config_setting_add(database, long_options[option_index].name, CONFIG_TYPE_STRING);
				}
				config_setting_set_string(setting, optarg);
				printf("\n");
				break;
			case 'u':
				printf("option -u with value '%s'\n", optarg);
				z_user = config_setting_add(zappi, "username", CONFIG_TYPE_STRING);
				config_setting_set_string(z_user, optarg);
				u_OK = TRUE;
				break;
/*			case 'p':
				printf("option -p with value '%s'\n", optarg);
				z_pwd = config_setting_add(zappi, "password", CONFIG_TYPE_STRING);
				config_setting_set_string(z_pwd, optarg);
				p_OK = TRUE;
				break;
				*/
			case 'a':
				printf("option -a with value '%s'\n", optarg);
				z_api = config_setting_add(zappi, "api", CONFIG_TYPE_STRING);
				config_setting_set_string(z_api, optarg);
				a_OK = TRUE;
				break;
			case 'h':
				printf("option -h with value '%s'\n", optarg);
				d_host = config_setting_add(database, "sqlhost", CONFIG_TYPE_STRING);
				config_setting_set_string(d_host, optarg);
				h_OK = TRUE;
				break;
			case 'd':
				printf("option -d with value '%s'\n", optarg);
				d_db = config_setting_add(database, "sqldbase", CONFIG_TYPE_STRING);
				config_setting_set_string(d_db, optarg);
				d_OK = TRUE;
				break;
			case 'q':
				printf("option -q with value '%s'\n", optarg);
				d_pwd = config_setting_add(database, "sqlpwd", CONFIG_TYPE_STRING);
				config_setting_set_string(d_pwd, optarg);
				q_OK = TRUE;
				break;
			case 's':
				printf("option -s with value '%s'\n", optarg);
				d_user = config_setting_add(database, "sqluser", CONFIG_TYPE_STRING);
				config_setting_set_string(d_user, optarg);
				s_OK = TRUE;
				break;
			case 'z':
				printf("option -z with value '%s'\n", optarg);
				z_serial = config_setting_add(zappi, "zappi", CONFIG_TYPE_STRING);
				config_setting_set_string(z_serial, optarg);
				z_OK = TRUE;
				break;
			case '?':
				break;
			default:
				printf("?? getopt returned character code 0%o ??\n", c);
		}
	}
	if (optind < argc)
	{
		printf("non-option ARGV-elements: ");
		while (optind < argc)
		printf("%s ", argv[optind++]);
		printf("\n");
	}
	if (u_OK == FALSE || h_OK == FALSE || d_OK == FALSE || q_OK == FALSE || s_OK == FALSE || z_OK == FALSE || a_OK == FALSE)
	{
		fprintf(stderr, "Missing option(s): ");
		if (u_OK == FALSE) fprintf(stderr, "\nZappi userid. Use: -uXXX or --username XXX ");
		//if (p_OK == FALSE) fprintf(stderr, "\nZappi password. Use:  -pXXX or --password XXX ");
		if (h_OK == FALSE) fprintf(stderr, "\nMariadb hostname or IP address. Use: -h192.168.3.14  or --sqlhost raspberrypi.local ");
		if (d_OK == FALSE) fprintf(stderr, "\nMariadb database name. Use: -dXXX or --sqldbase XXX ");
		if (q_OK == FALSE) fprintf(stderr, "\nMariadb password. Use: -qXXX or --sqlpwd XXX ");
		if (s_OK == FALSE) fprintf(stderr, "\nMariadb userid. Use -sXXXX or --sqluser XXX ");
		if (z_OK == FALSE) fprintf(stderr, "\nZappi serial. Use -zXXXX or --zappi XXX ");
		if (a_OK == FALSE) fprintf(stderr, "\nZappi API. Use -aXXXX or --api XXX");
		fprintf(stderr, "\n");
		exit(20);
	}
/*	if (u_OK == TRUE && p_OK == TRUE)
	{
		z_user = config_setting_get_member(zappi, "username");
		z_pwd = config_setting_get_member(zappi, "password");
		getASN(z_user, z_pwd);
		z_asn = config_setting_add(zappi, "asn", CONFIG_TYPE_STRING);
		config_setting_set_string(z_asn, matchedString);
	}
	else */ if (u_OK == TRUE && a_OK == TRUE)
	{
		z_user = config_setting_get_member(zappi, "username");
		z_api = config_setting_get_member(zappi, "api");
		getASN(z_user, z_api);
		z_asn = config_setting_add(zappi, "asn", CONFIG_TYPE_STRING);
		config_setting_set_string(z_asn, matchedString);
	}
	else
	{
		fprintf(stderr, "\nMissing Myenergi credentials");
		exit(20);
	}
	if(! config_write_file(&cfg, output_file))
	{
		fprintf(stderr, "Error while writing file.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}

	fprintf(stderr, "New configuration successfully written to: %s\n", output_file);
	config_destroy(&cfg);
	mode = 0600;
	chmod(output_file, mode);
	exit(EXIT_SUCCESS);
}
