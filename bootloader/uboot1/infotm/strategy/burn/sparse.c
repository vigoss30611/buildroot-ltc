/**********************************************************************************************
 * file		: sparse.c
 *descriment	: parase and burn sparse images to gaven address at the system disk. 
 *Author        : John
 *Date 		: May 4th 2012
 *Version 	: init (inherent codes from the android 4.03 source code)
 *********************************************************************************************/
#include <common.h>
#include <cdbg.h>
#include <isi.h>
#include <hash.h>
#include <crypto.h>
#include <ius.h>
#include <bootlist.h>
#include <vstorage.h>
#include <storage.h>
#include <malloc.h>
#include <nand.h>
#include <led.h>
#include <irerrno.h>
#include <ext4_utils.h>
#include <sparse_format.h>
#include <sparse_crc32.h>
#include <burn.h>
#if 1
struct data_source{
	int devid;
	int remain;
	int (* get_data)(void *);
	int step;
};
struct buffer{
	char* copybuf;
	unsigned  int size;
	unsigned int index;
	unsigned int deepth;
#define re_align_buffer() {\
	if(buffer.index){\
		memcpy( buffer.copybuf , buffer.copybuf + buffer.index , buffer.deepth - buffer.index);\
		buffer.deepth = buffer.deepth -buffer.index ;\
		buffer.index=0;\
	}\
}
	};

struct target{
	loff_t index;
	int devid;
	loff_t length;
	loff_t remain;
	char *cache;
#define TAGET_PART_CACHE_SIZE BLOCK_SIZE
	unsigned int cahcheindex;
	unsigned int blksz;

};
#define CONFIG_SPARSE_INTEEVAL              0x100000
#define CONFIG_SPARSE_COPY_SIZE             (0x100000)
#define CONFIG_SPARSE_BUF_SIZE              (0x100000) //real used size will be a little more than CONFIG_SPARSE_COPY_SIZE but
														//rarely larger than or equal to CONFIG_SPARSE_BUF_SIZE
#define SPARSE_HEADER_MAJOR_VER 1
#define SPARSE_HEADER_LEN					(sizeof(sparse_header_t))
#define CHUNK_HEADER_LEN					(sizeof(chunk_header_t))
#define BLOCK_SIZE 512
#define DATA_IN_BUFFER_NOW					(buffer.deepth-buffer.index)
static struct data_source data_source;
static struct buffer buffer;
static struct target target;
static int copycount=0;
static unsigned long int sparse_crc32(unsigned long int crc_in, const void *buf, int size)
{
	const unsigned char *p = buf;
	unsigned long int crc;

	crc = crc_in ^ ~0U;
	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
	return crc ^ ~0U;
}

