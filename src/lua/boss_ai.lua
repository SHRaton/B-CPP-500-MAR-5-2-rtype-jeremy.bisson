function boss_ai(entity_id, position_x, position_y)
    local speed = -5
    if position_x < 1300 then
        speed = 0
    end
    if position_y > 900 then
        update_velocity(entity_id, speed, -5) -- Déplace en haut
    elseif position_y < 200 then
        update_velocity(entity_id, speed, 5) -- Déplace en bas
    else
        update_velocity(entity_id, speed, 0)
    end
end
