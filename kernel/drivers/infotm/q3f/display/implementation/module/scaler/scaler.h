#ifndef __SCALER_H__
#define __SCALER_H__
enum {
	SCALER_IDS,
	SCALER_G2D,
	SCALER_CPU,
};

extern int dss_scaler_ids(int src_vic, int dst_vic);
extern int dss_scaler_g2d(int src_vic, int dst_vic);
extern int dss_scaler_cpu(int src_vic, int dst_vic);
extern int dss_scale(int idsx, int src_vic, int dst_vic, int num);
#endif
