/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>
#include <math.h>
#include "i2c_bus.h"
#include "board.h"
#include "esp_log.h"
#include "pca9539.h"

#define PCA9539_ADDR   0xEA

static char *TAG = "PCA9539";
static i2c_bus_handle_t i2c_handle;

#define ES_ASSERT(a, format, b, ...) \
    if ((a) != 0) { \
        ESP_LOGE(TAG, format, ##__VA_ARGS__); \
        return b;\
    }

#define SET_BITS(_m, _s, _v)  ((_v) ? (_m)|(1<<(_s)) : (_m)&~(1<<(_s)))
#define GET_BITS(_m, _s)      ((((_m) & (1<<(_s)))>>(_s)) ? true : false)

#define PCA9539_INPUT_PORT0              0x00
#define PCA9539_INPUT_PORT1              0x01
#define PCA9539_OUTPUT_PORT0             0x02
#define PCA9539_OUTPUT_PORT1             0x03
#define PCA9539_POLARITY_INVERSION_PORT0 0x04
#define PCA9539_POLARITY_INVERSION_PORT1 0x05
#define PCA9539_CONFIGURATION_PORT0      0x06
#define PCA9539_CONFIGURATION_PORT1      0x07

#define BUTTON1_IO_NUM     PCA9539_GPIO_NUM_14
#define BUTTON2_IO_NUM     PCA9539_GPIO_NUM_13
#define BUTTON3_IO_NUM     PCA9539_GPIO_NUM_12
#define BUTTON4_IO_NUM     PCA9539_GPIO_NUM_11
#define BUTTON5_IO_NUM     PCA9539_GPIO_NUM_10
#define BUTTON6_IO_NUM     PCA9539_GPIO_NUM_9

#define BUTTON_LONG_PRESS_TIME 3000
#define BUTTON_PRESS_ONE_TIME  20

typedef struct {
    pca_btn_state_t *st;
    pca_button_cb   func_cb;
    uint32_t        *count;
    uint8_t         id;
    uint8_t         before_id;
    uint8_t         num;
} pca9539_button_cfg_t;

static esp_err_t pca9539_write_reg(uint8_t reg_addr, uint8_t data)
{
    return i2c_bus_write_bytes(i2c_handle, PCA9539_ADDR, &reg_addr, sizeof(reg_addr), &data, sizeof(data));
}

static char pca9539_read_reg(uint8_t reg_addr)
{
    uint8_t data;
    i2c_bus_read_bytes(i2c_handle, PCA9539_ADDR, &reg_addr, sizeof(reg_addr), &data, sizeof(data));
    return data;
}

static int i2c_init(gpio_num_t clk, gpio_num_t sda)
{
    int res = 0;
    i2c_config_t es_i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .scl_io_num = clk,
        .sda_io_num = sda,
    };
    i2c_handle = i2c_bus_create(I2C_NUM_0, &es_i2c_cfg);
    return res;
}

esp_pca9539_io_level_t pca9539_get_input_state(esp_pca9539_gpio_num_t gpio_num)
{
    char data = 0;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_INPUT_PORT0);
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_INPUT_PORT1);
        gpio_num %= 8; 
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return PCA9539_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_pca9539_io_level_t pca9539_get_output_state(esp_pca9539_gpio_num_t gpio_num)
{
    char data = 0;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_OUTPUT_PORT0);
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_OUTPUT_PORT1);
        gpio_num %= 8;
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return PCA9539_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_err_t pca9539_set_output_state(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_level_t level)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_OUTPUT_PORT0);
        res = pca9539_write_reg(PCA9539_OUTPUT_PORT0, SET_BITS(data, gpio_num, level));
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_OUTPUT_PORT1);
        gpio_num %= 8;
        res = pca9539_write_reg(PCA9539_OUTPUT_PORT1, SET_BITS(data, gpio_num, level));
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        
    }
    return res;
}


esp_err_t pca9539_set_polarity_inversion(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_polarity_t polarity)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_POLARITY_INVERSION_PORT0);
        res = pca9539_write_reg(PCA9539_POLARITY_INVERSION_PORT0, SET_BITS(data, gpio_num, polarity));  
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_POLARITY_INVERSION_PORT1);
        gpio_num %= 8;
        res = pca9539_write_reg(PCA9539_POLARITY_INVERSION_PORT1, SET_BITS(data, gpio_num, polarity));  
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        
    }
    return res;
}



esp_pca9539_io_config_t pca9539_get_io_config(esp_pca9539_gpio_num_t gpio_num)
{
    char data = 0;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_CONFIGURATION_PORT0);
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_CONFIGURATION_PORT1);
        gpio_num %= 8;
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return PCA9539_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_err_t pca9539_set_io_config(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_config_t io_config)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if(gpio_num >=PCA9539_GPIO_NUM_0 && gpio_num < PCA9539_GPIO_NUM_8) {
        data = pca9539_read_reg(PCA9539_CONFIGURATION_PORT0);
        res = pca9539_write_reg(PCA9539_CONFIGURATION_PORT0, SET_BITS(data, gpio_num,io_config));  
    } else if (gpio_num >= PCA9539_GPIO_NUM_7 && gpio_num < PCA9539_GPIO_NUM_MAX) {
        data = pca9539_read_reg(PCA9539_CONFIGURATION_PORT1);
        gpio_num %= 8;
        res = pca9539_write_reg(PCA9539_CONFIGURATION_PORT1, SET_BITS(data, gpio_num,io_config));
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
    }
    return res;
}

void pca9539_read_all()
{
    for (int i = 0; i < 0x07; i++) {
        uint8_t reg = pca9539_read_reg(i);
        ets_printf("REG:%02x, %02x\n", i, reg);
    }

}

