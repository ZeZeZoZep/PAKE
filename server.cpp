// server.cpp
// TCP server che riceve un messaggio binario e risponde in binario

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include "ideal_cipher_pake.h"
namespace net = boost::asio;
using tcp = net::ip::tcp;


uint8_t ssid=223;
string password="password123";


void do_session(tcp::socket socket) {
    std::thread{[s = std::move(socket)]() mutable {
        try {
            std::vector<uint8_t> buffer(2550);
            std::size_t len = s.read_some(net::buffer(buffer));

/*             std::cout << "[Server] Received Epk (hex): ";
            for (std::size_t i = 0; i < len; ++i) {
                std::printf("%02X ", buffer[i]);
            } 
            std::cout << std::endl; */

            ID_PAKE server(ssid,password,vector<uint8_t>(buffer.begin(),buffer.begin()+len));
            auto ret = server.auth();
            auto c = ret.first;
            auto auth = ret.second;
/*             std::cout << "[Server] i'm sending c and auth (hex): ";
            for (std::size_t i = 0; i < concatVectors(c,auth).size(); ++i) {
                std::printf("%02X ", concatVectors(c,auth)[i]);
            }
            std::cout << std::endl; */
            // Rispondi con echo modificato
/*             std::cout << "[Server] i'm sending c and auth (hex): " << auth.size() << std::endl ; */
            net::write(s, net::buffer(concatVectors(c,auth)));

            vector<uint8_t> sessionkey_s= server.derive(c,auth);

            std::cout << "[Server] Exchanged SessionKey (hex): ";
            for (std::size_t i = 0; i < sessionkey_s.size(); ++i) {
                std::printf("%02X ", sessionkey_s[i]);
            }
            std::cout << std::endl;

        } catch (std::exception& e) {
            std::cerr << "[Errore sessione] " << e.what() << std::endl;
        }
    }}.detach();
}

int main() {
    try {
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

        std::cout << "[Server] In ascolto sulla porta 8080..." << std::endl;

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            do_session(std::move(socket));
        }

    } catch (std::exception& e) {
        std::cerr << "[Errore fatale] " << e.what() << std::endl;
    }
    return 0;
}
