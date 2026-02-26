
idf_component_register(SRCS "main.c" "sdcard.c" "../../../mnist/mnist.c" "../../../tsetlin/tsetlin.c" "../../../tsetlin/clause.c" "../../../protobuf-c/protobuf-c.c" "../../../protobuf/tsetlin.pb-c.c"
                    INCLUDE_DIRS "." "../../../" "../../../tsetlin" "../../../mnist" "../../../protobuf" "../../../utils" "../../../model"
                    REQUIRES "fatfs" "esp_psram")

if (CONFIG_TSETLIN_USING_PROTOBUF)
    target_compile_definitions(${COMPONENT_LIB}
        PRIVATE TSETLIN_USING_PROTOBUF
    )
endif()

if (CONFIG_TSETLIN_USING_STATIC_MODEL)
    target_compile_definitions(${COMPONENT_LIB}
        PRIVATE TSETLIN_USING_STATIC_MODEL
        PRIVATE TSETLIN_MODEL_TRAINABLE
    )
endif()
