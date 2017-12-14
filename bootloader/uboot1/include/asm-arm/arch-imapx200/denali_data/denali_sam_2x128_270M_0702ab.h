/* MID-270M 1.0519 */

_denali_ctl_pa_data:
	.word	.
	.word	0x00000101	@	Denali_CTL_PA_00
	.word	0x01010100	@	Denali_CTL_PA_01
	.word	0x01010101	@	Denali_CTL_PA_02
	.word	0x01000101	@	Denali_CTL_PA_03
	.word	0x01000000	@	Denali_CTL_PA_04
	.word	0x00010101	@	Denali_CTL_PA_05
	.word	0x00000000	@	Denali_CTL_PA_06
	.word	0x01000001	@	Denali_CTL_PA_07
	.word	0x00000000	@	Denali_CTL_PA_08
	.word	0x00000101	@	Denali_CTL_PA_09
	.word	0x01010101	@	Denali_CTL_PA_10
	.word	0x03030000	@	Denali_CTL_PA_11
	.word	0x03030303	@	Denali_CTL_PA_12
	.word	0x02030303	@	Denali_CTL_PA_13
	.word	0x03030303	@	Denali_CTL_PA_14
	.word	0x03030303	@	Denali_CTL_PA_15
#ifdef CONFIG_DENALI_CS
	.word	CONFIG_DENALI_CS
#else
	/* Select 1 chip as default */
	.word	0x00000100	@	Denali_CTL_PA_16
#endif
	.word	0x02010200	@	Denali_CTL_PA_17
	.word	0x03020101	@	Denali_CTL_PA_18
	.word	0x00030004	@	Denali_CTL_PA_19
	.word	0x02000100	@	Denali_CTL_PA_20
	.word	0x00030202	@	Denali_CTL_PA_21
	.word	0x00000203	@	Denali_CTL_PA_22
	.word	0x00000002	@	Denali_CTL_PA_23
	.word	0x080a0f01	@	Denali_CTL_PA_24
	.word	0x0f030308	@	Denali_CTL_PA_25
	.word	0x00000204	@	Denali_CTL_PA_26
	.word	0x02000400	@	Denali_CTL_PA_27
	.word	0x02000701	@	Denali_CTL_PA_28
	.word	0x04040200	@	Denali_CTL_PA_29
	.word	0x00030302	@	Denali_CTL_PA_30
	.word	0x02000000	@	Denali_CTL_PA_31
	.word	0x04020803	@	Denali_CTL_PA_32
	.word	0x0f0e0000	@	Denali_CTL_PA_33
	.word	0x00210021	@	Denali_CTL_PA_34
	.word	0x00210021	@	Denali_CTL_PA_35
	.word	0x00210021	@	Denali_CTL_PA_36
	.word	0x02000000	@	Denali_CTL_PA_37
	.word	0x0022050b	@	Denali_CTL_PA_38
	.word	0x00000000	@	Denali_CTL_PA_39
	.word	0x00000000	@	Denali_CTL_PA_40
	.word	0x00000000	@	Denali_CTL_PA_41
	.word	0x081b081b	@	Denali_CTL_PA_42
	.word	0x081b081b	@	Denali_CTL_PA_43
	.word	0x081b081b	@	Denali_CTL_PA_44
#ifdef CONFIG_DENALI_RFRATE
	.word	CONFIG_DENALI_RFRATE
#else
	/* HCLKx2 = 210~270 */
	.word	0x06420712	@	Denali_CTL_PA_45
#endif
	.word	0x00040642	@	Denali_CTL_PA_46
	.word	0x00000004	@	Denali_CTL_PA_47
	.word	0x00000000	@	Denali_CTL_PA_48
	.word	0xffff0000	@	Denali_CTL_PA_49
	.word	0xffffffff	@	Denali_CTL_PA_50
	.word	0xffffffff	@	Denali_CTL_PA_51
	.word	0x0000ffff	@	Denali_CTL_PA_52
	.word	0x00000000	@	Denali_CTL_PA_53
	.word	0x00000000	@	Denali_CTL_PA_54
	.word	0x006b0000	@	Denali_CTL_PA_55
	.word	0x000200c8	@	Denali_CTL_PA_56
	.word	0x003536a6	@	Denali_CTL_PA_57
	.word	0x000000c8	@	Denali_CTL_PA_58
	.word	0x00000036	@	Denali_CTL_PA_59
	.word	0x00000000	@	Denali_CTL_PA_60
	.word	0x00000000	@	Denali_CTL_PA_61
	.word	0x00098e09	@	Denali_CTL_PA_62
	.word	0x00098f09	@	Denali_CTL_PA_63
	.word	0x00099009	@	Denali_CTL_PA_64
	.word	0x00098e09	@	Denali_CTL_PA_65
	.word	0x000a1411	@	Denali_CTL_PA_66
	.word	0x000a1511	@	Denali_CTL_PA_67
	.word	0x000a1611	@	Denali_CTL_PA_68
	.word	0x000a1311	@	Denali_CTL_PA_69
	.word	0x00000000	@	Denali_CTL_PA_70
	.word	0x00000000	@	Denali_CTL_PA_71
	.word	0x00000000	@	Denali_CTL_PA_72
	.word	0x00000000	@	Denali_CTL_PA_73
	.word	0x00000004	@	Denali_CTL_PA_74
	.word	0xf4013927	@	Denali_CTL_PA_75
	.word	0xf4013927	@	Denali_CTL_PA_76
	.word	0xf4013927	@	Denali_CTL_PA_77
	.word	0xf4013927	@	Denali_CTL_PA_78
	.word	0x07400300	@	Denali_CTL_PA_79
	.word	0x07400300	@	Denali_CTL_PA_80
	.word	0x07400300	@	Denali_CTL_PA_81
	.word	0x07400300	@	Denali_CTL_PA_82
	.word	0x00000005	@	Denali_CTL_PA_83
	.word	0x01000101	@	Denali_CTL_PA_09

