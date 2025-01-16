#include "ServerNetwork.hpp"

ServerNetwork::ServerNetwork(uint16_t port, Mediator& med)
    : med(med),
        socket_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      running_(false) {
    std::cout << Colors::GREEN << "[Console] : Démarrage du serveur sur le port " << port << Colors::RESET << std::endl;
    med.register_network(this);
    launch_time_ = std::chrono::steady_clock::now();
}



std::string ServerNetwork::encode_action(GameAction action) {
    std::bitset<5> bits(static_cast<unsigned long>(action));
    return bits.to_string();
}

GameMessage ServerNetwork::decode_message(const std::string& message) {
    GameMessage msg;
    std::istringstream iss(message);
    std::string binary_code;
    iss >> binary_code;

    try {
        std::bitset<5> bits(binary_code);
        msg.action = static_cast<GameAction>(bits.to_ulong());
    } catch (...) {
        msg.action = GameAction::NONE;
    }

    std::string arg;
    while (iss >> arg) {
        msg.arguments.push_back(arg);
    }
    return msg;
}

std::string ServerNetwork::get_action_name(GameAction action) {
    switch (action) {
        case GameAction::SHOOT: return "SHOOT";
        case GameAction::MOVE: return "MOVE";
        case GameAction::DOWN: return "DOWN";
        case GameAction::LEFT: return "LEFT";
        case GameAction::RIGHT: return "RIGHT";
        case GameAction::UP: return "UP";
        case GameAction::STOP_X: return "STOP_X";
        case GameAction::STOP_Y: return "STOP_Y";
        case GameAction::POWER_UP: return "POWER_UP";
        case GameAction::ENTITY_SPAWN: return "ENTITY_SPAWN";
        case GameAction::MOB_SHOOT: return "MOB_SHOOT";
        case GameAction::RESPAWN: return "RESPAWN";
        case GameAction::CONNECT: return "CONNECT";
        case GameAction::DISCONNECT: return "DISCONNECT";
        case GameAction::GET_LEVELS: return "GET_LEVELS";
        case GameAction::QUIT: return "QUIT";
        case GameAction::START: return "START";
        case GameAction::LEVEL_EDITOR: return "LEVEL_EDITOR";
        case GameAction::SUPER_SHOOT: return "SUPER_SHOOT";
        case GameAction::SAVE_REPLAY: return "SAVE_REPLAY";
        case GameAction::PLAY_REPLAY: return "PLAY_REPLAY";
        case GameAction::SCORE_UPDATE: return "SCORE_UPDATE";
        case GameAction::HIGHSCORE: return "HIGHSCORE";
        default: return "UNKNOWN";
    }
}

void ServerNetwork::receive_messages() {
    while (running_) {
        std::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;

        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint);
        std::string message(buffer.data(), len);
        GameMessage game_msg = decode_message(message);
        handle_game_message(sender_endpoint, game_msg);
    }
}




void ServerNetwork::saveCommands(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier pour la sauvegarde.\n";
        return;
    }

    for (const auto& cmd : commands_) {
        file << cmd.message << "\n" << cmd.getRelativeTimeMillis() << "\n";
    }
    file.close();
}

// Charger les commandes depuis un fichier
std::vector<Command> ServerNetwork::loadCommands(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier pour le chargement.\n";
        return {};
    }

    std::vector<Command> commands;
    std::string message;
    long long relativeMillis;

    while (std::getline(file, message) && file >> relativeMillis) {
        file.ignore(); // Ignore le saut de ligne après la durée
        Command cmd;
        cmd.message = message;
        cmd.setRelativeTimeFromMillis(relativeMillis);
        commands.push_back(cmd);
    }
    file.close();

    return commands;
}










void ServerNetwork::handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::cout << Colors::RED << "[Console] : Un joueur s'est déconnecté : "
              << client.address().to_string() << ":"
              << client.port() << Colors::RESET << std::endl;

    if (params.empty()) {
        std::cout << Colors::RED << "[Erreur] : Pas d'ID de joueur fourni pour la déconnexion" 
                  << Colors::RESET << std::endl;
        return;
    }
    std::string player_id = params[0];
    std::string disconnect_message = encode_action(GameAction::DISCONNECT) + " " + player_id;
    broadcast_message(client, disconnect_message);
    clients_.erase(client);

    //TODO: broadcast à tout les autres l'id du joueur qui s'est déconnecté
}

void ServerNetwork::handleConnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (context.client.address().is_unspecified()) {
        broadcast_message(encode_action(GameAction::CONNECT) + " " + params[params.size() - 1]);
        return;
    }
    boost::asio::ip::udp::endpoint client = context.client;
    clients_[client] = ClientInfo{"", true};


    std::cout << Colors::YELLOW << "[Console] : Un joueur s'est connecté : "
              << client.address().to_string()
              << ":" << client.port() << Colors::RESET << std::endl;

    for (int i = 0; i < params.size() - 1; i++) {
        socket_.send_to(boost::asio::buffer(encode_action(GameAction::CONNECT) + " " + params[i]), client);
    }
    socket_.send_to(boost::asio::buffer("OK " + params[params.size() - 1]), client); //TODO: Remplacer OK par du binaire
    broadcast_message(client, encode_action(GameAction::CONNECT) + " " + params[params.size() - 1]);
}

