math.randomseed(os.time()) -- Initialiser la graine aléatoire

function player_ai(entity_id, x, y)
    -- Générer un comportement aléatoire
    local direction = math.random(0, 3) -- 0: monter, 1: descendre, 2: gauche, 3: droite
    local should_shoot = math.random(0, 1) -- 50% de chances de tirer
    local speed = 5

    -- Garder le joueur dans les limites de l'écran
    if y < 100 then
        direction = 1 -- Forcer la descente
    elseif y > 700 then
        direction = 0 -- Forcer la montée
    end

    if x < 250 then
        direction = 3 -- Forcer le déplacement à droite
    elseif x > 1150 then
        direction = 2 -- Forcer le déplacement à gauche
    end

    -- Effectuer le mouvement en fonction de la direction
    if direction == 0 then
        update_velocity(entity_id, 0, -speed) -- Monter
    elseif direction == 1 then
        update_velocity(entity_id, 0, speed) -- Descendre
    elseif direction == 2 then
        update_velocity(entity_id, -speed, 0) -- Aller à gauche
    elseif direction == 3 then
        update_velocity(entity_id, speed, 0) -- Aller à droite
    end

    -- Tirer une balle avec un comportement aléatoire
    if should_shoot == 1 then
        spawn_bullet(entity_id)
    end

    -- Ajouter une pause occasionnelle pour plus de réalisme
    if math.random(0, 10) == 0 then
        update_velocity(entity_id, 0, 0) -- Arrêter le mouvement temporairement
    end
end
