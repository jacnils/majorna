#pragma once

#if IMAGE

#include <errno.h>
#include <pwd.h>
#include <Imlib2.h>
#include <openssl/evp.h>

void setimagesize(int width, int height);
void setimageopts();
void cleanupimage();
void drawimage();
void flipimage();
void loadimage(const char *file, int *width, int *height);
void loadimagecache(const char *file, int *width, int *height);
void resizetoimageheight(int imageheight);
void store_image_vars();
void createifnexist_rec(const char* buf);
#if WAYLAND
void resizetoimageheight_wl(int imageheight);
#endif
#if X11
void resizetoimageheight_x11(int imageheight);
#endif

inline Imlib_Image image = NULL;

#endif
