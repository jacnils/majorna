#if IMAGE

#include <cerrno>
#include <pwd.h>
#include <Imlib2.h>
#include <openssl/evp.h>
#include <img.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <draw.hpp>
#include <x11/init.hpp>
#include <unistd.h>
#include <filesystem>

void setimagesize(int width, int height) {
    if (!image || hideimage || height < 5 || width < 5 || width > ctx.win_width - ctx.item_height) {
        return;
    }

    img.height = height;
    img.width = width;
}

void flipimage() {
    switch (img.flip) {
        case 1: // horizontal
            imlib_image_flip_horizontal();
            break;
        case 2: // vertical
            imlib_image_flip_vertical();
            break;
        case 3: // diagonal
            imlib_image_flip_diagonal();
            break;
        default:
            img.flip = img.flip ? 1 : 0;
            return;
    }
}

void cleanupimage() {
    if (image) { // free image using imlib2
        imlib_free_image();
        image = nullptr;
    }
}

void draw_image() {
    int width = 0, height = 0;
    char *limg = nullptr;

    if (!lines || !columns || hideimage || !imagetype) return;

    // load image cache
    if (selecteditem && selecteditem->image && strcmp(selecteditem->image, limg ? limg : "")) {
        if (img.longest_edge)
            loadimagecache(selecteditem->image, &width, &height);
    } else if ((!selecteditem || !selecteditem->image) && image) { // free image
        cleanupimage();
    }

    if (!image) {
        return;
    }

    // render the image
    if (img.longest_edge && width && height) {
        flipimage();

        int leftmargin = img.gaps; // gaps between image and menu
        int wtr = 0; // remove from w
        int wta = 0; // add to w
        int xta = 0; // add to x

        if (hideprompt && hideinput && hidemode && hidematchcount && hidecaps) {
            wtr = ctx.item_height;
        } else {
            wta = ctx.item_height;
        }

        // margin
        xta += menumarginh;
        wta += menumarginv;

        if (ctx.win_height != ctx.item_height + height + leftmargin * 2 - wtr && imageresize) { // menu height cannot be smaller than image height
            resizetoimageheight(imlib_image_get_height());
        }

        // render image on X11
        if (!imageposition) { // top mode = 0
            if (height > width)
                width = height;

            //draw_img(draw, leftmargin + (img.imagewidth - width) / 2 + xta, wta + leftmargin);
            draw.draw_image(imlib_image_get_data(), {
                .x = leftmargin + (img.width - width) / 2 + xta,
                .y = wta + leftmargin,
                .w = width,
                .h = height,
            });
        } else if (imageposition == 1) { // bottom mode = 1
            if (height > width)
                width = height;

            //draw_img(draw, leftmargin + (img.imagewidth - width) / 2 + xta, sp.mh - height - leftmargin);
            draw.draw_image(imlib_image_get_data(), {
                .x = leftmargin + (img.width - width) / 2 + xta,
                .y = ctx.win_height - height - leftmargin,
                .w = width,
                .h = height,
            });
        } else if (imageposition == 2) { // center mode = 2
            //draw_img(draw, leftmargin + (img.imagewidth - width) / 2 + xta, (sp.mh - wta - height) / 2 + wta);
            draw.draw_image(imlib_image_get_data(), {
                .x = leftmargin + (img.width - width) / 2 + xta,
                .y = (ctx.win_height - wta - height) / 2 + wta,
                .w = width,
                .h = height,
            });
        } else { // top center
            int minh = MIN(height, ctx.win_height - ctx.item_height - leftmargin * 2);
            //draw_img(draw, leftmargin + (img.imagewidth - width) / 2 + xta, (minh - height) / 2 + wta + leftmargin);
            draw.draw_image(imlib_image_get_data(), {
                .x = leftmargin + (img.width - width) / 2 + xta,
                .y = (minh - height) / 2 + wta + leftmargin,
                .w = width,
                .h = height,
            });
        }
    }

    if (selecteditem) {
        limg = selecteditem->image;
    } else {
        limg = nullptr;
    }
}

void setimageopts(void) {
    imlib_set_cache_size(8192 * 1024);
    imlib_set_color_usage(128);
}

