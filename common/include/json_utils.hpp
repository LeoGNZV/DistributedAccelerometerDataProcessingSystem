#pragma once

#include "accel_packet.hpp"
#include "accel_module.hpp"

#include <nlohmann/json.hpp>
#include <string>

std::string serializePacket(const AccelPacket& packet);
AccelPacket deserializePacket(const std::string& jsonStr);

std::string serializeModule(const AccelModule& module);
AccelModule deserializeModule(const std::string& jsonStr);
