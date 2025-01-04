math.randomseed(os.time()) -- Initialiser la graine aléatoire

function player_ai(entity_id)
    while true do
        -- Générer un mouvement aléatoire
        local direction = math.random(0, 1) -- 0 pour monter, 1 pour descendre

        if direction == 0 then
            update_velocity(entity_id, 0, -5) -- Monter
        else
            update_velocity(entity_id, 0, 5) -- Descendre
        end

        -- façon moche d'attendre 500 ms avant de changer
        -- local sec = tonumber(os.clock() + a); 
        -- while (os.clock() < sec) do 
        -- end
    end
end
