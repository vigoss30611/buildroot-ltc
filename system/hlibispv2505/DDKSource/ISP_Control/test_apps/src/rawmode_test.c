/**
******************************************************************************
@file rawmode_test.c

@brief Simple test to ensure raw-mode works with the used environment

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
#include <signal.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct termios g_orig_term_attr;

void printTermios(const struct termios *term)
{
    int i;
     if(sizeof(cc_t) != sizeof(unsigned char)
       || sizeof(speed_t) != sizeof(unsigned int)
       || sizeof(tcflag_t) != sizeof(unsigned int))
    {
        printf("cannot print original structure as sizes are unexpected:\n"\
            "sizeof(cc_t)=%u != sizeof(unsigned char)=%u\n"\
            "sizeof(speed_t)=%u != sizeof(unsigned int)=%u\n"\
            "sizeof(tcflag_t)=%u != sizeof(unsigned int)=%u\n",
            (unsigned)sizeof(cc_t), (unsigned)sizeof(unsigned char),
            (unsigned)sizeof(speed_t), (unsigned)sizeof(unsigned int),
            (unsigned)sizeof(tcflag_t), (unsigned)sizeof(unsigned int));
    }
    else
    {
        printf(
            "    .c_iflag = 0x%x\n"\
            "    .c_oflag = 0x%x\n"\
            "    .c_cflag = 0x%x\n"\
            "    .c_lflag = 0x%x\n"\
            "    .c_line = 0x%x\n"\
            "    .c_cc[NCCS=%u] = { 0x%x",
            term->c_iflag,
            term->c_oflag,
            term->c_cflag,
            term->c_lflag,
            (unsigned)term->c_line,
            NCCS, term->c_cc[0]);
        for(i=1;i<NCCS;i++)
        {
            if(i==VTIME)
            {
                printf(", VTIME=0x%x", (unsigned)term->c_cc[i]);
            }
            else if(i==VMIN)
            {
                printf(", VMIN=0x%x", (unsigned)term->c_cc[i]);
            }
            else
            {
                printf(", 0x%x", (unsigned)term->c_cc[i]);
            }
        }
        printf("}\n");
#if _HAVE_STRUCT_TERMIOS_C_ISPEED
        printf("   .c_ispeed = %u\n",
            term->c_ispeed);
#else
        printf("   no c_ispeed\n");
#endif
#if _HAVE_STRUCT_TERMIOS_C_OSPEED
        printf("   .c_ospeed = %u\n",
            term->c_ospeed);
#else
        printf("   no c_ospeed\n");
#endif
    }
}

/**
 * @brief Enable mode to allow proper key capture in the application
 */
void enableRawMode(struct termios *orig_term_attr)
{
    struct termios new_term_attr;

    printf("Enable Console RAW mode\n");

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), orig_term_attr);
    
    printf("original terminal attributes:\n");
    printTermios(orig_term_attr);

    memcpy(&new_term_attr, orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);//|ECHONL|IEXTEN);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;

    printf("new terminal attributes:\n");
    printTermios(&new_term_attr);
    
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
}

/**
 * @brief Disable mode to allow proper key capture in the application
 */
void disableRawMode(struct termios *orig_term_attr)
{
    printf("Disable Console RAW mode\n");
    tcsetattr(fileno(stdin), TCSANOW, orig_term_attr);
    
    printf("reconfigure terminal attributes:\n");
    printTermios(orig_term_attr);
}

#ifdef __cplusplus
}
#endif

void disableRawModeSig(sig_atomic_t sig)
{
    disableRawMode(&g_orig_term_attr);
    exit(EXIT_FAILURE);
}

char getch(void)
{
    static char line[2];
    if(read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int run = 1;
    char readChar = 0;
    
    enableRawMode(&g_orig_term_attr);
    signal(SIGINT, disableRawModeSig);
    
    while(run)
    {
        printf("will read from raw-mode console - press X to exit or CTRL-C if it fails\n");

        readChar = 0;
        //readChar = fgetc(stdin);
        readChar = getch();

        printf("read value %c (%d)\n", readChar, (int)readChar);

        if(readChar=='x')
        {
            run = 0;
            continue;
        }
        sleep(1);
    }


    disableRawMode(&g_orig_term_attr);

    return EXIT_SUCCESS;
}
