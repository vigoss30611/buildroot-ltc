#include <malloc.h>
#include <string.h>
#include <qsdk/event.h>
#include <stdio.h>
#include <log.h>
#include <audio_rpc.h>
#include <audiobox_listen.h>

int audiobox_rpc_parse(struct event_scatter sc[], int count, char *buf)
{
	char p = '|';
	char *m = buf;
	char *n = NULL;
	int i = 0;
	
	if (!buf || !sc)
		return -1;

	while (i < count) {
		n = m;
		for (;*m != p; m++)
			if ((m - buf) >= AUDIO_RPC_MAX_BUF) {
				AUD_ERR("RPC recall buf err: %s\n", buf);
				return -1;
			}
		if (sc[i].buf)
			memcpy(sc[i].buf, n, m - n);
		else
			sc[i].buf = n;
		sc[i].len = m - n;
		m++;
		i++;
	}
	return 0;
}

int audiobox_rpc_call_scatter(char *name, struct event_scatter sc[], int count)
{
	char *buf;
	int ret;
	int i;
	int len;
	char *p = "|";
	
	if (!sc || !name)
		return -1;

	buf = (char *)malloc(AUDIO_RPC_MAX_BUF);
	if (!buf)
		return -1;
	
	len = 0;
	for (i = 0; i < count; i++) {
		if ((sc[i].len + len) > AUDIO_RPC_MAX_BUF) {
			AUD_ERR("Audio rpc sc err: %d\n", sc[i].len + len);
			return -1;
		}
		if (sc[i].buf) {
			memcpy(buf + len, sc[i].buf, sc[i].len);
			len += sc[i].len;
		}
		memcpy(buf + len, p, 1);
		len += 1;
	}

	ret = event_rpc_call(name, buf, len);
	if (ret < 0)
		AUD_ERR("Audio RPC Call err: %d\n", ret);
	AUD_DBG("Event rpc  recall\n");	
	ret = audiobox_rpc_parse(sc, count, buf);
	free(buf);
	return ret;
}
