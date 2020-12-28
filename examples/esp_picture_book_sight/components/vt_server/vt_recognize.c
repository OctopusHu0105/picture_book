#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "vt_handle.h"
#include "vt_http.h"
#include "vt_recognize.h"
#include "audio_player.h"

static char *TAG = "VT RECOGNIZE";
typedef struct {
    pfun_vt_task_cb task_cb;
    void *task_ctx;
} vt_recognize_handle_t;

vt_recognize_handle_t vt_recognize_handle;

#ifdef PRINTF_CAMERA_TIME
extern uint64_t get_rec_sta_time;
#endif

int esp_vt_recongnize_info_cb(_recongnize_page_info_t *pageinfo, _recongnize_resinfo_t *resinfo, void *recong_userp)
{
	ESP_LOGD(TAG, "vt_recongnize_info_cb");
#ifdef PRINTF_CAMERA_TIME
	get_rec_sta_time = esp_timer_get_time();
#endif
	audio_player_stop();
	
	return 0;
}

int esp_vt_recongnize_error_cb(int errorCode, void *userp)
{
	ESP_LOGD(TAG, "esp_vt_recongnize_error_cb code %d", errorCode);
	return 0;
}

static void esp_vt_recognize_task(void *args)
{
	vt_recognize_handle_t *obj = (vt_recognize_handle_t *)args;
	
	while (1) {
		
		/*函数返回了 <0 表明任务请求退出TASK*/
		if (obj->task_cb(obj->task_ctx, E_VT_TASKS_RUNNING) < 0) break;
		
	}

	obj->task_cb(obj->task_ctx, E_VT_TASKS_EXITED);
	ESP_LOGD(TAG, "vt media exit");
	vTaskDelete(NULL);
}

static int esp_vt_recognize_start_task(pfun_vt_task_cb taskcb, void *ctx)
{
	ESP_LOGD(TAG, "vt_recognize_start_task");
	vt_recognize_handle.task_cb = taskcb;
	vt_recognize_handle.task_ctx = ctx;

	xTaskCreatePinnedToCore(esp_vt_recognize_task,   "v_r",  4 * 1024, &vt_recognize_handle,  12, NULL, 1);
	return 0;
}

void *esp_vt_recognize_init()
{
	_recog_ops_t *ops = heap_caps_calloc(1, sizeof(_recog_ops_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	ops->request_func	  =  esp_vt_http_recog_req_cb;
	ops->recog_start_task =  esp_vt_recognize_start_task;
	
	return ops;
}
