#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <libconfig.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
	int c;
	int digit_optind = 0;
	static const char *output_file = ".zappi.cfg";
	mode_t mode;
	config_t cfg;
	config_setting_t *root, *setting;
	config_setting_t *zappi, *z_user, *z_pwd;
	config_setting_t *database, *d_host, *d_db, *d_pwd, *d_user;
	config_init(&cfg);
	root = config_root_setting(&cfg);

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
				break;
			case 'p':
				printf("option -p with value '%s'\n", optarg);
				z_pwd = config_setting_add(zappi, "password", CONFIG_TYPE_STRING);
				config_setting_set_string(z_pwd, optarg);
				break;
			case 'h':
				printf("option -h with value '%s'\n", optarg);
				d_host = config_setting_add(database, "sqlhost", CONFIG_TYPE_STRING);
				config_setting_set_string(d_host, optarg);
				break;
			case 'd':
				printf("option -d  with value '%s'\n", optarg);
				d_db = config_setting_add(database, "sqldbase", CONFIG_TYPE_STRING);
				config_setting_set_string(d_db, optarg);
				break;
			case 'q':
				printf("option -q with value '%s'\n", optarg);
				d_pwd = config_setting_add(database, "sqlpwd", CONFIG_TYPE_STRING);
				config_setting_set_string(d_pwd, optarg);
				break;
			case 's':
				printf("option -s with value '%s'\n", optarg);
				d_user = config_setting_add(database, "sqluser", CONFIG_TYPE_STRING);
				config_setting_set_string(d_user, optarg);
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
