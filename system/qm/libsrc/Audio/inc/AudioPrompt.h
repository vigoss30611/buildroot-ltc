#ifndef  __AUDIO_PROMPT_H__ 
#define  __AUDIO_PROMPT_H__

int QMAPI_AudioPromptInit(void);
int QMAPI_AudioPromptUnInit(void);

int QMAPI_AudioPromptStart(void);
int QMAPI_AudioPromptStop(void);

int QMAPI_AudioPromptPlayFile(const char *fileName, int block);

#endif
