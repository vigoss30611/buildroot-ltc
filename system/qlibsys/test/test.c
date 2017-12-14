#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qsdk/sys.h>

int main(int argc, char *argv[])
{
	char owner[256], key[256], type[256], string[256], string_r[256];
	int d, d_r;
	double lf, lf_r;

	const char *r = setting_get_string("qiwo", "ledname", string_r, 128);
	printf("get null result is %p\n", r);

	for(; scanf("%s%s%s", owner, key, type) == 3;) {
		printf("command: %s %s %s\n",owner, key, type);
		if(!strcmp(type, "string")) {
			scanf("%s", string);
			printf("%s ", string);
			setting_set_string(owner, key, string);
		} else if(!strcmp(type, "int")) {
			scanf("%d", &d);
			printf("%d ", d);
			setting_set_int(owner, key, d);
		} else if(!strcmp(type, "double")) {
			scanf("%lf", &lf);
			printf("%lf ", lf);
			setting_set_double(owner, key, lf);
		} else if(!strcmp(type, "-")) {
			setting_remove_key(owner, key);
		} else if(!strcmp(type, "--")) {
			setting_remove_all(owner);
		} else {
			printf("unrecognized command \n");
			exit(1);
		}

		setting_get_string(owner, key, string_r, 256);
		d_r = setting_get_int(owner, key);
		lf_r = setting_get_double(owner, key);
		printf(":: %s %d %lf\n\n", string_r, d_r, lf_r);
	}

	return 0;
}

