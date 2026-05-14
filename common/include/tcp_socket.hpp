#pragma once

#include <string>

class TcpSocket {
public:
    explicit TcpSocket(int fd = -1);

    ~TcpSocket();

    bool connectTo(const std::string& host, int port);

    bool bindAndListen(int port);

    TcpSocket acceptClient();

    bool sendLine(const std::string& line);

    bool receiveLine(std::string& line);

    bool isValid() const;

    int fd() const;

private:
    int socketFd_;

    std::string buffer_;
};
