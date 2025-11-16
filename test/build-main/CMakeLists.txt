
project(Mo-PE-main LANGUAGES CXX)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Mortis::Hook
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
   WIN32_EXECUTABLE FALSE
)

add_compile_options(${PROJECT_NAME}
	$<$<COMPILE_LANGUAGE:CXX>:/W0 /WX->
    $<$<COMPILE_LANGUAGE:C>:/W0 /WX->
)

