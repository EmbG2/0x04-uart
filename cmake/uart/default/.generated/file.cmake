# The following variables contains the files used by the different stages of the build process.
set(uart_default_default_XC16_FILE_TYPE_assemble)
set(uart_default_default_XC16_FILE_TYPE_assemblePreproc)
set_source_files_properties(${uart_default_default_XC16_FILE_TYPE_assemblePreproc} PROPERTIES LANGUAGE C)
set(uart_default_default_XC16_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../newmainXC16.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../parser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../uart.c")
set(uart_default_default_XC16_FILE_TYPE_link)

# The (internal) path to the resulting build image.
set(uart_default_internal_image_name "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/uart/default/default.elf")

# The name of the resulting image, including namespace for configuration.
set(uart_default_image_name "uart_default_default.elf")

# The name of the image, excluding the namespace for configuration.
set(uart_default_original_image_name "default.elf")

# The output directory of the final image.
set(uart_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/uart")
