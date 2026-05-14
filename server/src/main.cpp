#include "accel_packet.hpp"
#include "accel_module.hpp"
#include "json_utils.hpp"
#include "logger.hpp"
#include "tcp_socket.hpp"
#include <cmath>
#include <mutex>
#include <optional>
#include <thread>

static std::mutex globalMutex;
static std::optional<AccelPacket> lastPacket;

bool isDuplicate(const AccelPacket& a, const AccelPacket& b) {
    auto round = [](double v) {
        return std::round(v * 1000.0) / 1000.0;
    };

    return round(a.x) == round(b.x)
    && round(a.y) == round(b.y)
    && round(a.z) == round(b.z);
} 
int main(int argc, char** argv) {
    if (argc < 3) {
        Logger::error("Usage: ./accel_server <port_a> <port_b>");
        return 1;
    }

    int portA = std::stoi(argv[1]);
    int portB = std::stoi(argv[2]);
    TcpSocket serverA;
    TcpSocket serverB;

    if (!serverA.bindAndListen(portA)) {
        Logger::error("Failed to start server A socket");
        return 1;
    }
    if (!serverB.bindAndListen(portB)) {
        Logger::error("Failed to start server B socket");
        return 1;
    }

    Logger::info("Waiting for Node A...");
    auto nodeA = serverA.acceptClient();
    Logger::info("Node A connected");
    Logger::info("Waiting for Node B...");
    auto nodeB = serverB.acceptClient();
    Logger::info("Node B connected");

    std::thread receiveFromA([&]() {
        while (true) {
            std::string line;
            if (!nodeA.receiveLine(line)) {
                Logger::error("Node A disconnected");
                break;
            }
            auto packet = deserializePacket(line);
            {
                std::lock_guard<std::mutex> lock(globalMutex);
                if (lastPacket && isDuplicate(packet, *lastPacket)) {
                    Logger::info("Duplicate packet dropped");
                    continue;
                }
                lastPacket = packet;
            }
            Logger::info("Forwarding packet to Node B");
            nodeB.sendLine(serializePacket(packet));
        }
    });

    std::thread receiveFromB([&]() {
        while (true) {
            std::string line;
            if (!nodeB.receiveLine(line)) {
                Logger::error("Node B disconnected");
                break;
            }

            auto module = deserializeModule(line);
            Logger::info("Forwarding module to Node A");
            nodeA.sendLine(serializeModule(module));
        }
    });

    receiveFromA.join();
    receiveFromB.join();
    return 0;
}
