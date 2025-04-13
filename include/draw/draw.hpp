#pragma once

#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#ifndef LIMHAMN_PRIMITIVE_X11
#ifndef LIMHAMN_PRIMITIVE_WAYLAND
#error "Either X11 or Wayland must be defined"
#endif
#endif

#if LIMHAMN_PRIMITIVE_X11
#include <X11/Xlib.h>
#include <cairo/cairo-xlib.h>
#endif

struct FontManager {
private:
    unsigned int h{};
    PangoLayout* layout{};
    bool active{false};
public:
    explicit FontManager() = default;
    void init_font(const std::string& font) {
        if (this->active) {
            throw std::runtime_error("FontManager already initialized");
        }

        active = true;

        PangoFontMap* fontmap{};
        PangoContext* context{};
        PangoFontDescription* desc{};
        PangoFontMetrics* metrics{};

        if (font.empty()) {
            throw std::invalid_argument("Font name cannot be empty");
        }

        fontmap = pango_cairo_font_map_new();
        context = pango_font_map_create_context(fontmap);
        desc = pango_font_description_from_string(font.c_str());

        this->layout = pango_layout_new(context);

        if (!this->layout) {
            throw std::runtime_error{"Failed to load fonts."};
        }

        pango_layout_set_font_description(this->layout, desc);

        metrics = pango_context_get_metrics(context, desc, pango_language_from_string ("en-us"));
        this->h = pango_font_metrics_get_height(metrics) / PANGO_SCALE;

        pango_font_metrics_unref(metrics);
        g_object_unref(context);
    }
    [[nodiscard]] std::pair<int,int> estimate_length(const std::string& text, const int length = -1, bool markup = true) const {
        if (!this->layout) {
            throw std::runtime_error("FontManager not initialized");
        }
        if (text.empty()) {
            return {0, 0};
        }

        if (text.find('<') != std::string::npos && text.find('>') != std::string::npos) {
            markup = true;
        }

        PangoRectangle r;

        if (markup) {
            pango_layout_set_markup(this->layout, text.c_str(), static_cast<int>(length == -1 ? text.length() : length));
        } else {
            pango_layout_set_text(this->layout, text.c_str(), static_cast<int>(length == -1 ? text.length() : length));
        }

        pango_layout_get_extents(this->layout, nullptr, &r);

        if (markup) {
            pango_layout_set_attributes(this->layout, nullptr);
        }

        return {
            (r.width / PANGO_SCALE),
            this->h
        };
    }
    explicit FontManager(const std::string& font) {
        init_font(font);
    }
    [[nodiscard]] bool is_active() const {
        return this->active;
    }
    [[nodiscard]] unsigned int get_height() const {
        if (!this->layout) {
            throw std::runtime_error("FontManager not initialized");
        }
        return this->h;
    }
    ~FontManager() {
        if (this->layout) {
            g_object_unref(this->layout);
        }
    }

    friend class DrawManager;
};

enum class Protocol : int {
    X = 0,
    Wayland = 1,
    Unknown = -1
};

#if LIMHAMN_PRIMITIVE_X11
struct XWindow {
    Display* dpy{};
    int screen{};
    Window root{};
    Visual* visual{};
    unsigned int depth{};
    Colormap cmap{};
    Drawable drawable{};
    GC gc{};
};
#endif

#if LIMHAMN_PRIMITIVE_WAYLAND
struct WaylandWindow {
    void* data{};
};
#endif

struct ImageManager {
    void* img_data{};
    cairo_surface_t* img_surface{};
    cairo_t* img_d{};
};

struct DrawProperties {
    std::string foreground{};
    std::string background{};
    int foreground_alpha{255};
    int background_alpha{255};
    bool invert{false};
    bool filled{true};
};

struct DrawShapeProperties {
    std::string prev{};
    std::string next{};
    int prev_alpha{255};
    int next_alpha{255};
};

struct DrawPosition {
    int x{};
    int y{};
    int w{};
    int h{};
};

struct DrawCoords {
    int x{};
    int y{};
};

struct DrawSize {
    int w{};
    int h{};
};

// you shall hold this black box and you shall not see inside
// that is ze magic of see blaz blaz dwendy
class DrawManager {
    int w{}, h{};
    Protocol protocol{Protocol::Unknown};
#if LIMHAMN_PRIMITIVE_X11
    XWindow xwin{};
#endif
#if LIMHAMN_PRIMITIVE_WAYLAND
    WaylandWindow wlwin{};
#endif
    ImageManager img_manager{};
    FontManager font{};
    cairo_surface_t* surface{};
    cairo_t* d{};

