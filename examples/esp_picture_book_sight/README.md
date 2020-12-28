# picture book example

The example shows how to use picture book demo, and connect vt_server

## 1. How to use example

#### Hardware Required

The example only support ESP32-DU1906-SIGHT board.

## 2. Setup software environment

Please refer to [Get Started](https://docs.espressif.com/projects/esp-adf/en/latest/get-started/index.html#get-started).

#### Configure the project
cmake menuconfig:
```
idf.py menuconfig
```
Or use legacy GNU make:
```
make menuconfig
```
Select compatible audio board in ``menuconfig > Audio HAL``, then compile the example.

#### Build and Flash
You can use `GNU make` or `cmake` to build the project.
If you are using make:
```bash
cd picture_book/esp_picture_book_sight
make clean
make menuconfig
make -j4 all
```

Or if you are using cmake:
```bash
cd picture_book/esp_picture_book_sight
idf.py fullclean
idf.py menuconfig
idf.py build
```
The firmware downloading flash address refer to follow table.

|Flash address | Bin Path|
|---|---|
|0x1000 | build/bootloader/bootloader.bin|
|0x8000 | build/partitiontable/partition-table.bin|
|0x10000 | build/picture_book.bin|

To exit the serial monitor, type ``Ctrl-]``.

## 3. Example Output

After download the follow logs should be output, here:

