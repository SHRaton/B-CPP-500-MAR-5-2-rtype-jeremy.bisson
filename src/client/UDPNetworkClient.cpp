/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** UDPNetworkClient
*/

#include "UDPNetworkClient.hpp"


UDPNetworkClient::UDPNetworkClient(const std::string& server_ip, uint16_t server_port)
    : m_io_context(std::make_shared<boost::asio::io_context>()),
      m_socket(*m_io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)),
      m_server_endpoint(boost::asio::ip::address::from_string(server_ip), server_port),
      m_is_running(true)
{
    m_io_thread = std::thread([this]() {
        start_receive();
        m_io_context->run();
    });
}

UDPNetworkClient::~UDPNetworkClient()
{
    stop();
}

void UDPNetworkClient::send(const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_send_mutex);
    m_send_queue.push(message);
    m_io_context->post([this]() { process_send_queue(); });
    //utils.printTerminal(Color::BLUE, "SEND MESSAGE (\"" + message + "\")");
    //utils.printLog("SEND MESSAGE (\"" + message + "\")");
}

std::optional<std::string> UDPNetworkClient::receive() {
    std::lock_guard<std::mutex> lock(m_receive_mutex);
    if (m_received_messages.empty()) {
        return std::nullopt;
    }
    std::string message = m_received_messages.front();
    m_received_messages.pop_front();
    //utils.printTerminal(Color::YELLOW, "RECEIVED MESSAGE (\"" + message + "\")");
    //utils.printLog("RECEIVED MESSAGE (\"" + message + "\")");
    return message;
}

std::vector<std::string> UDPNetworkClient::receive_all() {
    std::lock_guard<std::mutex> lock(m_receive_mutex);
    std::vector<std::string> messages(m_received_messages.begin(), m_received_messages.end());
    m_received_messages.clear();
    /*for (const auto& msg : messages) {
        utils.printTerminal(Color::YELLOW, "RECEIVED MESSAGE (\"" + msg + "\")");
        utils.printLog("RECEIVED MESSAGE (\"" + msg + "\")");
    }*/
    return messages;
}

void UDPNetworkClient::print_message_queue() {
    std::lock_guard<std::mutex> lock(m_receive_mutex);
    std::vector<std::string> messages(m_received_messages.begin(), m_received_messages.end());
    int i = 0;
    for (const auto& msg : messages) {
        utils.printTerminal(Color::YELLOW, "Queue message " + std::to_string(i) + " (\"" + msg + "\")");
        i++;
    }
    return;
}

void UDPNetworkClient::stop() {
    m_is_running = false;
    m_io_context->stop();
    utils.printTerminal(Color::RED, "CONNECTION UDP CLOSED");
    //utils.printLog("CONNECTION UDP CLOSED");
    if (m_io_thread.joinable()) {
        m_io_thread.join();
    }
}

void UDPNetworkClient::start_receive()
{
    m_socket.async_receive_from(
        boost::asio::buffer(m_receive_buffer), m_remote_endpoint,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                std::string received_message(m_receive_buffer.begin(), m_receive_buffer.begin() + bytes_recvd);
                {
                    std::lock_guard<std::mutex> lock(m_receive_mutex);
                    m_received_messages.push_back(received_message);
                }
                if (m_message_handler) {
                    m_message_handler(received_message);
                }
            }
            if (m_is_running) {
                start_receive();
            }
        }
    );
}

void UDPNetworkClient::process_send_queue()
{
    if (m_send_queue.empty()) return;
    std::string message = m_send_queue.front();
    m_send_queue.pop();
    m_socket.async_send_to(
        boost::asio::buffer(message), m_server_endpoint,
        [this](boost::system::error_code ec, std::size_t /*bytes_sent*/) {
            if (!ec && !m_send_queue.empty()) {
                process_send_queue();
            }
        }
    );
}
