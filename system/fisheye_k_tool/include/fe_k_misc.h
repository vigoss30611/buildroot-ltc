#ifndef __FE_K_MISC_H__
#define __FE_K_MISC_H__


#ifndef ARRY_SIZE
#define ARRY_SIZE(a) sizeof(a) / sizeof(a[0])
#endif


void print_split_line();
int get_view_file_name(fe_common_param_t *param_ptr, int x, int y,
                      char *func_name, char *mode_name, int idx,
                      char *out_name);
void tm_start();
long tm_get_ms();
void tm_print_diff_ms(char *str);
int fe_save_file(fe_common_param_t *param_ptr, char *part_filename_ptr, char *out_full_name);
#endif