void ServerNetwork::handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    if (action == "UP") {
        std::string message = encode_action(GameAction::UP) + " " + params[0];
        broadcast_message(client, message);
    }
    if (action == "DOWN") {
        std::string message = encode_action(GameAction::DOWN) + " " + params[0];
        broadcast_message(client, message);
    }
    if (action == "LEFT") {
        std::string message = encode_action(GameAction::LEFT) + " " + params[0];
        broadcast_message(client, message);
    }
    if (action == "RIGHT") {
        std::string message = encode_action(GameAction::RIGHT) + " " + params[0];
        broadcast_message(client, message);
    }
    if (action == "STOP_Y") {
        std::string message = encode_action(GameAction::STOP_Y) + " " + params[0];
        broadcast_message(client, message);
    }
    if (action == "STOP_X") {
        std::string message = encode_action(GameAction::STOP_X) + " " + params[0];
        broadcast_message(client, message);
    }
}



void ServerNetwork::handleShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::SHOOT) + " " + params[0] + " " + params[1];
    broadcast_message(message);
}

void ServerNetwork::handleMobSpawn(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::MOB_SPAWN) + " " + params[0] + " " + params[1] + " " + params[2];
    broadcast_message(client, message);
}

void ServerNetwork::handleConciliation(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::MOVE) + " " + params[0] + " " + params[1] + " " + params[2];
    broadcast_message(client, message);
}

void ServerNetwork::handleColision(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::COLLISION) + " " + params[0] + " " + params[1];
    broadcast_message(client, message);
}

void ServerNetwork::handleDeath(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::DEATH) + " " + params[0];
    broadcast_message(client, message);
}

void ServerNetwork::handlePowerUpSpawn(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::ENTITY_SPAWN) + " " + params[0] + " " + params[1] + " " + params[2];
    broadcast_message(message);
}

void ServerNetwork::handleMobShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::MOB_SHOOT) + " " + params[0] + " " + params[1];
    broadcast_message(message);
}

void ServerNetwork::handleWin(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::WIN);
    broadcast_message(message);
}

void ServerNetwork::handleStart(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::START);
    broadcast_message(message);
}

void ServerNetwork::handleLoose(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::LOOSE);
    broadcast_message(message);
}

void ServerNetwork::handleLaserShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::LASER_SHOOT) + " " + params[0] + " " + params[1];
    broadcast_message(message);
}
void ServerNetwork::handleScoreUpdate(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::SCORE_UPDATE) + " " + params[0] + " " + params[1];
    broadcast_message(message);
}

void ServerNetwork::handleGetLevels(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::GET_LEVELS);
    for (int i = 0; i < params.size(); i++) {
        message += " " + params[i];
    }
    broadcast_message(message);
}

void ServerNetwork::handleLevelEditor(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::LEVEL_EDITOR);
    for (int i = 0; i < params.size(); i++) {
        message += " " + params[i];
    }
    broadcast_message(message);
}

void ServerNetwork::handleHighScore(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::HIGHSCORE) + " " + params[0];
    broadcast_message(message);
}

void ServerNetwork::handleSaveReplay()
{
    saveCommands("../src/client/logs/command.txt");
}


void ServerNetwork::handlePlayReplay()
{
    commands_ = loadCommands("../src/client/logs/command.txt");
    for (const auto& cmd : commands_) {
        broadcast_message(cmd.message);
        std::this_thread::sleep_for(cmd.relative_time);
    }
}

void ServerNetwork::handleSuperShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::string message = encode_action(GameAction::SUPER_SHOOT) + " " + params[0] + " " + params[1];
    broadcast_message(message);
}


void ServerNetwork::handle_game_message(const boost::asio::ip::udp::endpoint& sender, const GameMessage& msg)
{
    std::string action_name = get_action_name(msg.action);

    //DEBUG
    std::ostringstream log_message;
    log_message << Colors::BLUE << "[Action] Player [" << sender.address().to_string()
                << ":" << sender.port() << "] " << std::endl;
    log_message << "Binary: " << encode_action(msg.action) << " (" << action_name << ")" << std::endl;
    log_message << "Arguments:";
    for (const auto& arg : msg.arguments) {
        log_message << " " << arg;
    }
    log_message << Colors::RESET;
    std::cout << log_message.str() << std::endl;
    //END DEBUG

    if (action_name == "SAVE_REPLAY") {
        handleSaveReplay();
    } else if (action_name == "PLAY_REPLAY") {
        handlePlayReplay();
    }

    MediatorContext context;
    context.client = sender;
    med.notify(Sender::NETWORK, action_name, msg.arguments, context);
}









void ServerNetwork::broadcast_message(const boost::asio::ip::udp::endpoint& sender, const std::string& message)
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    commands_.push_back(Command{message, now - launch_time_});
    launch_time_ = now;
    for (const auto& client : clients_) {
        if (client.first != sender) {
            socket_.send_to(boost::asio::buffer(message), client.first);
            std::cout << Colors::GREEN << "[ServerNetwork] Message forwarded to: [" 
                     << client.first.address().to_string() << ":" 
                     << client.first.port() << "]" << Colors::RESET << std::endl;
        }
    }
}

void ServerNetwork::broadcast_message(const std::string& message) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    commands_.push_back(Command{message, now - launch_time_});
    launch_time_ = now;
    for (const auto& client : clients_) {
        socket_.send_to(boost::asio::buffer(message), client.first);
    }
}









void ServerNetwork::start() {
    running_ = true;
    receive_thread_ = std::thread(&ServerNetwork::receive_messages, this);

    while(running_) {
        io_context_.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}