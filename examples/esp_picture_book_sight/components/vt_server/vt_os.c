#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "vt_os.h"
#include "vt_http.h"
#include "vt_types.h"

static char *TAG = "vt_os";

#define VT_USE_MALLOC_PSRAM

static void *esp_vt_os_create_event(uint8_t process_shared)
{
	ESP_LOGD(TAG, "vt_os_create_event");
	StaticQueue_t *s_queue =  heap_caps_malloc(sizeof(StaticQueue_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	return xSemaphoreCreateBinaryStatic(s_queue);
}

static int esp_vt_os_post_event(void *event)
{
	ESP_LOGD(TAG, "vt_os_post_event");
	
	return xSemaphoreGive(event);
}

static int esp_vt_os_wait_event(void *event, void *mutex, int time_ms)
{
	ESP_LOGD(TAG, "vt_os_wait_event");
	if (time_ms == -1) 
		return xSemaphoreTake(event, portMAX_DELAY);
	else
		return xSemaphoreTake(event, pdMS_TO_TICKS(time_ms));
}

static int esp_vt_os_destory_event(void *event)
{
	ESP_LOGD(TAG, "vt_os_destory_event");
	vSemaphoreDelete(event);
	return 0;
}

static void *esp_vt_os_create_semaphone(uint8_t process_shared, int max_count, int init_count)
{
	ESP_LOGD(TAG, "vt_os_create_semaphone");
	return xSemaphoreCreateCounting(max_count, init_count);
}

static int esp_vt_os_post_semaphone(void *semaphone)
{
	ESP_LOGD(TAG, "vt_os_post_semaphone");
	return xSemaphoreGive(semaphone);
}

static int esp_vt_os_wait_semaphone(void *semaphone, int time_ms)
{
	ESP_LOGD(TAG, "vt_os_wait_semaphone");
	if (time_ms == -1) 
		return xSemaphoreTake(semaphone, portMAX_DELAY);
	else
		return xSemaphoreTake(semaphone, pdMS_TO_TICKS(time_ms));
}

static int esp_vt_os_destory_semaphone(void *semaphone)
{
	ESP_LOGD(TAG, "vt_os_destory_semaphone");
	vSemaphoreDelete(semaphone);
	return 0;
}

static void *esp_vt_os_create_mutex(uint8_t process_shared)
{
	ESP_LOGD(TAG, "vt_os_create_mutex");
	return xSemaphoreCreateMutex();
}

static int esp_vt_os_mutex_lock(void *mutex)
{
	ESP_LOGD(TAG, "vt_os_mutex_lock");
	return xSemaphoreTake(mutex, portMAX_DELAY);
}

static int esp_vt_os_mutex_unlock(void *mutex)
{
	ESP_LOGD(TAG, "vt_os_mutex_unlock");
	return xSemaphoreGive(mutex);
}

static int esp_vt_os_destory_mutex(void *mutex)
{
	ESP_LOGD(TAG, "vt_os_destory_mutex");	
	vSemaphoreDelete(mutex);
	return 0;
}

static void *esp_vt_os_create_timer(vt_timer_cb cb, int period_ms, uint8_t one_short)
{
	ESP_LOGD(TAG, "vt_os_create_timer %x", one_short);
	return xTimerCreate("v_t", pdMS_TO_TICKS(period_ms), one_short, NULL, cb);
}

static int esp_vt_os_start_timer(void *timer)
{
	ESP_LOGD(TAG, "vt_os_start_timer");
	return xTimerStart(timer, 0);
}

static int esp_vt_os_stop_timer(void *timer)
{
	ESP_LOGD(TAG, "vt_os_stop_timer");
	return xTimerStop(timer, 0);
}

static int esp_vt_os_delete_timer(void *timer)
{
	ESP_LOGD(TAG, "vt_os_delete_timer");
	return xTimerDelete(timer, 0);
}

static void *esp_vt_os_create_message_queue(int queue_size, int item_length)
{
	ESP_LOGD(TAG, "vt_os_create_message_queue");
	return xQueueCreate(queue_size, item_length);
}

static int esp_vt_os_send_message(void *queue, const void *data, int wait_time_ms, uint8_t send_front)
{
	ESP_LOGD(TAG, "vt_os_send_message");
	if (send_front) 
		return xQueueSendToFront(queue, data, pdMS_TO_TICKS(wait_time_ms));
	else
		return xQueueSendToBack(queue, data, pdMS_TO_TICKS(wait_time_ms));
}

static int esp_vt_os_recv_message(void *queue, const void *data, int wait_time_ms, uint8_t peek_msg)
{
	ESP_LOGD(TAG, "vt_os_recv_message");
	if (peek_msg)
		return xQueuePeek(queue, (void *)data, pdMS_TO_TICKS(wait_time_ms));
	else
		return xQueueReceive(queue, (void *)data, pdMS_TO_TICKS(wait_time_ms));
}

static int esp_vt_os_get_message_size(void *queue)
{
	ESP_LOGD(TAG, "vt_os_get_message_size");
	//return uxQueueGetQueueNumber(queue);
	return uxQueueMessagesWaiting(queue);
}

static int esp_vt_os_destory_msgqueue(void *queue)
{
	ESP_LOGD(TAG, "vt_os_destory_msgqueue");
	vQueueDelete(queue);
	return 0;
}

static void esp_vt_os_task_delay(int time_ms)
{
	ESP_LOGD(TAG, "vt_os_task_delay %d", time_ms);
	vTaskDelay(pdMS_TO_TICKS(time_ms));
}

static unsigned long esp_vt_os_get_timestamp(void)
{
	ESP_LOGD(TAG, "vt_os_get_timestamp");
	// return (unsigned long)esp_log_timestamp();
    return 0;
}

static void *esp_vt_os_vt_plat_malloc(unsigned long size)
{
#ifdef VT_USE_MALLOC_PSRAM
    void *ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    void *ptr = malloc(size);
#endif
	
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "plat malloc %ld %p", size, ptr);
	return ptr;
}

static void *esp_vt_os_vt_plat_calloc(unsigned long nmemb, unsigned long size)
{
#ifdef VT_USE_MALLOC_PSRAM
	void *ptr = heap_caps_calloc(nmemb, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    void *ptr = calloc(nmemb, size);
#endif
	
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "plat calloc %ld %ld %p", nmemb, size, ptr);
	return ptr;
}

static void *esp_vt_os_vt_plat_realloc(void *ptr, unsigned long size)
{
#ifdef VT_USE_MALLOC_PSRAM
	void *ptx = heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    void *ptx = realloc(ptr, size);
#endif
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "plat calloc %p %ld %p", ptr, size, ptx);
	return ptx;
}