uint8_t pca9539_get_button_id()
{
    union {
        struct {
            uint8_t button1:       1;
            uint8_t button2:       1;
            uint8_t button3:       1;
            uint8_t button4:       1;
            uint8_t button5:       1;
            uint8_t button6:       1;
            uint8_t reserved2:     2;
        };
        uint8_t val;
    } button_id;
    
    button_id.val = 0xFF;
    button_id.button1 = pca9539_get_input_state(BUTTON1_IO_NUM);
    button_id.button2 = pca9539_get_input_state(BUTTON2_IO_NUM);
    button_id.button3 = pca9539_get_input_state(BUTTON3_IO_NUM);
    button_id.button4 = pca9539_get_input_state(BUTTON4_IO_NUM);
    button_id.button5 = pca9539_get_input_state(BUTTON5_IO_NUM);
    button_id.button6 = pca9539_get_input_state(BUTTON6_IO_NUM);

    button_id.button1 &= pca9539_get_input_state(BUTTON1_IO_NUM);
    button_id.button2 &= pca9539_get_input_state(BUTTON2_IO_NUM);
    button_id.button3 &= pca9539_get_input_state(BUTTON3_IO_NUM);
    button_id.button4 &= pca9539_get_input_state(BUTTON4_IO_NUM);
    button_id.button5 &= pca9539_get_input_state(BUTTON5_IO_NUM);
    button_id.button6 &= pca9539_get_input_state(BUTTON6_IO_NUM);

    return ~button_id.val;
}

static esp_err_t pca9539_call_button_cb(pca9539_button_cfg_t *cfg)
{
    AUDIO_NULL_CHECK(TAG, cfg, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, cfg->count, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, cfg->st, return ESP_FAIL);

    bool is_call_cb[6] = { false };
    for (int i = 0; i < cfg->num; i++) {
        if (cfg->id & (1 << i)) {
            cfg->count[i]++;
        } else {
            cfg->count[i] = 0;
        }
    }

    if (cfg->id != cfg->before_id) {
        for (int i = 0; i < cfg->num; i++) {
            if (cfg->count[i] == 0) {
                cfg->st[i] = PCA_BTN_STATE_IDLE;
                is_call_cb[i] = false;
            } else if (cfg->count[i] < (BUTTON_LONG_PRESS_TIME / BUTTON_PRESS_ONE_TIME)) {
                cfg->st[i] = PCA_BTN_STATE_PRESSED;
                is_call_cb[i] = true;
            } 
        }
    } else {
       for (int i = 0; i < cfg->num; i++) {
            if (cfg->st[i] == PCA_BTN_STATE_IDLE) {
                ESP_LOGD(TAG, "button is idle");
                is_call_cb[i] = false;
            } else if (cfg->st[i] == PCA_BTN_STATE_PRESSED) {
                if (cfg->count[i] > (BUTTON_LONG_PRESS_TIME / BUTTON_PRESS_ONE_TIME)) {
                    cfg->st[i] = PCA_BTN_STATE_LONG_PRESSED;
                    ESP_LOGD(TAG, "button is long press pressed");
                    is_call_cb[i] = true;
                } else {
                    ESP_LOGD(TAG, "button is pressed");
                    is_call_cb[i] = false;
                }
            } else {
                is_call_cb[i] = false;
            }
        } 

    }

    ESP_LOGD(TAG, "pressed1: ID:%d", cfg->id);
    
    for (int i = 0; i < cfg->num; i++) {
        if (is_call_cb[i]) {
            cfg->func_cb(i, cfg->st[i]);
        }
    }

    return ESP_OK;
}

static esp_err_t button_task(void *param)
{    
    esp_pca9539_config_t *pca_cfg = (esp_pca9539_config_t *)param;
    AUDIO_NULL_CHECK(TAG, pca_cfg, return ESP_FAIL);
    pca9539_button_cfg_t *cfg = (pca9539_button_cfg_t *)malloc(sizeof(pca9539_button_cfg_t));
    AUDIO_NULL_CHECK(TAG, cfg, return ESP_FAIL);
    cfg->num = 6;
    cfg->func_cb = pca_cfg->func_cb;

    cfg->count = malloc(sizeof(uint32_t) * cfg->num);
    memset(cfg->count, 0x00, sizeof(uint32_t) * cfg->num);
    cfg->st = malloc(sizeof(pca_btn_state_t) * cfg->num);
    memset(cfg->st, 0x00, sizeof(pca_btn_state_t) * cfg->num);

    while (1){
        cfg->id = pca9539_get_button_id();
        pca9539_call_button_cb(cfg);
        cfg->before_id = cfg->id;

        vTaskDelay(BUTTON_PRESS_ONE_TIME / portTICK_PERIOD_MS);
    }

    if (cfg->st) {
        free(cfg->st);
        cfg->st = NULL;
    }
    
    if (cfg->count) {
        free(cfg->count);
        cfg->count = NULL;
    }

    if (cfg) {
        free(cfg);
        cfg = NULL;
    }
    return ESP_OK;
}

esp_err_t pca9539_init(esp_pca9539_config_t *cfg)
{
    AUDIO_NULL_CHECK(TAG, cfg, return ESP_FAIL);
    esp_err_t ret = ESP_OK;
    ret = i2c_init(cfg->i2c_scl, cfg->i2c_sda);
    return ret;
}

esp_err_t pca9539_deinit()
{
    i2c_bus_delete(i2c_handle);
    return ESP_OK;
}

esp_err_t pca9539_start_button(esp_pca9539_config_t *cfg)
{
    AUDIO_NULL_CHECK(TAG, cfg, return ESP_FAIL);
    xTaskCreatePinnedToCore(button_task, "button task", 4 * 1024, (void *)cfg, 5, NULL, 0);
    return ESP_OK;
}