static int spare_init(int dev, loff_t offs, int size, int step, int (* get_data)(void *))
{
	memset(&data_source , 0 , sizeof(struct data_source));
	memset(&buffer , 0 , sizeof(struct buffer));
	memset(&target , 0 , sizeof(struct target));

	data_source.remain= size;
	step= (step==0)? CONFIG_SPARSE_COPY_SIZE : step;
	data_source.step = step;
	data_source.get_data = get_data;
	target.devid = dev;
	target.blksz = vs_align(dev);
	target.index = offs;
	target.blksz = vs_align(dev);
	buffer.size  = CONFIG_SPARSE_BUF_SIZE;	
	
	if((buffer.copybuf = malloc(CONFIG_SPARSE_BUF_SIZE))==NULL){ //make the buffer four times of step length to avoid over flow
		printf("Erro: failed to alloc a buffer for sparse image\n");
		return -1;
	}
	
	return 0;
}
static void sparse_deinit(void)
{
	free(buffer.copybuf);
	memset(&data_source , 0 , sizeof(struct data_source));
	memset(&buffer , 0 , sizeof(struct buffer));
	memset(&target , 0 , sizeof(struct target));
	return;
}
static int seek_buffer(int seek)
{
	buffer.index += seek ;
	if(buffer.index==buffer.deepth) buffer.index=buffer.deepth=0;
	return 0;
}
static int get_data_from_source(unsigned int len)
{
	/*Make sure it will not overflow the copy buffer.*/
	int still=buffer.deepth-buffer.index;
	if((buffer.size-still)<data_source.step||still>=target.blksz){
		return still;
	}
	if((still+len )>buffer.size ){
		len=buffer.size-still;
	}

	if((buffer.size - buffer.deepth)<len ||(buffer.size - buffer.deepth)<data_source.step){
		if(buffer.index){
			copycount++;
			memcpy( buffer.copybuf , buffer.copybuf + buffer.index , still);
			buffer.deepth = still ;
			buffer.index=0;
		}

	}
	if(len>data_source.step)
		len=len&(~(data_source.step-1));
	if(len>data_source.remain) len=data_source.remain;
	int stillto=len;
	int step=0;
	while(stillto){
		step=data_source.get_data((void*)(buffer.copybuf + buffer.deepth));
		if(step<0)
			printf("erro: step:0x%x\n",step);
		buffer.deepth+=step;	
		if(buffer.deepth>buffer.size){
			printf("Erro:read data over flow the buffer\n");
		}
		data_source.remain-=step;

		if(buffer.deepth>buffer.size-data_source.step)
			break;
		if((stillto>step)&&(step>0)) stillto-=step;
		else{
			break;
		}
	}
	printf("remain: %dM\n " ,data_source.remain>>20);//bufer deepth:0x%8x buffer index:0x%8x\n",data_source.remain,buffer.deepth,buffer.index);
	return buffer.deepth- buffer.index;
}
static int write_data_to_disk(unsigned int len,int blksz)
{
	/************commented for the write size  is always block size aligned**********/
	/*  if the last write is not block size aligned we fill the last write block first*/
	/*
	   if( target.cahcheindex ){ 
	   target.cache=malloc(TAGET_PART_CACHE_SIZE);
	   if(NULL==target.cache){
	   printf("Faile to alloc a chache buffer\n");
	   return -1;
	   }
	   memcpy(target.cache , buffer.copybuf + buffer.index , TAGET_PART_CACHE_SIZE - target.cahcheindex);
	   len -= TAGET_PART_CACHE_SIZE - target.cahcheindex;
	   buffer.index += TAGET_PART_CACHE_SIZE - target.cahcheindex;
	   oem_write_sys_disk(buffer.copybuf + buffer.index , target.startblk + target.index/BLOCK_SIZE , TAGET_PART_CACHE_SIZE , 0);
	   target.index += TAGET_PART_CACHE_SIZE-target.cahcheindex;
	   target.cahcheindex =0 ;
	   free!target.cache);
	   }
	 */
	if(buffer.deepth< (buffer.index + len )){
		printf("Write length beyond the data size it has right now: length:0x%8x\n",len);
		return -1;//expected never happen
	}
	//	vs_assign_by_id(id_i, 1);
	if(0 == vs_assign_by_id(target.devid, 0)) {
		/* skip bad blocks for nandflash
		 * check bad block at every block starting baudary
		 * if a bad block is found, the offset is increased
		 * by a blocksize. There should always be a leeway leaves
		 * there for nandflashes. if the leeway used up
		 * the write process is failed.
		 *
		 * FIXME: if the offset start at a not block-aligned
		 * value, the first address will not be checked. so
		 * error may happnens if the first relavant block is
		 * bad. thus: XXX burns start at a not block-aligned
		 * address is strongly NOT recommanded!
		 */
//		int leeway, skip = 0;
#if 0   
		it will never be a nand device for sparse image
		if(vs_is_device(target.devid, "nand") || vs_is_device(target.devid, "fnd")
				|| vs_is_device(target.devid, "bnd")) {
			struct nand_config *nc;
			if(vs_is_device(target.devid, "bnd")) {
				nc = nand_get_config(NAND_CFG_BOOT);
				/* BND data must reach 32KB to boot
				 * Jan 5, 2012 */
				if(target.length < BL_SIZE_FIXED)
					target.remain = target.length = BL_SIZE_FIXED;
			} else
				nc = nand_get_config(NAND_CFG_NORMAL);
			if(!nc) {
				printf("get nand config failed\n");
				return -ENOCFG;
			}

			skip = nc->blocksize;
		}
		leeway =target.length - target.remain;
		if(skip && !(target.index & (skip - 1))) {
			for(; vs_isbad(target.index); ) {
				if(leeway > skip) {
					printf("burn skip bad block at 0x%llx\n", target.index);
					target.index += skip;
					leeway -= skip;
				} else {
					printf("no more good block availible\n");
					return -EBADBLK;
				}
			}
		}
#endif		
		len=len&(~(target.blksz-1));
		vs_write((uint8_t *)(buffer.copybuf + buffer.index ), target.index, len, 0);
	}

	/*************************commented for the write size  is always block size aligned*********************/
	/*If write len is not block size aligned , cache the last nonaligned writen block*/
	/*  if(len &BLOCK_SIZE) { //if write size not block aligned, cache the unaligned data.
		printf("Not block aligned,need cache\n");
		target.cache = malloc(len &BLOCK_SIZE);
		memcpy(target.cache , buffer.copybuf + buffer.index -(len &BLOCK_SIZE) , len &BLOCK_SIZE);
		target.cahcheindex = len &BLOCK_SIZE ;
		}
	 */
	target.index +=len;
	target.remain -=len;
	return len;
}

