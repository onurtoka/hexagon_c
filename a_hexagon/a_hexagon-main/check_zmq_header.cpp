#include <zmq.hpp>
#include <iostream>

int main() {
    std::cout << "zmq.hpp dosyası başarıyla include edildi!" << std::endl;
    std::cout << "ZMQ version: " << ZMQ_VERSION_MAJOR << "." << ZMQ_VERSION_MINOR << "." << ZMQ_VERSION_PATCH << std::endl;
    return 0;
}
