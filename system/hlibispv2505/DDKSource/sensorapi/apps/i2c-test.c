/**
******************************************************************************
@file i2c-test.c

@brief Test i2c communication with OV4688

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/i2c-dev.h>

#define AUTOFOCUS_WRITE_ADDR    (0x18 >> 1)
#define OV4688_SLAVE_ADDR	(0x6C >> 1)

#define AR330_READ_ADDR (0x21 >> 1)

int main(int argc, char *argv[])
{
	/* Initial focus settings */
	char focus[] = { 0x04, 0x00, 0x05, 0x00 };
	char chipIdAddr[] = { 0x30, 0x0A };
	char chipId[2];
    char devName[32];

	int rc, i2c;
	unsigned i, j, k;
    int id = 0;
    int use_ar300 = 0;

    if (argc > 1) {
        if (1 != sscanf(argv[1], "%d", &id)) {
            printf("Failed to parse %s for an i2c device\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    if (argc > 2) {
        use_ar300 = 1;
    }
    
    sprintf(devName, "/dev/i2c-%d", id);
    printf("Reading from %s\n", devName);
    
	i2c = open(devName, O_RDWR);
	if (i2c < 0) {
		printf("Failed to open i2c device %s, err = %d\n", devName, i2c);
		goto err_open;
	}

	if (use_ar300)
    {
        chipIdAddr[0] = 0x30;
        chipIdAddr[1] = 0x00;
        
        printf("using AR330\n");
        rc = ioctl(i2c, I2C_SLAVE, AR330_READ_ADDR);
    }
    else  // ov4688
    {
        chipIdAddr[0] = 0x30;
        chipIdAddr[1] = 0x3A;
        
        printf("using OV4688\n");
        rc = ioctl(i2c, I2C_SLAVE, OV4688_SLAVE_ADDR);
    }
	if (rc)
	{                                                                            
		printf("Failed to write I2C slave address!\n");
		return -EBUSY;
	}

	rc = write(i2c, chipIdAddr, sizeof(chipIdAddr));
	if (rc != sizeof(chipIdAddr))
	{
		printf("Failed to write data to I2C device, err = %d\n", rc);
		goto err_write;
	}

	rc = read(i2c, chipId, sizeof(chipId));
	if (rc != sizeof(chipId))
	{
		printf("Failed to read data from I2C device, err = %d\n", rc);
		goto err_write;
	}
	printf("Succesfuly read sensor id (%#x %#x).\n", chipId[0] & 0xff, chipId[1] & 0xff);

    if (use_ar300)
    {
        printf("no focus test with ar330\n");
    }
    else  // use ov4688
    {
        if (ioctl(i2c, I2C_SLAVE, AUTOFOCUS_WRITE_ADDR))
        {                                                                            
            printf("Failed to write I2C slave address!\n");
            return -EBUSY;
        }

        /* Repeat focus settings couple of times */
        for (i = 0; i < 3; ++i)
        {
            /* Write focus minimum and maximum value */
            for (j = 0; j < 2; ++j)
            {
                /* Write two focus registers with given values */
                for (k = 0; k < sizeof(focus); k += 2)
                {
                    focus[k + 1] = j ? 0xff : 0x00;
                    rc = write(i2c, &focus[k], 2);
                    if (rc != 2)
                    {
                        printf("Failed to write data to I2C slave device, err = %d\n", rc);
                        goto err_write;
                    }
                }
                sleep(1);
            }
        }
        printf("Succesfuly sent data to focus slave device. Did you hear focus click?\n");
    }

err_write:
	close(i2c);
err_open:
	return rc;
}
