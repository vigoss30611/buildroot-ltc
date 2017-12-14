/* This simple program erases the MBR or GPT on a block device and builds
 * a new one conforming to our requirements. It is similar to what the
 * "fastboot oem format" command does and is used by the test machines
 * during a low-level de-brick.
 *
 * The tool assumes this will only be used on a PC hard disk and so sectors
 * are always 512 bytes, LBA0 is always the MS-DOS Master Boot Record sector,
 * and LBA1 is always the GPT header, the GPT backup header is always stored
 * on the last LBA, and there is a maximum 128 partitions (to match gdisk).
 *
 * At the moment we want to share the partition configuration with the
 * platform guide, which accommodates dual boot (which we don't care about
 * on the test systems) and which loop-back mounts the Android filesystem
 * images from a single ext4 filesystem labelled 'Android'.
 *
 * So this tool will create a valid GPT with a single Linux data partition
 * called 'Android', and an ext4 filesystem labelled 'Android'.
 */

#include <linux/fs.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

/* for crc32() */
#include <zlib.h>

/* enables us to format the partition ext4, like fastboot */
#include "make_ext4fs.h"
#include "ext4_utils.h"
void reset_ext4fs_info(void);

/* MBR PTE, see http://en.wikipedia.org/wiki/Master_boot_record#PTE */
/* NOTE: Bitfields are unused, don't panic */
struct mbr_partition {
	uint8_t       status;
    unsigned int  first_head     : 8;  /*   S */
    unsigned int  first_sector   : 6;  /*  H  */
    unsigned int  first_cylinder : 10; /* C   */
    uint8_t       type;
    unsigned int  last_head      : 8;  /*   S */
    unsigned int  last_sector    : 6;  /*  H  */
    unsigned int  last_cylinder  : 10; /* C   */
    uint32_t      first_sector_lba;
    uint32_t      num_sectors;
} __attribute__((packed));

/* GPT header, see http://en.wikipedia.org/wiki/GUID_Partition_Table */
struct gpt_header {
	char      signature[8];
	uint32_t  revision;
	uint32_t  size;
	uint32_t  crc32;
	uint32_t  reserved_0;
	uint64_t  lba_primary;
	uint64_t  lba_backup;
	uint64_t  lba_first_usable;
	uint64_t  num_lbas;
	char      disk_uuid[16];
	uint64_t  lba_partitions;
	uint32_t  num_partitions;
	uint32_t  partitions_entry_size;
	uint32_t  partitions_crc32;
	char      reserved[420];
} __attribute__((packed));

/* GPT partition entry */
struct gpt_partition_entry {
	char      type_uuid[16];
	char      uuid[16];
	uint64_t  first_lba;
	uint64_t  num_lbas;
	uint64_t  flags;
	char      name[72];
} __attribute__((packed));

struct header {
	char                       mbr_bootstrap_code_area[446]; /*   446 bytes */
	struct mbr_partition       mbr_partition[4];             /*   510 bytes */
	char                       mbr_boot_signature[2];        /*   512 bytes */
	struct gpt_header          gpt_primary_header;           /*  1024 bytes */
} __attribute__((packed));

struct footer {
	struct gpt_header          gpt_backup_header;            /*   512 bytes */
} __attribute__((packed));

/* This is a random uuid which we will use for all of our disks */
static char disk_uuid[16];

/* This is a random uuid which we will use for all of our partitions */
static char partition_uuid[16];

/* Partition type UUID for "Linux filesystem data" */
static const char linux_filesystem_data_uuid[16] = {
	0xAF, 0x3D, 0xC6, 0x0F, 0x83, 0x84, 0x72, 0x47,
	0x8E, 0x79, 0x3D, 0x69, 0xD8, 0x47, 0x7D, 0xE4,
};

/* Written to first LBA */
static struct header header;

/* Written to last LBA */
static struct footer footer;

/* Written after primary header and before backup header (footer) */
struct gpt_partition_entry partition_entries[128];

