#include "ServerNetwork.hpp"

ServerNetwork::ServerNetwork(uint16_t port, Mediator& med)
    : med(med),
        socket_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      running_(false) {
    std::cout << Colors::GREEN << "[Console] : Démarrage du serveur sur le port " << port << Colors::RESET << std::endl;
    med.register_network(this);
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
        case GameAction::CROUCH: return "CROUCH";
        case GameAction::POWER_UP: return "POWER_UP";
        case GameAction::SHIELD: return "SHIELD";
        case GameAction::RESPAWN: return "RESPAWN";
        case GameAction::CONNECT: return "CONNECT";
        case GameAction::DISCONNECT: return "DISCONNECT";
        case GameAction::QUIT: return "QUIT";
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







void ServerNetwork::handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::cout << Colors::RED << "[Console] : Un joueur s'est déconnecté : "
              << client.address().to_string() << ":"
              << client.port() << Colors::RESET << std::endl;
    clients_.erase(client);

    //TODO: broadcast à tout les autres l'id du joueur qui s'est déconnecté
}

void ServerNetwork::handleConnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    clients_[client] = ClientInfo{"", true};


    std::cout << Colors::YELLOW << "[Console] : Un joueur s'est connecté : "
              << client.address().to_string()
              << ":" << client.port() << Colors::RESET << std::endl;

    for (int i = 0; i < params.size() - 1; i++) {
        socket_.send_to(boost::asio::buffer(encode_action(GameAction::CONNECT) + " " + params[i]), client);
    }
    broadcast_message(encode_action(GameAction::CONNECT) + " " + params[params.size() - 1]);
}

void ServerNetwork::handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    if (action == "UP") {
        std::string message = encode_action(GameAction::UP);
        broadcast_message(client, message);
    }
    if (action == "DOWN") {
        std::string message = encode_action(GameAction::DOWN);
        broadcast_message(client, message);
    }
    if (action == "LEFT") {
        std::string message = encode_action(GameAction::LEFT);
        broadcast_message(client, message);
    }
    if (action == "RIGHT") {
        std::string message = encode_action(GameAction::RIGHT);
        broadcast_message(client, message);
    }
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

    MediatorContext context;
    context.client = sender;
    med.notify(Sender::NETWORK, action_name, msg.arguments, context);
}









void ServerNetwork::broadcast_message(const boost::asio::ip::udp::endpoint& sender, const std::string& message) {
    if (clients_.size() > 1) {
        for (const auto& client : clients_) {
            if (client.first != sender) {
                socket_.send_to(boost::asio::buffer(message), client.first);
                std::cout << Colors::GREEN << "[ServerNetwork] Message forwarded to: [" 
                         << client.first.address().to_string() << ":" 
                         << client.first.port() << "]" << Colors::RESET << std::endl;
            }
        }
    }
}

void ServerNetwork::broadcast_message(const std::string& message) {
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