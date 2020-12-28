#ifndef __VT_HTTP_H__
#define __VT_HTTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vt_types.h"

#define VT_HEAD_BOUNDARY				"----dyboundaryXd96AztrPD9eZT"
#define VT_HEAD_BOUNDARY_LEN			(sizeof(VT_HEAD_BOUNDARY) - 1)
#define VT_TAIL_BOUNDARY				"\r\n------dyboundaryXd96AztrPD9eZT--\r\n"
#define VT_TAIL_BOUNDARY_LEN			(sizeof(VT_TAIL_BOUNDARY) - 1)
#define VT_HTTP_BUFF_SIZE	 			(4 * 1024)

#define VT_HTTP_BODY_FIELD				"--"VT_HEAD_BOUNDARY"\r\n"\
										"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"\
										"Content-Type: %s\r\n\r\n"

int esp_vt_http_close(void *params);

int esp_vt_http_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond);

int esp_vt_http_comm_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond);

int esp_vt_http_recog_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond);

#ifdef __cplusplus
}
#endif

#endif //__VT_HTTP_H__



