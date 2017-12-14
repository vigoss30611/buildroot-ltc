/*
 * definition copied from yaffs project
 */

#ifndef __UNYAFFS_H__
#define __UNYAFFS_H__

typedef struct {
    unsigned sequenceNumber;
    unsigned objectId;
    unsigned chunkId;
    unsigned byteCount;
} yaffs_PackedTags2TagsPart;

#define YTAG(x) ((yaffs_PackedTags2TagsPart *)(x))

struct yaffs_conv_info {

	int om;		// out-main
	int os;		// out-spare
	int oo;		// out-full
	int im;		// in-main
	int is;		// in-spare
	int ii;		// in-full
	int iodiv;	// "out" divided by "in"

	int (*get_page)(uint8_t *buf);
	int (*put_page)(uint8_t *buf);

	void *buf;
};

extern int yaffs_conv(struct yaffs_conv_info *conv);
extern int yaffs_burn(loff_t offs, int part_size,
			int (* get_page)(uint8_t *buf));

#endif /* __UNYAFFS_H__ */

