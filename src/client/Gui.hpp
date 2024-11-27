#pragma once

#include "../Entity.hpp"
#include "../Registry.hpp"
#include "../SparseArray.hpp"
#include "../Components.hpp"
#include "../Systems.hpp"
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Sprite.hpp"
#include "client.hpp"

class Core {
    public :
        Core() :
            window(sf::VideoMode(1920, 1080), "R-Type"),
            m_io_context(),
            m_network_client(nullptr)
        {
            // Start the Asio IO context in a separate thread
            m_io_thread = std::thread([this]() {
                boost::asio::executor_work_guard<boost::asio::io_context::executor_type> 
                    work_guard(m_io_context.get_executor());
                m_io_context.run();
            });
            loadAssets();
        };
        ~Core() {
            // Clean up network resources
            if (m_network_client) {
                m_network_client->stop();
            }

            // Stop IO context thread
            m_io_context.stop();
            if (m_io_thread.joinable()) {
                m_io_thread.join();
            }
        }

        void initialize_network(const std::string& server_ip, uint16_t port) {
            // Convert port to string
            std::string port_str = std::to_string(port);

            // Create network client
            m_network_client = std::make_shared<UDPNetworkClient>(
                m_io_context, 
                server_ip, 
                port
            );
        }
        void update_network() {
            // Check for received messages in your game loop

            if (m_network_client) {
                auto messages = m_network_client->receive_messages();
                for (const auto& msg : messages) {
                    // Process received network messages
                    handle_network_message(msg);
                    buffer = msg;
                }
            } else {
                buffer = "";
            }
        }
        void send_network_message(const std::string& message) {
            if (m_network_client) {
                m_network_client->send_message(message);
            }
        }


        //*************** Display ***************//
        void gui(int argc, char**argv);
        // Menu D'acceuil
        void gui_menu();
        // Menu de Login NAME IP PORT
        void gui_login();
        void handleKeyboard(sf::Event::KeyEvent key);
        void handleMouseClick(sf::Vector2i mousePosition);
        // Game
        void gui_game();

        //*************** Display ***************//

        // main.cpp
        std::vector<std::string> str_to_word_array(std::string s);
        void loadAssets();

        registry reg;
        std::map<std::string, Sprite> sprites_menu;
        std::vector<std::string> drawOrder_menu;

        std::map<std::string, Sprite> sprites_login;
        std::vector<std::string> drawOrder_login;

    private:
        void handle_network_message(const std::string& message) {
            std::cout << "Received network message: " << message << std::endl;
        }

        // Asio context and network client
        boost::asio::io_context m_io_context;
        std::thread m_io_thread;
        std::shared_ptr<UDPNetworkClient> m_network_client;

        // Window
        sf::RenderWindow window;

        // Strings
        std::string str_name;
        std::string str_ip;
        std::string str_port;
        std::string str_failed;

        std::string buffer;

        // Int
        int select_button;
        int failed_connection;

        // Textes
        sf::Font font;
        sf::Text text_name;
        sf::Text text_ip;
        sf::Text text_port;
        sf::Text text_failed;

};