static int process_raw_chunk( unsigned int blocks, unsigned int blk_sz, unsigned int *crc32)
{
	//u64 len = (u64)blocks * blk_sz;
	unsigned int len = blocks * blk_sz; // in our use the data_source file will never bigger than 1GBytes, so an unsigned int parameter will be enough
	int ret;
//	printf("Raw chunk: blocks %d blk_sz %d len %d\n", blocks, blk_sz, len);
	while (len) {
		ret = get_data_from_source(len);
		if (ret == -1 ) {
			printf( "read returned an error copying a raw chunk: %d %d\n",
					ret, (int)len);
			return -1;
		}
		if(ret >= len) ret= len;
		*crc32 = sparse_crc32(*crc32, buffer.copybuf+buffer.index, ret);
		ret = write_data_to_disk(ret,blk_sz);
		if (ret ==-1) {
			printf( "write returned an error copying a raw chunk\n");
			return -1;
		}
		seek_buffer(ret);
		len -= ret;
	}

	return blocks;
}
static int process_fill_chunk(unsigned int blocks, unsigned int blk_sz, unsigned int *crc32)
{
	u64 len = (u64)blocks * blk_sz;
	int ret;
	int chunk;
	unsigned int fill_val;
	unsigned int *fillbuf;
	unsigned int i;

	/* Fill copy_buf with the fill value */
//	printf("fill chunk: %d blocks\n",blocks);
	int fvsz=sizeof(unsigned int);
	ret=DATA_IN_BUFFER_NOW;
	if(ret<fvsz)
		ret = get_data_from_source(fvsz);
	fill_val =*((unsigned int*) ( buffer.copybuf +buffer.index));
	buffer.index += fvsz;

	fillbuf = malloc(CONFIG_SPARSE_COPY_SIZE);
	if(NULL== fillbuf) {
		printf("Failed to alloc memory for fillbuf\n");
		return -1;
	}
	for (i = 0; i < (CONFIG_SPARSE_COPY_SIZE / fvsz); i++) {
		fillbuf[i] = fill_val;
	}

	struct buffer* tmpbuf=(struct buffer*) malloc(sizeof(struct buffer));
	copycount++;
	memcpy(tmpbuf , &buffer ,sizeof(struct buffer));
	memset(&buffer , 0 , sizeof(struct buffer));
	buffer.index =0;
	buffer.deepth = CONFIG_SPARSE_COPY_SIZE;
	buffer.copybuf =(char *)fillbuf;
	while (len) {
		chunk = (len > CONFIG_SPARSE_COPY_SIZE) ? CONFIG_SPARSE_COPY_SIZE : len;
		*crc32 = sparse_crc32(*crc32, fillbuf, chunk);
		ret = write_data_to_disk( chunk,blk_sz);
	//	if (ret != chunk) {
	//		printf( "write returned an error copying a raw chunk\n");
	//		free((void *)tmpbuf);
	//		return -1;
	//	}
		len -= chunk;
	}
	copycount++;
	memcpy(&buffer , tmpbuf ,sizeof(struct buffer));
	free((void *)tmpbuf);
	return blocks;
}

static int process_skip_chunk(unsigned int blocks, unsigned int blk_sz, unsigned int *crc32)
{
	/* len needs to be 64 bits, as the sparse file specifies the skip amount
	 * as a 32 bit value of blocks.
	 */
	//  u64 len = (u64)blocks * blk_sz;
	unsigned int len = blocks * blk_sz;
	target.index += len;
	return blocks;
}

