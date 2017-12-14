#include <string.h>
#include <stdlib.h>
#include "spv_app.h"
#include "spv_config_keys.h"
#include "spv_debug.h"
#include "spv_common.h"
#include "config_manager.h"
#include "spv_properties.h"
#include "spv_language_res.h"
#include "sys_server.h"

#include "config_camera.h"

static void *g_Handle = NULL;

const char *g_status_keys[] = {
	"status.camera.working", //On/Off
	"status.camera.locating", //On/Off
	"status.camera.battery", //(0-7)(full, high, medium, low, full-charing, high-charging, medium-charging, low-charging)
	"status.camera.capacity", //availableSpace+videoCount+photoCounth
	"status.camera.rear", //on/off
	"status.camera.recordtime",
	"video.zoom",
	"photo.zoom",
	//"current.mode",
};

const char *g_action_keys[] = {
    "action.shutter.press", //
    "action.file.delete", //last/all
    "action.time.set", //time
    "action.wifi.set" //Off/name\n+pswd
};

static int CheckConfigFile()
{
    int result = 0;
    void* originHandle = NULL;
    int ret = PropertiesInit(RESOURCE_PATH"./config/spv_gui.config.bak", &originHandle);
    if(ret || originHandle == NULL) {
        ERROR("backup config file could not be resolved\n");
        return -1;
    }

    char **keys = NULL;
    char **originKeys = NULL;
    int count = 0, originCount = 0;
    getKeys(originHandle, &originKeys, &originCount);

    if(originCount <= 0 || originKeys == NULL) {
        if(originKeys != NULL) {
            free(originKeys);
            originKeys == NULL;
        }
        ERROR("backup config is wrong\n");
        return -1;
    }

    if(g_Handle != NULL) {
        getKeys(g_Handle, &keys, &count);
        if(count <= 0 || keys == NULL || count != originCount) {
            ERROR("config file count not the same, count: %d, originCount: %d\n", count, originCount);
            releaseHandle(&g_Handle);
            g_Handle = originHandle;
            result = 1;
        } else {
            int i = 0;
            for(i = 0; i < originCount; i ++) {
                if(strcasecmp(keys[i], originKeys[i])) {
                    ERROR("Keys changed, key: %s, originkey: %s\n", keys[i], originKeys[i]);
                    releaseHandle(&g_Handle);
                    g_Handle = originHandle;
                    result = 2;
                    break;
                }
            }
        }
    } else {
        g_Handle = originHandle;
        result = 1;
    }

    if(keys != NULL) {
        free(keys);
        keys = NULL;
    }
    if(originKeys != NULL) {
        free(originKeys);
        originKeys = NULL;
    }

    if(g_Handle != originHandle) {
        releaseHandle(&originHandle);
        originHandle = NULL;
    }
    return result;
}

static void OnConfigChanged(const char *key, const char *oldValue, const char *newValue, FROM_TYPE type)
{
    char *keys[1], *values[1];
    char tmpKey[KEY_SIZE] = {0}, tmpValue[VALUE_SIZE] = {0} ;
    strcpy(tmpKey, key);
    strcpy(tmpValue, newValue);
    keys[0] = tmpKey;
    values[0] = tmpValue;
    INFO("onConfigChanged, key: %s, oldval: %s, newval: %s, from: %d\n", key, oldValue, newValue, type);
    if(type != TYPE_FROM_GUI) {
        notify_app_value_updated(keys, values, 1, type);
    }

}

int IsStatusKey(const char *key)
{
    int i = 0;
    int tmpKeyCount = sizeof(g_status_keys) / sizeof(char *);
    for(i = 0; i < tmpKeyCount; i ++) {
        if(!strcasecmp(g_status_keys[i], key))
            return 1;
    }

    return 0;
}

int InitConfigManager(char *file_path)
{
	int ret;
	ret = PropertiesInit(file_path, &g_Handle);

	if(CheckConfigFile() < 0) {
		ERROR("ERROR occured, CheckConfigFile, Goint to exit\n");
		ret = -1;
	}

	return ret;
}

int DeinitConfigManager()
{
	int ret = releaseHandle(&g_Handle);
	g_Handle = NULL;
	return ret;
}

int GetConfigValue(const char *key, char *value)
{
	if(g_Handle == NULL || key == NULL || value == NULL)
		return ERROR_NOT_INITIALIZE;
	memset(value, 0, VALUE_SIZE);

	return getValue(g_Handle, key, value);
}

int SetConfigValue(const char *key, const char *value, FROM_TYPE type)
{
    int ret, i;
    char valueExist[KEY_SIZE] = {0};

    if(g_Handle == NULL || key == NULL || value == NULL)
        return ERROR_NOT_INITIALIZE;

    ret = getValue(g_Handle, key, valueExist);
    if(ret) {
        if(IsStatusKey(key)) {
            addKey(g_Handle, key, "NA");
        } else {
            return ERROR_KEY_NOT_EXIST;
        }
    }
    
    if(!strcmp(valueExist, value)) {
        INFO("value exist\n");
        return 0;
    }

    ret = setValue(g_Handle, key, value);
    if(!ret) {
        //OnConfigChanged(key, valueExist, value, type);
        DispatchMsgToClient(MSG_CONFIG_CHANGED, (int)key, (long)value, type);
    } else {
        return ERROR_SET_FAILED;
    }
#if SYSTEM_IS_READ_ONLY
    SaveConfigToDisk();
#endif
    return 0;
}

