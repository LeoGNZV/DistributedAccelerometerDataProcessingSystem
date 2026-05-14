#include "accel_module.hpp"
#include "accel_packet.hpp"
#include "json_utils.hpp"
#include "logger.hpp"
#include "tcp_socket.hpp"
#include <cmath>

static constexpr const char* SERVER_IP = "127.0.0.1";

int main(int argc, char** argv) {
    int SERVER_PORT;

    if (argc < 2) {
        Logger::error("Not enough arguments");
        return 1;
    }

    if (argc > 2) {
        Logger::error("Too many arguments");
        return 1;
    }

    SERVER_PORT = std::stoi(argv[1]);
    TcpSocket socket;
    if (!socket.connectTo(SERVER_IP, SERVER_PORT)) {
        Logger::error("Failed to connect to server");
        return 1;
    }
    Logger::info("Connected to server");
    while (true) {
        std::string line;
        if (!socket.receiveLine(line)) {
            Logger::error("Server disconnected");
            break;
        }
        auto packet = deserializePacket(line);

        double moduleValue = std::sqrt(
        packet.x * packet.x +
        packet.y * packet.y +
        packet.z * packet.z);

        AccelModule module;
        module.timestamp = packet.timestamp;
        module.module = moduleValue;
        socket.sendLine(serializeModule(module));
        Logger::info("Module calculated and sent");
    }

    return 0;
}
