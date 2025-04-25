#include "asio/local/stream_protocol.hpp"
#include <iostream>
#include <istream>
#include <sockets.hpp>
#include <options.hpp>
#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <thread>

using asio::local::stream_protocol;
using json = nlohmann::json;

class Session : public std::enable_shared_from_this<Session> {
    public:
        explicit Session(stream_protocol::socket socket,
                const std::function<std::string(const std::string&)>& callback) : socket_(std::move(socket)), callback_(callback) {};

        void start() {
            read();
        }
    private:
        void read() {
            auto self = shared_from_this();
            asio::async_read_until(socket_, buffer_, '\n',
                    [this](asio::error_code ec, std::size_t bytes_transferred) {
                        if (!ec) {
                            std::istream stream(&buffer_);
                            std::string data{};
                            std::getline(stream, data);

                            const std::string& ret = callback_(data);

                            if (!ret.empty()) {
                                write(ret);
                            }
                        } else if (ec == asio::error::eof) {
                            return;
                        } else {
                            std::cerr << "Failed to read from socket: " << ec.message() << "\n";
                        }
                    }
            );
        }
        void write(const std::string& response) {
            auto self = shared_from_this();

            asio::async_write(socket_, asio::buffer(response),
                    [this](asio::error_code ec, std::size_t) {
                        if (!ec) {
                            read();
                        }
                    }
            );
        }

        stream_protocol::socket socket_;
        asio::streambuf buffer_{};
        std::function<std::string(const std::string&)> callback_{};
};

class Server {
    public:
        Server(asio::io_context& ctx, const std::string& path, const std::function<std::string(const std::string&)>& callback) : acceptor_(ctx, stream_protocol::endpoint(path)), callback_(callback) {
                accept();
            }
    private:
        void accept() {
            acceptor_.async_accept(
                    [this](asio::error_code ec, stream_protocol::socket socket) {
                        if (!ec) {
                            std::make_shared<Session>(std::move(socket), callback_)->start();
                        }
                        accept();
                    }
            );
        }

        stream_protocol::acceptor acceptor_;
        std::function<std::string(const std::string&)> callback_;
};

void initialize_socket() {
    std::thread t([&]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::filesystem::remove(socketfile);

        asio::io_context ctx;
        Server server(ctx, socketfile, handler);

        ctx.run();
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
            if (!call.contains("arg")) continue;

            const std::string& fn = call["function"].get<std::string>();

            const auto it = arg_map.find(fn);
            if (it == arg_map.end()) {
                const auto v_it = void_map.find(fn);
                if (v_it == void_map.end()) {
                    return_json["error_str"] = "Function not found.";
                    return_json["error_type"] = "MAJORNA_FUNCTION_NOT_FOUND";
                }

                if (call.contains("arg")) {
                    return_json["error_str"] = "Function does not accept arguments.";
                    return_json["error_type"] = "MAJORNA_INVALID_ARGUMENT";
                    return return_json.dump();
                }

                if (v_it->second) {
                    v_it->second();
                    functions_called.push_back(fn);
                }
            } else {
                Arg arg{};
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

                it->second(arg);
                functions_called.push_back(fn);
            }
        }

        return_json["actions_performed"].push_back("calls");
        return_json["functions_called"] = functions_called;
    }

    return return_json.dump();
}
