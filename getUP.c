/* 
 Copyright © Dougie Lawson, 2020, All rights reserved 
*/

#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <string.h>
#include <getopt.h>
#include <libconfig.h>
#include <sys/stat.h>
#define CONFIG_FILE "/home/pi_d/.zappi.cfg"

#define FALSE 0
#define TRUE !(FALSE)

int main(int argc, char **argv)
{
	int c;
	int digit_optind = 0;
	static const char *output_file = CONFIG_FILE;
	mode_t mode;
	config_t cfg;
	config_setting_t *root, *setting;
	config_setting_t *zappi, *z_user, *z_pwd;
	config_setting_t *database, *d_host, *d_db, *d_pwd, *d_user;
	config_init(&cfg);
	root = config_root_setting(&cfg);
	_Bool u_OK, p_OK, h_OK, d_OK, q_OK, s_OK;

	zappi = config_setting_add(root, "Zappi", CONFIG_TYPE_GROUP);
	database = config_setting_add(root, "Database", CONFIG_TYPE_GROUP);

	while (1)
	{
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"username", required_argument, 0,  0 },
			{"password", required_argument, 0,  0 },
			{"sqluser", required_argument, 0,  0 },
			{"sqlpwd", required_argument, 0,  0 },
			{"sqlhost", required_argument, 0,  0 },
			{"sqldbase", required_argument, 0,  0 },
			{0,          0,                 0,  0 }
		};
		c = getopt_long(argc, argv, "u:p:h:d:q:s:", long_options, &option_index);
		if (c == -1) break;
		switch (c)
		{
			case 0:
				printf("option --%s", long_options[option_index].name);

				if (!strcmp(long_options[option_index].name, "username")) u_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "password")) p_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqluser")) s_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqlhost")) h_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqldbase")) d_OK = TRUE;
				else if (!strcmp(long_options[option_index].name, "sqlpwd")) q_OK = TRUE;

				if (optarg) printf(" with value %s", optarg);
				if (option_index == 0 || option_index == 1)
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
			case 'p':
				printf("option -p with value '%s'\n", optarg);
				z_pwd = config_setting_add(zappi, "password", CONFIG_TYPE_STRING);
				config_setting_set_string(z_pwd, optarg);
				p_OK = TRUE;
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
	if (u_OK == FALSE || p_OK == FALSE || h_OK == FALSE || d_OK == FALSE || q_OK == FALSE || s_OK == FALSE)
	{
		fprintf(stderr, "Missing option(s): ");
		if (u_OK == FALSE) fprintf(stderr, "Zappi userid. Use: -uXXX or --username XXX ");
		if (p_OK == FALSE) fprintf(stderr, "Zappi password. Use:  -pXXX or --password XXX ");
		if (h_OK == FALSE) fprintf(stderr, "Mariadb hostname or IP address. Use: -h192.168.3.14  or --sqlhost raspberrypi.local ");
		if (d_OK == FALSE) fprintf(stderr, "Mariadb database name. Use: -dXXX or --sqldbase XXX ");
		if (q_OK == FALSE) fprintf(stderr, "Mariadb password. Use: -qXXX or --sqlpwd XXX ");
		if (s_OK == FALSE) fprintf(stderr, "Mariadb userid. Use -sXXXX or --sqluser XXX ");
		fprintf(stderr, "\n");
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
