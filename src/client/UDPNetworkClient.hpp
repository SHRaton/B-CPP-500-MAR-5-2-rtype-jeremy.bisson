#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <functional>
#include <optional>
#include "Utils.hpp"

/**
 * @brief Client réseau UDP pour la communication avec le serveur
 * @class UDPNetworkClient
 * 
 * @details Cette classe gère la communication réseau en UDP avec le serveur de jeu,
 * incluant l'envoi et la réception de messages, la gestion de la connexion et
 * la synchronisation des données.
 * 
 * @startuml{UDPNetworkClient_class.png}
 * class UDPNetworkClient {
 * - m_io_context: shared_ptr<io_context>
 * - m_socket: udp::socket
 * - m_server_endpoint: udp::endpoint
 * - m_received_messages: deque<string>
 * + UDPNetworkClient(server_ip: string, server_port: uint16_t)
 * + send(message: string)
 * + receive(): optional<string>
 * + receive_all(): vector<string>
 * + stop()
 * }
 * 
 * class boost::asio::io_context
 * class boost::asio::ip::udp::socket
 * class boost::asio::ip::udp::endpoint
 * 
 * UDPNetworkClient *-- "1" boost::asio::io_context
 * UDPNetworkClient *-- "1" boost::asio::ip::udp::socket
 * UDPNetworkClient *-- "2" boost::asio::ip::udp::endpoint
 * @enduml
 */

class UDPNetworkClient {
public:
    /**
     * @brief Constructeur initialisant la connexion au serveur
     * @param server_ip Adresse IP du serveur
     * @param server_port Port du serveur
     */
    UDPNetworkClient(const std::string& server_ip, uint16_t server_port);
    ~UDPNetworkClient();

    void check_connection();
    void send(const std::string& message);
    // Receive and delete from queue the first message of the stack
    std::optional<std::string> receive();
    // Receive and delete from queue all messages of the stack
    std::vector<std::string> receive_all();
    void print_message_queue();
    void stop();
    void setId(int id) {
        _id = id;
    };
    int getId() {
        return(_id);
    };

private:
    void start_receive();
    void process_send_queue();

    Utils utils;

    std::shared_ptr<boost::asio::io_context> m_io_context;
    std::thread m_io_thread;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_server_endpoint;
    boost::asio::ip::udp::endpoint m_remote_endpoint;

    std::function<void()> m_disconnect_handler;

    std::queue<std::string> m_send_queue;
    std::mutex m_send_mutex;

    std::deque<std::string> m_received_messages;
    std::mutex m_receive_mutex;

    std::atomic<bool> m_is_running;
    std::function<void(const std::string&)> m_message_handler;
    std::array<char, 1024> m_receive_buffer;

    int _id;
};
