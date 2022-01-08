#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <cstdlib>

using Server = websocketpp::server<websocketpp::config::asio>;

constexpr auto ELEVEL_ALL = websocketpp::log::elevel::all;
constexpr auto ALEVEL_ALL = websocketpp::log::alevel::all;
constexpr auto ALEVEL_FRAME_PAYLOAD = websocketpp::log::alevel::frame_payload;

int main(int argc, char* argv[]) {

    Server endpoint;

    endpoint.set_error_channels(ELEVEL_ALL);
    endpoint.set_access_channels(ALEVEL_ALL ^ ALEVEL_FRAME_PAYLOAD);

    endpoint.init_asio();

    // endpoint.set_open_handler()

    // endpoint.listen(8080);
    // endpoint.start_accept();
    // endpoint.run();
    
    return EXIT_SUCCESS;
}
