#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <array>
#include <iostream>

class UDPNetworkClient {
public:
    UDPNetworkClient(boost::asio::io_context& io_context, 
                     const std::string& server_ip, 
                     uint16_t server_port)
        : m_io_context(io_context),
          m_socket(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)),
          m_server_endpoint(boost::asio::ip::address::from_string(server_ip), server_port),
          m_is_running(true)
    {
        start_receive();
    }

    ~UDPNetworkClient() {
        stop();
    }

    void send_message(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_send_mutex);
        m_send_queue.push(message);
        
        // Trigger send if not already in progress
        if (m_send_queue.size() == 1) {
            async_send();
        }
    }

    std::vector<std::string> receive_messages() {
        std::vector<std::string> messages;
        std::lock_guard<std::mutex> lock(m_receive_mutex);
        messages.swap(m_received_messages);
        return messages;
    }

    void stop() {
        m_is_running = false;
        m_socket.close();
        m_io_context.stop();
    }

private:
    void start_receive() {
        m_receive_buffer.fill(0);
        m_socket.async_receive_from(
            boost::asio::buffer(m_receive_buffer), 
            m_remote_endpoint,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error && bytes_transferred > 0) {
                    // Process received message
                    std::string received_message(
                        m_receive_buffer.begin(), 
                        m_receive_buffer.begin() + bytes_transferred
                    );
                    
                    {
                        std::lock_guard<std::mutex> lock(m_receive_mutex);
                        m_received_messages.push_back(received_message);
                    }

                    // Continue receiving
                    start_receive();
                } else {
                    // Handle error or restart receive
                    if (m_is_running) {
                        start_receive();
                    }
                }
            }
        );
    }

    void async_send() {
        if (m_send_queue.empty()) return;

        std::string message = m_send_queue.front();
        m_socket.async_send_to(
            boost::asio::buffer(message),
            m_server_endpoint,
            [this](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                if (!error) {
                    std::lock_guard<std::mutex> lock(m_send_mutex);
                    m_send_queue.pop();
                    
                    // Continue sending if more messages
                    if (!m_send_queue.empty()) {
                        async_send();
                    }
                } else {
                    // Handle send error
                    std::cerr << "Send error: " << error.message() << std::endl;
                }
            }
        );
    }

    // Asio context and socket
    boost::asio::io_context& m_io_context;
    boost::asio::ip::udp::socket m_socket;
    
    // Server endpoint
    boost::asio::ip::udp::endpoint m_server_endpoint;
    boost::asio::ip::udp::endpoint m_remote_endpoint;

    // Send queue
    std::queue<std::string> m_send_queue;
    std::mutex m_send_mutex;

    // Receive buffer and messages
    std::array<char, 1024> m_receive_buffer;
    std::vector<std::string> m_received_messages;
    std::mutex m_receive_mutex;

    // Running state
    std::atomic<bool> m_is_running;
};