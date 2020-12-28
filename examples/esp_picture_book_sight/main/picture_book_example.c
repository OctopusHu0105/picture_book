
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "esp_camera.h"
#include "vt_handle.h"
#include "app_player_init.h"
#include "app_sys_tool.h"
#include "pca9539.h"

#include "audio_player.h"
#define Y9_GPIO_NUM                 36
#define Y8_GPIO_NUM                 37
#define Y7_GPIO_NUM                 39
#define Y6_GPIO_NUM                 35
#define Y5_GPIO_NUM                 32
#define Y4_GPIO_NUM                 0
#define Y3_GPIO_NUM                 5
#define Y2_GPIO_NUM                 22

#define VSYNC_GPIO_NUM              19
#define HREF_GPIO_NUM               21
#define PCLK_GPIO_NUM               12
#define PWDN_GPIO_NUM               -1
#define RESET_GPIO_NUM              -1
#define XCLK_GPIO_NUM               -1
#define SIOD_GPIO_NUM               18   
#define SIOC_GPIO_NUM               23
#define XCLK_FREQ  20000000

static const char *TAG = "PICTURE_BOOK_EXAMPLE";

static esp_err_t esp_camera_start()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = XCLK_FREQ;
    config.pixel_format = PIXFORMAT_YUV422;  
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return ESP_FAIL;
    } 

    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_QVGA);
    s->set_hmirror(s, 1);
    s->set_vflip(s, 0);
    return ESP_OK;
}

void audio_player_callback(audio_player_state_t *audio, void *ctx)
{
    ESP_LOGW(TAG, "AUDIO_PLAYER_CALLBACK send OK, status:%d, err_msg:%x, media_src:%x, ctx:%p",
             audio->status, audio->err_msg, audio->media_src, ctx);
}

void audio_pca_button_callback(pca_btn_id_t id, pca_btn_state_t st)
{
     ESP_LOGW(TAG, "AUDIO_PCA_BUTTON_CALLBACK, ID:%d, st:%d",id, st);
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    esp_log_level_set("*", ESP_LOG_INFO);

    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    // esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_WARN);
    // esp_log_level_set("ESP_AUDIO_CTRL", ESP_LOG_DEBUG);
    // esp_log_level_set("ESP_AUDIO_TASK", ESP_LOG_DEBUG);
    // esp_log_level_set("AUDIO_MANAGER", ESP_LOG_DEBUG);

    esp_pca9539_config_t pca_cfg = {
         .i2c_scl = GPIO_NUM_23,
         .i2c_sda = GPIO_NUM_18,
         .interrupt_output = -1,
         .func_cb = audio_pca_button_callback,
    };

    pca9539_init(&pca_cfg);
    pca9539_set_io_config(LED1_R_CTRL, 0);
    pca9539_set_io_config(LED1_G_CTRL, 0);
    pca9539_set_io_config(LED1_B_CTRL, 0);
    pca9539_set_io_config(LED2_R_CTRL, 0);
    pca9539_set_io_config(LED2_G_CTRL, 0);
    pca9539_set_io_config(LED2_B_CTRL, 0);
    pca9539_set_io_config(PA_CTRL, 0);
    pca9539_set_io_config(CAM_RESET, 0);
    pca9539_set_io_config(TCXO_EN, 0);

    pca9539_set_output_state(LED1_R_CTRL, 0);
    pca9539_set_output_state(LED1_G_CTRL, 0);
    pca9539_set_output_state(LED1_B_CTRL, 1);
    pca9539_set_output_state(LED2_R_CTRL, 0);
    pca9539_set_output_state(LED2_G_CTRL, 0);
    pca9539_set_output_state(LED2_B_CTRL, 1);
    pca9539_set_output_state(PA_CTRL, 1);
    pca9539_set_output_state(CAM_RESET, 1);
    pca9539_set_output_state(TCXO_EN, 1);

    ESP_LOGI(TAG, "[ 3 ] Start and wait for Wi-Fi network");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    //  periph_cfg.extern_stack = true;
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    
    // The control of PA does not use MCU, and uses hardware jump wire.
    // Please set the correct i2s1 gpio in the function "get_i2s_pins"
    // please add "tas5805m_set_damp_mode(TAS5805M_DAMP_MODE_PBTL)" to "tas5805m_init"
    app_player_init(NULL, audio_player_callback, set, NULL);
    audio_player_vol_set(60);

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = CONFIG_WIFI_SSID,
        .password = CONFIG_WIFI_PASSWORD,
    };

    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);

    esp_camera_start();    
    esp_vt_handle_init();
    esp_vt_handle_login_in();
    esp_vt_handle_start();
    
    // pca9539_start_button(&pca_cfg);
    
    // start_sys_monitor();
}


