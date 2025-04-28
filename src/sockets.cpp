#include <sockets.hpp>
#include <options.hpp>
#include <nlohmann/json.hpp>
#define LIMHAMN_SOCKET_UDS_IMPL
#include <limhamn/socket/socket_uds.hpp>

using json = nlohmann::json;

void initialize_socket() {
    std::thread t([&]{
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (std::filesystem::exists(socketfile)) {
            std::filesystem::remove(socketfile);
        }

        limhamn::socket::uds_server server(socketfile, handler, "\n", true);
    });

    t.detach();
}

std::string handler(const std::string& input) {
    nlohmann::json input_json{};

    try {
        input_json = nlohmann::json::parse(input);
    } catch (const std::exception&) {
        nlohmann::json return_json;
        return_json["error_str"] = "Invalid JSON.";
        return_json["error_type"] = "MAJORNA_INVALID_JSON";
        return return_json.dump();
    }

    nlohmann::json return_json{};

    return_json["actions_performed"] = nlohmann::json::array();

    std::vector<std::string> functions_called{};
    if (input_json.contains("calls") && input_json["calls"].is_array()) {
        for (const auto& call : input_json["calls"]) {
            if (!call.is_object()) continue;
            if (!call.contains("function") || !call["function"].is_string()) continue;

            const std::string& fn = call["function"].get<std::string>();

            const auto it = arg_map.find(fn);
            if (it == arg_map.end()) {
                const auto v_it = void_map.find(fn);
                if (v_it == void_map.end()) {
                    return_json = {};
                    return_json["error_str"] = "Function '" + fn + "' not found.";
                    return_json["error_type"] = "MAJORNA_FUNCTION_NOT_FOUND";
                    return return_json.dump();
                }

                if (call.contains("arg")) {
                    return_json = {};
                    return_json["error_str"] = "Function does not accept arguments.";
                    return_json["error_type"] = "MAJORNA_INVALID_ARGUMENT";
                    return return_json.dump();
                }

                if (v_it->second) {
                    v_it->second();
                    functions_called.push_back(fn);
                } else {
                    return_json = {};
                    return_json["error_str"] = "Function is not valid. This is probably an error out of your control. Function call: " + fn;
                    return_json["error_type"] = "MAJORNA_INVALID_FUNCTION";
                    return return_json.dump();
                }
            } else {
                Arg arg{};
                if (call.contains("arg")) {
                    const auto& arg_val = call["arg"];

                    if (arg_val.is_string()) {
                        arg.c = arg_val.get<std::string>();
                    } else if (arg_val.is_number_unsigned()) {
                        arg.ui = arg.i = arg_val.get<unsigned int>();
                    } else if (arg_val.is_number_integer()) {
                        arg.i = arg_val.get<int>();
                    } else if (arg_val.is_number_float()) {
                        arg.f = arg_val.get<float>();
                    }
                }

                if (it->second) {
                    it->second(arg);
                    functions_called.push_back(fn);
                } else {
                    return_json = {};
                    return_json["error_str"] = "Function is not valid. This is probably an error out of your control. Function call: " + fn;
                    return_json["error_type"] = "MAJORNA_INVALID_FUNCTION";
                    return return_json.dump();
                }
            }
        }

        return_json["actions_performed"].push_back("calls");
        return_json["functions_called"] = functions_called;
    }

    return return_json.dump();
}
