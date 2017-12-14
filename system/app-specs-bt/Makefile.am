AUTOMAKE_OPTIONS=foreign
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread \
				$(EVENT_CFLAGS)

##get sysserver
bin_PROGRAMS=specsbt
specsbt_SOURCES = \
		  app_manager.c \
		  app_manager_main.c \
		  app_specs_main.c \
		  app_disc.c \
		  app_utils.c \
		  app_xml_param.c \
		  app_xml_utils.c \
		  app_services.c \
		  app_mgt.c \
		  app_ble_main.c \
		  app_ble.c \
		  app_ble_client.c \
		  app_ble_server.c \
		  app_ble_client_xml.c \
		  app_ble_client_db.c \
		  app_dm.c \
		  app_tm_vse.c \
		  app_opc.c \
		  app_opc_main.c \
		  app_ops.c \
		  app_ops_main.c \
		  nanoxml.c


specsbt_CFLAGS= $(EXTRA_CFLAGS) -DAPP_TRACE_COLOR
specsbt_CFLAGS += -DBSA_DM_INCLUDED -DBSA_SEC_INCLUDED -DBSA_DISC_INCLUDED -DBSA_TM_INCLUDED\
				-DBSA_AV_INCLUDED -DBSA_AVK_INCLUDED -DBSA_AG_INCLUDED -DBSA_HS_INCLUDED \
				-DBLE_INCLUDED -DBTA_GATT_INCLUDED -DBSA_AVK_AV_AUDIO_RELAY \
				-DSBC_FOR_EMBEDDED_LINUX -DBLE_AUDIO_INCLUDED -DBT_USE_TRACES \
				-DBT_USE_TRACES_TIMESTAMP -DBRCM_GKI_INCLUDED -DBRCM_DRV_INCLUDED \
				-DBSA_AV_UIPC_BUFFER_SIZE=20001 -DBNEP_MAX_PROT_FILTERS=10 \
				-DBNEP_MAX_MULTI_FILTERS=10 -DCBACK_TASK=0 -DNSA_CBACK_TASK=1 \
				-DAPP_TASK=2 -DGKI_MAX_TASKS=5 -DGKI_NUM_FIXED_BUF_POOLS=4 \
				-DGKI_BUF3_SIZE=10240 -DHID_DEV_MTU_SIZE=800 \
				-DPTHREAD_CONDATTR_SETCLOCK_UNDEFINED -DCLOCK_NANOSLEEP_UNDEFINED \
				-D_FORTIFY_SOURCE=2

specsbt_LDFLAGS= -version-info 1:0:0 \
				$(LIBSYSTEM_LIBS) \
				$(EVENT_LIBS) \
				-lrt

specsbt_LDADD = lib/libbsa.a

noinst_PROGRAMS=start_bsa_server
start_bsa_server_SOURCES = start_bsa_server.c ${libwifi_la_SOURCES}
start_bsa_server_CFLAGS= $(EXTRA_CFLAGS) -DAPP_TRACE_COLOR
start_bsa_server_LDFLAGS= -version-info 1:0:0 \
				$(LIBSYSTEM_LIBS) \
				$(EVENT_LIBS) \
				-lrt
