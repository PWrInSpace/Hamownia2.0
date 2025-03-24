idf_component_register(SRCS "Hamownia2.0.c"
                    INCLUDE_DIRS ".")

set(KCONFIG_PATH "Kconfig.projbuild") 
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
