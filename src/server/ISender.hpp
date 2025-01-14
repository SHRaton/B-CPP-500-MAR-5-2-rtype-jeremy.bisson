#pragma once
#include <vector>
#include <string>
#include <boost/asio.hpp>

/**
 * @brief Interface pour la gestion des communications réseau
 * @class ISender
 * 
 * @details Cette interface définit le contrat pour la gestion des différents événements
 * du jeu, notamment les connexions, les mouvements, et les actions des joueurs.
 * 
 * @startuml{ISender_class.png}
 * interface ISender {
 * + handleConnect(context: MediatorContext, params: vector<string>)
 * + handleDisconnect(context: MediatorContext, params: vector<string>)
 * + handleMoves(action: string, context: MediatorContext, params: vector<string>)
 * + handleShoot(context: MediatorContext, params: vector<string>)
 * + handleMobSpawn(context: MediatorContext, params: vector<string>)
 * + {and other handle methods...}
 * }
 * 
 * class MediatorContext {
 * + client: boost::asio::ip::udp::endpoint
 * }
 * 
 * ServerNetwork --|> ISender
 * ServerGame --|> ISender
 * @enduml
 */

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
    virtual void handlePowerUpSpawn(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleColision(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleDeath(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleMobShoot(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleWin(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleLoose(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleStart(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleLaserShoot(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleSuperShoot(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleScoreUpdate(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleLevelEditor(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleHighScore(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual void handleBossSpawn(const MediatorContext& context, const std::vector<std::string>& params) = 0;
    virtual ~ISender() = default;
};