    static void cairo_set_source_hex(cairo_t* cr, const char *col, int alpha) {
        unsigned int hex;

        sscanf(col, "#%x", &hex);

        double r = ((hex >> 16) & 0xFF) / 255.0;
        double g = ((hex >> 8) & 0xFF) / 255.0;
        double b = (hex & 0xFF) / 255.0;

        cairo_set_source_rgba(cr, r, g, b, alpha / 255.0);
    }
public:
    explicit DrawManager() = default;
#if LIMHAMN_PRIMITIVE_WAYLAND
    void initialize_wayland(void* data, int w, int h) {
        if (!data || !w || !h) {
            throw std::invalid_argument("Invalid arguments to DrawManager constructor");
        }

        this->protocol = Protocol::Wayland;
        this->wlwin.data = data;
        this->w = w;
        this->h = h;

        this->surface = cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->wlwin.data), CAIRO_FORMAT_ARGB32, w, h, w * 4);
        this->d = cairo_create(this->surface);

        if (!this->d) {
            throw std::runtime_error("Failed to create cairo context");
        }
    }
#endif
#if LIMHAMN_PRIMITIVE_X11
    void initialize_x11(Display* dpy, int screen, Window root, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap) {
        if (!dpy || !root || !visual || !depth) {
            throw std::invalid_argument("Invalid arguments to DrawManager constructor");
        }

        this->protocol = Protocol::X;
        this->xwin.dpy = dpy;
        this->xwin.screen = screen;
        this->xwin.root = root;
        this->w = static_cast<int>(w);
        this->h = static_cast<int>(h);
        this->xwin.visual = visual;
        this->xwin.depth = depth;
        this->xwin.cmap = cmap;

        this->xwin.drawable = XCreatePixmap(dpy, root, w, h, depth);

        if (!this->xwin.drawable) {
            throw std::runtime_error("Failed to create pixmap");
        }

        this->xwin.gc = XCreateGC(dpy, this->xwin.drawable, 0, nullptr);

        if (!this->xwin.gc) {
            XFreePixmap(dpy, this->xwin.drawable);
            throw std::runtime_error("Failed to create graphics context");
        }
        XSetLineAttributes(dpy, this->xwin.gc, 1, LineSolid, CapButt, JoinMiter);
    }
    DrawManager(Display* dpy, int screen, Window root, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap) {
        initialize_x11(dpy, screen, root, w, h, visual, depth, cmap);
    }
#endif
#if LIMHAMN_PRIMITIVE_WAYLAND
    DrawManager(void* data, int w, int h) {
        initialize_wayland(data, w, h);
    }
#endif
    void resize(const DrawSize& size) {
        this->w = size.w;
        this->h = size.h;
#if LIMHAMN_PRIMITIVE_X11
        if (this->protocol == Protocol::X) {
            if (!this->xwin.drawable) {
                throw std::runtime_error("Drawable not initialized");
            }

            XFreePixmap(this->xwin.dpy, this->xwin.drawable);

            this->xwin.drawable = XCreatePixmap(this->xwin.dpy, this->xwin.root, size.w, size.h, this->xwin.depth);
            if (!this->xwin.drawable) {
                throw std::runtime_error("Failed to create new pixmap");
            }
        } // no need for anything with wayland
#endif
    }
    void draw_image(void* data, const DrawPosition& coords) {
        if (!data || !coords.x || !coords.y || Protocol::Unknown == this->protocol) {
            throw std::invalid_argument("Invalid arguments to draw_image");
        }

        this->img_manager.img_data = data;
        this->img_manager.img_surface =
            cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->img_manager.img_data),
                CAIRO_FORMAT_ARGB32, coords.w, coords.h, coords.w * 4);

        if (!this->img_manager.img_surface) {
            throw std::runtime_error("Failed to create image surface");
        }

        cairo_set_operator(this->d, CAIRO_OPERATOR_OVER);

        cairo_set_source_surface(this->d, this->img_manager.img_surface, coords.x, coords.y);
        cairo_mask_surface(this->d, this->img_manager.img_surface, coords.x, coords.y);

        cairo_set_source_surface(this->d, this->surface, this->w, this->h);
    }
    void draw_arrow(const DrawPosition& pos, int direction, int slash, const DrawShapeProperties& props) const {
        int x = pos.x;
        int y = pos.y;
        int w = pos.w;
        int h = pos.h;

        x = direction ? x : x + w;
        w = direction ? w : - w;

        cairo_surface_t* sf = nullptr;

        if (this->protocol == Protocol::Wayland) {
#if LIMHAMN_PRIMITIVE_WAYLAND
            sf = cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->wlwin.data), CAIRO_FORMAT_ARGB32, this->w, this->h, this->w * 4);
