#include "main.h"

#define WIN_WIDTH 1200
#define WIN_HEIGT 900
#define ESPACE_NODE 60.0f
#define LAYOUT_ITERS 400
#define TAILLE_NODE 6.0f
#define ZOOM_LABELS 0.6f
#define ZOOM_MIN 0.05f
#define ZOOM_MAX 20.0f

typedef struct s_edge {
    int from;
    int to;
} t_edge;

typedef struct s_view {
    Vector2 *positions;
    Rectangle bounds;
    int *room_path;
    bool show_unused;
    bool dragging;
    Camera2D camera;
    t_anim anim;
} t_view;

Color COLORS[] = {
    {230, 25, 75, 255}, {60, 180, 75, 255}, {255, 200, 0, 255},
    {245, 130, 48, 255}, {145, 30, 180, 255}, {70, 240, 240, 255},
    {240, 50, 230, 255}, {128, 128, 0, 255}, {0, 128, 128, 255},
    {170, 110, 40, 255}, {128, 0, 0, 255}, {0, 0, 128, 255},
};

Color path_color(int index){
    return COLORS[index % 12];
}

int get_index_room(t_room_list *list, t_room *target){
    if(!list || !target || !list->rooms)
        return -1;
    for(int i = 0; i < list->count; i++){
        if(list->rooms[i] == target)
            return i;
    }
    return -1;
}

bool room_visible(t_room_list *list, int *room_path, int index, bool show_unused){
    if(show_unused || room_path[index] >= 0)
        return true;
    return list->rooms[index] == list->start_room || list->rooms[index] == list->end_room;
}

t_edge *build_edges(t_room_list *list, int *nb_edges){
    int max_edges = 0;

    for(int i = 0; i < list->count; i++){
        if(list->rooms[i])
            max_edges += list->rooms[i]->nb_connections;
    }
    t_edge *edges = malloc(sizeof(t_edge) * (max_edges + 1));
    if(!edges)
        return NULL;
    *nb_edges = 0;
    for(int i = 0; i < list->count; i++){
        t_room *room = list->rooms[i];
        if(!room)
            continue;
        for(int j = 0; j < room->nb_connections; j++){
            int other = get_index_room(list, room->connections[j]);
            if(other > i){
                edges[*nb_edges].from = i;
                edges[*nb_edges].to = other;
                (*nb_edges)++;
            }
        }
    }
    return edges;
}

void apply_repulsion(Vector2 *positions, Vector2 *force, int count){
    float dist = 3.0f * ESPACE_NODE;

    for(int i = 0; i < count; i++){
        for(int j = i + 1; j < count; j++){
            float diff_x = positions[i].x - positions[j].x;
            float diff_y = positions[i].y - positions[j].y;
            if(fabsf(diff_x) > dist || fabsf(diff_y) > dist)
                continue;
            float distance = sqrtf(diff_x * diff_x + diff_y * diff_y);
            if(distance > dist)
                continue;
            if(distance < 0.01f){
                diff_x = GetRandomValue(-100, 100) / 100.0f;
                diff_y = GetRandomValue(-100, 100) / 100.0f;
                distance = sqrtf(diff_x * diff_x + diff_y * diff_y) + 0.01f;
            }
            float push = (ESPACE_NODE * ESPACE_NODE) / distance;
            force[i].x += diff_x / distance * push;
            force[i].y += diff_y / distance * push;
            force[j].x -= diff_x / distance * push;
            force[j].y -= diff_y / distance * push;
        }
    }
}

void apply_attraction(Vector2 *positions, Vector2 *force, t_edge *edges, int nb_edges){
    for(int i = 0; i < nb_edges; i++){
        int from = edges[i].from;
        int to = edges[i].to;
        float diff_x = positions[from].x - positions[to].x;
        float diff_y = positions[from].y - positions[to].y;
        float distance = sqrtf(diff_x * diff_x + diff_y * diff_y);
        if(distance < 0.01f)
            continue;
        float pull = (distance * distance) / ESPACE_NODE;
        force[from].x -= diff_x / distance * pull;
        force[from].y -= diff_y / distance * pull;
        force[to].x += diff_x / distance * pull;
        force[to].y += diff_y / distance * pull;
    }
}

