#ifndef __AUDIO_RPC_H__
#define __AUDIO_RPC_H__

extern int audiobox_rpc_parse(struct event_scatter sc[], int count, char *buf);
extern int audiobox_rpc_call_scatter(char *name, struct event_scatter sc[], int count);

#endif
