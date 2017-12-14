#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include "sensors/sensor_name.h"

#define LOG_TAG "BRIDGE_DEV"
#include <felixcommon/userlog.h>

#include "sensors/bridge/list.h"
#include "sensors/bridge/bridge_dev.h"

#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"

#ifdef BRIDGE_SX2_DVP
#include "sensors/bridge/sx2_bridge.h"
#endif

#ifdef BRIDGE_XC9080_MIPI
#include "sensors/bridge/xc9080_bridge.h"
#endif

#ifdef BRIDGE_OV683_MIPI
#include "sensors/bridge/ov683_bridge.h"
#endif

#ifdef BRIDGE_STK3855_DVP
#include "sensors/bridge/stk3855_bridge.h"
#endif

#define BRIDGE0_DEV_NODE ("/dev/bridge0")
#define DEV_PATH ("/dev/ddk_sensor")
#define EXTRA_CFG_PATH  ("/root/.ispddk/")

#define SENSOR_CFG_MAX_SZ 16

static int bridge_fd = -1;
static struct _bridge_struct* bridge_ins = NULL;
static struct _bridge_dev dev_list;

BridgeDevInitFunc bridge_dev_init[] = {
#if defined(BRIDGE_SX2_DVP)
    sx2_dvp_register,
#endif
#if defined(BRIDGE_XC9080_MIPI)
    xc9080_mipi_register,
#endif
#if defined(BRIDGE_OV683_MIPI)
    ov683_mipi_register,
#endif
#if defined(BRIDGE_STK3855_DVP)
    stk3855_dvp_register,
#endif
};

void bridge_dev_register(BRIDGE_DEV *dev)
{
    list_add_tail(dev, &dev_list);
}

BRIDGE_DEV* bridge_dev_get_instance(const char* name)
{
    //LOG_DEBUG("%s\n", __func__);

    int i = 0;
    int n = sizeof(bridge_dev_init)/sizeof(bridge_dev_init[0]);
    BRIDGE_DEV *dev = NULL;

    list_init(dev_list);
    for (i = 0; i < n; i++)
        bridge_dev_init[i]();

    list_for_each(dev, dev_list)
        if (0 == strncmp(dev->name, name, 64))
            return dev;

    return NULL;
}

int bridge_dev_open(void)
{
    //LOG_DEBUG("%s\n", __func__);

    bridge_fd = open(BRIDGE0_DEV_NODE, O_RDWR);
    if (bridge_fd < 0)
    {
        LOG_ERROR("open bridge devices %s filed\n", BRIDGE0_DEV_NODE);
        return -1;
    }
    return 0;
}

void bridge_dev_close(void)
{
    close(bridge_fd);
}

int bridge_dev_get_cfg(void* arg)
{
    //LOG_DEBUG("%s\n", __func__);

    int ret  =-1;

    ret = ioctl(bridge_fd, BRIDGE_G_CFG, (unsigned int)arg);
    if (ret < 0)
        return -1;
    return 0;
}

int bridge_dev_set_cfg(void* arg)
{
   // LOG_DEBUG("%s\n", __func__);

    int ret  =-1;

    ret = ioctl(bridge_fd, BRIDGE_S_CFG, (unsigned int)arg);
    if (ret < 0)
        return -1;
    return 0;
}

int bridge_dev_get_info(struct bridge_uinfo* info)
{
    //LOG_DEBUG("%s\n", __func__);

    int ret  =-1;

    ret = ioctl(bridge_fd, BRIDGE_G_INFO, info);
    if (ret < 0)
        return -1;
    return 0;
}

int bridge_dev_get_name(char *name)
{
    int ret = 0;
    struct bridge_uinfo info;
    memset(&info, 0, sizeof(info));

    ret = bridge_dev_get_info(& info);
    if (ret < 0)
        return -1;

    strncpy(name, info.name, MAX_STR_LEN);
    return 0;
}

