cmake_minimum_required(VERSION 3.16)
project(majorna VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(X11 "Enable X11 support" ON)
option(WAYLAND "Enable Wayland support" ON)
option(IMLIB2 "Enable Imlib2 support" ON)
option(OPENSSL "Enable OpenSSL support" ON)
option(FRIBIDI "Enable Fribidi support" ON)
option(XINERAMA "Enable Xinerama support" ON)
option(LIBCONFIG "Enable Libconfig support" ON)
option(XRESOURCES "Enable Xresources support" ON)
option(REGEX "Enable Regex support" ON)
option(FIFO "Enable FIFO support" ON)

include_directories(include)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set(WAYLAND OFF) # not supported on macOS
endif()

set(PROJECT_SOURCE_FILES
        src/config/config.cpp
        src/draw/draw.cpp
        src/regex/regex.c
        src/arg.cpp
        src/argv.cpp
        src/draw.cpp
        src/fifo.cpp
        src/history.cpp
        src/img.cpp
        src/macros.cpp
        src/majorna.cpp
        src/match.cpp
        src/rtl.cpp
        src/schemes.cpp
        src/sort.cpp
        src/stream.cpp
)

# TODO replace with a more modern alternative
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(PANGOCAIRO REQUIRED pangocairo)
find_library(M_LIB m)

set(PROJECT_DEPENDENCIES
    ${CAIRO_LIBRARIES}
    ${PANGO_LIBRARIES}
    ${PANGOCAIRO_LIBRARIES}
    ${M_LIB}
)

set(BUILD_ARGS
    -DVERSION="${PROJECT_VERSION}"
)

if(X11)
    pkg_check_modules(X11 REQUIRED x11)
    pkg_check_modules(XRENDER REQUIRED xrender)
    list(APPEND PROJECT_DEPENDENCIES ${X11_LIBRARIES} ${XRENDER_LIBRARIES})
    list(APPEND BUILD_ARGS -DX11)
    list(APPEND PROJECT_SOURCE_FILES
        src/x11/client.cpp
        src/x11/clipboard.cpp
        src/x11/event.cpp
        src/x11/focus.cpp
        src/x11/init.cpp
        src/x11/key.cpp
        src/x11/mouse.cpp
        src/x11/xim.cpp
        src/x11/xrdb.cpp
    )
endif()

if(WAYLAND)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_SCANNER REQUIRED wayland-scanner)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
    list(APPEND PROJECT_DEPENDENCIES
        ${WAYLAND_CLIENT_LIBRARIES}
        ${WAYLAND_SCANNER_LIBRARIES}
        ${WAYLAND_PROTOCOLS_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}
    )
    add_custom_command(
        OUTPUT protocols_generated
        COMMAND bash protocols/generate.sh
        DEPENDS protocols/generate.sh
    )
    list(APPEND PROJECT_SOURCE_FILES
        src/wl/wlr-layer-shell-unstable-v1-protocol.c
        src/wl/xdg-output-unstable-v1-protocol.c
        src/wl/xdg-shell-protocol.c
        src/wl/init.cpp
        src/wl/shm.cpp
        src/wl/wayland.cpp
    )
    list(APPEND BUILD_ARGS -DWAYLAND)
endif()

if(IMLIB2 AND OPENSSL)
    pkg_check_modules(IMLIB2 REQUIRED imlib2)
    pkg_check_modules(OPENSSL REQUIRED openssl)
    list(APPEND PROJECT_DEPENDENCIES ${IMLIB2_LIBRARIES} ${OPENSSL_LIBRARIES})
    list(APPEND BUILD_ARGS -DIMAGE)
endif()

if(FRIBIDI)
    pkg_check_modules(FRIBIDI REQUIRED fribidi)
    list(APPEND PROJECT_DEPENDENCIES ${FRIBIDI_LIBRARIES})
    list(APPEND BUILD_ARGS -DRTL)
endif()

if(XINERAMA AND X11)
    pkg_check_modules(XINERAMA REQUIRED xinerama)
    list(APPEND PROJECT_DEPENDENCIES ${XINERAMA_LIBRARIES})
    list(APPEND BUILD_ARGS -DXINERAMA)
endif()

if(LIBCONFIG)
    pkg_check_modules(LIBCONFIG REQUIRED libconfig)
    list(APPEND PROJECT_DEPENDENCIES ${LIBCONFIG_LIBRARIES})
    list(APPEND BUILD_ARGS -DCONFIG)
endif()

if(XRESOURCES AND X11)
    list(APPEND BUILD_ARGS -DXRESOURCES)
endif()

if(REGEX)
    list(APPEND BUILD_ARGS -DREGEX)
endif()

if(FIFO)
    list(APPEND BUILD_ARGS -DFIFO)
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CAIRO_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${PANGOCAIRO_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
    ${WAYLAND_SCANNER_INCLUDE_DIRS}
    ${WAYLAND_PROTOCOLS_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
    ${IMLIB2_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    ${FRIBIDI_INCLUDE_DIRS}
    ${LIBCONFIG_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_DEPENDENCIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${BUILD_ARGS})
