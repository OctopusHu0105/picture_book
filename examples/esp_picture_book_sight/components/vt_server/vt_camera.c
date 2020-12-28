#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "vt_handle.h"
#include "vt_types.h"
#include "vt_camera.h"
#include "esp_camera.h"
#include "lwip/sockets.h"
#include "tcpip_adapter.h"
#include "esp_transport_tcp.h"
#include "audio_mem.h"

static const char *TAG = "VT_CAMERA";

// #define SEND_PCCTURE_TO_HTTP

#ifdef SEND_PCCTURE_TO_HTTP
#include "lwip/sockets.h"
#include "tcpip_adapter.h"
#include "esp_transport_tcp.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#endif

#ifdef PRINTF_CAMERA_TIME
uint64_t get_url_sta_time;
uint64_t get_rec_sta_time;
#endif

#ifdef USE_MORE_TASK
typedef enum {
   
    PICTURE_YUV = 0,
    PICTURE_JPG = 1,
} vt_picture_type_t;

typedef struct {
    uint8_t           *addr;
	int               len;
	vt_picture_type_t type;                           
} vt_picture_info_t;

static QueueHandle_t queue_cb = NULL;
static QueueHandle_t queue_p_cb = NULL;

#endif

typedef struct {
    void *camera;
    void *userp;
    void *task_ctx;
	
    pfun_camera_preview_cb preview_cb;
    pfun_camera_picture_cb picture_cb;
    pfun_vt_task_cb task_cb;
	
	uint8_t u_jpg : 1;
	uint8_t u_ctl : 1;
} esp_vt_camera_handle_t;

static esp_vt_camera_handle_t camera_handle = {
	.camera     = NULL,
	.userp      = NULL,
	.task_ctx   = NULL,
	.preview_cb = NULL,
	.picture_cb = NULL,
	.task_cb    = NULL,
	.u_jpg      = 0,
	.u_ctl      = 0,
};

static void esp_vt_camera_sdk_task(void *args) 
{
	esp_vt_camera_handle_t *obj = (esp_vt_camera_handle_t *)args;
	
	while (1) {

		if (obj->task_cb(obj->task_ctx, E_VT_TASKS_RUNNING) < 0) break;
		
	}

	obj->task_cb(obj->task_ctx, E_VT_TASKS_EXITED);
	ESP_LOGD(TAG, "camera exit");
	vTaskDelete(NULL);
}

static void esp_vt_camera_image_task(void *args)
{
	esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)args;
#ifndef USE_MORE_TASK
    size_t len;
    uint8_t *buf = NULL;
#endif
    
#ifdef SEND_PCCTURE_TO_HTTP
	esp_transport_handle_t transport = esp_transport_tcp_init();
	int sock = esp_transport_connect(transport, "192.168.1.111", 8078, 10000000);
	printf("socket: %d\n", sock);
#endif

	camera_fb_t *fb = NULL;

#ifdef PRINTF_CAMERA_TIME
	int picture_count = 0, preview_count = 0;
	uint64_t picture_time = 0, preview_time = 0;
    uint64_t sta_time = 0, end_time = 0;
    get_url_sta_time = 0;
	get_rec_sta_time = 0;
#endif

#ifdef USE_MORE_TASK
    vt_picture_info_t picture_info;
#endif

	while (handle->u_ctl != 1) {
#ifdef PRINTF_CAMERA_TIME
        sta_time = esp_timer_get_time();
#endif
		fb = esp_camera_fb_get();
		
		if (fb == NULL) {
			continue;
		}
#ifdef PRINTF_CAMERA_TIME
		end_time = esp_timer_get_time();
		picture_count++;
		picture_time += (end_time - sta_time);
#endif

#ifdef USE_MORE_TASK
		picture_info.addr = fb->buf;
		picture_info.len = fb->len;
#endif		
	if (handle->u_jpg == 1) {

#ifdef PRINTF_CAMERA_TIME
			sta_time = esp_timer_get_time();
#endif

#ifdef USE_MORE_TASK
        picture_info.type = PICTURE_JPG;
		xQueueSend(queue_p_cb, &picture_info, 500 / portTICK_PERIOD_MS);
#else
        frame2jpg(fb, 80, &buf, &len);
		handle->picture_cb(buf,len, handle->userp);
		
#ifdef SEND_PCCTURE_TO_HTTP
		int wlen = esp_transport_write(transport, (char *)buf, len, 10000000);
        printf("wlen %d \n",  wlen);
#endif
		
#endif

#ifdef PRINTF_CAMERA_TIME
			end_time = esp_timer_get_time();
			ESP_LOGW(TAG, " picture             time %llums", (end_time - sta_time) / 1000);
			ESP_LOGW(TAG, " picture_count : %d, time %llums", picture_count, picture_time / 1000);
			ESP_LOGW(TAG, " preview_count : %d  time %llums", preview_count, preview_time / 1000);
			picture_count = 0; 
			preview_count = 0;
			preview_time = 0;
			picture_time = 0;
			get_rec_sta_time = 0;
			get_url_sta_time = esp_timer_get_time();
#endif		
			handle->u_jpg = 0;
		} 
		else {
#ifdef PRINTF_CAMERA_TIME
			sta_time = esp_timer_get_time();
#endif
#ifdef USE_MORE_TASK
			picture_info.type = PICTURE_YUV;
			xQueueSend(queue_cb, &picture_info, 500 / portTICK_PERIOD_MS);
#else
			handle->preview_cb(fb->buf, fb->len, handle->userp);
#endif

#ifdef PRINTF_CAMERA_TIME
			end_time = esp_timer_get_time();
			preview_count++;
			preview_time += (end_time - sta_time);
#endif
		}

#ifndef USE_MORE_TASK
		RELEASE(buf, free);
#endif
		RELEASE(fb, esp_camera_fb_return);	
	}
	
	ESP_LOGD(TAG, "image task exit");
	vTaskDelete(NULL);
}

