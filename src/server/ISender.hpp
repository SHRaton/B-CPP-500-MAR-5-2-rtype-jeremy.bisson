#pragma once
#include <vector>
#include <string>
#include <boost/asio.hpp>

typedef struct MediatorContext_s {
    boost::asio::ip::udp::endpoint client;
} MediatorContext;

class ISender {
public:
    virtual void handleConnect(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleShoot(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleMobSpawn(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleConciliation(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual ~ISender() = default;
};