
#ifndef _IROM_BURN_H_
#define _IROM_BURN_H_
#include <ius.h>

extern int burn(uint32_t mask);
extern int issparse(char* buf );
extern int burn_image_sparse(struct ius_desc *desc, int id_i, int id_o, uint8_t *buf);
extern int burn_image_loop(struct ius_desc *desc, int id_i, uint32_t channel,
			int id_o, uint32_t max, uint8_t *buf);
extern int burn_extra(struct ius_desc *desc, int id_i);
extern int burn_images(struct ius_hdr *hdr, int id, uint32_t channel);
extern int try_seperate_source(void);
extern int burn_single_image(uint8_t *buf, int imagenum);

extern void set_ius_card_state(int val);
extern int get_ius_card_state(void);

#endif /* _IROM_BURN_H_ */