int main(int argc, char *argv[])
{
	struct gpt_header gpt_header = { .signature = {} };
	uint64_t size, num_lbas, lba_tmp;
	struct stat stat;
	int err = 1, fd;
	uLong n;

	if (argc != 2) {
		fprintf(stderr, "usage: %s /dev/block/sdX\n", argv[0]);
		goto err_out;
	}

	/* Read in some random data for our disk and partition uuids */

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "error opening /dev/urandom (%s)\n", strerror(errno));
		goto err_out;
	}

	if (read(fd, &disk_uuid, 16) != 16) {
		fprintf(stderr, "error reading /dev/urandom (%s)\n", strerror(errno));
		goto err_out;
	}

	if (read(fd, &partition_uuid, 16) != 16) {
		fprintf(stderr, "error reading /dev/urandom (%s)\n", strerror(errno));
		goto err_out;
	}

	close(fd);

	/* Start processing the file or block device */

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "error opening %s (%s)\n", argv[1], strerror(errno));
		goto err_out;
	}

	if (fstat(fd, &stat) < 0) {
		fprintf(stderr, "fstat failed on %s (%s)\n", argv[1], strerror(errno));
		goto err_close;
	}

	if ((stat.st_mode & S_IFMT) == S_IFBLK) {
		if (ioctl(fd, BLKGETSIZE64, &size) < 0) {
			fprintf(stderr, "BLKGETSIZE64 failed on %s (%s)\n",
			                argv[1], strerror(errno));
			goto err_close;
		}
	} else {
		size = stat.st_size;
	}

	num_lbas = size / 512;

	/* LBA0 + GPT primary header + primary partition table +
	 * minimum partition alignment +
	 * backup partition table + GPT backup header
	 */
	if (num_lbas < 1 + 1 + 32 + 2048 + 32 + 1) {
		fprintf(stderr, "%s is too small (%llu lbas)\n",
						argv[1], (unsigned long long)num_lbas);
		goto err_close;
	}

	/* Build the fake GPT partition entries */
	memcpy(&partition_entries[0].type_uuid, linux_filesystem_data_uuid, 16);
	memcpy(&partition_entries[0].uuid, &partition_uuid, 16);
	memcpy(partition_entries[0].name, "A\0n\0d\0r\0o\0i\0d\0", 14);
	partition_entries[0].first_lba = 2048;
	partition_entries[0].num_lbas  = num_lbas - partition_entries[0].first_lba;

	/* Create a protective MBR partition table to keep gdisk happy */
	header.mbr_partition[0].type             = 0xEE;
	header.mbr_partition[0].first_sector_lba = 1;
	header.mbr_partition[0].num_sectors      = num_lbas - 1;
	header.mbr_boot_signature[0]             = 0x55;
	header.mbr_boot_signature[1]             = 0xAA;

	/* Build the fake GPT header */
	memcpy(&gpt_header.signature, "EFI PART", 8);
	memcpy(&gpt_header.disk_uuid, &disk_uuid, 16);
	gpt_header.revision              = 0x00010000;
	gpt_header.size                  = 0x5C;
	gpt_header.lba_primary           = 1;
	gpt_header.lba_backup            = num_lbas - 1;
	gpt_header.lba_first_usable      = 1 + 1 + 32;
	gpt_header.num_lbas              = num_lbas - gpt_header.lba_first_usable;
	gpt_header.lba_partitions        = 2;
	gpt_header.num_partitions        = 128;
	gpt_header.partitions_entry_size = 128;

	/* Build the fake GPT footer (backup header) */

	/* Compute partition entry CRC32 */
	gpt_header.partitions_crc32 = 0;
	n = crc32(0, Z_NULL, 0);
	n = crc32(n, (const Bytef *)partition_entries,
	          gpt_header.num_partitions * gpt_header.partitions_entry_size);
	gpt_header.partitions_crc32 = n;

	/* Compute gpt header CRC32 */
	gpt_header.crc32 = 0;
	n = crc32(0, Z_NULL, 0);
	n = crc32(n, (const Bytef *)&gpt_header, gpt_header.size);
	gpt_header.crc32 = n;

	header.gpt_primary_header = gpt_header;

	/* Modify the footer (backup header) slightly */
	lba_tmp                   = gpt_header.lba_backup;
	gpt_header.lba_backup     = gpt_header.lba_primary;
	gpt_header.lba_primary    = lba_tmp;
	gpt_header.lba_partitions = num_lbas - gpt_header.lba_first_usable + 1;

	/* Re-compute gpt footer CRC32 */
	gpt_header.crc32 = 0;
	n = crc32(0, Z_NULL, 0);
	n = crc32(n, (const Bytef *)&gpt_header, gpt_header.size);
	gpt_header.crc32 = n;

	footer.gpt_backup_header = gpt_header;

	/* Write LBA0 (null MBR, MS-DOS partition table) and primary GPT header */
	if (write(fd, &header, sizeof(header)) != sizeof(header)) {
		fprintf(stderr, "error writing header to %s (%s)\n",
		                argv[1], strerror(errno));
		goto err_close;
	}

	/* Write GPT partition table (primary) */
	if (write(fd, &partition_entries,
	          sizeof(partition_entries)) != sizeof(partition_entries)) {
		fprintf(stderr, "error writing header to %s (%s)\n",
		                argv[1], strerror(errno));
		goto err_close;
	}

	/* Scan to ~end of disk to write backups */

	if (lseek64(fd, gpt_header.lba_partitions * 512, SEEK_SET) < 0) {
		fprintf(stderr, "error seeking to offset %llu (%s)\n",
		                (unsigned long long)gpt_header.lba_partitions * 512,
		                strerror(errno));
		goto err_close;
	}

	/* Write GPT partition table (backup) */
	if (write(fd, &partition_entries,
	          sizeof(partition_entries)) != sizeof(partition_entries)) {
		fprintf(stderr, "error writing header to %s (%s)\n",
		                argv[1], strerror(errno));
		goto err_close;
	}

	/* Write GPT header backup to terminal LBA */
	if (write(fd, &footer, sizeof(footer)) != sizeof(footer)) {
		fprintf(stderr, "error writing footer to %s (%s)\n",
		                argv[1], strerror(errno));
		goto err_close;
	}

	/* Some kernels have issues syncing the writes at close time */
	if (fsync(fd)) {
		fprintf(stderr, "error calling fsync on %s (%s)\n",
		                argv[1], strerror(errno));
		goto err_close;
	}

	if ((stat.st_mode & S_IFMT) == S_IFBLK) {
		char partition_bdev[512];
		int i;

		/* Ask kernel to re-read the partition table */
		if (ioctl(fd, BLKRRPART, NULL) < 0) {
			fprintf(stderr, "error re-reading partition table for %s (%s)\n",
			                argv[1], strerror(errno));
			goto err_close;
		}

		close(fd);

		snprintf(partition_bdev, sizeof(partition_bdev), "%s1", argv[1]);

		/* The new partition block device might take a while to appear.
		 * Give it 1000ms..
		 */
		for (i = 0; i < 20; i++) {
			fd = open(partition_bdev, O_RDWR);
			if (fd >= 0)
				break;
			usleep(50 * 1000);
		}

		if (i == 20) {
			fprintf(stderr, "error opening %s (%s)\n",
			                partition_bdev, strerror(errno));
			goto err_out;
		}

		if (ioctl(fd, BLKGETSIZE64, &size) < 0) {
			fprintf(stderr, "BLKGETSIZE64 failed on %s (%s)\n",
			                partition_bdev, strerror(errno));
			goto err_close;
		}

		reset_ext4fs_info();

		info.len = size;
		info.label = "Android";

		if (make_ext4fs_internal(fd, NULL, NULL, NULL, 0, 0, 0, 0, NULL, 0, -1, NULL)) {
			fprintf(stderr, "mkfs.ext4 failed on %s\n", partition_bdev);
			goto err_close;
		}
	}

	err = 0;
err_close:
	close(fd);
err_out:
	return err;
}