void createifnexist(const char *dir) {
    // exists, so return
    if (access(dir, F_OK) == 0)
        return;

    // fatal: permission denied
    if (errno == EACCES)
        fprintf(stderr, "majorna: no access to create directory: %s\n", dir);

    std::filesystem::create_directories(dir);
}

void createifnexist_rec(const char *dir) {
    char *buf, *s = (char*)dir, *bs;

    if(!(buf = static_cast<char*>(malloc(strlen(s)+1))))
        return;

    memset(buf, 0, strlen(s)+1);

    for(bs = buf; *s; ++s, ++bs) {
        if(*s == '/' && *buf)
            createifnexist(buf);

        *bs = *s;
    }

    free(buf);
}

void loadimage(const char *file, int *width, int *height) {
    if (!file) return;

    image = imlib_load_image(file);

    if (!image)
        return;

    imlib_context_set_image(image);

    *width = imlib_image_get_width();
    *height = imlib_image_get_height();
}

void scaleimage(int *width, int *height) {
    int new_width, new_height;
    float aspect = 1.0f;

    // depending on what size, we determine aspect ratio
    if (img.width > *width) {
        aspect = (float)(*width)/img.width;
    } else {
        aspect = (float)img.width/(*width);
    }

    new_width = *width * aspect;
    new_height = *height * aspect;

    if ((new_width == *width && new_height == *height) || (!image))
        return;

    image = imlib_create_cropped_scaled_image(0,0,*width,*height,new_width,new_height);

    imlib_free_image();

    if(!image)
        return;

    imlib_context_set_image(image);

    *width = new_width;
    *height = new_height;
}

void loadimagecache(const char *file, int *width, int *height) {
    int slen = 0, i;
    unsigned int digest_len = EVP_MD_size(EVP_md5());
    unsigned char *digest = (unsigned char *)OPENSSL_malloc(digest_len);
    char *xdg_cache, *home = nullptr, *cachesize, *buf = nullptr;
    struct passwd *user_id = nullptr;

    // just load and don't store or try cache
    if (img.longest_edge > maxcache) {
        loadimage(file, width, height);
        if (image)
            scaleimage(width, height);
        return;
    }

    if (generatecache) {
        // try find image from cache first
        if(!(xdg_cache = getenv("XDG_CACHE_HOME"))) {
            if(!(home = getenv("HOME")) && (user_id = getpwuid(getuid()))) {
                home = user_id->pw_dir;
            }

            if(!home) { // no home directory could be grabbed.. somehow
                fprintf(stderr, "majorna: could not find home directory");
                return;
            }
        }

        // which cache do we try?
        cachesize = "normal";
        if (img.longest_edge > 128)
            cachesize = "large";

        slen = snprintf(nullptr, 0, "file://%s", file)+1;

        if(!(buf = static_cast<char*>(malloc(slen)))) {
            return;
        }

        // calculate md5 from path
        sprintf(buf, "file://%s", file);

        EVP_MD_CTX *mdcontext = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdcontext, EVP_md5(), nullptr);
        EVP_DigestUpdate(mdcontext, buf, slen);

        EVP_DigestFinal_ex(mdcontext, digest, &digest_len);
        EVP_MD_CTX_free(mdcontext);

        free(buf);

        char md5[digest_len*2+1];

        for (i = 0; i < digest_len; ++i)
            sprintf(&md5[i*2], "%02x", (unsigned int)digest[i]);

        // path for cached thumbnail
        if (cachedir.empty() || cachedir == "default") {
            if (xdg_cache || cachedir == "xdg")
                slen = snprintf(nullptr, 0, "%s/thumbnails/%s/%s.png", xdg_cache, cachesize, md5)+1;
            else
                slen = snprintf(nullptr, 0, "%s/.cache/thumbnails/%s/%s.png", home, cachesize, md5)+1;
        } else {
            slen = snprintf(nullptr, 0, "%s/%s/%s.png", cachedir, cachesize, md5)+1;
        }

        if(!(buf = static_cast<char*>(malloc(slen)))) {
            return;
        }

        if (cachedir.empty() || cachedir == "default") {
            if (xdg_cache)
                sprintf(buf, "%s/thumbnails/%s/%s.png", xdg_cache, cachesize, md5);
            else
                sprintf(buf, "%s/.cache/thumbnails/%s/%s.png", home, cachesize, md5);
        } else {
                sprintf(buf, "%s/%s/%s.png", cachedir, cachesize, md5);
        }

        loadimage(buf, width, height);

        if (image && *width < img.width && *height < img.height) {
            imlib_free_image();
            image = nullptr;
        } else if(image && (*width > img.width || *height > img.height)) {
            scaleimage(width, height);
        }

        // we are done
        if (image) {
            free(buf);
            return;
        }
    }

    // we din't find anything from cache, or it was just wrong
    loadimage(file, width, height);
    scaleimage(width, height);

    if (image) imlib_image_set_format("png");

    if (buf && generatecache && image) {
        createifnexist_rec(buf);
        imlib_save_image(buf);
        free(buf);
    }
}

