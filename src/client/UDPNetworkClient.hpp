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

class UDPNetworkClient {
public:
    UDPNetworkClient(const std::string& server_ip, uint16_t server_port);
    ~UDPNetworkClient();

    void send(const std::string& message);
    // Receive and delete from queue the first message of the stack
    std::optional<std::string> receive();
    // Receive and delete from queue all messages of the stack
    std::vector<std::string> receive_all();
    void print_message_queue();
    void stop();

private:
    void start_receive();
    void process_send_queue();

    Utils utils;

    std::shared_ptr<boost::asio::io_context> m_io_context;
    std::thread m_io_thread;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_server_endpoint;
    boost::asio::ip::udp::endpoint m_remote_endpoint;

    std::queue<std::string> m_send_queue;
    std::mutex m_send_mutex;

    std::deque<std::string> m_received_messages;
    std::mutex m_receive_mutex;

    std::atomic<bool> m_is_running;
    std::function<void(const std::string&)> m_message_handler;
    std::array<char, 1024> m_receive_buffer;
};
