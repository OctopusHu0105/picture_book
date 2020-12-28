#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include <freertos/semphr.h>
#include "esp_audio.h"
#include "esp_log.h"
#include "audio_player.h"
#include "audio_error.h"
#include "vt_handle.h"
#include "vt_types.h"
#include "vt_media.h"


static char *TAG = "VT MEDIA";

#ifdef PRINTF_CAMERA_TIME
extern uint64_t get_url_sta_time;
extern uint64_t get_rec_sta_time;
#endif

typedef struct {
    pfun_vt_player_state_cb state_cb;
    pfun_vt_task_cb task_cb;
	
    void *userp;
    void *task_ctx;

	char url[256];

	vt_mediaplayer_state states;
} esp_vt_media_handle_t;

esp_vt_media_handle_t media_handle = {
	.userp    = NULL,
	.task_cb  = NULL,
	.state_cb = NULL,
	.task_ctx = NULL,
	.states   = 0,
};

static void esp_vt_media_task(void *args)
{
	esp_vt_media_handle_t *obj = (esp_vt_media_handle_t *)args;
	
	while (1) {
		/*函数返回了 <0 表明任务请求退出TASK*/
		if (obj->task_cb(obj->task_ctx, E_VT_TASKS_RUNNING) < 0) break;
		
	}

	obj->task_cb(obj->task_ctx, E_VT_TASKS_EXITED);
	ESP_LOGI(TAG, "vt media exit");
	vTaskDelete(NULL);
}

static int esp_vt_meida_init_url(char *url)
{
	ESP_LOGI(TAG, "vt_meida_init_url: %s ", url);
	snprintf(media_handle.url, 256, "%s", url);
	return 0;
}

static int esp_vt_meida_play()
{
	ESP_LOGI(TAG, "vt_meida_play: url : %s", media_handle.url);
	media_handle.states = E_VTPLAYER_STATE_PLAYING;
	
#ifdef PRINTF_CAMERA_TIME
    ESP_LOGW(TAG, "get info time : %llums \n", (get_rec_sta_time - get_url_sta_time) / 1000);
	ESP_LOGW(TAG, "get url  time : %llums \n", (esp_timer_get_time() - get_rec_sta_time) / 1000);
#endif
	audio_player_music_play(media_handle.url, 0, MEDIA_SRC_TYPE_MUSIC_HTTP);
	return 0;
}

static int esp_vt_meida_stop()
{
	ESP_LOGI(TAG, "vt_meida_stop");
	media_handle.states = E_VTPLAYER_STATE_STOPED;
	audio_player_stop();
	return 0;
}

static int esp_vt_meida_pause()
{
	ESP_LOGI(TAG, "vt_meida_pause");
	media_handle.states = E_VTPLAYER_STATE_PAUSED;
	audio_player_stop();
	return 0;
}

static int esp_vt_meida_resume()
{
	ESP_LOGI(TAG, "vt_meida_resume");
	if (media_handle.states != E_VTPLAYER_STATE_PAUSED) 
		return 0;
	
	media_handle.states = E_VTPLAYER_STATE_PLAYING;
	
	return 0;
}

static int esp_vt_meida_is_playing(char *url)
{
	ESP_LOGI(TAG, "vt_meida_is_playing");
	return media_handle.states == E_VTPLAYER_STATE_PLAYING;
}

static int esp_vt_meida_set_state(pfun_vt_player_state_cb state_cb, void *userp)
{
	ESP_LOGI(TAG, "vt_meida_set_state");
    esp_vt_media_handle_t *handle = &media_handle;
    handle->state_cb = state_cb;
	handle->userp = userp;
	return 0;
}

static int esp_vt_meida_start_task(pfun_vt_task_cb taskcb, void *ctx)
{
	ESP_LOGI(TAG, "vt_meida_start_task");
    esp_vt_media_handle_t *handle = &media_handle;
    handle->task_cb = taskcb;
	handle->task_ctx = ctx;

	xTaskCreatePinnedToCore(esp_vt_media_task, "v_m",  4 * 1024, handle,  8, NULL, 1);
	return 0;
}

void vt_meida_state_notiy(vt_mediaplayer_state state)
{
	ESP_LOGI(TAG, "vt_meida_state_notiy");
	esp_vt_media_handle_t *handle = &media_handle;
	handle->states = state;
	AUDIO_MEM_CHECK(TAG, handle->url, return);
	AUDIO_MEM_CHECK(TAG, handle->state_cb, return);
	handle->state_cb(handle->url, state, handle->userp);
}

void *esp_vt_meida_init()
{
	media_player_ops_t *ops = heap_caps_calloc(1, sizeof(media_player_ops_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	ops->init 				= esp_vt_meida_init_url;
	ops->play 				= esp_vt_meida_play;
	ops->stop 				= esp_vt_meida_stop;
	ops->pause 				= esp_vt_meida_pause;
	ops->resume 			= esp_vt_meida_resume;
	ops->playing 			= esp_vt_meida_is_playing;
	ops->set_state_cb 		= esp_vt_meida_set_state;
	ops->player_start_task 	= esp_vt_meida_start_task;
	return ops;
}