void move_nodes(Vector2 *positions, Vector2 *force, int count, float max_iter){
    for(int i = 0; i < count; i++){
        float max_step = sqrtf(force[i].x * force[i].x + force[i].y * force[i].y);
        if(max_step < 0.01f)
            continue;
        float step = max_step < max_iter ? max_step : max_iter;
        positions[i].x += force[i].x / max_step * step;
        positions[i].y += force[i].y / max_step * step;
    }
}

Rectangle compute_bounds(Vector2 *positions, int count){
    float min_x = positions[0].x;
    float max_x = positions[0].x;
    float min_y = positions[0].y;
    float max_y = positions[0].y;

    for(int i = 1; i < count; i++){
        if(positions[i].x < min_x)
            min_x = positions[i].x;
        if(positions[i].x > max_x)
            max_x = positions[i].x;
        if(positions[i].y < min_y)
            min_y = positions[i].y;
        if(positions[i].y > max_y)
            max_y = positions[i].y;
    }
    return (Rectangle){min_x, min_y, max_x - min_x, max_y - min_y};
}

int set_elements(t_room_list *list, Vector2 **positions, Rectangle *bounds){
    if(!list || !list->rooms || list->count <= 0 || !positions || !bounds)
        return -1;
    *positions = NULL;

    int count = list->count;
    int nb_edges = 0;
    Vector2 *pos = malloc(sizeof(Vector2) * count);
    Vector2 *force = malloc(sizeof(Vector2) * count);
    t_edge *edges = build_edges(list, &nb_edges);
    if(!pos || !force || !edges){
        free(pos);
        free(force);
        free(edges);
        return -1;
    }

    float side = sqrtf((float)count) * ESPACE_NODE;
    SetRandomSeed(42);
    for(int i = 0; i < count; i++){
        pos[i].x = GetRandomValue(0, 10000) / 10000.0f * side;
        pos[i].y = GetRandomValue(0, 10000) / 10000.0f * side;
    }

    float max_iter = side / 10.0f;
    for(int iter = 0; iter < LAYOUT_ITERS; iter++){
        for(int i = 0; i < count; i++)
            force[i] = (Vector2){0, 0};
        apply_repulsion(pos, force, count);
        apply_attraction(pos, force, edges, nb_edges);
        move_nodes(pos, force, count, max_iter);
        max_iter *= 0.9f;
    }

    *bounds = compute_bounds(pos, count);
    *positions = pos;
    free(force);
    free(edges);
    return 0;
}

Camera2D fit_camera(Rectangle bounds){
    float marge = 80.0f;
    float width = bounds.width + 2 * marge;
    float height = bounds.height + 2 * marge;
    Camera2D camera = {0};

    camera.offset = (Vector2){WIN_WIDTH / 2.0f, WIN_HEIGT / 2.0f};
    camera.target = (Vector2){bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f};
    camera.zoom = fminf(WIN_WIDTH / width, WIN_HEIGT / height);
    return camera;
}

void zoom_on_mouse(Camera2D *camera, float wheel){
    Vector2 mouse = GetMousePosition();
    Vector2 world_pos = GetScreenToWorld2D(mouse, *camera);

    camera->offset = mouse;
    camera->target = world_pos;
    camera->zoom *= (wheel > 0) ? 1.15f : 1.0f / 1.15f;
    camera->zoom = Clamp(camera->zoom, ZOOM_MIN, ZOOM_MAX);
}

