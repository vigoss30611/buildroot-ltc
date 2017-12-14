
#define MAX_ADC_KEYS_NUM 8

struct adc_keys_button {
	bool valid;
	unsigned int code; /* input event code (KEY_*, SW_*) */
	int chan;         /* adc channel for the key*/
	int ref_val;	  /* reference voltage value for this key*/	
	char *desc;
};

struct adc_keys_platform_data {
	struct adc_keys_button buttons[MAX_ADC_KEYS_NUM];
	int nbuttons;
};
