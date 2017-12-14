/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _EXT4_UTILS_H_
#define _EXT4_UTILS_H_

#ifdef __BIONIC__
extern void*  __mmap2(void *, size_t, int, int, int, off_t);
static inline void *mmap64(void *addr, size_t length, int prot, int flags,
        int fd, off64_t offset)
{
    return __mmap2(addr, length, prot, flags, fd, offset >> 12);
}
#endif

extern int force;

//#define warn(fmt, args...) do { fprintf(stderr, "warning: %s: " fmt "\n", __func__, ## args); } while (0)
//#define error(fmt, args...) do { fprintf(stderr, "error: %s: " fmt "\n", __func__, ## args); if (!force) longjmp(setjmp_env, EXIT_FAILURE); } while (0)
//#define error_errno(s, args...) error(s ": %s", ##args, strerror(errno))
//#define critical_error(fmt, args...) do { fprintf(stderr, "critical error: %s: " fmt "\n", __func__, ## args); longjmp(setjmp_env, EXIT_FAILURE); } while (0)
#define critical_error_errno(s, args...) critical_error(s ": %s", ##args, strerror(errno))

#define EXT4_SUPER_MAGIC 0xEF53
#define EXT4_JNL_BACKUP_BLOCKS 1

//#define min(a, b) ((a) < (b) ? (a) : (b))

//#define DIV_ROUND_UP(x, y) (((x) + (y) - 1)/(y))
//#define ALIGN(x, y) ((y) * DIV_ROUND_UP((x), (y)))
/*
//#define unsidgened long long int unsigned long long int
#define unsigned int unsigned int
#define unsigned short int unsigned short int

#define __be64 unsigned long long int
#define __be32 unsigned int
#define __be16 unsigned short int

#define __unsigned long long int unsigned long long int
#define __unsigned int unsigned int
#define __unsigned short int unsigned short int
#define __unsigned char unsigned char
*/

struct block_group_info;

struct ext2_group_desc {
	unsigned int bg_block_bitmap;
	unsigned int bg_inode_bitmap;
	unsigned int bg_inode_table;
	unsigned short int bg_free_blocks_count;
	unsigned short int bg_free_inodes_count;
	unsigned short int bg_used_dirs_count;
	unsigned short int bg_pad;
	unsigned int bg_reserved[3];
};

struct fs_info {
	signed long long int len;	/* If set to 0, ask the block device for the size,
			 * if less than 0, reserve that much space at the
			 * end of the partition, else use the size given. */
	unsigned int block_size;
	unsigned int blocks_per_group;
	unsigned int inodes_per_group;
	unsigned int inode_size;
	unsigned int inodes;
	unsigned int journal_blocks;
	unsigned short int feat_ro_compat;
	unsigned short int feat_compat;
	unsigned short int feat_incompat;
	unsigned int bg_desc_reserve_blocks;
	const char *label;
	unsigned char no_journal;
};

struct fs_aux_info {
	struct ext4_super_block *sb;
	struct ext4_super_block **backup_sb;
	struct ext2_group_desc *bg_desc;
	struct block_group_info *bgs;
	unsigned int first_data_block;
	unsigned long long int len_blocks;
	unsigned int inode_table_blocks;
	unsigned int groups;
	unsigned int bg_desc_blocks;
	unsigned int default_i_flags;
	unsigned int blocks_per_ind;
	unsigned int blocks_per_dind;
	unsigned int blocks_per_tind;
};

extern struct fs_info info;
extern struct fs_aux_info aux_info;

//extern jmp_buf setjmp_env;

static inline int log_2(int j)
{
	int i;

	for (i = 0; j > 0; i++)
		j >>= 1;

	return i - 1;
}

int ext4_bg_has_super_block(int bg);
void write_ext4_image(const char *filename, int gz, int sparse, int crc,
		int wipe);
void ext4_create_fs_aux_info(void);
void ext4_free_fs_aux_info(void);
void ext4_fill_in_sb(void);
void ext4_create_resize_inode(void);
void ext4_create_journal_inode(void);
void ext4_update_free(void);
void ext4_queue_sb(void);
unsigned long long int get_file_size(const char *filename);
unsigned long long int parse_num(const char *arg);
void ext4_parse_sb(struct ext4_super_block *sb);

#endif
