#include <zmq.h>
#include <iostream>

int main() {
    void* context = zmq_ctx_new();
    if (context) {
        std::cout << "ZMQ context created successfully!" << std::endl;
        zmq_ctx_term(context);
        return 0;
    } else {
        std::cout << "Failed to create ZMQ context!" << std::endl;
        return 1;
    }
}
