include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(uart_default_library_list )

# Handle files with suffix s, for group default-XC16
if(uart_default_default_XC16_FILE_TYPE_assemble)
add_library(uart_default_default_XC16_assemble OBJECT ${uart_default_default_XC16_FILE_TYPE_assemble})
    uart_default_default_XC16_assemble_rule(uart_default_default_XC16_assemble)
    list(APPEND uart_default_library_list "$<TARGET_OBJECTS:uart_default_default_XC16_assemble>")
endif()

# Handle files with suffix S, for group default-XC16
if(uart_default_default_XC16_FILE_TYPE_assemblePreproc)
add_library(uart_default_default_XC16_assemblePreproc OBJECT ${uart_default_default_XC16_FILE_TYPE_assemblePreproc})
    uart_default_default_XC16_assemblePreproc_rule(uart_default_default_XC16_assemblePreproc)
    list(APPEND uart_default_library_list "$<TARGET_OBJECTS:uart_default_default_XC16_assemblePreproc>")
endif()

# Handle files with suffix c, for group default-XC16
if(uart_default_default_XC16_FILE_TYPE_compile)
add_library(uart_default_default_XC16_compile OBJECT ${uart_default_default_XC16_FILE_TYPE_compile})
    uart_default_default_XC16_compile_rule(uart_default_default_XC16_compile)
    list(APPEND uart_default_library_list "$<TARGET_OBJECTS:uart_default_default_XC16_compile>")
endif()

add_executable(${uart_default_image_name} ${uart_default_library_list})

target_link_libraries(${uart_default_image_name} PRIVATE ${uart_default_default_XC16_FILE_TYPE_link})

# Add the link options from the rule file.
uart_default_link_rule(${uart_default_image_name})

# Add bin2hex target for converting built file to a .hex file.
add_custom_target(uart_default_Bin2Hex ALL
    ${MP_BIN2HEX} ${uart_default_image_name})
add_dependencies(uart_default_Bin2Hex ${uart_default_image_name})

# Post build target to copy built file to the output directory.
add_custom_command(TARGET ${uart_default_image_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${uart_default_output_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy ${uart_default_image_name} ${uart_default_output_dir}/${uart_default_original_image_name}
                    BYPRODUCTS ${uart_default_output_dir}/${uart_default_original_image_name})
