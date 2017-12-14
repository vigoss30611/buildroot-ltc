#include <stdint.h>

#ifndef __SB_RND_H__
#define __SB_RND_H__

extern void rnd_init(uint32_t seed);
extern uint32_t rnd_roll(uint32_t bottom, uint32_t top);
extern uint64_t rnd_roll64(uint64_t bottom, uint64_t top);
extern int rnd_file(const char * path, uint32_t size);
extern int rnd_data(uint8_t *buf, uint32_t size);

#endif /* __SB_RND_H__ */

