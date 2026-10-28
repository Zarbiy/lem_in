#include "main.h"

t_room_list *init_room_list(){
    t_room_list *list = malloc(sizeof(t_room_list));
    if (!list)
        return NULL;

    list->capacity = 32;
    list->rooms = malloc(sizeof(t_room *) * list->capacity);
    if (!list->rooms){
        free(list);
        return NULL;
    }
    list->count = 0;
    list->start_room = NULL;
    list->end_room = NULL;
    return list;
}

int add_room(t_room_list *list_rooms, t_room *room){
    if(list_rooms->count >= list_rooms->capacity){
        int new_capacity = list_rooms->capacity * 2;
        t_room **new_rooms = malloc(sizeof(t_room *) * new_capacity);
        if(!new_rooms)
            return -1;

        for(int i = 0; i < list_rooms->count; i++)
            new_rooms[i] = list_rooms->rooms[i];

        free(list_rooms->rooms);
        list_rooms->rooms = new_rooms;
        list_rooms->capacity = new_capacity;
    }
    room->id = list_rooms->count;
    list_rooms->rooms[list_rooms->count++] = room;
    return 0;
}

t_room *init_room(char *line)
{
    t_room *room;
    char **split;
    int count = 0;

    split = ft_split(line, ' ');
    if (!split)
        return NULL;

    while (split[count] != NULL)
        count++;
    if (count != 3)
    {
        free_split(split);
        return NULL;
    }

    room = malloc(sizeof(t_room));
    if (!room)
    {
        free_split(split);
        return NULL;
    }

    room->name = ft_strdup(split[0]);
    if (!room->name)
    {
        free(room);
        free_split(split);
        return NULL;
    }

    room->x = ft_atoi(split[1]);
    room->y = ft_atoi(split[2]);
    
    room->nb_connections = 0;
    room->connections = NULL;
    room->has_ant = false;
    room->ant = -1;

    free_split(split);
    return room;
}

void print_data_room(t_room_list *list_rooms, char *room_name){
    for (int i = 0; i < list_rooms->count; i++){
        if(ft_strcmp(list_rooms->rooms[i]->name, room_name) == 0){
            // ft_printf("Data room %s(%d), nb connections: %d\n", list_rooms->rooms[i]->name, list_rooms->rooms[i]->id, list_rooms->rooms[i]->nb_connections);
        }
    }
}

void print_data_path(t_path_list *path_list){
    for(int i = 0; i < path_list->count; i++){
        t_path *path = path_list->paths[i];

        if(!path)
            continue;

        ft_printf("Path %d/%d: ", i + 1, path->count);
        ft_printf("nb ants: %d: ", path->ants);

        for(int j = 0; j < path->count; j++){
            if(!path->rooms[j])
                continue;

            ft_printf("%s", path->rooms[j]->name);

            if(j < path->count - 1)
                ft_printf(" -> ");
        }
        ft_printf("\n");
    }
}

void free_room(t_room *room){
    free(room->name);
    free(room->connections);
    free(room);
}

void free_list_room(t_room_list *list_rooms){
    for (int i = 0; i < list_rooms->count; i++)
        free_room(list_rooms->rooms[i]);
    free(list_rooms->rooms);
    free(list_rooms);
}

t_path *init_path(){
    t_path *list = malloc(sizeof(t_path));
    if(!list)
        return NULL;
    list->capacity = 8;
    list->rooms = malloc(sizeof(t_room *) * list->capacity);
    if(!list->rooms){
        free(list);
        return NULL;
    }
    list->count = 0;
    list->ants = 0;
    return list;
}

int add_room_in_path(t_path *path, t_room *room){
    if(path->count >= path->capacity){
        int new_capacity = path->capacity * 2;
        t_room **new_rooms = malloc(sizeof(t_room *) * new_capacity);
        if(!new_rooms){
            free_room(room);
            return -1;
        }

        for(int i = 0; i < path->count; i++)
            new_rooms[i] = path->rooms[i];

        free(path->rooms);
        path->rooms = new_rooms;
        path->capacity = new_capacity;
    }
    path->rooms[path->count++] = room;
    return 0;
}

t_path_list *init_path_list(){
    t_path_list *list = malloc(sizeof(t_path_list));
    if(!list)
        return NULL;
    list->capacity = 8;
    list->paths = malloc(sizeof(t_path *) * list->capacity);
    if(!list->paths){
        free(list);
        return NULL;
    }
    list->count = 0;
    list->max_ants = 0;
    return list;
}

int add_path_in_path_list(t_path_list *list_path, t_path *path){
    if (list_path->count >= list_path->capacity){
        int new_capacity = list_path->capacity * 2;
        t_path **new_paths = malloc(sizeof(t_path *) * new_capacity);
        if (!new_paths)
            return -1;

        for (int i = 0; i < list_path->count; i++)
            new_paths[i] = list_path->paths[i];
        
        free(list_path->paths);
        list_path->paths = new_paths;
        list_path->capacity = new_capacity;
    }
    list_path->paths[list_path->count++] = path;
    return 0;
}

void free_path(t_path *path){
    free(path->rooms);
    free(path);
}

void free_path_list(t_path_list *path_list){
    for (int i = 0; i < path_list->count; i++)
        free_path(path_list->paths[i]);
    free(path_list->paths);
    free(path_list);
}
   
void free_capacity(int **capacity, int count){
    for(int i = 0; i < count; i++)
        free(capacity[i]);
    free(capacity);
}

bool is_link_line(char *line){
    return (ft_strchr(line, '-') != NULL);
}

void print_execution_time(double time){
	long	seconds;
	long	decimal;

	seconds = (long)time;
	decimal = (long)((time - seconds) * 1000000);
	if (decimal < 0)
		decimal = -decimal;
	write(1, "Temps d'execution : ", 20);
	ft_printf("%d", seconds);
	write(1, ".", 1);
	ft_printf("%d", decimal);
	write(1, " secondes\n", 10);
}

double get_time_sec(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}