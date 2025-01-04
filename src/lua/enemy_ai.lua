function enemy_ai(entity_id, position_x, position_y)
    if position_x > 1500 then
        update_velocity(entity_id, -5, 0) -- Déplace à gauche
    elseif position_x < 100 then
        update_velocity(entity_id, 5, 0) -- Déplace à droite
    end
    if position_y > 1000 then
        update_velocity(entity_id, 0, -5) -- Déplace en haut
    elseif position_y < 100 then
        update_velocity(entity_id, 0, 5) -- Déplace en bas
    end
end
