if(WIN32)
    set(ENV{http_proxy} "http://127.0.0.1:10808")
    set(ENV{https_proxy} "http://127.0.0.1:10808")
    message(STATUS "Set proxy for Windows build")
endif()
    message(STATUS "Fetching third-party packages...")
include(FetchContent)

FetchContent_Declare(
    MortisUtils
    GIT_REPOSITORY  https://github.com/masoame/MortisUtils.git
)
FetchContent_MakeAvailable(MortisUtils)

include(${CMAKE_CURRENT_LIST_DIR}/thirdparty/detours_config.cmake)