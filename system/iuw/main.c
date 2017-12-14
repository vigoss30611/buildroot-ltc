#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <string.h>
#include <types.h>
#include <genisi.h>
#include <push.h>

extern int genisi(int argc, char * argv[]);
extern int gencfg(int argc, char * argv[]);
extern int server(int argc, char * argv[]);
extern int wrap(int argc, char * argv[]);
extern int mkius(int argc, char * argv[]);
extern int mkburn(int argc, char * argv[]);
extern int media_inject(int argc, char * argv[]);
extern int tt_rw_main(int argc, char * argv[]);
extern int tt_aes(int argc, char * argv[]);
extern int tt_digest(int argc, char * argv[]);
extern int tt_wire(int argc, char * argv[]);
extern int partition(int argc, char * argv[]);
extern int tt_crypto_server(int argc, char * argv[]);
extern int tt_mix_test(int argc, char * argv[]);
void display_help(void)
{
	printf(
" iuw server\n"
" iuw wrap\n"
" iuw genisi\n"
" iuw m-inject\n"
" iuw tt-rw\n"
" iuw tt-aes\n"
" iuw tt-digest\n"
" iuw gencfg\n"
" iuw help\n"
);
}

int main(int argc, char *argv[])
{
	int rc = 0;

	if(argc < 2)
	  goto __exit_help;

	if(!strncmp(argv[1], "help", 4)
				|| !strncmp(argv[1], "--help", 6)
				|| !strncmp(argv[1], "-h", 2))
	  display_help();
	else if(!strncmp(argv[1], "mkius", 5))
	  rc = mkius(--argc, ++argv);
	else if(!strncmp(argv[1], "genisi", 6))
	  rc = genisi(--argc, ++argv);
	else if(!strncmp(argv[1], "mkburn", 6))
	  rc = mkburn(--argc, ++argv);
#if 0
    else if(!strncmp(argv[1], "m-inject", 12))
	  rc = media_inject(argc--, argv++);
    else if(!strncmp(argv[1], "server", 7))
      rc = server(--argc, ++argv);
    else if(!strncmp(argv[1], "push", 4))
      rc = push(--argc, ++argv);
    else if(!strncmp(argv[1], "tt-rw", 5))
	  rc = tt_rw_main(argc--, argv++);
	else if(!strncmp(argv[1], "tt-aes", 6))
	  rc = tt_aes(argc--, argv++);
	else if(!strncmp(argv[1], "tt-digest", 9))
	  rc = tt_digest(argc--, argv++);
	else if(!strncmp(argv[1], "gencfg", 6))
	  rc = gencfg(--argc, ++argv);
	else if(!strncmp(argv[1], "ttwire", 6))
	  rc = tt_wire(--argc, ++argv);
	else if(!strncmp(argv[1], "partition", 11))
      rc = partition(--argc, ++argv);
	else if(!strncmp(argv[1], "ttcs", 4))
      rc = tt_crypto_server(--argc, ++argv);
    else if(!strncmp(argv[1], "mix", 3))
      rc = tt_mix_test(--argc, ++argv);
#endif
    else
	  goto __exit_help;

	return rc;
__exit_help:
	printf("Input ius --help for usage.\n");

	return 0;
}
