function enemy_ai(entity_id, position_x, position_y)
    if position_y > 900 then
        update_velocity(entity_id, 0, -5) -- Déplace en haut
    elseif position_y < 200 then
        update_velocity(entity_id, 0, 5) -- Déplace en bas
    end
end
