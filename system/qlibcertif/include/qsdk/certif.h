#ifndef __QSDK_CERTIF_H__
#define __QSDK_CERTIF_H__


#ifdef __cplusplus
extern "C" {

#endif /*__cplusplus */

extern int certif_check_algorithm_permit(int alg_id);
extern int certif_check_certification(char *certif, int certif_len, int alg_id);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __QSDK_CERTIF_H__ */
