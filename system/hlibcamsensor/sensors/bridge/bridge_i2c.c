#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "sensors/bridge/bridge_i2c.h"

#if !file_i2c
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#endif

#include <sys/ioctl.h>
#ifdef INFOTM_ISP
#include <linux/i2c.h>
#endif

#define LOG_TAG "BRIDGE_I2C"
#include <felixcommon/userlog.h>

IMG_RESULT bridge_i2c_read8(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16* data)
{
#ifdef INFOTM_ISP
    uint8_t buf[2];
    uint8_t count=0;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    if(client->i2c_addr_bits == 8)
    {
        buf[count++] = (uint8_t)(offset & 0xff);
    }
    else
    {
        buf[count++] = (uint8_t)((offset >> 8) & 0xff);
        buf[count++] = (uint8_t)(offset & 0xff);
    }

    messages[0].addr  = client->i2c_addr;
    messages[0].flags = 0;
    messages[0].len   = count;//sizeof(buf);
    messages[0].buf   = buf;

    messages[1].addr  = client->i2c_addr;
    messages[1].flags = I2C_M_RD;
    messages[1].len   = 1;
    messages[1].buf   = &buf[0];

    packets.msgs      = messages;
    packets.nmsgs     = 2;

    if(ioctl(client->i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to read data: 0x%04X.\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = buf[0];

    //printf("Reading 0x%04X = 0x%04X\n", offset, *data);
    //usleep(2);

#else
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];

    IMG_ASSERT(data);  // null pointer forbidden

    if (ioctl(client->i2c, I2C_SLAVE_FORCE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave read address!\n", offset);
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    if (sizeof(buff) > 2)
    {
        buff[2] = 0;
        buff[3] = 0;
    }

    LOG_DEBUG("Reading 0x%04x\n", offset);

    // write to set the address
    ret = write(client->i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
    }

    // read to get the data
    ret = read(client->i2c, buff[0], 1);
    if (1 != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = buff[0];

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif  /* file_i2c */
#endif //INFOTM_ISP
    return IMG_SUCCESS;

}

IMG_RESULT bridge_i2c_read16(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16* data)
{
#ifdef INFOTM_ISP
    uint8_t buf[2];

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    buf[0] = (uint8_t)((offset >> 8) & 0xff);
    buf[1] = (uint8_t)(offset & 0xff);

    messages[0].addr  = client->i2c_addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(buf);
    messages[0].buf   = buf;

    messages[1].addr  = client->i2c_addr;
    messages[1].flags = I2C_M_RD;
    messages[1].len   = sizeof(buf);
    messages[1].buf   = buf;

    packets.msgs      = messages;
    packets.nmsgs     = 2;

    if(ioctl(client->i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to read data: 0x%04X.\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = (buf[0] << 8) + buf[1];
    //printf("Reading 0x%04X = 0x%04X\n", offset, *data);
    //usleep(2);

#else
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];

    IMG_ASSERT(data);  // null pointer forbidden

    if (ioctl(client->i2c, I2C_SLAVE_FORCE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave read address!\n", offset);
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    if (sizeof(buff) > 2)
    {
        buff[2] = 0;
        buff[3] = 0;
    }

    //LOG_DEBUG("Reading 0x%04x\n", offset);

    // write to set the address
    ret = write(client->i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
    }

    // read to get the data
    ret = read(client->i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = (buff[0] << 8) | buff[1];

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif  /* file_i2c */
#endif //INFOTM_ISP
    return IMG_SUCCESS;

}

IMG_RESULT bridge_i2c_write8(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
#ifdef INFOTM_ISP
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];
    uint8_t count=0;
    uint8_t buf[3];

    if(client->i2c_addr_bits == 8)
    {
        buf[count++] = (uint8_t)(offset & 0xff);
        buf[count++] = (uint8_t)(data & 0xff);
    }
    else
    {
        buf[count++] = (uint8_t)((offset >> 8) & 0xff);
        buf[count++] = (uint8_t)(offset & 0xff);
        buf[count++] = (uint8_t)(data & 0xff);
    }

    messages[0].addr  = client->i2c_addr;
    messages[0].flags = 0;
    messages[0].len = count;//sizeof(buf);
    messages[0].buf   = buf;

    packets.msgs  = messages;
    packets.nmsgs = 1;

    //printf("[8] Writing 0x%04X, 0x%04X\n", offset, data);

   if(ioctl(client->i2c, I2C_RDWR, &packets) < 0)
   {
       LOG_ERROR("Unable to write data: 0x%04x 0x%02x.\n", offset, data);
       return IMG_ERROR_FATAL;
   }

    //TODO, I thank that we should remove usleep
    //usleep(2);

#else
#if !file_i2c
    IMG_UINT8 buff[3];

    /* Set I2C slave address */
    if (ioctl(client->i2c, I2C_SLAVE_FORCE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave write address!\n");
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    buff[2] = data & 0xFF;

    LOG_ERROR("[%d] Writing 0x%04x, 0x%04X\n", client->i2c, offset, data);
    write(client->i2c, buff, 4);
#endif
#endif //INFOTM_ISP
    return IMG_SUCCESS;
}

IMG_RESULT bridge_i2c_write16(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
#ifdef INFOTM_ISP
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    uint8_t buf[4];
    buf[0] = (uint8_t)((offset >> 8) & 0xff);
    buf[1] = (uint8_t)(offset & 0xff);
    buf[2] = (uint8_t)((data >> 8) & 0xff);
    buf[3] = (uint8_t)(data & 0xff);

    messages[0].addr  = client->i2c_addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(buf);
    messages[0].buf   = buf;

    packets.msgs  = messages;
    packets.nmsgs = 1;

    //printf("[16] Writing 0x%04X, 0x%04X\n", offset, data);

    if(ioctl(client->i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to write data: 0x%04x 0x%04x.\n", offset, data);
        return IMG_ERROR_FATAL;
    }

    //usleep(2);

#else
#if !file_i2c
    IMG_UINT8 buff[4];

    /* Set I2C slave address */
    if (ioctl(client->i2c, I2C_SLAVE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave write address!\n");
        return IMG_ERROR_BUSY;
    }

    /*write(client->i2c, buf, len * sizeof(*data));*/
    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    buff[2] = data >> 8;
    buff[3] = data & 0xFF;

    LOG_DEBUG("Writing 0x%04x, 0x%04X\n", offset, data);
    write(client->i2c, buff, 4);
#endif
#endif //INFOTM_ISP
    return IMG_SUCCESS;
}

IMG_RESULT __bridge_i2c_write(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
    if (client->i2c_data_bits == 8)
        return bridge_i2c_write8(client, offset, data);
    else if (client->i2c_data_bits == 16)
        return bridge_i2c_write16(client, offset, data);
    else
    {
        LOG_ERROR("i2c data bits is no valid\n", client->i2c_data_bits);
        return IMG_ERROR_FATAL;
    }
}

IMG_RESULT bridge_i2c_read(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16* data)
{
    if (!client->exist)
        return 0;

    if (client->i2c_data_bits == 8)
        return bridge_i2c_read8(client, offset, data);
    else if (client->i2c_data_bits == 16)
        return bridge_i2c_read16(client, offset, data);
    else
    {
        LOG_ERROR("i2c data bits is no valid\n", client->i2c_data_bits);
        return IMG_ERROR_FATAL;
    }
}

IMG_RESULT bridge_i2c_sync(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
    IMG_RESULT ret;
    // write-sync mode enable
   unsigned long i2c_sync_mode = 1;

    ioctl(client->i2c, I2C_FUNCS, &i2c_sync_mode);
    usleep(1);
    ret = __bridge_i2c_write(client, offset, data);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to write I2C0 slave \n");
    }

    usleep(1);
    i2c_sync_mode = 0;
    ioctl(client->i2c, I2C_FUNCS, &i2c_sync_mode);

    return ret;
}

IMG_RESULT bridge_i2c_write(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
    if (!client->exist)
    {
        LOG_ERROR("%s exist=%d\n", __func__, client->exist);
        return IMG_SUCCESS;
    }

    if (client->mode == 0)
    {
        LOG_DEBUG("__bridge_i2c_write addr = 0x%x offset= 0x%x data =0x%x\n",
            client->i2c_addr, offset, data);
        return __bridge_i2c_write(client, offset, data);
    }
    else if (client->mode == 1)
    {
        /*apollo3 i2c sync mode*/
        LOG_DEBUG("%s->bridge_i2c_sync\n", __func__);
        return bridge_i2c_sync(client, offset, data);
    }
    else
        return IMG_ERROR_FATAL;
}

IMG_RESULT bridge_i2c_client_copy(BRIDGE_I2C_CLIENT* dst,
    int new_addr, BRIDGE_I2C_CLIENT* src)
{
    if (!dst || !new_addr || !src)
        return IMG_ERROR_FATAL;

    memcpy(dst, src, sizeof(BRIDGE_I2C_CLIENT));
    dst->i2c_addr = new_addr;
    return IMG_SUCCESS;
}
