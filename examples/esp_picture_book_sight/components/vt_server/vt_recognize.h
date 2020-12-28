#ifndef __VT_RECOGNIZE_H__
#define __VT_RECOGNIZE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vt_types.h"

int esp_vt_recongnize_info_cb(_recongnize_page_info_t *pageinfo, _recongnize_resinfo_t *resinfo, void *recong_userp);

int esp_vt_recongnize_error_cb(int errorCode, void *userp);

void *esp_vt_recognize_init();

#ifdef __cplusplus
}
#endif

#endif //__VT_RECOGNIZE_H__



