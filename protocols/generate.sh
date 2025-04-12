#!/bin/sh
headers() {
    [ ! -x "$(command -v wayland-scanner)" ] && printf "wayland-scanner must be installed for this action.\n" && exit 1

    wayland-scanner \
        client-header \
        protocols/wlr-layer-shell-unstable-v1.xml \
        include/wl/wlr-layer-shell-unstable-v1-client-protocol.h

    wayland-scanner \
        client-header \
        protocols/xdg-shell.xml \
        include/wl/xdg-shell-client-protocol.h

    wayland-scanner \
        client-header \
        protocols/xdg-output-unstable-v1.xml \
        include/wl/xdg-output-unstable-v1-client-protocol.h

    wayland-scanner \
        public-code \
        protocols/wlr-layer-shell-unstable-v1.xml \
        src/wl/wlr-layer-shell-unstable-v1-protocol.c

    wayland-scanner \
        public-code \
        protocols/xdg-shell.xml \
        src/wl/xdg-shell-protocol.c

    wayland-scanner \
        public-code \
        protocols/xdg-output-unstable-v1.xml \
        src/wl/xdg-output-unstable-v1-protocol.c
}

clean_headers() {
    rm -f \
        src/wl/xdg-output-unstable-v1-protocol.c \
        src/wl/xdg-shell-protocol.c \
        src/wl/wlr-layer-shell-unstable-v1-protocol.c \
        include/wl/xdg-output-unstable-v1-client-protocol.h \
        include/wl/xdg-shell-client-protocol.h \
        include/wl/wlr-layer-shell-unstable-v1-client-protocol.h
}

clean_headers;headers

# workaround for c++ incompatibility
sed -i "s/namespace/_namespace/g" include/wl/wlr-layer-shell-unstable-v1-client-protocol.h