#endif
        } else {
#if LIMHAMN_PRIMITIVE_X11
            sf = cairo_xlib_surface_create(this->xwin.dpy, this->xwin.drawable, this->xwin.visual, this->w, this->h);
#endif
        }

        if (!sf) {
            throw std::runtime_error("Failed to create surface");
        }

        double hh = slash ? (direction ? 0 : h) : h / 2;

        cairo_t* cr = cairo_create(sf);
        cairo_set_source_hex(cr, props.prev.c_str(), props.prev_alpha);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

        cairo_rectangle(cr, x, y, w, h);
        cairo_fill(cr);

        cairo_move_to(cr, x, y);
        cairo_line_to(cr, x + w, y + hh);
        cairo_line_to(cr, x, y + h);
        cairo_close_path(cr);

        cairo_set_source_hex(cr, props.next.c_str(), props.next_alpha);
        cairo_fill(cr);

        cairo_destroy(cr);
        cairo_surface_destroy(sf);
    }
    void draw_circle(const DrawPosition& pos, int direction, const DrawShapeProperties& props) const {
        cairo_surface_t *sf = nullptr;

        if (this->protocol == Protocol::Wayland) {
#if LIMHAMN_PRIMITIVE_WAYLAND
            sf = cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->wlwin.data), CAIRO_FORMAT_ARGB32, this->w, this->h, this->w * 4);
#endif
        } else {
#if LIMHAMN_PRIMITIVE_X11
            sf = cairo_xlib_surface_create(this->xwin.dpy, this->xwin.drawable, this->xwin.visual, this->w, this->h);
#endif
        }

        if (!sf) {
            throw std::runtime_error("Failed to create surface");
        }

        cairo_t *cr = cairo_create(sf);

        cairo_set_source_hex(cr, props.prev.c_str(), props.prev_alpha);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

        cairo_rectangle(cr, pos.x, pos.y, pos.w, pos.h);
        cairo_fill(cr);

        double cx = direction ? pos.x + pos.w - pos.h / 2.0 : pos.x + pos.h / 2.0;
        double cy = pos.y + pos.h / 2;
        double rad = pos.h / 2.0;

        double start = direction ? -M_PI_2 : M_PI_2;
        double end = direction ? M_PI_2 : 3 * M_PI_2;

        cairo_arc(cr, cx, cy, rad, start, end);
        cairo_close_path(cr);

        cairo_set_source_hex(cr, props.next.c_str(), props.next_alpha);
        cairo_fill(cr);

        cairo_destroy(cr);
        cairo_surface_destroy(sf);
    }
    void draw_rect(const DrawPosition& pos, const DrawProperties& props) const {
        cairo_surface_t *sf;

        if (this->protocol == Protocol::Wayland) {
#if LIMHAMN_PRIMITIVE_WAYLAND
            sf = cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->wlwin.data), CAIRO_FORMAT_ARGB32, this->w, this->h, this->w * 4);
#endif
        } else {
#if LIMHAMN_PRIMITIVE_X11
            sf = cairo_xlib_surface_create(this->xwin.dpy, this->xwin.drawable, this->xwin.visual, this->w, this->h);
#endif
        }

        cairo_t *cr = cairo_create(sf);

        if (!cr) {
            throw std::runtime_error("Failed to create cairo context");
        }

        cairo_set_source_hex(cr, props.invert ? props.background.c_str() : props.foreground.c_str(), props.invert ? props.background_alpha : props.foreground_alpha);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

        if (props.filled) {
            cairo_rectangle(cr, pos.x, pos.y, pos.w, pos.h);
            cairo_fill(cr);
        } else {
            cairo_rectangle(cr, pos.x, pos.y, pos.w - 1, pos.h - 1);
            cairo_stroke(cr);
        }

        cairo_destroy(cr);
        cairo_surface_destroy(sf);
    }
#if LIMHAMN_PRIMITIVE_X11
    void map(Window win) const {
        if (this->protocol == Protocol::X) {
            if (!this->xwin.drawable) {
                throw std::runtime_error("Drawable not initialized");
            }
            XCopyArea(this->xwin.dpy, this->xwin.drawable, win, this->xwin.gc, 0, 0, this->w, this->h, 0, 0);
            XFlush(this->xwin.dpy);
        } else {
            throw std::runtime_error("Mapping not supported for this protocol");
        }
    }
