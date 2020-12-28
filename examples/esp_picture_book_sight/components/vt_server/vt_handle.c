#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "audio_error.h"
#include "vt_handle.h"
#include "vt_bk.h"
#include "vt_os.h"
#include "vt_http.h"
#include "vt_types.h"
#include "vt_media.h"
#include "vt_camera.h"
#include "vt_recognize.h"

static const char *TAG = "ESP_VT_HANDLE";

#define VT_TEST_LICENSE 	"ODU3RDM1MUJGRTVENEIwMkE3MTMyRTFBOUUzODk4QUE5OTBEN0YxNzc2ODVDMEZEN0YxM0MzNUYxMDVGNURDMkQxNjc5MDE0RUMxMjk3RjMxNDRDOTJEQjBEOUIzQ0ZEMDJBNjBCNkQyMUE5MEVBNkU5NEQ2OTgyODAwQkYwMUVBOTU2QzdFQjhDQjUxODQ2QUYzODI0QkNCNjdDNzY2NDI1MTQ3NzA0RTM2MTY1RjhCMjMyN0EyMjU0MENGQzdENDkyMDFDRkUzNzIwMjRFNjk3MENGQTE3MUMzOEVGODA1QTVBQUQ3NzQ0ODMzMzZDNzExMkY1RkJFM0JCREQ3OTlGNDc4NjhEQUU4ODcyNEE1NzExNjQ3RDIwQzJFRkY4OUM2RURGRjVFQjVERDUzNkE2MDY0NkJGQzZFRDc3NUVFMkRBMTVENUFGMkVENDY3MjdDRkZFOUZDQzEyMjkwODVERkFDRUU0MzdBMUJEMEI1MTM5MTU0M0U0NDRGNUUzNEFFQTIxNTAyMjBGM0EzRDJDMjM3RTA1Nzk3Q0E4Q0Q1NDlGQ0MzQzI0QUUxMjQ5NDA0NDQ2NUFDQUQ4NUJCQUUyNkMzMjBEMDUyNkFFMDc1MUE5MDc5M0M2MEM3Q0Q0MUE1N0E0OEU2Njg3NkFBMEE1M0VDNzBBRDIzNjYzMzMxODBBMDQ2QTMzOUU3QzFDRTNEQzEwQjMyQkJCOUUzNDI4OTQ1NDFFNjQ4OTEzNTc2QTdFOTE1NjdCNkY5MDM0"
#define VT_TEST_MODLE_TYPE	"esp32_dd_default"
#define VT_TEST_USER_ID		"esp32_test"

#define VT_STATE_PARAM_DEFAULT() { \
	.x            = 0,             \
	.y            = 0,             \
	.width        = 320,           \
	.height       = 240,           \
	.w            = 320,           \
	.h            = 240,           \
	.channels     = 1,             \
	.propose_hand = 0,             \
	.handle_edge  = 0,             \
	.thresh       = 2,             \
	.ds_factor    = 4,             \
	.in_type      = 1,             \
	.fds          = 0              \
}

typedef enum {
	ESP_VT_STATE_IDLE,
	ESP_VT_STATE_INIT,
	ESP_VT_STATE_READY,
	ESP_VT_STATE_RUNNING,
	ESP_VT_STATE_PAUSE,
	ESP_VT_STATE_STOPPED,
	ESP_VT_STATE_DEINIT,
} esp_vt_state_t;

typedef struct {
	char           *device;
	vt_bk_handle_t instans;
	esp_vt_state_t states;
} esp_vt_handle_t;

static esp_vt_handle_t *vt_handle;

int esp_vt_finger_state_cb(_finger_data_info_t *data, void *userp)
{
	AUDIO_NULL_CHECK(TAG, data, return ESP_FAIL);
	ESP_LOGI(TAG, "finger data, URL: %s, hot index: %d", data->url, data->hot_index);
	return 0;
}

int esp_vt_handle_state()
{
	if (vt_handle == NULL) {
		return 0;
	}
	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_READY, return -1, "vt server not ready");
	return vt_bk_get_recongstate(vt_handle->instans);
}

