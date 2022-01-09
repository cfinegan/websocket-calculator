#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
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

void onMessage(Server* endpoint, ConnHandle handle, MessagePtr message) {
    //auto messageJson = json::parse(message->get_payload());
    endpoint->send(handle, message->get_payload(), message->get_opcode());
}

int main(int argc, char* argv[]) {

    Server endpoint;

    endpoint.set_error_channels(ELEVEL_ALL);
    endpoint.set_access_channels(ALEVEL_ALL ^ ALEVEL_FRAME_PAYLOAD);

    endpoint.init_asio();

    endpoint.set_message_handler(bind(&onMessage, &endpoint, ::_1, ::_2));

    endpoint.listen(8080);
    endpoint.start_accept();
    endpoint.run();
    
    return EXIT_SUCCESS;
}
