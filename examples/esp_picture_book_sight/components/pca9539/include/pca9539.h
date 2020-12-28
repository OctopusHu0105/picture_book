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

#ifndef _PCA9539_H
#define _PCA9539_H

#include "driver/gpio.h"
#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED1_R_CTRL PCA9539_GPIO_NUM_4
#define LED1_G_CTRL PCA9539_GPIO_NUM_5
#define LED1_B_CTRL PCA9539_GPIO_NUM_3
#define LED2_R_CTRL PCA9539_GPIO_NUM_7
#define LED2_G_CTRL PCA9539_GPIO_NUM_6
#define LED2_B_CTRL PCA9539_GPIO_NUM_8
#define TCXO_EN     PCA9539_GPIO_NUM_0
#define CAM_RESET   PCA9539_GPIO_NUM_1
#define SD_DET      PCA9539_GPIO_NUM_2
#define PA_CTRL     PCA9539_GPIO_NUM_15 

typedef enum {
    PCA_BUTTON_SET_ID,
    PCA_BUTTON_VOLDOWN_ID,
    PCA_BUTTON_VOLUP_ID,
    PCA_BUTTON_MUTE_ID,
    PCA_BUTTON_SIGHT_ID,
    PCA_BUTTON_MODE_ID,
} pca_btn_id_t;

typedef enum {
    PCA_BTN_STATE_IDLE,             // 0: idle
    PCA_BTN_STATE_PRESSED,          // 1: pressed
    PCA_BTN_STATE_LONG_PRESSED,     // 2: long pressed
} pca_btn_state_t;

typedef int (*pca_button_cb)(pca_btn_id_t id, pca_btn_state_t st);

typedef enum {
    PCA9539_GPIO_NUM_0 = 0,   
    PCA9539_GPIO_NUM_1 = 1,   
    PCA9539_GPIO_NUM_2 = 2,                   
    PCA9539_GPIO_NUM_3 = 3,   
    PCA9539_GPIO_NUM_4 = 4,   
    PCA9539_GPIO_NUM_5 = 5,   
    PCA9539_GPIO_NUM_6 = 6,   
    PCA9539_GPIO_NUM_7 = 7,   
    PCA9539_GPIO_NUM_8 = 8,   
    PCA9539_GPIO_NUM_9 = 9,   
    PCA9539_GPIO_NUM_10 = 10,  
    PCA9539_GPIO_NUM_11 = 11,  
    PCA9539_GPIO_NUM_12 = 12,  
    PCA9539_GPIO_NUM_13 = 13,  
    PCA9539_GPIO_NUM_14 = 14,  
    PCA9539_GPIO_NUM_15 = 15,
    PCA9539_GPIO_NUM_MAX
} esp_pca9539_gpio_num_t;

typedef enum {
    PCA9539_IO_LOW,        
    PCA9539_IO_HIGH,         
    PCA9539_LEVEL_ERROR
} esp_pca9539_io_level_t;

typedef enum {
    PCA9539_IO_RETAINED,        
    PCA9539_IO_INVERTED         
} esp_pca9539_io_polarity_t;

typedef enum {
    PCA9539_IO_OUTPUT,        
    PCA9539_IO_INPUT         
} esp_pca9539_io_config_t;

typedef struct {
    gpio_num_t i2c_sda;         
    gpio_num_t i2c_scl;         
    gpio_num_t interrupt_output;
    pca_button_cb func_cb;
} esp_pca9539_config_t;

/*
 * @brief Initialize PCA9539 chip
 *
 * @param codec_cfg  configuration of PCA9539
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t pca9539_init(esp_pca9539_config_t *cfg);

/**
 * @brief Deinitialize PCA9539 chip
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t pca9539_deinit(void);

/*
 * @brief Get PCA9539 input level
 *
 * @param gpio_num  GPIO of PCA9539
 *
 * @return
 *      - esp_pca9539_io_level_t
 */
esp_pca9539_io_level_t pca9539_get_input_state(esp_pca9539_gpio_num_t gpio_num);

/*
 * @brief Get PCA9539 output level
 *
 * @param gpio_num  GPIO of PCA9539
 *
 * @return
 *      - esp_pca9539_io_level_t
 */
esp_pca9539_io_level_t pca9539_get_output_state(esp_pca9539_gpio_num_t gpio_num);

/*
 * @brief Get PCA9539 output state
 *
 * @param gpio_num  GPIO of PCA9539
 *
 * @return
 *      - esp_pca9539_io_level_t
 */
esp_err_t pca9539_set_output_state(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_level_t level);


/*
 * @brief Set PCA9539 polarity
 *
 * @param gpio_num  GPIO of PCA9539
 *        polarity  polarity
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t pca9539_set_polarity_inversion(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_polarity_t polarity);

/*
 * @brief Get PCA9539 output level
 *
 * @param gpio_num  GPIO of PCA9539
 *
 * @return
 *      - esp_pca9539_io_level_t
 */
esp_pca9539_io_config_t pca9539_get_io_config(esp_pca9539_gpio_num_t gpio_num);

/*
 * @brief Set PCA9539 io config
 *
 * @param gpio_num  GPIO of PCA9539
 *        io_config  io config
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t pca9539_set_io_config(esp_pca9539_gpio_num_t gpio_num, esp_pca9539_io_config_t io_config);

/**
 * @brief Print all PCA9539 registers
 *
 * @return
 *     - void
 */
void pca9539_read_all();

/**
 * @brief start button
 *
 * @param codec_cfg  configuration of PCA9539
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t pca9539_start_button(esp_pca9539_config_t *cfg);


#ifdef __cplusplus
}
#endif

#endif
