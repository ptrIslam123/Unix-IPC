#include <iostream>
#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/acceptor/multiplex_acceptor/tcp_multiplex_acceptor.h"

#include "engine/utils/scope_guard/scope_guard.h"


int main(int argc, char **argv) {
    if (argc != 3) {
        throw std::runtime_error("no passing input arguments");
    }

//    const std::string serverIpAddress = argv[1];
//    const short port = std::stoi(argv[2]);
//    net::Socket socket(std::make_unique<net::address::SocketAddressIpv4>(serverIpAddress, port));
//    socket.connect();
//
//    io::StaticBuffer<1024> buffer;
//    std::string msg;
//    std::cin >> msg;
//
//    buffer.write(msg.data(), msg.size());
//    socket.write(buffer, buffer.size());
//    buffer.clear();
//
//    socket.read(buffer, buffer.capacity());
//    std::cout << "receive from server: " << buffer.data() << std::endl;

//    auto deleter = [](int *ptr) {
//        std::cout << "Call deleter for scope guard: value: " << *ptr << std::endl;
//    };



    struct Foo {
        Foo(int val = 0): value_(val) {}
        Foo(Foo &&other): value_(other.value_) {
            other.value_ = 0;
        }
        Foo(const Foo &) = delete;

        int foo() {return value_;}
        int value_;
    };

    typedef Foo Type;


    struct Deleter {
        void operator()(Type *ptr) {
            std::cout << "Destroy ptr : " << ptr << std::endl;
        }
    };



   
    return 0;
}