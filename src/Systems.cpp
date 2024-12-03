#include "Systems.hpp"


void Systems::player_control_system(registry &r, int playerId, int x, int y)
{
    auto &velocities = r.get_components<component::velocity>();
    auto &vel = velocities[playerId];
    if (vel)
    {
        vel.value().vx = x;
        vel.value().vy = y;
        std::vector<std::string> params = {std::to_string(playerId), std::to_string(x), std::to_string(y)};
    }
}

void Systems::position_system(registry &r)
{
    auto &positions = r.get_components<component ::position>();
    auto const &velocities = r.get_components<component ::velocity>();
    for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i)
    {
        auto &pos = positions[i];
        auto &vel = velocities[i];
        if (pos && vel)
        {
            pos.value().x += vel.value().vx;
            pos.value().y += vel.value().vy;
        }
    }
}

void Systems::draw_system(registry &r, sf::RenderWindow &window)
{
    auto const &positions = r.get_components<component ::position>();
    auto const &drawables = r.get_components<component ::drawable>();
    for (size_t i = 0; i < positions.size() && i < drawables.size(); ++i)
    {
        auto const &pos = positions[i];
        auto const &drawable = drawables[i];
        if (pos && drawable)
        {
            sf::Sprite sprite(drawable.value().sprite);
            sprite.setPosition(pos.value().x, pos.value().y);
            window.draw(sprite);
        }
    }
}

void Systems::logging_system(registry &r)
{
    auto const &positions = r.get_components<component ::position>();
    auto const &velocities = r.get_components<component ::velocity>();
    for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i)
    {
        auto const &pos = positions[i];
        auto const &vel = velocities[i];
        if (pos && vel)
        {
            std ::cerr << i << ": Position = { " << pos.value().x << ", " << pos.value().y << " } , Velocity = { " << vel.value().vx << ", " << vel.value().vy << " }" << std ::endl;
        }
    }
}
