#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "audio_error.h"
#include "vt_http.h"
#include "vt_types.h"
#include "vt_handle.h"
#include "esp_http_client.h"

static const char *TAG = "VT_HTTP";

typedef struct {
	esp_http_client_handle_t client;
	char                     *cookie;
	uint8_t                  state;
} esp_vt_http_handle_t;

static esp_vt_http_handle_t http_handle;

static int esp_vt_http_event(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            //ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            //ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
			if (strcmp(evt->header_key, "Set-Cookie") != 0) break;
			if (http_handle.cookie && strncmp(http_handle.cookie, evt->header_value, strlen(http_handle.cookie))) break;
			RELEASE(http_handle.cookie, free);
			char *end = strchr(evt->header_value, ';');
			if (end == NULL) break;
			end++;
			http_handle.cookie = calloc(1, end - evt->header_value + 1);
			if (http_handle.cookie == NULL) break;
			memcpy(http_handle.cookie, evt->header_value, end - evt->header_value);
			http_handle.cookie[end - evt->header_value] = '\0';
			ESP_LOGD(TAG, "cookie update %s", http_handle.cookie);
            break;
        case HTTP_EVENT_ON_DATA:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d chunked %d", evt->data_len, esp_http_client_is_chunked_response(evt->client));
            break;
        case HTTP_EVENT_ON_FINISH:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            //ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

static int esp_vt_http_get_handle(str_vt_networkPara *param, str_vt_networkRespond *respond, uint8_t set_cookie)
{
	int res = -1;
    esp_http_client_config_t config = {
		.url = param->url,
		.timeout_ms = 3 * 1000,
		.event_handler = esp_vt_http_event,
    };

	ESP_LOGD(TAG, "vt url : %s", config.url);
	
    esp_http_client_handle_t client = esp_http_client_init(&config);
	AUDIO_MEM_CHECK(TAG, client, return -1);
	http_handle.client = client;

	if (set_cookie && http_handle.cookie) esp_http_client_set_header(client, "Cookie", http_handle.cookie);
    esp_http_client_set_header(client, "Connection:", "close");
    
	res = esp_http_client_open(client, 0);
	AUDIO_CHECK(TAG, res == ESP_OK, goto _exit, "http open error in get handle");
     
	int len = esp_http_client_fetch_headers(client);
	AUDIO_CHECK(TAG, len != ESP_FAIL, goto _exit, "http fetch error");

	if (len == 0) len = 2048;

	respond->data = heap_caps_calloc(1, len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	AUDIO_MEM_CHECK(TAG, respond->data, goto _exit);
	
	res = esp_http_client_read(client, (char *)respond->data, len);
	
	if (res <= 0) {
		ESP_LOGE(TAG, "HTTP READ");
		res = -1;
		RELEASE(respond->data, free);
		goto _exit;
	}
	
	respond->size = res;
	ESP_LOGE(TAG, "vt http %d %s", respond->size, (char *)respond->data);

	_exit:
	esp_http_client_cleanup(client);
	http_handle.client = NULL;
	return res;
}

static int esp_vt_http_post_handle(str_vt_networkPara *param, str_vt_networkRespond *respond)
{
	int res = -1;
    esp_http_client_config_t config = {
		.url = param->url,
		.method = HTTP_METHOD_POST,
		.event_handler = esp_vt_http_event,
		.timeout_ms = 3 * 1000,
    };

	//ESP_LOGD(TAG, "vt url post %u : %s", param->data_size, config.url);
	
    esp_http_client_handle_t client = esp_http_client_init(&config);
	AUDIO_MEM_CHECK(TAG, client, return -1);
	http_handle.client = client;

	esp_http_client_set_header(client, "Cookie", http_handle.cookie);
    esp_http_client_set_header(client, "Connection:", "close");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	res = esp_http_client_open(client, param->data_size);
    AUDIO_CHECK(TAG, res == ESP_OK, goto _exit, "http open error in post handle");
	
	res = esp_http_client_write(client, (char *)param->mime_data, param->data_size);
	AUDIO_CHECK(TAG, res == param->data_size, goto _exit, "http write failed");
	
	int len = esp_http_client_fetch_headers(client);
	AUDIO_CHECK(TAG, len != ESP_FAIL, goto _exit, "http fetch error");
	if (len == 0) len = 2048;
	
	respond->data = heap_caps_calloc(1, len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	AUDIO_MEM_CHECK(TAG, respond->data, goto _exit);

	res = esp_http_client_read(client, (char *)respond->data, len);
	if (res <= 0) {
		ESP_LOGE(TAG, "HTTP READ");
		res = -1;
		RELEASE(respond->data, free);
		goto _exit;
	}
	
	respond->size = res;
	ESP_LOGD(TAG, "vt http %d %s", respond->size, (char *)respond->data);
	
_exit:
	esp_http_client_cleanup(client);
	http_handle.client = NULL;
	return res;
}

static int esp_vt_http_post_vds_handle(str_vt_networkPara *param, str_vt_networkRespond *respond)
{
	int res = -1;
    esp_http_client_config_t config = {
		.url = param->url,
		.method = HTTP_METHOD_POST,
		.buffer_size = VT_HTTP_BUFF_SIZE,
		.event_handler = esp_vt_http_event,
		.timeout_ms = 3 * 1000,
    };
	
	//ESP_LOGD(TAG, "vt url pic %u : %s", param->data_size, config.url);
    esp_http_client_handle_t client = esp_http_client_init(&config);
	AUDIO_MEM_CHECK(TAG, client, return -1);
	http_handle.client = client;

	esp_http_client_set_header(client, "Cookie", http_handle.cookie);
    esp_http_client_set_header(client, "Connection:", "close");
	esp_http_client_set_header(client, "Content-Type", "multipart/form-data; boundary=----dyboundaryXd96AztrPD9eZT");

	char *http_buff = heap_caps_calloc(1,  VT_HTTP_BUFF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	AUDIO_MEM_CHECK(TAG, http_buff, goto _exit);

	snprintf(http_buff, VT_HTTP_BUFF_SIZE, VT_HTTP_BODY_FIELD, 
			 param->mime_name, param->mime_filename, param->mime_type);

	int field_len = strlen(http_buff);
	
	res = esp_http_client_open(client, field_len + param->data_size + VT_TAIL_BOUNDARY_LEN);
	AUDIO_CHECK(TAG, res == ESP_OK, goto _exit, "http open error in post vds handle");

	res = esp_http_client_write(client, http_buff, field_len);
    AUDIO_CHECK(TAG, res == field_len, goto _exit, "http write failed");

	int sends = 0;
	int send_cnt = 0;
	char *pos = (char *)param->mime_data;
	while (send_cnt != param->data_size) {
		sends = ((send_cnt + VT_HTTP_BUFF_SIZE) < param->data_size ? VT_HTTP_BUFF_SIZE : param->data_size - send_cnt);
		if ((res = esp_http_client_write(client, (char *)(pos + send_cnt), sends)) <= 0) {
			ESP_LOGE(TAG, "Failed to write data to http stream, res = %d", res);
			goto _exit;
        }
		
		send_cnt += res;		
	}
	
	res = esp_http_client_write(client, VT_TAIL_BOUNDARY, VT_TAIL_BOUNDARY_LEN);
	AUDIO_CHECK(TAG, res == VT_TAIL_BOUNDARY_LEN, goto _exit, "http write failed");

	int len = esp_http_client_fetch_headers(client);
	AUDIO_CHECK(TAG, len != ESP_FAIL, goto _exit, "http fetch error");

	if (len == 0) len = VT_HTTP_BUFF_SIZE;
	
	res = esp_http_client_read(client, http_buff, len);
	if (res > 0) {
		ESP_LOGD(TAG, "vt res %d %s", res, http_buff);
		http_buff[res] = '\0';
		respond->size = res;		
		respond->data = http_buff;	
	} else 
		res = -1;
	
_exit:
	if (res <= 0) RELEASE(http_buff, free);
	esp_http_client_cleanup(client);
	http_handle.client = NULL;
	return res;
}

int esp_vt_http_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond)
{
	ESP_LOGD(TAG, "vt http get");
	return esp_vt_http_get_handle(param, respond, 1);
}

int esp_vt_http_comm_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond)
{
	ESP_LOGD(TAG, "vt comm http method %s", param->method ? "post" : "get");
	if (param->method == GET)
		return esp_vt_http_get_handle(param, respond, 0);
	else
		return esp_vt_http_post_handle(param, respond);
}

int esp_vt_http_recog_req_cb(str_vt_networkPara *param, str_vt_networkRespond *respond)
{
	ESP_LOGD(TAG, "vt http recognize req");
	return esp_vt_http_post_vds_handle(param, respond);
}

int esp_vt_http_close(void *params)
{
	if (!http_handle.client) return 0;
	esp_http_client_close(http_handle.client);
	return 0;
}
