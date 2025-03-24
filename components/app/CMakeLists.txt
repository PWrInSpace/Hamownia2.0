idf_component_register( SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES device_config data timers)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")