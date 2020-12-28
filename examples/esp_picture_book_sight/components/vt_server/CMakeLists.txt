set(COMPONENT_SRCS "vt_camera.c" "vt_handle.c" "vt_http.c" "vt_media.c" "vt_os.c" "vt_recognize.c")


set(COMPONENT_ADD_INCLUDEDIRS . include)

set(COMPONENT_REQUIRES  esp-adf-libs audio_pipeline audio_board esp_http_client esp32-camera tcp_transport audio_hal audio_player vtbk_lib)
#audio_pipeline playlist audio_stream  esp_peripherals bluetooth_service esp_event_cast
register_component()

#target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
#target_link_libraries(${COMPONENT_TARGET} INTERFACE vtbk)
