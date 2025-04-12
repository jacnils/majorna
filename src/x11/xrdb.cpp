/* See LICENSE file for copyright and license details. */

#include <x11/xrdb.hpp>
#include <x11/x11_libs.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <cstdio>
#include <cstring>

#if XRESOURCES
void resource_load(XrmDatabase db, char *name, enum resource_type rtype, void *dst) {
    char *sdst = nullptr;
    int *idst = nullptr;
    float *fdst = nullptr;
    sdst = static_cast<char*>(dst);
    idst = static_cast<int*>(dst);
    fdst = static_cast<float*>(dst);
    char fullname[256];
    char *type;
    XrmValue ret;
    snprintf(fullname, sizeof(fullname), "%s.%s", "majorna", name);
    fullname[sizeof(fullname) - 1] = '\0';
    XrmGetResource(db, fullname, "*", &type, &ret);
    if (!xresources) return;
    if (!(ret.addr == nullptr || strncmp("String", type, 64))) {
        switch (rtype) { // type
            case STRING:
                sp_strncpy(sdst, ret.addr, strlen(sdst));
                break;
            case INTEGER:
                *idst = strtoul(ret.addr, nullptr, 10);
                break;
            case FLOAT:
                *fdst = strtof(ret.addr, nullptr);
                break;
        }
    }
}

void load_xresources(void) {
    Display *display;
    char *resm;
    XrmDatabase db;
    ResourcePref *p;
    display = XOpenDisplay(nullptr);
    resm = XResourceManagerString(display);
    if (!resm || !xresources)
        return;
    db = XrmGetStringDatabase(resm);

    for (p = cols; p < cols + LENGTH(cols); p++)
        resource_load(db, p->name, p->type, p->dst);

    XCloseDisplay(display);
}
#endif
