#include "accel_module.hpp"
#include "accel_packet.hpp"
#include "json_utils.hpp"
#include "logger.hpp"
#include "tcp_socket.hpp"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <thread>

static constexpr const char* SERVER_IP = "127.0.0.1";

// Получение текущего timestamp в миллисекундах
int64_t currentTimestamp() {
    using namespace std::chrono;

    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

// Эмуляция данных акселерометра
AccelPacket generatePacket(double t) {
    AccelPacket p;

    p.timestamp = currentTimestamp();

    // Плавные синусоидальные сигналы
    p.x = std::sin(t);

    // Имитация гравитации
    p.y = std::cos(t) * 9.8;

    // Немного другая частота
    p.z = std::sin(t * 0.5);

    return p;
}

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

    // Создаем директорию для логов
    std::filesystem::create_directories("accel");

    // Открываем файл лога
    std::ofstream logFile(
        "accel/module.log",
        std::ios::app // новые данные записываются в конец файла
    );

    if (!logFile.is_open()) {
        Logger::error("Failed to open log file");
        return 1;
    }

    // Создаем TCP socket
    TcpSocket socket;

    // Подключаемся к серверу
    if (!socket.connectTo(SERVER_IP, SERVER_PORT)) { // возвращает признак заверешния t/f
        Logger::error("Connection to server failed");
        return 1;
    }

    Logger::info("Connected to server");

    // Поток отправки данных
    std::thread sender([&]() {

        double t = 0.0;

        while (true) {

            // Генерация accel packet
            auto packet = generatePacket(t);

            // Struct -> JSON
            auto json = serializePacket(packet);

            // Отправка серверу
            if (!socket.sendLine(json)) {
                Logger::error("Failed to send packet");
                break;
            }

            Logger::info(
                "Packet sent: x=" +
                std::to_string(packet.x) +
                " y=" +
                std::to_string(packet.y) +
                " z=" +
                std::to_string(packet.z)
            );

            // Сдвигаем фазу сигнала
            t += 0.05;

            // 50 Гц = 20 ms
            // частота: 50 пакетов в секунду
            std::this_thread::sleep_for(
                std::chrono::milliseconds(20)
            );
        }
    });

    // Поток получения результатов
    std::thread receiver([&]() {

        while (true) {

            std::string line;

            // Чтение строки из сокета
            if (!socket.receiveLine(line)) {
                Logger::error("Disconnected from server");
                break;
            }

            try {

                // JSON -> struct
                auto module = deserializeModule(line);

                // Запись в лог
                logFile << module.timestamp << " " << module.module << std::endl;

                Logger::info(
                    "Module received: " +
                    std::to_string(module.module)
                );

            } catch (const std::exception& e) {

                Logger::error(
                    std::string("JSON parse error: ") + e.what()
                );
            }
        }
    });

    // Ожидание потоков
    sender.join();
    receiver.join();

    return 0;
}