static int process_crc32_chunk(unsigned int crc32)
{
	unsigned int file_crc32;
	int ret;
//	printf("===================chunk crc32=======================\n");
	ret=DATA_IN_BUFFER_NOW;
	if(ret<4)
		ret = get_data_from_source(4);
	file_crc32=*((unsigned int*)(buffer.copybuf+buffer.index));
	buffer.index += 4;
	if (ret != 4) {
		printf( "read returned an error copying a crc32 chunk\n");
		return -1;
	}

	if (file_crc32 != crc32) {
		printf( "computed crc32 of 0x%8.8x, expected 0x%8.8x\n",
				(unsigned int)crc32, (unsigned int)file_crc32);
		return -1;
	}

	return 0;
}
int issparse(char* buf )
{
	sparse_header_t* sparse_header =(sparse_header_t*) buf;
	int dev_id=storage_device();
	int blksz = vs_align(dev_id);
	if (sparse_header->magic != SPARSE_HEADER_MAGIC) {
		//printf( "Bad magic\n");
		return 0;
	}

	if (sparse_header->major_version != SPARSE_HEADER_MAJOR_VER) {
		printf( "Unknown major version number\n");
		return 0;
	}
	if((sparse_header->blk_sz)&(blksz-1)) { //soppose that blksz will ever be power of 2
		printf("Erro: The image_data_source image is not for the disk, Block size not match %d\n",(int)sparse_header->blk_sz);
		return 0;
	}

	return 1;
}
int sparse_burn(int dev, loff_t offs, int size, int step, int (* get_data)(void *))
{
	unsigned int i;
	sparse_header_t sparse_header;
	sparse_header_t* sparse_header_buf;
	chunk_header_t	chunk_header;
	unsigned int crc32 = 0;
	unsigned int total_blocks = 0, chunk_blks=0;
	int ret =0;


	printf("step:0x%8x\n",step);
	if( spare_init(dev, offs , size , step , get_data ))return -1;
	 vs_assign_by_id(dev, 1);
	ret = get_data_from_source(CONFIG_SPARSE_COPY_SIZE);
	if (ret< sizeof(sparse_header)) {
		printf( "Error reading sparse file header ret:0x%8x\n",ret);
	//	return -1;
	}
//	copycount++;
	sparse_header_buf=(sparse_header_t*)(buffer.copybuf +buffer.index);
	sparse_header=*sparse_header_buf;
//	memcpy(&sparse_header , buffer.copybuf +buffer.index, sizeof(sparse_header));
	seek_buffer(sizeof(sparse_header));
//	printf("Magic:0x%8x\n",sparse_header.magic);

	if(!issparse((char*) &sparse_header )) return -1;

	/* Skip the remaining bytes in a header that is longer than
	 * we expected.
	 */

	target.length = target.remain = sparse_header.total_blks*sparse_header.blk_sz;

	if(vs_device_erasable(target.devid)) {
		vs_assign_by_id(target.devid, 0);
		vs_erase(target.index, target.remain);
	}
	int chunk_fills=0, chunk_nocare=0;
	int headersz=sizeof(chunk_header);
	for (i=0; i<sparse_header.total_chunks; i++) {
		ret= headersz;
		if(DATA_IN_BUFFER_NOW<headersz)
			ret = get_data_from_source(headersz);
		//memcpy(&chunk_header , buffer.copybuf+buffer.index , sizeof(chunk_header));
		chunk_header=*((chunk_header_t *) (buffer.copybuf+buffer.index));
		if (ret <sizeof(chunk_header) || ret==-1) {
			printf( "Error reading chunk header\n");
			return -1;
		}
		/* Skip the remaining bytes in a header that is longer than
		 * we expected.
		 */
		seek_buffer(headersz);
		//  printf(" buffer index %d:0x%x chunck type:0x%x\n", (int)i  , (int)buffer.index , (int)chunk_header.chunk_type);
		switch (chunk_header.chunk_type) {
			case CHUNK_TYPE_RAW:
				if (chunk_header.total_sz != (sparse_header.chunk_hdr_sz +
							(chunk_header.chunk_sz * sparse_header.blk_sz)) ) {
					printf( "Bogus chunk size for chunk %d, type Raw\n", (int)i);
					return -1;
				}
				chunk_blks = process_raw_chunk( chunk_header.chunk_sz, sparse_header.blk_sz, &crc32);
				total_blocks += chunk_blks;
				break;
			case CHUNK_TYPE_FILL:
				if (chunk_header.total_sz != (sparse_header.chunk_hdr_sz + sizeof(unsigned int)) ) {
					printf( "Bogus chunk size for chunk %d, type Fill\n", (int)i);
					return -1;
				}
				chunk_blks = process_fill_chunk( chunk_header.chunk_sz, sparse_header.blk_sz, &crc32);
				total_blocks += chunk_blks;
				chunk_fills+=chunk_blks;
				break;
			case CHUNK_TYPE_DONT_CARE:
				if (chunk_header.total_sz != sparse_header.chunk_hdr_sz) {
					printf( "Bogus chunk size for chunk %d, type Dont Care\n", (int)i);
					return -1;
				}
				chunk_blks = process_skip_chunk( chunk_header.chunk_sz, sparse_header.blk_sz, &crc32);
				total_blocks += chunk_blks;
				chunk_nocare +=chunk_blks;
				break;
			case CHUNK_TYPE_CRC32:
				process_crc32_chunk( crc32);
				break;
			default:
				printf( "Unknown chunk type 0x%4.4x\n", (int)chunk_header.chunk_type);
				break;
		}
		if(chunk_blks<0) return -1;
		chunk_blks=0;
	}

	if (sparse_header.total_blks != total_blocks) {
		printf( "Wrote %d blocks, expected to write %d blocks\n",
				(int)total_blocks, (int)sparse_header.total_blks);
		return 0;
	}
//	printf("sparse image bur ok , chunk_fills:%d chunk_nocare:%d\n",chunk_fills,chunk_nocare);
	sparse_deinit();
//	printf("memory copy tmes: %d\n",copycount);
	return 0;
}

#endif



