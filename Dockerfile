FROM archlinux:latest

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm bash base-devel git cmake \
    libxinerama cairo pango wayland wayland-protocols libxkbcommon libx11 imlib2 fribidi libconfig git wl-clipboard

WORKDIR /workspace
VOLUME /workspace
CMD ["/bin/bash"]