```
I (67) boot: Chip Revision: 3
I (67) boot_comm: chip revision: 3, min. bootloader chip revision: 0
I (40) boot: ESP-IDF v3.3.2-107-g722043f 2nd stage bootloader
I (40) boot: compile time 15:05:24
I (40) boot: Enabling RNG early entropy source...
I (46) boot: SPI Speed      : 80MHz
I (50) boot: SPI Mode       : DIO
I (54) boot: SPI Flash Size : 4MB
I (58) boot: Partition Table:
I (62) boot: ## Label            Usage          Type ST Offset   Length
I (69) boot:  0 nvs              WiFi data        01 02 00009000 00004000
I (77) boot:  1 phy_init         RF data          01 01 0000d000 00001000
I (84) boot:  2 factory          factory app      00 00 00010000 00300000
I (92) boot: End of partition table
I (96) boot_comm: chip revision: 3, min. application chip revision: 0
I (103) esp_image: segment 0: paddr=0x00010020 vaddr=0x3f400020 size=0x5d274 (381556) map
I (223) esp_image: segment 1: paddr=0x0006d29c vaddr=0x3ffb0000 size=0x02d74 ( 11636) load
I (227) esp_image: segment 2: paddr=0x00070018 vaddr=0x400d0018 size=0x106194 (1073556) map
0x400d0018: _flash_cache_start at ??:?

I (543) esp_image: segment 3: paddr=0x001761b4 vaddr=0x3ffb2d74 size=0x0160c (  5644) load
I (545) esp_image: segment 4: paddr=0x001777c8 vaddr=0x40080000 size=0x00400 (  1024) load
0x40080000: _WindowOverflow4 at /home/zhanghu/esp/esp-adf-internal/esp-idf/components/freertos/xtensa_vectors.S:1779

I (550) esp_image: segment 5: paddr=0x00177bd0 vaddr=0x40080400 size=0x1cebc (118460) load
I (620) boot: Loaded app from partition at offset 0x10000
I (620) boot: Disabling RNG early entropy source...
I (621) psram: This chip is ESP32-D0WD
I (626) spiram: Found 64MBit SPI RAM device
I (630) spiram: SPI RAM mode: flash 80m sram 80m
I (635) spiram: PSRAM initialized, cache is in low/high (2-core) mode.
I (642) cpu_start: Pro cpu up.
I (646) cpu_start: Application information:
I (651) cpu_start: Project name:     esp-idf
I (656) cpu_start: App version:      1f692f4
I (661) cpu_start: Compile time:     Nov 30 2020 15:05:19
I (667) cpu_start: ELF file SHA256:  774c3a79835b4313...
I (673) cpu_start: ESP-IDF:          v3.3.2-107-g722043f
I (679) cpu_start: Starting app cpu, entry point is 0x40081568
0x40081568: call_start_cpu1 at /home/zhanghu/esp/esp-adf-internal/esp-idf/components/esp32/cpu_start.c:268

I (0) cpu_start: App cpu up.
I (1182) spiram: SPI SRAM memory test OK
I (1182) heap_init: Initializing. RAM available for dynamic allocation:
I (1183) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (1189) heap_init: At 3FFBCD38 len 000232C8 (140 KiB): DRAM
I (1195) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (1201) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (1208) heap_init: At 4009D2BC len 00002D44 (11 KiB): IRAM
I (1214) cpu_start: Pro cpu start user code
I (1219) spiram: Adding pool of 4096K of external SPI memory to heap allocator
I (121) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (122) spiram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (172) PICTURE_BOOK_EXAMPLE: [ 3 ] Start and wait for Wi-Fi network
I (172) TAS5805M: Power ON CODEC with GPIO 12
I (172) gpio: GPIO[12]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 1| Intr:0 
W (402) I2C_BUS: i2c_bus_create:57: I2C bus has been already created, [port:0]
I (1062) TAS5805M: tas5805m_transmit_registers:  write 1677 reg done
W (1062) TAS5805M: volume = 0x44
I (1062) AUDIO_HAL: Codec mode is 3, Ctrl:1
I (1072) ESP_AUDIO_TASK: media_ctrl_task running...,0x3f8000dc

----------------------------- ESP Audio Platform -----------------------------
|                                                                            |
|                         ESP_AUDIO-v1.6.5-2-ga0f56e2                        |
|                     Compile date: Nov  4 2020-20:44:15                     |
------------------------------------------------------------------------------
I (1112) ESP_AUDIO_CTRL: Func:media_ctrl_create, Line:339, MEM Total:4403780 Bytes, Inter:254500 Bytes, Dram:242948 Bytes

I (1122) APP_PLAYER_INIT: http_stream_reader0:0x3f800b7c
I (1122) APP_PLAYER_INIT: http_stream_reader1:0x3f80113c
I (1132) APP_PLAYER_INIT: http_stream_reader2:0x3f8016fc
I (1142) ESP_DECODER: esp_decoder_init, stack size is 5120
W (1142) TAS5805M: volume = 0x4a
I (1152) APP_PLAYER_INIT: Func:setup_app_esp_audio_instance, Line:133, MEM Total:4393732 Bytes, Inter:250492 Bytes, Dram:238940 Bytes

I (1162) APP_PLAYER_INIT: esp_audio instance is:0x3f8000dc
I (1172) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=3
I (1172) I2S: APLL: Req RATE: 48000, real rate: 47999.961, BITS: 16, CLKM: 1, BCK_M: 8, MCLK: 12287990.000, SCLK: 1535998.750000, diva: 1, divb: 0
I (1192) ESP32_Korvo_DU1906: I2S1, MCLK output by GPIO0
I (1192) SD_HELPER: default_sdcard_player_init
I (1202) SD_HELPER: default_sdcard_player_init  playlist_create
I (1202) RAW_HELPER: default_raw_player_init :106, que:0x3ffce650
I (1212) MP3_DECODER: MP3 init
W (1212) I2S: I2S driver already installed
I (1222) ESP32_Korvo_DU1906: I2S1, MCLK output by GPIO0
W (1232) TAS5805M: volume = 0x4a
I (1242) wifi:wifi driver task: 3ffd309c, prio:23, stack:3584, core=0
I (1242) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (1252) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (1272) wifi:wifi firmware version: 5f8804c
I (1272) wifi:config NVS flash: enabled
I (1272) wifi:config nano formating: disabled
I (1272) wifi:Init dynamic tx buffer num: 32
I (1282) wifi:Init data frame dynamic rx buffer num: 32
I (1282) wifi:Init management frame dynamic rx buffer num: 32
I (1292) wifi:Init management short buffer num: 32
I (1292) wifi:Init static tx buffer num: 16
I (1302) wifi:Init static rx buffer size: 1600
I (1302) wifi:Init static rx buffer num: 10
I (1302) wifi:Init dynamic rx buffer num: 32
I (1402) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (1402) wifi:mode : sta (b8:f0:09:8f:fd:a4)
I (2732) wifi:new:<11,0>, old:<1,0>, ap:<255,255>, sta:<11,0>, prof:1
I (2732) wifi:state: init -> auth (b0)
I (2732) wifi:state: auth -> assoc (0)
I (2742) wifi:state: assoc -> run (10)
I (2742) wifi:connected with TP-LINK_5F05, aid = 1, channel 11, BW20, bssid = b0:95:8e:03:5f:05
I (2742) wifi:security type: 0, phy: bgn, rssi: -28
I (2752) wifi:pm start, type: 1

I (2762) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (3652) event: sta ip: 192.168.1.129, mask: 255.255.255.0, gw: 192.168.1.1
I (3652) sccb: pin_sda 18 pin_scl 23

I (3662) sccb: camera addr :3c
I (3672) camera: Camera PID=0x36 VER=0x60
I (3982) camera: Detected OV3660 camera
I (3982) gpio: GPIO[36]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (3982) gpio: GPIO[37]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (3992) gpio: GPIO[39]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4002) gpio: GPIO[35]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4012) gpio: GPIO[32]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4022) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4022) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4032) gpio: GPIO[22]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4042) gpio: GPIO[19]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4052) gpio: GPIO[21]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4062) gpio: GPIO[12]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (4072) camera: Allocating 1 frame buffers (150 KB total)
I (4102) camera: Allocating 150 KB frame buffer in OnBoard RAM
E (4112) gpio: gpio_install_isr_service(416): GPIO isr service already installed
[vtbk][INFO]:vt_bk_instance()>line[268]:******sdk_version:4.0.2******

[vtbk][INFO]:vt_bk_instance()>line[269]:******platform:rtos ******

[vtbk][INFO]:vt_bk_instance()>line[271]:******STATE_VERSION: 203*****

[vtbk][INFO]:vt_bk_register_reconginfo_cb()>line[694]:vt_bk_register_reconginfo_cb cb: 0x40130648 recong_userp: 0x0
0x40130648: esp_vt_recongnize_info_cb at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_recognize.c:21

[vtbk][INFO]:vt_bk_register_fingerinfo_cb()>line[710]:vt_bk_register_fingerinfo_cb cb: 0x4012f29c finger_userp: 0x0
0x4012f29c: esp_vt_finger_state_cb at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_handle.c:122

[vtbk][INFO]:vt_bk_register_error_cb()>line[678]:vt_bk_register_error_cb cb: 0x401d255c recong_userp: 0x0
0x401d255c: esp_vt_recongnize_error_cb at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_recognize.c:32

[img_process][INFO]:vt_imgprocess_init_ops()>line[125]:vt_imgprocess_init_ops, camera_open:0x4013067c
0x4013067c: esp_vt_camera_open at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_camera.c:241

[media_manager][INFO]:vt_media_manager_init_ops()>line[93]:vt_media_manager_init_ops, play:0x4012ffcc
0x4012ffcc: esp_vt_meida_play at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_media.c:136

[login][INFO]:setLicenseText()>line[155]:License text: ODU3RDM1MUJGRTVENEIwMkE3MTMyRTFBOUUzODk4QUE5OTBEN0YxNzc2ODVDMEZEN0YxM0MzNUYxMDVGNURDMkQxNjc5MDE0RUMxMjk3RjMxNDRDOTJEQjBEOUIzQ0ZEMDJBNjBCNkQyMUE5MEVBNkU5NEQ2OTgyODAwQkYwMUVBOTU2QzdFQjhDQjUxODQ2QUYzODI0QkNCNjdDNzY2NDI1MTQ3NzA0RTM2MTY1RjhCMjMyN0EyMjU0MENGQzdENDkyMDFDRkUzNzIwMjRFNjk3MENGQTE3MUMzOEVGODA1QTVBQUQ3NzQ0ODMzMzZDNzExMkY1RkJFM0JCREQ3OTlGNDc4NjhEQUU4ODcyNEE1NzExNjQ3RDIwQzJFRkY4OUM2RURGRjVFQjVERDUzNkE2MDY0NkJGQzZFRDc3NUVFMkRBMTVENUFGMkVENDY3MjdDRkZFOUZDQzEyMjkwODVERkFDRUU0MzdBMUJEMEI1MTM5MTU0M0U0NDRGNUUzNEFFQTIxNTAyMjBGM0EzRDJDMjM3RTA1Nzk3Q0E4Q0Q1NDlGQ0MzQzI0QUUxMjQ5NDA0NDQ2NUFDQUQ4NUJCQUUyNkMzMjBEMDUyNkFFMDc1MUE5MDc5M0M2MEM3Q0Q0MUE1N0E0OEU2Njg3NkFBMEE1M0VDNzBBRDIzNjYzMzMxODBBMDQ2QTMzOUU3QzFDRTNEQzEwQjMyQkJCOUUzNDI4OTQ1NDFFNjQ4OTEzNTc2QTdFOTE1NjdCNkY5MDM0
[login][INFO]:setModelType()>line[173]:Model Type: esp32_dd_default
[login][INFO]:setDeviceID()>line[164]:Device ID: esp32_test
[login][WARN]:initLogin()>line[66]:already initLogin
[login][INFO]:setRequestFunc()>line[150]:Login Request Function: 0x4012fe24
0x4012fe24: esp_vt_http_req_cb at /home/zhanghu/esp/picture_book/examples/esp_picture_book_sight/build/../components/vt_server/vt_http.c:230

[login][INFO]:resolve()>line[369]:appCode: 547AE540-51F3-4C49-9596-047DBC3EA664
E (4922) VT_HTTP: vt http 405 {"code":0,"msg":"ok","data":{"expire":5400,"openId":"904217d42b1911ebabd4506b4b2a538e","verification":"d86Kk61l9xAIHkmyFWY5HuoKV5EGLpoxdV5GHJHc4DQrOTJcZSYFIHwpHiUZZDf60QKicFJMkMDsZEQKZa7sgvfLR2p5OZFHR/bLSrSHzPzE3BwgfDmJcrogTh3jwZZ8Ng5Y0iAicLnM17m4A+u3NptXpdd6mOsWp/UAXmVG+o8=","encryptKey":null,"fingerRead":0,"activeExpired":false,"qrCode":null,"evaluation":0,"token":"dc7312e7ed93de24ab999a7e0a14c28c"}}
[json_parse][INFO]:resolve_login_bean()>line[698]:isFinger: 0
[statedet_module][INFO]:vt_statedet_init()>line[100]:params: x:0 y: 0 w: 320 h:240 ds_factor: 4 in_type: 1
[statedet_module][INFO]:vt_statedet_init()>line[101]:params: thresh: 2 fds: 1 handle_edge: 0 propose_hand: 0
E (7982) VT_HTTP: vt http 1178 {"code":0,"msg":"ok","data":{"projectParam":"{\"cameraId\": 0,\"isFinger\": true,\"previewSize\": {\"height\": 240,\"width\": 320}}","edgeAlgParam":"{\"fingerDelay\":0,\"frontGap\":0,\"coverGap\":0,\"proposeHand\":false,\"handleEdge\":false,\"roi\":{\"width\":320,\"x\":0,\"y\":0,\"height\":240},\"uploadGap\":0}","fixAlgParam":"","cloudParam":"{\"lowres\":1,\"scale\":2.0}","imageConfigParam":[{"id":176,"name":"DEFAULT_HR","parameters":"{\n\"model_id\": 1,\n\"model_off\": {\"x\": 952, \"y\": 0},\n\"model_final\": {\"height\": 864, \"width\": 1152},\n\"temp_scale\": 0.25,\n\"view_scale\": 0.5,\n\"model_warp_H\": [3.2888342626615191e-01, -4.5271045615911998e-01, 1.5643797370970419e+03, 6.5315626157753989e-03, 3.0379578600397017e-01, 7.6624558711605883e-01, -1.1124979314217152e-06, -1.9370148094933291e-04, 1]\n}"},{"id":177,"name":"DEFAULT_A3T1","parameters":"{\n\"comment\": \"matching model hardware info: DEMO_V3\",\n\"model_off\": {\"x\": 0, \"y\": 0},\n\"model_final\": {\"height\": 360, \"width\": 640},\n\"model_warp_H\": [8.67886244e-01, -9.17971655e-01, 3.29704734e+02, -7.86245110e-03, 8.05280649e-01, 5.39317038e+00, 6.47627243e-06, -1.52980207e-03, 1]\n}"}]}}
[json_parse][INFO]:vt_resolve_cloud_config()>line[1068]:isFinger is 1
[json_parse][INFO]:vt_resolve_cloud_config()>line[1151]:frontGap: 0 coverGap: 0 uploadGap: 0
[statedet_module][INFO]:vt_statedet_init()>line[166]:{"lowres":1,"scale":2.0}
[statedet_module][INFO]:vt_statedet_init()>line[167]:{
"comment": "matching model hardware info: DEMO_V3",
"model_off": {"x": 0, "y": 0},
"model_final": {"height": 360, "width": 640},
"model_warp_H": [8.67886244e-01, -9.17971655e-01, 3.29704734e+02, -7.86245110e-03, 8.05280649e-01, 5.39317038e+00, 6.47627243e-06, -1.52980207e-03, 1]
}
[statedet_module][WARN]:vt_statedet_init()>line[168]:is_support_xxx: 1
[recognize][INFO]:vt_recongnize_init_ops()>line[257]:vt_recongnize_init_ops, args:0x3f82aedc
[vtbk][INFO]:vt_bk_start_recognize()>line[484]:vt_bk_start_recognize - begin
I (8162) VT MEDIA: vt_meida_set_state
I (8162) VT MEDIA: vt_meida_start_task
[vtpolicy][INFO]:vt_recognize_task_()>line[729]:vt_recognize_task ...
[recognize][INFO]:vt_get_thirdrepos_config()>line[184]:update_third_repos_config - begin
[vtbk][INFO]:vt_bk_start_recognize()>line[535]:vt_bk_start_recognize - end
[vtpolicy][INFO]:vt_camera_preview_cb_()>line[207]:consuming time request DST_BRS: 0 
E (8412) VT_CAMERA: esp_vt_camera_take_picture
[vtpolicy][INFO]:vt_camera_preview_cb_()>line[215]:request NONE ret:2, resource type: -1
E (12492) VT_HTTP: ../components/vt_server/vt_http.c:83 (esp_vt_http_get_handle): http fetch error
[recognize][ERROR]:vt_get_thirdrepos_config()>line[205]:get third party config failed !
[json_parse][ERROR]:vt_resolve_thirdrepos_config()>line[1257]:cjson parse failed !
{"code":0,"msg":"ok","data":{"brs":{"code":0,"msg":"ok","data":{"page":{"pageId":224411,"bookId":2230,"modelId":27,"type":1,"pagination":0,"imageId":"ac95817b-f65c-4ebf-8b71-917d50613d2d","extraData":"","videoText":"","physicalIndex":1,"physicalState":0,"audios":{"voice":[{"fileName":"http://client-api1.51wanxue.com/api-client/file/audio/1e89d7b5869611e9a6166c92bf315f73/voice/61a6098e-1972-4d88-b89e-0a11ca98d7ef.mp3?CUSTOM_ENCRYPT_KEY=false&appId=675","clientFileName":"0封面.mp3","loop":0,"startAt":0.0,"type":0}]}},"book":{"thumbnailCoverImage":"http://files-api1.51wanxue.com//brs/content/basebook/27/2230/3efa0484-fa02-4b57-9317-77341250b79d.jpg","totalPage":23,"bookId":2230,"securityId":"1e89d7b5869611e9a6166c92bf315f73","bookName":"遇见春天","author":"原京子","publisher":"二十一世纪出版社","isbn":"9787539143866","seriesTitle":"","description":"初次遇见了春天，小熊兄弟看见的世界是如此的美丽！——带给孩子喜悦的春天绘本。接下来会发生哪些有趣的故事呢，《遇见春天(精装)》带你去探索答案。这是一本带给孩子喜悦的春天绘本。","extraData":"","mainPage":17,"resourceType":1,"currentRepo":{"id":27,"name":"原文精读","bookId":2230,"language":1,"soundRay":1}},"recType":0}},"fds":null}}
[json_parse][INFO]:parse2BRSBookBean()>line[119]:this book resource type: 1
[json_parse][INFO]:parse2BRSBookBean()>line[139]:current_res_id: 27
[json_parse][INFO]:parse2BRSBean()>line[395]:voice length: 1
[vtpolicy][INFO]:recognize_handle()>line[1082]:book[2230] changed.
[vtpolicy][INFO]:recognize_handle()>line[1083]:index[1] consuming time -get picture data:0ms, -request server and recognize:0ms
W (13752) AUDIO_MANAGER: ap_manager_stop, not found the current operations
I (13762) VT MEDIA: vt_meida_is_playing
[vtpolicy][INFO]:vt_media_manager_task_()>line[434]:---------------- consuming time: 0, media task url: http://client-api1.51wanxue.com/api-client/file/audio/1e89d7b5869611e9a6166c92bf315f73/voice/61a6098e-1972-4d88-b89e-0a11ca98d7ef.mp3?CUSTOM_ENCRYPT_KEY=false&appId=675
I (13792) VT MEDIA: vt_meida_init_url: http://client-api1.51wanxue.com/api-client/file/audio/1e89d7b5869611e9a6166c92bf315f73/voice/61a6098e-1972-4d88-b89e-0a11ca98d7ef.mp3?CUSTOM_ENCRYPT_KEY=false&appId=675 
I (13812) VT MEDIA: vt_meida_play: url : http://client-api1.51wanxue.com/api-client/file/audio/1e89d7b5869611e9a6166c92bf315f73/voice/61a6098e-1972-4d88-b89e-0a11ca98d7ef.mp3?CUSTOM_ENCRYPT_KEY=false&appId=675
E (13832) AUDIO_PLAYER: music play, type:102, cur media type:102
E (13832) AP_HELPER: audio_player_helper_default_play, type:102,url:http://client-api1.51wanxue.com/api-client/file/audio/1e89d7b5869611e9a6166c92bf315f73/voice/61a6098e-1972-4d88-b89e-0a11ca98d7ef.mp3?CUSTOM_ENCRYPT_KEY=false&appId=675,pos:0,block:0,auto:0,mixed:0,inter:0
E (13862) AUDIO_MANAGER: AP_MANAGER_PLAY, Enter:ap_manager_play, pos:0, block:0, auto:0, mix:0, inter:0, type:102, st:UNKNOWN
I (13872) AUDIO_MANAGER: AP_MANAGER_PLAY, Call esp_audio_stop before playing
I (13882) ESP_AUDIO_CTRL: Enter media_ctrl_stop procedure, src:0, type:0, st:UNKNOWN, phase:0
W (13892) ESP_AUDIO_CTRL: [media_ctrl_stop]-Already stopped, status is UNKNOWN, phase:0
W (13902) AUDIO_MANAGER: AP_MANAGER_PLAY, Call esp_audio_stop error, ret:81009
W (13902) AUDIO_MANAGER: AP_MANAGER_PLAY, Unblock playing, type:102, auto:0, block:0
I (13912) ESP_AUDIO_CTRL: Enter play procedure, src:102
I (13922) ESP_AUDIO_CTRL: Play procedure, URL is ok, src:102
I (13922) ESP_AUDIO_CTRL: Request_CMD_Queue CMD:0, Available:5, que:0x3ffcae74
I (14052) ESP_AUDIO_CTRL: Func:_ctrl_play, Line:760, MEM Total:4106104 Bytes, Inter:118320 Bytes, Dram:106768 Bytes

I (14052) ESP_AUDIO_TASK: It's a decoder
I (14062) ESP_AUDIO_TASK: 1.CUR IN:[IN_http],CODEC:[DEC_auto],RESAMPLE:[48000],OUT:[OUT_iis],rate:0,ch:0,pos:0
I (14072) ESP_AUDIO_TASK: 2.Handles,IN:0x3f800b7c,CODEC:0x3f801940,FILTER:0x3f82cc6c,OUT:0x3f801aec
I (14082) ESP_AUDIO_TASK: 2.2 Update all pipeline
I (14082) ESP_AUDIO_TASK: 2.3 Linked new pipeline
I (14132) ESP_AUDIO_TASK: 3. Previous starting...
I (14132) AUDIO_ELEMENT: [IN_http-0x3f800b7c] Element task created
I (14132) AUDIO_ELEMENT: [IN_http] AEL_MSG_CMD_RESUME,state:1
I (14142) AUDIO_ELEMENT: [DEC_auto-0x3f801940] Element task created
I (14142) AUDIO_ELEMENT: [DEC_auto] AEL_MSG_CMD_RESUME,state:1
I (14152) ESP_AUDIO_TASK: Blocking MEDIA_CTRL_EVT_PLAY until received AEL_MSG_CMD_REPORT_MUSIC_INFO
I (14162) ESP_DECODER: Ready to do audio type check, pos:0, (line 104)
I (14252) HTTP_STREAM: total_bytes=51813
I (14652) ESP_DECODER: Detect audio type is MP3
I (14652) MP3_DECODER: MP3 opened
I (14652) ADF_BIT_STREAM: The element is 0x3f801940. The reserve data 2 is 0x0.
I (14702) RSP_FILTER: reset sample rate of source data : 24000, reset channel of source data : 2
I (14702) ESP_AUDIO_CTRL: Request_CMD_Queue CMD:1, Available:5, que:0x3ffcae74
I (14712) ESP_AUDIO_TASK: Received muisc info then send MEDIA_CTRL_EVT_PLAY
I (14722) ESP_AUDIO_TASK: MEDIA_CTRL_EVT_PLAY, status:UNKNOWN, 0
I (14722) AUDIO_ELEMENT: [resample-0x3f82cc6c] Element task created
I (14732) AUDIO_ELEMENT: [resample] AEL_MSG_CMD_RESUME,state:1
I (14742) AUDIO_ELEMENT: [OUT_iis-0x3f801aec] Element task created
I (14742) AUDIO_ELEMENT: [OUT_iis] AEL_MSG_CMD_RESUME,state:1
I (14752) I2S_STREAM: AUDIO_STREAM_WRITER
I (14772) RSP_FILTER: sample rate of source data : 24000, channel of source data : 2, sample rate of destination data : 48000, channel of destination data : 2
I (14782) ESP_AUDIO_TASK: ESP_AUDIO status is AEL_STATUS_STATE_RUNNING, 0, src:102
I (14782) ESP_AUDIO_TASK: Func:media_ctrl_task, Line:840, MEM Total:3512572 Bytes, Inter:84492 Bytes, Dram:72940 Bytes

I (14802) AUDIO_MANAGER: AUDIO MANAGER RECV STATUS:RUNNING, err:0, media_src:102
W (14802) PICTURE_BOOK_EXAMPLE: AUDIO_PLAYER_CALLBACK send OK, status:1, err_msg:0, media_src:102, ctx:0x0
I (14892) ESP_AUDIO_CTRL: Exit play procedure, ret:0
I (14892) AUDIO_MANAGER: AP_MANAGER_PLAY done exit
[vtpolicy][INFO]:vt_camera_preview_cb_()>line[215]:request NONE ret:2, resource type: 1
W (16622) HTTP_STREAM: No more data,errno:0, total_bytes:51813, rlen = 0
I (16622) AUDIO_ELEMENT: IN-[IN_http] AEL_IO_DONE,0
[vtpolicy][INFO]:vt_camera_preview_cb_()>line[215]:request NONE ret:2, resource type: 1
I (21482) AUDIO_ELEMENT: IN-[DEC_auto] AEL_IO_DONE,-2
I (22062) MP3_DECODER: Closed
I (22062) ESP_AUDIO_TASK: Received last pos: 51813 bytes
I (22162) AUDIO_ELEMENT: IN-[resample] AEL_IO_DONE,-2
I (22212) AUDIO_ELEMENT: IN-[OUT_iis] AEL_IO_DONE,-2
I (22252) ESP_AUDIO_TASK: Received last time: 7466 ms
I (22252) ESP_AUDIO_TASK: ESP_AUDIO status is AEL_STATUS_STATE_FINISHED, 0, src:102
I (22252) ESP_AUDIO_TASK: Func:media_ctrl_task, Line:840, MEM Total:3559692 Bytes, Inter:93332 Bytes, Dram:81780 Bytes

W (22262) ESP_AUDIO_TASK: Destroy the old pipeline
W (22332) ESP_AUDIO_TASK: The old pipeline destroyed
I (22332) AUDIO_MANAGER: AUDIO MANAGER RECV STATUS:FINISHED, err:0, media_src:102
W (22332) PICTURE_BOOK_EXAMPLE: AUDIO_PLAYER_CALLBACK send OK, status:4, err_msg:0, media_src:102, ctx:0x0
I (22342) AUDIO_MANAGER: AUDIO_STATUS_FINISHED, resume:0, is_backup:0, prepare_playing:0
I (22352) AUDIO_MANAGER: AUDIO_PLAYER_MODE_ONE_SONG

```

Load and run the example:

- The board will read book automatically, when you put book in front of the camera.

