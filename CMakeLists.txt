cmake_minimum_required(VERSION 3.10)
project(lvgl)

set(CONF_PATH "${PROJECT_SOURCE_DIR}/lv_conf.h")

foreach(BACKEND_NAME "SDL" "LINUX_DRM" "LINUX_FBDEV" "X11" "WAYLAND" "OPENGLES" "EVDEV")

    execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    COMMAND "scripts/backend_conf.sh" ${BACKEND_NAME} ${CONF_PATH} OUTPUT_VARIABLE IS_BACKEND_ENABLED)
    set("LV_USE_${BACKEND_NAME}" ${IS_BACKEND_ENABLED})

endforeach()

# Uncomment if the program needs debugging
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if (LV_USE_EVDEV)
    message("Including EVDEV support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EVDEV REQUIRED libevdev)

    list(APPEND PKG_CONFIG_LIB ${EVDEV_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${EVDEV_INCLUDE_DIRS})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/indev_backends/evdev.c)

endif()

# Configuration
if (LV_USE_SDL)

    message("Including SDL2 support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

    list(APPEND PKG_CONFIG_LIB ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/sdl.c)
endif()

if (LV_USE_LINUX_FBDEV)

    # FBDEV has no dependencies
    message("Including FBDEV support")
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/fbdev.c)

endif()

foreach(arg ${PKG_CONFIG_LIB})
    string(APPEND LVGL_PKG_CONFIG_EXT_LIB " -l${arg}")
endforeach()

string(APPEND LVGL_PKG_CONFIG_LIB "-llvgl_linux")

file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src/lib ${PKG_CONFIG_INC})
add_library(lvgl_linux STATIC ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
target_include_directories(lvgl_linux PRIVATE ${LV_LINUX_INC} ${PROJECT_SOURCE_DIR})

# Add app and launcher source files
set(SYSTEM_SRC
    src/system/theme_manager/theme_manager.c
    src/system/app_manager.c
    src/lib/lv_fs_if/lv_fs_if.c # for the file driver system
)

set(APP_SOURCES
    src/system/app_registry/app_registry.c
    src/launcher/launcher.c
    # src/apps/sms/sms.c
    # src/apps/notes/notes.c
    # src/apps/calculator/calculator.c
)

# add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
add_executable(
    lvglsim src/main.c
    ${LV_LINUX_SRC}
    ${LV_LINUX_BACKEND_SRC}
    ${APP_SOURCES}
    ${SYSTEM_SRC}
)

# 👇 Add this line after `add_executable`: for enabling dynamic access of global variables
target_link_options(lvglsim PRIVATE -rdynamic)

# adding cJSON
find_path(CJSON_INCLUDE_DIR cJSON.h PATH_SUFFIXES cjson)
find_library(CJSON_LIBRARY NAMES cjson)

if(NOT CJSON_INCLUDE_DIR OR NOT CJSON_LIBRARY)
    message(FATAL_ERROR "cJSON library not found")
endif()

include_directories(${CJSON_INCLUDE_DIR})
set(CJSON_LIBRARIES ${CJSON_LIBRARY})

# adding libpng
find_path(PNG_INCLUDE_DIR png.h
          PATHS /usr/include /usr/local/include /opt/homebrew/include # Common locations
          PATH_SUFFIXES libpng) # Sometimes it's in a subdirectory

find_library(PNG_LIBRARY
             NAMES png libpng libpng16 libpng12 # Common names
             PATHS /usr/lib /usr/local/lib /opt/homebrew/lib # Common locations
             )

if(NOT PNG_INCLUDE_DIR OR NOT PNG_LIBRARY)
    message(FATAL_ERROR "libpng library not found. Please install it.")
endif()

include_directories(${PNG_INCLUDE_DIR})

set(PNG_LIBRARY ${PNG_LIBRARY}) # Optionally, explicitly set this variable

target_link_libraries(lvglsim lvgl_linux lvgl lvgl::thorvg m pthread ${PKG_CONFIG_LIB} ${CJSON_LIBRARIES} ${PNG_LIBRARY})

# Install the lvgl_linux library and its headers
install(DIRECTORY src/lib/
    DESTINATION include/lvgl
    FILES_MATCHING
    PATTERN "backends*" EXCLUDE
    PATTERN "*.h")

install(TARGETS lvgl_linux
    ARCHIVE DESTINATION lib
)

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)
