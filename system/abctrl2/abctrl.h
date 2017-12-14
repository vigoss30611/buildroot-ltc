#ifndef __AUDIOBOX_TEST_H__
#define __AUDIOBOX_TEST_H__

#define  ABCTL_DEBUG

#ifdef ABCTL_DEBUG
#define ABCTL_DBG(arg...)		fprintf(stderr, arg) 
#else
#define ABCTL_DBG(arg...)
#endif
#define ABCTL_ERR(arg...)     fprintf(stderr, arg)
#define ABCTL_INFO(arg...)     fprintf(stderr, arg)

#define DEVICE_OUT_ALL (0)
#define DEVICE_IN_ALL  (1)

extern int playback(int argc, char **argv);
extern int capture(int argc, char **argv);
extern int mute(int argc, char **argv);
extern int unmute(int argc, char **argv);
extern int set(int argc, char **argv);
extern int encode(int argc, char **argv);
extern int decode(int argc, char **argv);
extern int readi(int argc, char **argv);
#endif
