#ifndef _FR1023_H
#define _FR1023_H

/* FR1023 register space */

#define FR1023_CONTROL0     0x00        //digital dsm
#define FR1023_CONTROL1     0x01        //dither power 1
#define FR1023_CONTROL2     0x02        //dither power 2
#define FR1023_CONTROL3     0x03        //dither power 3
#define FR1023_CONTROL4     0x04        //R/L interpolate filter mute
#define FR1023_CONTROL5     0x05        //Right interpolate filter vol
#define FR1023_CONTROL6     0x06        //Right interpolate filter vol
#define FR1023_CONTROL7     0x07        //Left interpolate filter vol
#define FR1023_CONTROL8     0x08        //Left interpolate filter vol
#define FR1023_CONTROL9     0x09        //FORMAT
#define FR1023_CONTROL10     0x0a       //clock
#define FR1023_CONTROL11     0x0b       //playback 02;cap 01;all 03
#define FR1023_CONTROL12     0x0c       //BAIS mode
#define FR1023_CONTROL13     0x0d       //PGA input select
#define FR1023_CONTROL14     0x0e       //PGA boost
#define FR1023_CONTROL15     0x0f       //ADC
#define FR1023_CONTROL16     0x10       //DAC
#define FR1023_CONTROL17     0x11       //PGA output vol
#define FR1023_CONTROL18     0x12       //Left amplifier
#define FR1023_CONTROL19     0x13       //Right amplifier
#define FR1023_CONTROL20     0x14       //channel enable
#define FR1023_CONTROL21     0x15       //mute
#define FR1023_CONTROL22     0x16       //power
#define FR1023_CONTROL23     0x17       //power
#define FR1023_CONTROL24     0x18       //Reserved
#define FR1023_CONTROL25     0x19       //Reserved
#define FR1023_CONTROL26     0x1a       //Reserved
#define FR1023_CONTROL27     0x1b       
#define FR1023_CONTROL28     0x1c
#define FR1023_CONTROL29     0x1d
#define FR1023_CONTROL30     0x1e
#define FR1023_CONTROL31     0x1f
#define FR1023_CONTROL32     0x20
#define FR1023_CONTROL33     0x21
#define FR1023_CONTROL34     0x22       //Reserved
#define FR1023_CONTROL35     0x23       //data copy

#define BAIS_POWER          7
#define PWR_MIC_BIT         6
#define PGA_STAGE_1_SHIFT   5
#define PGA_STAGE_2_SHIFT   4
#define PGA_STAGE_INV_SHIFT 3
#define ADC_CHANNEL         2
#define PA_OUTPUT           0

#define LEFT_DAC_SHIFT      7
#define RIGHT_DAC_SHIFT     6
#define LEFT_MIX_SHIFT      5
#define RIGHT_MIX_SHIFT     4
#define LEFT_VMID_BUFFER    3
#define RIGHT_VMID_BUFFER   2
#define LEFT_OUTPUT         1
#define RIGHT_OUTPUT        0

#define CLOCK_SHIFT         7
#define CLOCK_INT_SHIFT     1
#define CLOCK_DEC_SHIFT     0

/* clock inputs */
#define FR1023_MCLK         0
#define FR1023_PCM_CLK      1

/* clock divider id's */
#define FR1023_PCMDIV       0
#define FR1023_BCLKDIV      1
#define FR1023_VXCLKDIV     2

#define FR1023_1536FS       1536
#define FR1023_1024FS       1024
#define FR1023_768FS        768
#define FR1023_512FS        512
#define FR1023_384FS        384
#define FR1023_256FS        256
#define FR1023_128FS        128

#endif