#endif
    void map() const {
        if (this->protocol == Protocol::X) {
            throw std::runtime_error("Window not provided. This method is a no-op for Wayland");
        }
    }
    void save_screen(const std::string& file) const {
        if (file.empty()) {
            throw std::invalid_argument("Invalid filename");
        }

        if (!this->surface) {
            throw std::runtime_error("No surface to save");
        }

        if (cairo_surface_write_to_png(this->surface, file.c_str()) != CAIRO_STATUS_SUCCESS) {
            throw std::runtime_error("Failed to save screen");
        }
    }
    void initialize_font(const std::string& font) {
        if (font.empty()) {
            throw std::invalid_argument("Font name cannot be empty");
        }

        this->font.init_font(font);
    }
    int draw_text(const DrawPosition& pos, int padding, const std::string& _text, bool markup, const DrawProperties& props) {
        int x = pos.x;
        int y = pos.y;
        unsigned int w = pos.w;
        unsigned int h = pos.h;

        const int render = x || y || w || h;

        if (!this->font.is_active()) {
            throw std::runtime_error("FontManager not initialized");
        }

        if (_text.empty()) {
            throw std::invalid_argument("Text cannot be empty");
        }

        if (!render) {
            w = ~w;
        } else {
            x += padding;
            w -= padding;

            if (protocol == Protocol::Wayland) {
#if LIMHAMN_PRIMITIVE_WAYLAND
                this->surface = cairo_image_surface_create_for_data(static_cast<unsigned char*>(this->wlwin.data), CAIRO_FORMAT_ARGB32, this->w, this->h, this->w * 4);
#endif
            } else {
#if LIMHAMN_PRIMITIVE_X11
                this->surface = cairo_xlib_surface_create(this->xwin.dpy, this->xwin.drawable, this->xwin.visual, this->w, this->h);
#endif
            }
            this->d = cairo_create(this->surface);

            cairo_set_source_hex(this->d, props.invert ? props.foreground.c_str() : props.background.c_str(), props.invert ? props.foreground_alpha : props.background_alpha);
            cairo_set_operator(this->d, CAIRO_OPERATOR_SOURCE);
            cairo_rectangle(this->d, x - padding, y, w + padding, h);
            cairo_fill(this->d);
        }

        // HACK: guess that there is no markup if there is no presence of "</"
        // not sure if it's really even necessary
        if (_text.find("</") == std::string::npos) {
            markup = false;
        }

        if (_text.empty()) {
            return x + (render ? w : 0);
        }

        int length = static_cast<int>(_text.length());
        int estimated_width = font.estimate_length(_text, _text.length(), markup).first;

        for (length = std::min(length, static_cast<int>(_text.length())); length && estimated_width > w; font.estimate_length(_text, length--, markup).first) {
            estimated_width = font.estimate_length(_text, length, markup).first;
        }

        if (!length) {
            return x + (render ? w : 0);
        }

        std::string text = _text.substr(0, length);
        if (length < static_cast<int>(_text.length())) {
            for (std::size_t i = length; i && i > length - 3; text[--i] = '.')
                ; // NOP
        }

        estimated_width = font.estimate_length(text, -1, markup).first;

        // HACK: guess that there is no markup if there is no presence of "</"
        // not sure if it's really even necessary
        if (text.find("</") == std::string::npos) {
            markup = false;
        }

        if (!render) {
            return x + estimated_width;
        }

        if (markup) {
            pango_layout_set_markup(this->font.layout, text.c_str(), length);
        } else {
            pango_layout_set_text(this->font.layout, text.c_str(), length);
        }

        pango_layout_set_single_paragraph_mode(this->font.layout, true);

        cairo_set_source_hex(this->d, props.foreground.c_str(), props.foreground_alpha);
        cairo_move_to(this->d, x, y + (h - this->font.get_height()) / 2);

        pango_cairo_update_layout(this->d, this->font.layout);
        pango_cairo_show_layout(this->d, this->font.layout);

        cairo_set_operator(this->d, CAIRO_OPERATOR_SOURCE);

        if (markup) {
            pango_layout_set_attributes(this->font.layout, nullptr);
        }

        return (x + estimated_width) + (w - estimated_width);
    }
    unsigned int get_text_width(const std::string& str, bool markup = false) {
        try {
            return draw_text({0, 0, 0, 0}, 0, str, markup, {"#000000", "#000000", 0, 0});
        } catch (const std::exception&) {
            return 0;
        }
    }
    unsigned int get_text_width_clamp(const std::string& str, int n, bool markup = false) {
        try {
            return std::min(n, draw_text({0, 0, 0, 0}, 0, str, markup, {"#000000", "#000000", 0, 0}));
        } catch (const std::exception&) {
            return n;
        }
    }
    FontManager& get_font_manager() {
        return this->font;
    }
    ~DrawManager() {
#if LIMHAMN_PRIMITIVE_X11
        if (this->protocol == Protocol::Unknown && this->xwin.drawable) {
            XFreePixmap(this->xwin.dpy, this->xwin.drawable);
        }
        if (this->protocol == Protocol::X && this->xwin.gc) {
            XFreeGC(this->xwin.dpy, this->xwin.gc);
        }
#endif
    }
};
