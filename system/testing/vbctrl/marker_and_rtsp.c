#include <stdio.h>     /*  for printf */
#include <stdlib.h>    /*  for exit */
#include <unistd.h>
#include <getopt.h>
#include <qsdk/videobox.h>

extern const struct option overlay_long_options[14];
extern char set_flag;
extern char channel_flag;
extern char  overlay_fmt[128];
extern char *channel;
extern char **tmp_argv;
extern struct font_attr font_attr;

int overlay_parse_long_options(char *program_name, int index)
{
    if(0 == strcmp(overlay_long_options[index].name, "fmt")) {
        if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
            memset(overlay_fmt,0,sizeof(overlay_fmt));
            strcpy(overlay_fmt,optarg);
            marker_set_mode(channel, "auto", overlay_fmt, &font_attr);
        }
        else
        {
            printf("please check your command \n");
            printf("ie:%s overlay -cs string \n", program_name);
            return -1;
        }

    } else if(0 == strcmp(overlay_long_options[index].name, "color")) {
        if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
            font_attr.font_color = strtol(optarg, NULL, 16);
            font_attr.font_color = strtol(tmp_argv[optind], NULL, 16);
            marker_set_mode(channel, "auto", overlay_fmt, &font_attr);
        }
        else
        {
            printf("please check your command \n");
            printf("ie:%s overlay -color string \n", program_name);
            return -1;
        }
    }else if(0 == strcmp(overlay_long_options[index].name, "resize")) {
        if((1 == channel_flag) && (NULL != optarg)) {
            unsigned int u32Width = 0;
            unsigned int u32Height = 0;
            char s8OvPortName[32] = {0};
            sprintf(s8OvPortName,"ispost-%s",optarg);
            u32Width = atoi(tmp_argv[optind]);
            u32Height = atoi(tmp_argv[optind + 1]);
            video_set_resolution(s8OvPortName, u32Width, u32Height);
            font_attr.back_color = 0xFF000000;
            marker_set_mode(channel, "manual", overlay_fmt, &font_attr);
            marker_set_string(channel, " ");
        }
        else
        {
            printf("please check your command \n");
            printf("ie:%s overlay -size string \n", program_name);
            return -1;
        }
      }else if(0 == strcmp(overlay_long_options[index].name, "string")) {
        if((1 == set_flag) && (1 == channel_flag) && (NULL != optarg)) {
            marker_set_mode(channel, "manual", overlay_fmt, &font_attr);
            marker_set_string(channel, optarg);
        }
        else
        {
            printf("please check your command \n");
            printf("ie:%s overlay -string string \n", program_name);
            return -1;
        }
    }else if(0 == strcmp(overlay_long_options[index].name, "shelter")) {
        if(1 == channel_flag) {
            font_attr.back_color = 0xFF000000;
            marker_set_mode(channel, "manual", overlay_fmt, &font_attr);
            marker_set_string(channel, " ");
        }
        else
        {
            printf("please check your command \n");
            printf("ie:%s overlay -shelter string \n", program_name);
            return -1;
        }
    }
    else
    {
        printf("Please check your command \n");
        printf("Try `overlay' for more information.\n");
        return -1;
    }
}

int overlay_set_bmp(char *channel,char *bmp_path)
{
    marker_set_mode(channel, "manual", "%t%Y/%M/%D  %H:%m:%S", &font_attr);
    struct fr_buf_info ref;
    fr_INITBUF(&ref, NULL);
    marker_get_frame(channel, &ref);
    FILE *fpbmp = NULL;
    unsigned int OffSet = 0;
    // OffSet from Header part to Data Part
    long width = 0;
    // The Width of the Data Part
    long height = 0;
    // The Height of the Data Part
    fpbmp= fopen(bmp_path, "rb");
    if (fpbmp == NULL)
    {
        printf("Open bmp failed!!!\n");
        return -1;
    }
    fseek(fpbmp, 10L, SEEK_SET);
    fread(&OffSet, sizeof(char), 4, fpbmp);
    fseek(fpbmp, 18L, SEEK_SET);
    fread(&width, sizeof(char), 4, fpbmp);
    fseek(fpbmp, 22L, SEEK_SET);
    fread(&height, sizeof(char), 4, fpbmp);
    fseek(fpbmp, OffSet, SEEK_SET);
    //seek set to position of r,g,b data
    fread(ref.virt_addr, 1, ref.size, fpbmp);
    fclose(fpbmp);
    marker_put_frame(channel, &ref);

    return 0;
}