void jumptoindex(unsigned int index) {
    unsigned int i;
    selecteditem = currentitem = matches;

    calcoffsets();

    for (i = 1; i < index; ++i) { // move to item index
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem;
            calcoffsets();
        }
    }
}

void resizetoimageheight(int imageheight) {
    if (!imagetype) return;
#if X11
    if (!protocol) {
        resizetoimageheight_x11(imageheight);
    } else {
#if WAYLAND
        resizetoimageheight_wl(imageheight);
#endif
    }
#elif WAYLAND
    resizetoimageheight_wl(imageheight);
#endif
}

#if X11
void resizetoimageheight_x11(int imageheight) {
    int mh = ctx.win_height, olines = lines;
    lines = img.set_lines;

    int x, y;

    if (lines * ctx.item_height < imageheight + img.gaps * 2) {
        lines = (imageheight + img.gaps * 2) / ctx.item_height;
    }

    get_mh();

    if (menuposition == 2) { // centered
        ctx.win_width = MIN(MAX(max_textw() + ctx.prompt_width, centerwidth), monitor.output_width);
        x = monitor.output_xpos + ((monitor.output_width  - ctx.win_width) / 2 + xpos);
        y = monitor.output_ypos + ((monitor.output_height - ctx.win_height) / 2 - ypos);
    } else { // top or bottom
        x = monitor.output_xpos + xpos;
        y = monitor.output_ypos + menuposition ? (-ypos) : (monitor.output_height - ctx.win_height - ypos);
        ctx.win_width = (menuwidth > 0 ? menuwidth : monitor.output_width);
    }

    if (!win || mh == ctx.win_height) {
        return;
    }

    XMoveResizeWindow(dpy, win, x + ctx.hpadding, y + ctx.vpadding, ctx.win_width - 2 * ctx.hpadding - borderwidth * 2, ctx.win_height);
    draw.resize({
        .w = ctx.win_width - 2 * ctx.hpadding - borderwidth * 2,
        .h = ctx.win_height,
    });

    if (olines != lines) {
        struct item *item;
        unsigned int i = 1;

        // walk through all matches
        for (item = matches; item && item != selecteditem; item = item->right)
            ++i;

        jumptoindex(i);
    }

    draw_menu();
}
#endif

#if WAYLAND
void resizetoimageheight_wl(int imageheight) {
    int mh = ctx.win_height, olines = lines;
    lines = img.set_lines;

    if (lines * ctx.item_height < imageheight + img.gaps * 2) {
        lines = (imageheight + img.gaps * 2) / ctx.item_height;
    }

    get_mh();

    if (mh == ctx.win_height) {
        return;
    }

    if (olines != lines) {
        struct item *item;
        unsigned int i = 1;

        // walk through all matches
        for (item = matches; item && item != selecteditem; item = item->right)
            ++i;

        jumptoindex(i);
    }

    state.width = ctx.win_width;
    state.height = ctx.win_height;

    state.buffer = create_buffer(&state);

    if (state.buffer == nullptr) {
        die("state.buffer == null");
    }

    set_layer_size(&state, state.width, state.height);
    draw.initialize(state.data, state.width, state.height);

    draw_menu();

    wl_surface_set_buffer_scale(state.surface, 1);
    wl_surface_attach(state.surface, state.buffer, 0, 0);
    wl_surface_damage(state.surface, 0, 0, state.width, state.height);
    wl_surface_commit(state.surface);
}
#endif

void store_image_vars(void) {
    img.width = imagewidth;
    img.height = imageheight;
    img.gaps = imagegaps;

    img.longest_edge = MAX(img.width, img.height);
}
#endif