static void esp_vt_os_vt_plat_free(void *ptr)
{
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "plat free %p", ptr);
#ifdef VT_USE_MALLOC_PSRAM
	heap_caps_free(ptr);
#else
    free(ptr);
#endif
}

static void *esp_vt_os_vt_alg_malloc(unsigned long size)
{
	
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "alg malloc %ld %p", size, ptr);
	return ptr;
}

static void *esp_vt_os_vt_alg_realloc(void *ptr, unsigned long size)
{
	void *ptx = realloc(ptr, size);
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "alg calloc %p %ld %p", ptr, size, ptx);
	return ptx;
}

static void esp_vt_os_vt_alg_free(void *ptr)
{
	if (ptr == NULL) {
		printf("vt os ptr is null %s %d\n", __func__, __LINE__);
	}
	ESP_LOGD(TAG, "alg free %p", ptr);
	free(ptr);
}

void *esp_vt_os_init()
{
	vt_os_ops_t *ops = heap_caps_calloc(1, sizeof(vt_os_ops_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	ops->create_event 			= esp_vt_os_create_event;
    ops->post_event 			= esp_vt_os_post_event;
    ops->wait_event 			= esp_vt_os_wait_event;
    ops->destory_event 			= esp_vt_os_destory_event;
    ops->create_semaphone 		= esp_vt_os_create_semaphone;
    ops->post_semaphone 		= esp_vt_os_post_semaphone;
    ops->wait_semaphone 		= esp_vt_os_wait_semaphone;
    ops->destory_semaphone 		= esp_vt_os_destory_semaphone;
    ops->create_mutex 			= esp_vt_os_create_mutex;
    ops->mutex_lock 			= esp_vt_os_mutex_lock;
    ops->mutex_unlock 			= esp_vt_os_mutex_unlock;
    ops->destory_mutex 			= esp_vt_os_destory_mutex;
    ops->create_timer 			= esp_vt_os_create_timer;
    ops->start_timer 			= esp_vt_os_start_timer;
    ops->stop_timer 			= esp_vt_os_stop_timer;
    ops->delete_timer 			= esp_vt_os_delete_timer;
    ops->create_message_queue 	= esp_vt_os_create_message_queue;
    ops->send_message 			= esp_vt_os_send_message;
    ops->recv_message 			= esp_vt_os_recv_message;
    ops->get_message_size 		= esp_vt_os_get_message_size;
    ops->destory_msgqueue 		= esp_vt_os_destory_msgqueue;
    ops->task_delay 			= esp_vt_os_task_delay;
    ops->get_timestamp 			= esp_vt_os_get_timestamp;
    ops->vt_plat_malloc 		= esp_vt_os_vt_plat_malloc;
    ops->vt_plat_calloc 		= esp_vt_os_vt_plat_calloc;
    ops->vt_plat_realloc 		= esp_vt_os_vt_plat_realloc;
    ops->vt_plat_free 			= esp_vt_os_vt_plat_free;
    ops->vt_alg_malloc 			= esp_vt_os_vt_alg_malloc;
    ops->vt_alg_realloc 		= esp_vt_os_vt_alg_realloc;
    ops->vt_alg_free 			= esp_vt_os_vt_alg_free;
    ops->http_cancle 			= esp_vt_http_close;

	return ops;
}