#ifdef USE_MORE_TASK
static void esp_vt_camera_cb_task(void *args)
{
	esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)args;
    vt_picture_info_t picture_info = {0};

	while(1) {
		if (xQueueReceive(queue_cb, &picture_info, portMAX_DELAY) == pdTRUE) {
			if (picture_info.type == PICTURE_YUV) {
				handle->preview_cb(picture_info.addr, picture_info.len, handle->userp);
			}
		} else {
			vTaskDelay(1);
		}

	}
	vTaskDelete(NULL);
}

static void esp_vt_camera_picture_cb_task(void *args)
{
	esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)args;
    vt_picture_info_t picture_info = {0};
     size_t len;
    uint8_t *buf = NULL;

	while(1) {
		if (xQueueReceive(queue_p_cb, &picture_info, portMAX_DELAY) == pdTRUE) {
			if (picture_info.type == PICTURE_JPG) {
				fmt2jpg(picture_info.addr, picture_info.len, 320, 240, PIXFORMAT_YUV422, 80, &buf, &len);
				handle->picture_cb(buf,len, handle->userp);
				RELEASE(buf, free);
				continue;
			}
		} else {
			vTaskDelay(1);
		}

	}
	vTaskDelete(NULL);
}
#endif

static void *esp_vt_camera_open(void)
{
	ESP_LOGD(TAG, "esp_vt_camera_open");
    return &camera_handle;
}

static int esp_vt_camera_release(void *dev)
{
	ESP_LOGD(TAG, "esp_vt_camera_release");
    return 0;
}

static int esp_vt_camera_set_params(void *dev, vt_camera_param_t *params)
{	
	ESP_LOGD(TAG, "esp_vt_camera_set_params");
    return 0;
}

static int esp_vt_camera_get_params(void *dev, vt_camera_param_t *params)
{
	ESP_LOGD(TAG, "esp_vt_camera_get_params");
    return 0;
}

static int esp_vt_camera_set_preview(void *dev, pfun_camera_preview_cb preview_cb, void *userp)
{
	ESP_LOGD(TAG, "esp_vt_camera_set_preview");
    esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)dev;
	handle->userp = userp;
	handle->preview_cb = preview_cb;
	return 0;
}

static int esp_vt_camera_set_picture(void *dev, pfun_camera_picture_cb picture_cb, void *userp)
{
	ESP_LOGD(TAG, "esp_vt_camera_set_picture");
    esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)dev;
	handle->userp = userp;
	handle->picture_cb = picture_cb;
    return 0;
}

static int esp_vt_camera_start_preview(void *dev)
{
	ESP_LOGD(TAG, "esp_vt_camera_start_preview");
    esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)dev;
    handle->u_ctl = 0;
    
	xTaskCreatePinnedToCore(esp_vt_camera_image_task,   "esp_vt_camera image task",  4 * 1024, dev, 15, NULL, 1);

#ifdef USE_MORE_TASK
	if (queue_cb == NULL) {
		queue_cb = xQueueCreate(1, sizeof(vt_picture_info_t));
	}

	if (queue_p_cb == NULL) {
		queue_p_cb = xQueueCreate(1, sizeof(vt_picture_info_t));
	}
	xTaskCreatePinnedToCore(esp_vt_camera_cb_task,   "esp_vt_camera cb task",  4 * 1024, dev, 13, NULL, 1);
	xTaskCreatePinnedToCore(esp_vt_camera_picture_cb_task,   "esp_vt_camera picture cb task",  4 * 1024, dev, 13, NULL, 1);
#endif

    return 0;
}

static int esp_vt_camera_stop_preview(void *dev)
{
	ESP_LOGD(TAG, "esp_vt_camera_stop_preview");
    esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)dev;
    handle->u_ctl = 1;	
	return 0;
}

static int esp_vt_camera_take_picture(void *dev)
{
	ESP_LOGE(TAG, "esp_vt_camera_take_picture");
    esp_vt_camera_handle_t *handle = (esp_vt_camera_handle_t *)dev;
    handle->u_jpg = 1;
	
    return 0;
}

static int esp_vt_camera_start_task(pfun_vt_task_cb taskcb, void *ctx) 
{	
	ESP_LOGD(TAG, "esp_vt_camera_start_task");
    esp_vt_camera_handle_t *handle = &camera_handle;
    handle->task_cb = taskcb;
	handle->task_ctx = ctx;

	xTaskCreatePinnedToCore(esp_vt_camera_sdk_task,   "v_c",  4 * 1024, handle, 10, NULL, 1);
	return 0;
}

void *esp_vt_camera_init()
{
	camera_ops_t *ops = heap_caps_calloc(1, sizeof(camera_ops_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    ops->camera_open 			= esp_vt_camera_open;
    ops->camera_release 		= esp_vt_camera_release;
    ops->camera_set_params 		= esp_vt_camera_set_params;
    ops->camera_get_params 		= esp_vt_camera_get_params;
    ops->camera_set_preview_cb 	= esp_vt_camera_set_preview;
    ops->camera_set_picture_cb 	= esp_vt_camera_set_picture;
    ops->camera_start_preview 	= esp_vt_camera_start_preview;
    ops->camera_stop_preview 	= esp_vt_camera_stop_preview;
    ops->camera_take_picture 	= esp_vt_camera_take_picture;
    ops->camera_start_task 		= esp_vt_camera_start_task;

	return ops;
}
