#include "main.h"

#define ANT_RADIUS 3.0f
#define DEFAULT_SPEED 1.5f
#define SPEED_MIN 0.1f
#define SPEED_MAX 30.0f
#define ZOOM_IDS 1.5f

int build_route(t_anim *anim, int p, t_room_list *rooms, t_path *path){
    int *route = malloc(sizeof(int) * path->count);
    if(!route)
        return -1;

    for(int i = 0; i < path->count; i++)
        route[i] = get_index_room(rooms, path->rooms[i]);
    anim->routes[p] = route;
    anim->route_len[p] = path->count;
    return 0;
}

int init_anmation(t_anim *anim, t_room_list *rooms, t_path_list *paths){
    *anim = (t_anim){0};
    anim->speed = DEFAULT_SPEED;
    anim->playing = true;
    if(!paths || !paths->paths || paths->count <= 0)
        return 0;

    anim->routes = ft_calloc(paths->count, sizeof(int *));
    anim->route_len = ft_calloc(paths->count, sizeof(int));
    anim->first_ant = ft_calloc(paths->count, sizeof(int));
    if(!anim->routes || !anim->route_len || !anim->first_ant){
        anim_free(anim);
        return -1;
    }
    anim->nb_paths = paths->count;

    int next_ant = 1;
    for(int p = 0; p < paths->count; p++){
        t_path *path = paths->paths[p];
        anim->first_ant[p] = next_ant;
        if(!path)
            continue;
        if(build_route(anim, p, rooms, path) == -1){
            anim_free(anim);
            return -1;
        }
        next_ant += path->ants;
        if(path->ants > 0){
            int turns = (anim->route_len[p] - 1) + (path->ants - 1);
            if(turns > anim->total_turns)
                anim->total_turns = turns;
        }
    }
    return 0;
}

void anim_update(t_anim *anim){
    if(IsKeyPressed(KEY_SPACE))
        anim->playing = !anim->playing;
    if(IsKeyPressed(KEY_UP))
        anim->speed *= 1.5f;
    if(IsKeyPressed(KEY_DOWN))
        anim->speed /= 1.5f;
    anim->speed = Clamp(anim->speed, SPEED_MIN, SPEED_MAX);
    if(IsKeyPressed(KEY_ENTER)){
        anim->turn = 0;
        anim->playing = true;
    }
    if(IsKeyPressed(KEY_RIGHT)){
        anim->turn = floorf(anim->turn) + 1;
        anim->playing = false;
    }
    if(IsKeyPressed(KEY_LEFT)){
        anim->turn = ceilf(anim->turn) - 1;
        anim->playing = false;
    }
    if(anim->playing)
        anim->turn += GetFrameTime() * anim->speed;
    if(anim->turn >= anim->total_turns){
        anim->turn = (float)anim->total_turns;
        anim->playing = false;
    }
    if(anim->turn < 0)
        anim->turn = 0;
}

bool ant_position(t_anim *anim, int p, int k, Vector2 *positions, Vector2 *out){
    float progress = anim->turn - k;
    int len = anim->route_len[p];

    if(progress < 0 || progress >= len - 1)
        return false;
    int segment = (int)progress;
    float fraction = progress - segment;
    Vector2 from = positions[anim->routes[p][segment]];
    Vector2 to = positions[anim->routes[p][segment + 1]];
    *out = Vector2Lerp(from, to, fraction);
    return true;
}

void anim_draw(t_anim *anim, t_path_list *paths, Vector2 *positions, float zoom){
    Vector2 pos;

    for(int p = 0; p < anim->nb_paths; p++){
        if(!paths->paths[p] || !anim->routes[p])
            continue;
        for(int k = 0; k < paths->paths[p]->ants; k++){
            if(!ant_position(anim, p, k, positions, &pos))
                continue;
            DrawCircleV(pos, ANT_RADIUS / zoom, ORANGE);
        }
    }
}

void anim_draw_ids(t_anim *anim, t_path_list *paths, Vector2 *positions, Camera2D camera){
    Vector2 pos;

    if(camera.zoom < ZOOM_IDS)
        return;
    for(int p = 0; p < anim->nb_paths; p++){
        if(!paths->paths[p] || !anim->routes[p])
            continue;
        for(int k = 0; k < paths->paths[p]->ants; k++){
            if(!ant_position(anim, p, k, positions, &pos))
                continue;
            Vector2 screen = GetWorldToScreen2D(pos, camera);
            DrawText(TextFormat("%d", anim->first_ant[p] + k), (int)screen.x + 8, (int)screen.y - 18, 12, BLACK);
        }
    }
}

void anim_draw_hud(t_anim *anim, t_path_list *paths){
    int total = 0;
    int arrived = 0;
    int y = GetScreenHeight() - 50;

    for(int p = 0; p < anim->nb_paths; p++){
        if(!paths->paths[p] || !anim->routes[p])
            continue;
        total += paths->paths[p]->ants;
        for(int k = 0; k < paths->paths[p]->ants; k++){
            if(anim->turn - k >= anim->route_len[p] - 1)
                arrived++;
        }
    }
    DrawText(TextFormat("Tour %d / %d  |  %d / %d fourmis arrivees  |  %.1f tours/s  |  %s", (int)anim->turn, anim->total_turns, arrived, total, anim->speed, anim->playing ? "lecture" : "pause"), 10, y, 16, DARKGRAY);
    DrawText("Espace: pause | Haut/Bas: vitesse | Gauche/Droite: tour par tour | Entree: relancer", 10, y + 22, 14, GRAY);
}

void anim_free(t_anim *anim){
    for(int p = 0; p < anim->nb_paths; p++)
        free(anim->routes ? anim->routes[p] : NULL);
    free(anim->routes);
    free(anim->route_len);
    free(anim->first_ant);
    *anim = (t_anim){0};
}