int parse_config_file(const char* filename,
        SENSOR_CONFIG* sensor_cfg, BRIDGE_I2C_CLIENT* client, int i2c_forbid)
{
    //LOG_DEBUG("%s\n", __func__);

    int ret=0;
    FILE * fp = NULL;
    IMG_UINT16 val[2] = {0};
    char ln[256] = {0};
    char *pstr = NULL;
    char *pt = NULL; //for macro delete_space used

    fp = fopen(filename, "r");
    if (NULL == fp)
    {
        LOG_ERROR("open %s failed\n", filename);
        return -1;
    }

    //printf("use configuration file configure sensor!\n");
    memset(ln, 0, sizeof(ln));
    while(fgets(ln, sizeof(ln), fp) != NULL)
    {
        pt = ln;
        delete_space(pt);

        pstr = strtok(ln, ",");
        if (pstr == NULL)
            continue;

        if (0 == strncmp(pstr, "0x", 2))
        {
            val[0] = strtoul(pstr, NULL, 0);
            pstr = strtok(NULL, ",");
            if(0 == strncmp(pstr, "0x", 2))
            {
                val[1] = strtoul(pstr, NULL, 0);
            }

            //printf("val[0] = 0x%x, val[1]=0x%04x,\n", val[0], val[1]);
            if (!i2c_forbid)
            {
                ret = bridge_i2c_write(client, val[0], val[1]);
            }
            else
                return 0;

            if(ret != IMG_SUCCESS)
            {
                LOG_ERROR("i2c write val[0] = 0x%x, val[1]=0x%04x failed \n", val[0], val[1]);
                return ret;
            }
        }
        else if (0 == strncmp(pstr, "w", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->width = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "h", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->height= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "tw", SENSOR_CFG_MAX_SZ)) /*HorizontalTotal*/
        {
            sensor_cfg->widthTotal= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(ln, "th", SENSOR_CFG_MAX_SZ)) /*VerticalTotal*/
        {
            sensor_cfg->heightTotal= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "fps", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->fps = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "pclk", SENSOR_CFG_MAX_SZ))
        {
            //sensor_cfg->pclk = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            sensor_cfg->pclk = strtod((pstr = strtok(NULL, ",")), NULL);
        }
        else if (0 == strncmp(pstr, "bits", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->bits = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "i2c_bits", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->i2c_data_bits = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            client->i2c_data_bits = sensor_cfg->i2c_data_bits;
        }
        else if (0 == strncmp(pstr, "i2c_mode", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->i2c_mode = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            client->mode = sensor_cfg->i2c_mode;
        }
        else if (0 == strncmp(pstr, "i2c_forbid", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->i2c_forbid = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
        else if (0 == strncmp(pstr, "i2c_addr_bits", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->i2c_addr_bits = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            client->i2c_addr_bits = sensor_cfg->i2c_addr_bits;
        }
        else if (0 == strncmp(pstr, "hsync", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->hsync = strtoul((pstr = strtok(NULL, ",")), NULL, 0);

            if (sensor_cfg->hsync !=0)
                sensor_cfg->hsync = IMG_TRUE;
            else
                sensor_cfg->hsync = IMG_FALSE;
        }
        else if (0 == strncmp(pstr, "vsync", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->vsync = strtoul((pstr = strtok(NULL, ",")), NULL, 0);

            if (sensor_cfg->vsync !=0)
                sensor_cfg->vsync = IMG_TRUE;
            else
                sensor_cfg->vsync = IMG_FALSE;
        }
        else if (0 == strncmp(pstr, "sensor", SENSOR_CFG_MAX_SZ))
        {
            pstr = strtok(NULL, ",");
            strncpy(sensor_cfg->sensor_name, pstr, sizeof(sensor_cfg->sensor_name));
        }
        else if (0 == strncmp(pstr, "mosaic", SENSOR_CFG_MAX_SZ))
        {
            pstr = strtok(NULL, ",");
            strncpy(sensor_cfg->mosaic_type, pstr, sizeof(sensor_cfg->mosaic_type));
        }
        else if (0 == strncmp(pstr, "parallel", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->parallel= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            if (sensor_cfg->parallel !=0)
                sensor_cfg->parallel = IMG_TRUE;
            else
                sensor_cfg->parallel = IMG_FALSE;
        }
        else if (0 == strncmp(pstr, "mipi_lanes", SENSOR_CFG_MAX_SZ))
        {
            sensor_cfg->mipi_lanes = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
        }
    }

    return ret;
}

int parse_bridge_config_head(BRIDGE_STRUCT* dev)
{
    //LOG_ERROR("%s\n", __func__);

    int index = 0;
    //TODO , will get config file from dir;

    memset((void *)dev->bridge_cfg.config_file, 0, sizeof(dev->bridge_cfg.config_file));
    sprintf(dev->bridge_cfg.config_file,
        "%s%s%d-config.txt", EXTRA_CFG_PATH,
        "sensor" , index);

    //LOG_INFO("%s %s\n", __func__, dev->bridge_cfg.config_file);
    return parse_config_file(dev->bridge_cfg.config_file,
        &dev->bridge_cfg, &dev->client, 1);
}

int parse_sensor_config_file(BRIDGE_STRUCT* dev)
{
    //LOG_ERROR("%s\n", __func__);

    int index = 0;
    //TODO , will get config file from dir;

    memset((void *)dev->sensor_cfg.config_file, 0, sizeof(dev->sensor_cfg.config_file));
    sprintf(dev->sensor_cfg.config_file,
        "%s%s%d-bridge-%s%d-config.txt", EXTRA_CFG_PATH,
        "sensor" , index, "sensor" , index);

    //LOG_INFO("%s %s exist=%d\n", __func__, dev->sensor_cfg.config_file, dev->sensor_client.exist);
    return parse_config_file(dev->sensor_cfg.config_file,
        &(dev->sensor_cfg), &(dev->sensor_client), dev->sensor_cfg.i2c_forbid);
}

int parse_bridge_config_file(BRIDGE_STRUCT* dev)
{
    //LOG_ERROR("%s\n", __func__);

    int index = 0;
    //TODO , will get config file from dir;

    memset((void *)dev->bridge_cfg.config_file, 0, sizeof(dev->bridge_cfg.config_file));
    sprintf(dev->bridge_cfg.config_file,
        "%s%s%d-config.txt", EXTRA_CFG_PATH,
        "sensor" , index);

    //LOG_INFO("%s %s\n", __func__, dev->bridge_cfg.config_file);
    return parse_config_file(dev->bridge_cfg.config_file,
        &dev->bridge_cfg, &dev->client, dev->bridge_cfg.i2c_forbid);
}

int bridge_i2c_client_init(BRIDGE_STRUCT* dev)
{
    char i2c_dev_path[255];
    char adaptor[64] = {0};

    BRIDGE_I2C_CLIENT* client = &dev->client;
    BRIDGE_I2C_CLIENT* sensor_client = &dev->sensor_client;

    client->i2c_addr = dev->info.i2c_addr;
    client->chn = dev->info.chn;
    client->exist = dev->info.i2c_exist;

    if (client->exist)
    {
        sprintf(adaptor, "%s-%d", "i2c", client->chn);
        // open i2c-3 devices
        if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), client->i2c_addr, adaptor))
        {
            LOG_ERROR("Failed to find I2C device!\n");
            return -1;
        }
        //LOG_DEBUG("%s %s %s\n", __func__, adaptor, i2c_dev_path);
        client->i2c = open(i2c_dev_path, O_RDWR);
        if (client->i2c < 0)
        {
            LOG_ERROR("Failed to open I2C0 device: \"%s\", err = %d\n", i2c_dev_path, client->i2c);
            return -1;
        }
    }
    else
    {
        client->i2c = -1;
    }

    sensor_client->i2c_addr = dev->info.bsinfo[0].i2c_addr;
    sensor_client->chn = dev->info.bsinfo[0].chn;
    sensor_client->exist = dev->info.bsinfo[0].exist;
    //LOG_DEBUG("%s i2c_addr=%d chn=%d exist=%d\n", __func__,
        //sensor_client->i2c_addr, sensor_client->chn, sensor_client->exist);

    if (sensor_client->exist)
    {
        sprintf(adaptor, "%s-%d", "i2c", sensor_client->chn);
        // open i2c-4 devices
        if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), sensor_client->i2c_addr, adaptor))
        {
            LOG_ERROR("Failed to find I2C device!\n");
            return -1;
        }
        //LOG_DEBUG("%s %s %s\n", __func__, adaptor, i2c_dev_path);
        sensor_client->i2c = open(i2c_dev_path, O_RDWR);
        if (sensor_client->i2c < 0)
        {
            LOG_ERROR("Failed to open I2C1 device: \"%s\", err = %d\n", i2c_dev_path,
                sensor_client->i2c);
            return -1;
        }
    }
    else
    {
        sensor_client->i2c = -1;
    }

    return 0;
}

BRIDGE_STRUCT* Bridge_Create(void)
{
    int ret = 0;
    bridge_ins = (struct _bridge_struct *)malloc(sizeof(struct _bridge_struct));
    if (!bridge_ins)
        goto err;

    memset(bridge_ins, 0 , sizeof(struct _bridge_struct));

    ret = bridge_dev_open();
    if (ret  < 0)
        goto err1;

    ret = bridge_dev_get_info(&(bridge_ins->info));
    if (ret  < 0)
        goto err2;

    ret = bridge_i2c_client_init(bridge_ins);
    if (ret < 0)
        goto err2;

    return bridge_ins;

err2:
    bridge_dev_close();
err1:
    free(bridge_ins);
err:
    return NULL;
}

int Bridge_DoSetup(void)
{
    //LOG_DEBUG("%s\n", __func__);
    int ret;
    unsigned int mode = 1;
    BRIDGE_DEV* dev = NULL;

    if (!bridge_ins)
        return -1;

    bridge_ins->dev =
        bridge_dev_get_instance(bridge_ins->info.name);

    bridge_ins->sensor =
        bridge_sensor_get_instance(bridge_ins->info.bsinfo[0].name);

    //LOG_INFO("bridge sensor name=%s sensor=%p\n",
        //bridge_ins->info.bsinfo[0].name, bridge_ins->sensor);

    ret = parse_bridge_config_head(bridge_ins);
    if (ret < 0)
        goto err;

    dev = bridge_ins->dev;
    if (dev && dev->init)
    {
        ret = dev->init(bridge_ins);
        if (ret < 0)
            goto err;
    }

    if (dev && dev->get_hw_cfg)
    {
        ret = dev->get_hw_cfg(bridge_ins, mode);
        if (ret < 0)
            goto err;
    }

    /*Note: At bootup bridge don't call hw config */
    if (dev && dev->set_hw_cfg)
    {
        ret = dev->set_hw_cfg(bridge_ins, mode);
        if (ret < 0)
            goto err;
    }

    ret = parse_bridge_config_file(bridge_ins);
    if (ret < 0)
        goto err;

    if (dev && dev->i2c_switch)
    {
        ret = dev->i2c_switch(bridge_ins, I2C_CHN_SWITCH_TO_SENSOR);
        if (ret < 0)
            goto err;
    }

    ret = parse_sensor_config_file(bridge_ins);
    if (ret < 0)
        goto err;

    if (!bridge_ins->sensor)
    {
        bridge_ins->sensor =
            bridge_sensor_get_instance(bridge_ins->sensor_cfg.sensor_name);
    }

    /*Note: it must behind parse_sensor_config, it get sensor info to config  bridge info */
    if (dev && dev->set_cfg)
    {
        ret = dev->set_cfg(bridge_ins, mode);
        if (ret < 0)
            goto err;
    }

    return 0;
err:
    return -1;
}

int Bridge_Enable(int enable)
{
    LOG_DEBUG("%s\n", __func__);
    int ret;
    BRIDGE_DEV* dev;

    dev = bridge_ins->dev;
    if (dev && dev->set_hw_cfg)
    {
        ret = dev->set_hw_cfg(bridge_ins, enable);
        if (ret < 0)
            goto err;
    }

    return 0;
err:
    return -1;
}

void Bridge_Destory(void)
{
    LOG_DEBUG("%s\n", __func__);
    int ret;
    BRIDGE_DEV* dev;

    dev = bridge_ins->dev;

    if (dev && dev->i2c_switch)
    {
        ret = dev->i2c_switch(bridge_ins, I2C_CHN_SWITCH_TO_BRIDGE);
        if (ret < 0)
            LOG_ERROR("bridge_dev:%s i2c switch\n", dev->name);
    }

    if (dev && dev->exit)
    {
        ret = dev->exit(bridge_ins);
        if (ret < 0)
            LOG_ERROR("bridge_dev:%s exit failed\n", dev->name);
    }

    bridge_dev_close();

    if (bridge_ins)
        free(bridge_ins);
}