void handle_input(t_view *view, Rectangle button){
    bool on_button = CheckCollisionPointRec(GetMousePosition(), button);
    float wheel = GetMouseWheelMove();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        view->dragging = !on_button;
        if(on_button)
            view->show_unused = !view->show_unused;
    }
    if(IsKeyPressed(KEY_H))
        view->show_unused = !view->show_unused;
    if(wheel != 0)
        zoom_on_mouse(&view->camera, wheel);
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && view->dragging){
        Vector2 delta = GetMouseDelta();
        view->camera.target.x -= delta.x / view->camera.zoom;
        view->camera.target.y -= delta.y / view->camera.zoom;
    }
    if(IsKeyPressed(KEY_R))
        view->camera = fit_camera(view->bounds);
}

int *map_rooms_to_paths(t_room_list *rooms, t_path_list *paths){
    int *room_path = malloc(sizeof(int) * rooms->count);
    if(!room_path)
        return NULL;
    for(int i = 0; i < rooms->count; i++)
        room_path[i] = -1;
    if(!paths || !paths->paths)
        return room_path;
    for(int p = 0; p < paths->count; p++){
        t_path *path = paths->paths[p];
        if(!path)
            continue;
        for(int r = 0; r < path->count; r++){
            int index = get_index_room(rooms, path->rooms[r]);
            if(index >= 0)
                room_path[index] = p;
        }
    }
    return room_path;
}

void draw_edges(t_room_list *rooms, t_view *view){
    for(int i = 0; i < rooms->count; i++){
        t_room *room = rooms->rooms[i];
        if(!room || !room_visible(rooms, view->room_path, i, view->show_unused))
            continue;
        for(int j = 0; j < room->nb_connections; j++){
            int other = get_index_room(rooms, room->connections[j]);
            if(other > i && room_visible(rooms, view->room_path, other, view->show_unused))
                DrawLineEx(view->positions[i], view->positions[other], 1.5f / view->camera.zoom, GRAY);
        }
    }
}

void draw_path(t_room_list *rooms, t_path *path, Color color, t_view *view){
    float thickness = 4.0f / view->camera.zoom;

    for(int r = 0; r + 1 < path->count; r++){
        int current = get_index_room(rooms, path->rooms[r]);
        int next = get_index_room(rooms, path->rooms[r + 1]);
        if(current >= 0 && next >= 0)
            DrawLineEx(view->positions[current], view->positions[next], thickness, color);
    }
}

void draw_paths(t_room_list *rooms, t_path_list *paths, t_view *view){
    if(!paths || !paths->paths)
        return;
    for(int p = 0; p < paths->count; p++){
        if(paths->paths[p] && paths->paths[p]->count > 0)
            draw_path(rooms, paths->paths[p], path_color(p), view);
    }
}

void draw_room(t_room_list *rooms, t_view *view, int index){
    t_room *room = rooms->rooms[index];
    Color color = BLUE;
    float radius = TAILLE_NODE;

    if(room == rooms->start_room){
        color = GREEN;
        radius *= 1.8f;
    }
    else if(room == rooms->end_room){
        color = RED;
        radius *= 1.8f;
    }
    else if(view->room_path[index] >= 0)
        color = path_color(view->room_path[index]);
    DrawCircleV(view->positions[index], radius / view->camera.zoom, color);
    if(room->has_ant){
        DrawCircleV(view->positions[index], radius * 0.55f / view->camera.zoom, ORANGE);
        DrawCircleLinesV(view->positions[index], radius / view->camera.zoom, BLACK);
    }
}

void draw_rooms(t_room_list *rooms, t_view *view){
    int start = get_index_room(rooms, rooms->start_room);
    int end = get_index_room(rooms, rooms->end_room);

    for(int i = 0; i < rooms->count; i++){
        if(!rooms->rooms[i] || i == start || i == end)
            continue;
        if(room_visible(rooms, view->room_path, i, view->show_unused))
            draw_room(rooms, view, i);
    }
    if(start >= 0)
        draw_room(rooms, view, start);
    if(end >= 0)
        draw_room(rooms, view, end);
}

