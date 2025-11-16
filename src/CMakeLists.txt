project(MoHook LANGUAGES CXX)


file(GLOB UtilsSourceFiles
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/ExportHeader/MoHook.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*.cpp
)

message(STATUS "Utils Source Files: ${UtilsSourceFiles}")

add_library(${PROJECT_NAME} STATIC ${UtilsSourceFiles})
add_library(Mortis::Hook ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/Base/APi
	${CMAKE_CURRENT_SOURCE_DIR}/Base/Concept
	${CMAKE_CURRENT_SOURCE_DIR}/Base/Template
	${CMAKE_CURRENT_SOURCE_DIR}/Base/Type

	${CMAKE_CURRENT_SOURCE_DIR}/ExecutableFile/ELF
	${CMAKE_CURRENT_SOURCE_DIR}/ExecutableFile/PE

	${CMAKE_CURRENT_SOURCE_DIR}/ExportHeader
	${CMAKE_CURRENT_SOURCE_DIR}/Hooker
)


target_link_libraries(${PROJECT_NAME} PUBLIC
	Mortis::Utils
	Detours::Detours
)