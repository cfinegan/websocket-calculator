#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib>

using Server = websocketpp::server<websocketpp::config::asio>;
using MessagePtr = Server::message_ptr;
using ConnHandle = websocketpp::connection_hdl;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using nlohmann::json;

constexpr auto ELEVEL_ALL = websocketpp::log::elevel::all;
constexpr auto ALEVEL_ALL = websocketpp::log::alevel::all;
constexpr auto ALEVEL_FRAME_PAYLOAD = websocketpp::log::alevel::frame_payload;

constexpr auto INVALID_PAYLOAD = websocketpp::close::status::invalid_payload;

void onMessage(Server* endpoint, ConnHandle handle, MessagePtr message) {

    std::string result;
    
    try {
        json messageJSON = json::parse(message->get_payload());
        std::string type = messageJSON["type"].get<std::string>();
        const json& payload = messageJSON["payload"];
        double a = payload["a"].get<double>();
        double b = payload["b"].get<double>();

        json resultJSON;
        if (type == "add") {
            resultJSON["type"] = "sum";
            resultJSON["payload"] = a + b;
        } else if (type == "subtract") {
            resultJSON["type"] = "difference";
            resultJSON["payload"] = a - b;
        } else {
            std::string reason = "Invalid operation type: ";
            reason += type;
            endpoint->close(handle, INVALID_PAYLOAD, reason);
            return;
        }
        result = resultJSON.dump();
    } catch (const json::exception& e) {
        // exception from the JSON layer means the payload is malformed
        endpoint->close(handle, INVALID_PAYLOAD, e.what());
        return;
    }

    endpoint->send(handle, result, message->get_opcode());
}

int main(int argc, char* argv[]) {

    Server endpoint;

    endpoint.set_error_channels(ELEVEL_ALL);
    endpoint.set_access_channels(ALEVEL_ALL ^ ALEVEL_FRAME_PAYLOAD);

    endpoint.init_asio();

    endpoint.set_message_handler(bind(&onMessage, &endpoint, ::_1, ::_2));

    // TODO: Set up a signal handler to close down server 
    // cleanly, then remove this.
    endpoint.set_reuse_addr(true);

    endpoint.listen(8080);
    endpoint.start_accept();
    endpoint.run();
    
    return EXIT_SUCCESS;
}
