#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#include "tcp_socket.hpp"

TcpSocket::TcpSocket(int fd)
    : socketFd_(fd) {
}

TcpSocket::~TcpSocket() {
    if (socketFd_ >= 0) {
        close(socketFd_);
    }
}

bool TcpSocket::connectTo(const std::string& host, int port) {
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0); // Address Family - Internet (IPv4), SOCK_STREAM - потоковый сокет (TCP)

    if (socketFd_ < 0) {
        return false;
    }

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1)
        return false;

    return connect(socketFd_, (sockaddr*)&addr, sizeof(addr)) == 0;
}

bool TcpSocket::bindAndListen(int port) {
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd_ < 0) {
        return false;
    }

    int opt = 1;

    setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(socketFd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        return false;
    }

    return listen(socketFd_, 5) == 0;
}

TcpSocket TcpSocket::acceptClient() {
    int clientFd = accept(socketFd_, nullptr, nullptr);

    return TcpSocket(clientFd);
}

bool TcpSocket::sendLine(const std::string& line) {
    return send(socketFd_, line.c_str(), line.size(), 0) >= 0;
}

bool TcpSocket::receiveLine(std::string& line) {
    line.clear();

    while (true) {
        // ищем \n в уже полученных данных
        size_t pos = buffer_.find('\n');

        if (pos != std::string::npos) { // проверка на то, найден ли '\n'
            line = buffer_.substr(0, pos);
            buffer_.erase(0, pos + 1);
            return true;
        }

        // читаем новые данные
        char temp[4096];
        auto n = recv(socketFd_, temp, sizeof(temp), 0);

        if (n <= 0) {
            return false;
        }

        buffer_.append(temp, n);
    }
}

bool TcpSocket::isValid() const {
    return socketFd_ >= 0;
}

int TcpSocket::fd() const {
    return socketFd_;
}