int SaveConfigToDisk()
{
	if(g_Handle == NULL) {
		ERROR("SaveConfigToDisk failed(g_Handle is NULL).\n");
		return -1;
	}
	return saveConfig(((PROPS_HANDLE *)g_Handle)->filepath, ((PROPS_HANDLE *)g_Handle)->pHead);
}

SPV_MODE GetCurrentMode()
{
    int ret;
    char value[VALUE_SIZE] = {0};
    SPV_MODE mode = MODE_VIDEO;

    ret = GetConfigValue(GETKEY(ID_CURRENT_MODE), value);
    if(!strcasecmp("Video", value))
        mode = MODE_VIDEO;
    else if(!strcasecmp("Photo", value))
        mode = MODE_PHOTO;
    else if(!strcasecmp("Playback", value))
        mode = MODE_PLAYBACK;
    else if(!strcasecmp("Setup", value))
        mode = MODE_SETUP;

    return mode;
}

static void SetPlateNumberConfig(unsigned char *dst)
{
	if(dst == NULL) {
		ERROR("setup_platenumber = NULL\n");
		return;
	}
	char prvStr[] = {"川鄂甘赣桂贵黑沪吉冀津晋京辽鲁蒙闽宁青琼陕苏皖湘新渝豫粤云藏浙"};
	char value[VALUE_SIZE] = {0};
	//GetConfigValue(GETKEY(ID_SETUP_PLATE_NUMBER), value);
	int index = -1, i = 0;
	for(i = 0; i < sizeof(prvStr); i += 3) {
		if(!strncmp(value, prvStr + i, 3)) {
			index = i / 3;
			break;
		}
	}
	if(index == -1) {
		index = 12;
	}
	dst[0] = index + 128;
	dst[1] = 0;
	strcat(dst, value + 3);
	return;
}


int GetCameraConfig(config_camera *config)
{
	if(config == NULL) {
		ERROR("GetCameraConfig failed(argument is NULL)\n");
		return -1;
	}
    memset(config, 0, sizeof(config_camera));

	GetConfigValue(GETKEY(ID_CURRENT_MODE), config->current_mode);
	GetConfigValue("current.reverseimage", config->current_reverseimage);
	//video
	GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), config->video_resolution);
	GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), config->video_looprecording);
	if(!strcasecmp(config->video_looprecording, GETVALUE(STRING_OFF)) && IsCardvEnable()) {
		strcpy(config->video_looprecording, GETVALUE(STRING_5_MINUTES));
	}
#ifdef GUI_CAMERA_REAR_ENABLE
	GetConfigValue(GETKEY(ID_VIDEO_REAR_CAMERA), config->video_rearcamera);
	GetConfigValue(GETKEY(ID_VIDEO_PIP), config->video_pip);
#endif
	//GetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), config->video_frontbig);
	GetConfigValue(GETKEY(ID_VIDEO_WDR), config->video_wdr);
	GetConfigValue(GETKEY(ID_VIDEO_EV), config->video_ev);
	GetConfigValue(GETKEY(ID_VIDEO_RECORD_AUDIO), config->video_recordaudio);
	GetConfigValue(GETKEY(ID_VIDEO_DATE_STAMP), config->video_datestamp);
	//GetConfigValue(GETKEY(ID_VIDEO_GSENSOR), config->video_gsensor);
	//GetConfigValue(GETKEY(ID_VIDEO_PLATE_STAMP), config->video_platestamp);
	//GetConfigValue(GETKEY(ID_VIDEO_ZOOM), config->video_zoom);
	//photo
	//GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), config->photo_capturemode);
	GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), config->photo_resolution);
	//GetConfigValue(GETKEY(ID_PHOTO_SEQUENCE), config->photo_sequence);
	GetConfigValue(GETKEY(ID_PHOTO_QUALITY), config->photo_quality);
	GetConfigValue(GETKEY(ID_PHOTO_SHARPNESS), config->photo_sharpness);
	GetConfigValue(GETKEY(ID_PHOTO_WHITE_BALANCE), config->photo_whitebalance);
	GetConfigValue(GETKEY(ID_PHOTO_COLOR), config->photo_color);
	GetConfigValue(GETKEY(ID_PHOTO_ISO_LIMIT), config->photo_isolimit);
	GetConfigValue(GETKEY(ID_PHOTO_EV), config->photo_ev);
	GetConfigValue(GETKEY(ID_PHOTO_ANTI_SHAKING), config->photo_antishaking);
	GetConfigValue(GETKEY(ID_PHOTO_DATE_STAMP), config->photo_datestamp);
	//GetConfigValue(GETKEY(ID_PHOTO_ZOOM), config->photo_zoom);
	//setup
	//GetConfigValue(GETKEY(ID_SETUP_TV_MODE), config->setup_tvmode);
	//GetConfigValue(GETKEY(ID_SETUP_FREQUENCY), config->setup_frequency);
	//GetConfigValue(GETKEY(ID_SETUP_IR_LED), config->setup_irled);
	GetConfigValue(GETKEY(ID_SETUP_IMAGE_ROTATION), config->setup_imagerotation);
	strcpy(config->other_collide, "Off");
	SetPlateNumberConfig(config->setup_platenumber);

	return 0;
}









