
// client.cpp
// TCP client che invia un messaggio binario e stampa la risposta

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include "timerset.h"
#include "ideal_cipher_pake.h"

namespace net = boost::asio;
using tcp = net::ip::tcp;
//using clock = std::chrono::high_resolution_clock;
uint8_t ssid=223;
string password="password123";

int main() {
    try {
        ID_PAKE client(ssid,password);
        net::io_context ioc;
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        TimerSet ts("OCAKE", "Exchange");
        ts.start("Total");
        auto endpoints = resolver.resolve("127.0.0.1", "8080");
        net::connect(socket, endpoints);
/*         std::cout <<client.Epk.size()<< std::endl;
         std::cout << "[Client] Sending Epk (hex): ";
        for (std::size_t i = 0; i < client.Epk.size(); ++i) {
            std::printf("%02X ", client.Epk[i]);
        }  
        std::cout << std::endl; */
        net::write(socket, net::buffer(client.Epk));

        // Ricevi risposta
        std::vector<uint8_t> buffer(2550);
        std::size_t len = socket.read_some(net::buffer(buffer));
/*         std::cout << "[Client] Received c and auth (hex): ";
        for (std::size_t i = 0; i < len; ++i) {
            std::printf("%02X ", buffer[i]);
        } */


        vector<uint8_t> c(buffer.begin(),buffer.begin()+len-32);

        vector<uint8_t> auth(buffer.begin()+len-32,buffer.begin()+len);

        client.check(c,auth);
        vector<uint8_t> sessionkey_c = client.derive(c,auth);
        
                ts.stop("Total");


        std::cout << "[Client] Exchanged SessionKey (hex): ";
        for (std::size_t i = 0; i < sessionkey_c.size(); ++i) {
            std::printf("%02X ", sessionkey_c[i]);
        }
        std::cout << std::endl;
        std::cout << std::endl;
        ts.commit(PARAM_D);
    } catch (std::exception& e) {
        std::cerr << "[Errore client] " << e.what() << std::endl;
    }
    return 0;
}


