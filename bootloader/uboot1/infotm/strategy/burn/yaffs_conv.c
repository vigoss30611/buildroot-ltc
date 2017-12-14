// by warits, Fri May 4, 2012

#include <common.h>
#include <vstorage.h>
#include <malloc.h>
#include <iuw.h>
#include <ius.h>
#include <hash.h>
#include <net.h>
#include <udc.h>
#include <led.h>
#include <nand.h>
#include <cdbg.h>
#include <crypto.h>
#include <sparse.h>
#include <irerrno.h>
#include <storage.h>
#include <yaffs.h>

void page_fit(struct yaffs_conv_info *conv,
			uint8_t *p1, uint8_t *p2) {
	if(!p1) {
		memcpy(p2 + conv->oo, p2 + conv->im, conv->is);
	} else {
		if(p2) {
			memcpy(p2 + conv->om, p2 + conv->im, conv->is);
			memcpy(p1 + conv->om, p1 + conv->oo, conv->is);
		} else
		  memcpy(p1 + conv->om, p1 + conv->im, conv->is);
	}
}

int yaffs_conv(struct yaffs_conv_info *conv)
{
	int sub, bytes, count = 0;
	void *p, *q = conv->buf;

	for(p = q, sub = bytes = 0;
		conv->get_page(p) == 0;) {

		count++;
		if(!(count & 0x1ff))
			printf("%d M converted.\n", count >> 9);

		if(conv->im == conv->om) {  // directly put page if in-main & out-main are the same
			conv->put_page(p);
			continue;
		}

		if(!YTAG(p + conv->im)->chunkId) {
			if(sub) { // first flush the file buffer
				page_fit(conv, q, p);
				YTAG(q + conv->om)->chunkId =
					(YTAG(q + conv->om)->chunkId - 1)
					/ conv->iodiv + 1;
				YTAG(q + conv->om)->byteCount = bytes;
				conv->put_page(q);
				conv->put_page(p);
				sub = bytes = 0;
				p = q;
			} else {
				page_fit(conv, p, NULL);
				conv->put_page(p);
			}
		} else {

			if(!sub++)
			  page_fit(conv, NULL, q); // back up file chunk spare

			if(sub == conv->iodiv) {
				YTAG(p + conv->im)->chunkId =
					(YTAG(p + conv->im)->chunkId - 1)
					/ conv->iodiv + 1;
				YTAG(p + conv->im)->byteCount += bytes;

				conv->put_page(q);
				sub = bytes = 0;
				p = q;
				continue;
			}

			bytes += YTAG(p + conv->im)->byteCount;
			p += conv->im;
		}
	}

	if(sub) { // first flush the file buffer
		page_fit(conv, q, p);
		conv->put_page(q);
	} 
	return 0;
}

