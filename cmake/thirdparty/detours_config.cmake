project(Detours)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(FATAL_ERROR "Detours Only Windows")
endif()

file(GLOB Detours_PATH ${CMAKE_BINARY_DIR}/_deps/Detours*)

if(NOT Detours_PATH)
    file(DOWNLOAD 
        https://github.com/microsoft/Detours/archive/refs/tags/v4.0.1.zip
        Detours.zip
    )
    execute_process(COMMAND 
        tar -xzvf Detours.zip -C ${CMAKE_BINARY_DIR}/_deps
    )
    file(GLOB Detours_PATH ${CMAKE_BINARY_DIR}/_deps/Detours*)
endif()

file(GLOB CPP_FILES 
    ${Detours_PATH}/src/*.cpp
)

list(REMOVE_ITEM
    CPP_FILES
    ${Detours_PATH}/src/uimports.cpp
)

add_library(${PROJECT_NAME} STATIC 
    ${CPP_FILES} 
    ${Detours_PATH}/src/detours.h
    ${Detours_PATH}/src/detver.h
)
add_library(Detours::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC  
    ${Detours_PATH}/src
)

target_compile_options(${PROJECT_NAME} PRIVATE /Zc:gotoScope- /wd4163 /WX )