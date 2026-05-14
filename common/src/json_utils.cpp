#include "json_utils.hpp"

using json = nlohmann::json;

std::string serializePacket(const AccelPacket& packet) {
    json j;

    j["version"] = packet.version;
    j["timestamp"] = packet.timestamp;
    j["x"] = packet.x;
    j["y"] = packet.y;
    j["z"] = packet.z;

    return j.dump() + "\n"; // преобразование в строку, добавление символа для разделения пакетов между собой
}

AccelPacket deserializePacket(const std::string& jsonStr) {
    auto j = json::parse(jsonStr);

    AccelPacket p;

    p.version = j.value("version", 1);
    p.timestamp = j["timestamp"];
    p.x = j["x"];
    p.y = j["y"];
    p.z = j["z"];

    return p;
}

std::string serializeModule(const AccelModule& module) {
    json j;

    j["version"] = module.version;
    j["timestamp"] = module.timestamp;
    j["module"] = module.module;

    return j.dump() + "\n";
}

AccelModule deserializeModule(const std::string& jsonStr) {
    auto j = json::parse(jsonStr);

    AccelModule m;

    m.version = j.value("version", 1);
    m.timestamp = j["timestamp"];
    m.module = j["module"];

    return m;
}
