/*
 * dwl_lock.h
 *
 * This lock is based on asysops device driver to do synchronism
 * to protect 9170 hardware resource. Using shared memory to poll
 * for correspond mark.
 *
 * InfoTM All Rights Reserved.
 *
 * Sololz <sololz.luo@gmail.com>
 */

#ifndef __DWL_LOCK_H__
#define __DWL_LOCK_H__

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

#define ASYSOPS_DEV_NAME		"/dev/asysops"

/* Correspond ioctl commands macros */
#define ASYSOPS_CMD_MAGIC		'a'
#define ASYSOPS_CMD_GET_SHMEM		_IO(ASYSOPS_CMD_MAGIC, 1)
#define ASYSOPS_CMD_FREE_SHMEM		_IO(ASYSOPS_CMD_MAGIC, 2)

#define ASYSOPS_RET_EXIST		0x2b2b2b2b
#define DWL_LOCK_MARK			0x12345678
#define DWL_LOCK_SIZE			(4 * 1024)
#define DWL_LOCK_KEY			(0x72809170)

	/* Shared memory parameter structure */
	typedef struct
	{
		unsigned int    phys;
		unsigned int    size;
		unsigned int    key;
	}shmem_param_t;

	typedef struct
	{
		int		fd;
		unsigned int	key;
		unsigned int	phys;
		unsigned int	*virt;
		unsigned int	size;
	}shmem_inst_t;

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* __DWL_LOCK_H__ */
