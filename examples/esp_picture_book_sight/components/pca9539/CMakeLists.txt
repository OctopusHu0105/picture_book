set(COMPONENT_SRCS "pca9539.c")


set(COMPONENT_ADD_INCLUDEDIRS . include)

set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES audio_sal audio_board mbedtls esp_peripherals display_service esp_dispatcher)

register_component()

