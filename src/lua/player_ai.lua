math.randomseed(os.time()) -- Initialiser la graine aléatoire

function player_ai(entity_id, x , y)
    -- Générer un mouvement aléatoire
    local direction = math.random(0, 1) -- 0 pour monter, 1 pour descendre

    if y < 100 then
        direction = 1
    elseif y > 700 then
        direction = 0
    end

    if direction == 0 then
        update_velocity(entity_id, 0, -5) -- Monter
    else
        update_velocity(entity_id, 0, 5) -- Descendre
    end
    spawn_bullet(entity_id)
end
