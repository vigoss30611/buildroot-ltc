#include <stdio.h>

int sensors_get_luminous(void)
{
	FILE *fp;
	int lu = -1;

	fp = fopen(LUDEV, "r");
	fscanf(fp, "%d", &lu);
	fclose(fp);

	return lu;
}

