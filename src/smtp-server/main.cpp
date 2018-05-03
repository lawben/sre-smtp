#include <iostream>

#include <smtp-lib/rawsocket.hpp>


int main() {
    RawSocket socket;
    socket.bind(5555);
    socket.listen(5);

    int finished = 0;
    while (!finished) {
        auto subSocket = socket.accept();

        auto buffer = subSocket.read(1024);
        for (auto c : buffer)
        {
            std::cout << c;
        }
        std::cout << '\n';

        subSocket.write("OK");
        std::cout << "Message Complete\n";
    }
}