/* See LICENSE file for copyright and license details. */

#include <x11/xrdb.hpp>
#include <x11/x11_libs.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <cstdio>
#include <cstring>

#if X11

void resource_load(XrmDatabase db, const std::string& name, resource_type rtype, std::any& dst) {
    std::string fullname = "majorna." + name;
    char* type = nullptr;
    XrmValue ret;

    XrmGetResource(db, fullname.c_str(), "*", &type, &ret);
    if (!xresources || ret.addr == nullptr || strncmp("String", type, 64) != 0) {
        return;
    }

    try {
        switch (rtype) {
        case String: {
                auto& sdst = std::any_cast<std::string&>(dst);
                sdst = std::string(ret.addr);
                break;
        }
        case Integer: {
                auto& idst = std::any_cast<int&>(dst);
                idst = std::stoi(ret.addr);
                break;
        }
        case Float: {
                auto& fdst = std::any_cast<float&>(dst);
                fdst = std::stof(ret.addr);
                break;
        }
        }
    } catch (const std::bad_any_cast& e) {
        fprintf(stderr, "Error: %s\n", e.what());
    }
}

void load_xresources() {
    Display* disp = XOpenDisplay(nullptr);
    char* resm = XResourceManagerString(disp);

    if (!resm || !xresources)
        return;

    XrmDatabase db = XrmGetStringDatabase(resm);

    for (auto& it : cols) {
        resource_load(db, it.name, it.type, it.dst);
    }
}

#endif
