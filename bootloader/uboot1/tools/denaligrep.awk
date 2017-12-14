{
#	print index("PORTA_TYPE", $0);
#	print $0;
	if (index($0, "PORTA_TYPE") && index($0, "=") && index($0, "DDR2")) begin = 1;
	if (index($0, "Setup IRQ handler")) begin = 0;
	if(begin)
	{
		if($1 == "ldr" && index($0, "r1") && index($0, "=0x")) denali_tmp = substr($0, index($0, "=0x") + 1, 10);
		if($1 == "str" && index($0, "r1") && index($0, "[r0]")) denali_data[COUNT++] = denali_tmp;
	}} END {
		printf("/* Auto generated denali data */\n\n");
		printf("_denali_ctl_pa_data:\n");
		printf("\t.word\t.\n");
		for (i = 0; i < COUNT; i++) printf("\t.word\t%s\t@\tDenali_CTL_PA_%02d\n", tolower(denali_data[i]), (i == 84? 9 :i));
		printf("\n");
	}
