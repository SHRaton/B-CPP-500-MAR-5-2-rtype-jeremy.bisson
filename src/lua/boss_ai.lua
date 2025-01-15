function boss_ai(entity_id, position_x, position_y)
    -- Définir les limites de la zone de mouvement
    local min_x = 1200  -- Limite gauche
    local max_x = 1700 -- Limite droite
    local speed = -5

    -- Si le boss est dans la zone acceptable, on le stoppe
    if position_x <= max_x and position_x >= min_x then
        speed = 0
    -- Si le boss est trop à gauche, on le fait revenir vers la droite
    elseif position_x < min_x then
        speed = 5
    end

    -- Gestion du mouvement vertical
    local vertical_speed = 0
    if position_y > 900 then
        vertical_speed = -5  -- Déplace en haut
    elseif position_y < 200 then
        vertical_speed = 5   -- Déplace en bas
    end

    update_velocity(entity_id, speed, vertical_speed)
end