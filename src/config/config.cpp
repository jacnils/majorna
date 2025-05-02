/* See LICENSE file for copyright and license details. */
#include <config/config.hpp>
#include <options.hpp>
#include <filesystem>
#include <iostream>

void conf_init() {
    std::string file{};

    // get path for configuration file
    if (configfile.empty()) {
        const char* xdg_conf = getenv("XDG_CONFIG_HOME");
        if (!xdg_conf) {
            // ~/.config/majorna/majorna.py
            const char* home = getenv("HOME");
            if (!home) {
                die("majorna: HOME environment variable is not set");
            }
            file = std::string(home) + "/.config/majorna/majorna.py";
        } else {
            // XDG_CONFIG_HOME is set, so let's use that instead
            configfile = std::string(xdg_conf) + "/majorna/majorna.py";
        }
    } else { // custom config path
        file = configfile;
    }

    if (!std::filesystem::is_regular_file(file) || std::system("python3 --version > /dev/null 2>&1") != 0) {
        return;
    }

    int result = std::system(std::string("python3 " + file).c_str());
    if (result != 0) {
        throw std::runtime_error{"Failed to execute the Python configuration file. Halting."};
    }

    // handling is performed by the socket server
}
