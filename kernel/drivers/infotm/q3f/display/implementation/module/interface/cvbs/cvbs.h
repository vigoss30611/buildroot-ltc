#ifndef __CVBS_H__
#define __CVBS_H__

struct cvbs_ops {
	int (*config)(int VIC);
	int (*disable)(void);
};

#endif