void show_name_rooms(t_room_list *rooms, t_view *view){
    Vector2 mouse = GetMousePosition();

    for(int i = 0; i < rooms->count; i++){
        t_room *room = rooms->rooms[i];
        if(!room || !room->name || !room_visible(rooms, view->room_path, i, view->show_unused))
            continue;
        Vector2 screen_pos = GetWorldToScreen2D(view->positions[i], view->camera);
        if(screen_pos.x < -50 || screen_pos.x > WIN_WIDTH + 50 || screen_pos.y < -50 || screen_pos.y > WIN_HEIGT + 50)
            continue;
        float diff_x = screen_pos.x - mouse.x;
        float diff_y = screen_pos.y - mouse.y;
        bool hovered = (diff_x * diff_x + diff_y * diff_y) < 100.0f;
        bool important = (room == rooms->start_room || room == rooms->end_room);
        if(hovered || important || view->camera.zoom > ZOOM_LABELS)
            DrawText(room->name, (int)screen_pos.x + 10, (int)screen_pos.y - 7, 14, DARKGRAY);
    }
}

void draw_data_path(t_path_list *paths){
    if(!paths || !paths->paths)
        return;
    for(int p = 0; p < paths->count && p < 30; p++){
        t_path *path = paths->paths[p];
        if(!path)
            continue;
        int y = 40 + p * 20;
        DrawRectangle(10, y, 14, 14, path_color(p));
        DrawText(TextFormat("Chemin %d (%d salles, %d fourmis)", p + 1, path->count, path->ants), 30, y, 14, DARKGRAY);
    }
}

void draw_interface(t_path_list *paths, t_view *view, Rectangle button){
    bool on_button = CheckCollisionPointRec(GetMousePosition(), button);
    char *button_text = view->show_unused ? "Masquer rooms (H)" : "Afficher rooms (H)";

    DrawRectangle(0, 0, WIN_WIDTH, 30, Fade(RAYWHITE, 0.9f));
    DrawText("Depart", 10, 8, 16, GREEN);
    DrawText("Arrivee", 100, 8, 16, RED);
    DrawText("Room", 190, 8, 16, BLUE);
    DrawRectangleRec(button, on_button ? LIGHTGRAY : Fade(GRAY, 0.4f));
    DrawRectangleLinesEx(button, 1, DARKGRAY);
    DrawText(button_text, (int)button.x + 8, (int)button.y + 5, 14, BLACK);
    draw_data_path(paths);
}

int init_window(t_path_list *list_path, t_room_list *list_rooms){
    t_view view = {0};
    Rectangle button = {WIN_WIDTH - 170, 3, 160, 24};

    if(!list_rooms || !list_rooms->rooms || list_rooms->count <= 0)
        return -1;
    if(set_elements(list_rooms, &view.positions, &view.bounds) != 0)
        return -1;
    view.room_path = map_rooms_to_paths(list_rooms, list_path);
    if(!view.room_path){
        free(view.positions);
        return -1;
    }
    if(init_anmation(&view.anim, list_rooms, list_path) == -1){
        free(view.positions);
        free(view.room_path);
        return -1;
    }
    
    view.show_unused = true;
    InitWindow(WIN_WIDTH, WIN_HEIGT, "lem_in - Visualisation");
    SetTargetFPS(60);
    view.camera = fit_camera(view.bounds);

    while(!WindowShouldClose()){
        handle_input(&view, button);
        anim_update(&view.anim);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(view.camera);
        draw_edges(list_rooms, &view);
        draw_paths(list_rooms, list_path, &view);
        draw_rooms(list_rooms, &view);
        anim_draw(&view.anim, list_path, view.positions, view.camera.zoom);
        EndMode2D();
        show_name_rooms(list_rooms, &view);
        anim_draw_ids(&view.anim, list_path, view.positions, view.camera);
        anim_draw_hud(&view.anim, list_path);
        draw_interface(list_path, &view, button);
        EndDrawing();
    }

    CloseWindow();
    free(view.room_path);
    free(view.positions);
    anim_free(&view.anim);
    return 0;
}