esp_err_t esp_vt_handle_start()
{
	if (vt_handle == NULL) {
		return ESP_FAIL;
	}
	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_READY, return -1, "vt server not ready");
	int res = vt_bk_get_recongstate(vt_handle->instans);
	ESP_LOGD(TAG, "vt_bk_get_recongstate %x", res);
	
	if (res != VT_BK_STATE_STOPED) return -1;
	
	return vt_bk_start_recognize(vt_handle->instans);
}

esp_err_t esp_vt_handle_stop()
{
	if (vt_handle == NULL) {
		return ESP_FAIL;
	}
	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_READY, return ESP_FAIL, "vt server not ready");
	int res = vt_bk_get_recongstate(vt_handle->instans);
	ESP_LOGI(TAG, "vt_bk_get_recongstate %x", res);
	
	if (res != VT_BK_STATE_STARTED) return ESP_FAIL;
	
	return vt_bk_stop_recognize(vt_handle->instans);
}

esp_err_t esp_vt_handle_pause()
{
	if (vt_handle == NULL) {
		return ESP_FAIL;
	}
	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_READY, return ESP_FAIL, "vt server not ready");
	int res = vt_bk_pause(vt_handle->instans);
	ESP_LOGI(TAG, "vt_bk_pause %x", res);

	return res;
}

esp_err_t esp_vt_handle_resume()
{
	if (vt_handle == NULL) {
		return ESP_FAIL;
	}
	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_READY, return ESP_FAIL, "vt server not ready");
	
	int res = vt_bk_resume(vt_handle->instans);
	ESP_LOGI(TAG, "vt_bk_resume %x", res);

	return res;
}

esp_err_t esp_vt_handle_login_in()
{
	int res = 0;
	if (vt_handle == NULL) {
		return ESP_FAIL;
	}

	AUDIO_CHECK(TAG, vt_handle->states == ESP_VT_STATE_INIT, return ESP_FAIL, "vt server not ready");
	res += vt_bk_env_init(vt_handle->instans, VT_TEST_LICENSE, VT_TEST_USER_ID, VT_TEST_MODLE_TYPE);
	res += vt_bk_login(vt_handle->instans, esp_vt_http_req_cb);
	AUDIO_CHECK(TAG, res == 0, return ESP_FAIL, "VT LOGIN FAIL");
	
	_recog_ops_t *recog = esp_vt_recognize_init();

	StatedetParamEx states = VT_STATE_PARAM_DEFAULT();
	res = vt_bk_register_recognize_ops(vt_handle->instans, recog, (void *)&states);
	RELEASE(recog, free);
	AUDIO_CHECK(TAG, res == 0, return ESP_FAIL, "VT RECOG FAIL");
	vt_handle->states = ESP_VT_STATE_READY;
	return ESP_OK;
}

esp_err_t esp_vt_handle_init()
{		
	int res = 0;
	vt_os_ops_t	 *osops = esp_vt_os_init();
	camera_ops_t *camer = esp_vt_camera_init();
	media_player_ops_t *media = esp_vt_meida_init();

	esp_vt_handle_t *handle = heap_caps_calloc(1, sizeof(esp_vt_handle_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
	
	handle->instans = vt_bk_instance();
	AUDIO_NULL_CHECK(TAG, handle->instans, return ESP_FAIL);

	res += vt_bk_register_os_wrapper(handle->instans, osops); 
	res += vt_bk_register_common_request(handle->instans, esp_vt_http_comm_req_cb);
	res += vt_bk_register_reconginfo_cb(handle->instans, esp_vt_recongnize_info_cb, NULL);
	res += vt_bk_register_fingerinfo_cb(handle->instans, esp_vt_finger_state_cb, NULL);
	res += vt_bk_register_error_cb(handle->instans, esp_vt_recongnize_error_cb, NULL);
	res += vt_bk_register_camera_ops(handle->instans, camer);
	res += vt_bk_register_mediaplayer_ops(handle->instans, media);
		
	RELEASE(osops, free);
	RELEASE(media, free);
	RELEASE(camer, free);
	RELEASE(vt_handle, free);
	
	vt_handle = handle;
	AUDIO_CHECK(TAG, res == 0, return ESP_FAIL, "vt server init error");
	
	handle->states = ESP_VT_STATE_INIT;
	return ESP_OK;
}
