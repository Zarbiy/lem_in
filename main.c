#include "main.h"

int add_connection(t_room *room, t_room *other){
    t_room **new_connections = malloc(sizeof(t_room *) * (room->nb_connections + 1));
    if (!new_connections)
        return -1;

    for (int i = 0; i < room->nb_connections; i++)
        new_connections[i] = room->connections[i];

    new_connections[room->nb_connections] = other;

    free(room->connections);
    room->connections = new_connections;
    room->nb_connections++;

    return 0;
}

t_room *find_room(t_room_list *list_rooms, char *name){
    for (int i = 0; i < list_rooms->count; i++){
        if (ft_strcmp(list_rooms->rooms[i]->name, name) == 0)
            return list_rooms->rooms[i];
    }
    return NULL;
}

int add_link(t_room_list *list_rooms, char *line){
    char name1[10];
    char name2[10];
    char *dash;
    int len1;
    int len2;

    dash = ft_strchr(line, '-');
    if (!dash || dash == line)
        return -1;

    len1 = dash - line;
    if (len1 >= 10)
        return -1;
    ft_strlcpy(name1, line, len1 + 1);

    dash++; 
    
    len2 = 0;
    while (dash[len2] != '\0' && dash[len2] != '\n')
        len2++;
        
    if (len2 == 0 || len2 >= 10)
        return -1;
    ft_strlcpy(name2, dash, len2 + 1);

    t_room *ra = find_room(list_rooms, name1);
    t_room *rb = find_room(list_rooms, name2);
    if (!ra || !rb)
        return -1;

    if (add_connection(ra, rb) == -1)
        return -1;
    if (add_connection(rb, ra) == -1)
        return -1;

    return 0;
}

int extract_data(t_room_list *list_rooms){
    char *line;
    int nb_line = 0;
    bool type = false;

    bool s_room = false;
    bool e_room = false;

    line = get_next_line(0);
    while (line != NULL){
        ft_printf("%s",line);
        if(line[0] == '#'){
            if(s_room == false || e_room == false){
                if(ft_strncmp(line, "##start", 7) == 0)
                    s_room = true;
                else if(ft_strncmp(line, "##end", 5) == 0)
                    e_room = true;
            }
            free(line);
            line = get_next_line(0);
            // nb_line++;
            continue ;
        }
        else if(line[0] == '\n'){
            write(2, "Error: empty line found\n", 24);
            free(line);
            return -1;
        }

        type = is_link_line(line);

        if(type == false){
            t_room *room = init_room(line);
            if (room == NULL){
                write(2, "Error: init room\n", 17);
                free(line);
                return -1;
            }
            if(s_room == true && list_rooms->start_room == NULL)
                list_rooms->start_room = room;
            if(e_room == true && list_rooms->end_room == NULL)
                list_rooms->end_room = room;
            if (add_room(list_rooms, room) == -1){
                free(line);
                return -1;
            }
        }
        else if(type == true){
            if(add_link(list_rooms, line) == -1){
                write(2, "Error: add room link\n", 21);
                free(line);
                return -1;
            }
        }

        free(line);
        line = get_next_line(0);
        nb_line++;
    }

    if(list_rooms->start_room == NULL || list_rooms->end_room == NULL){
        write(2, "Error: Missing start and/or end\n", 32);
        free(line);
        return -1;
    }

    // ft_printf("start: %s, end: %s\n", list_rooms->start_room->name, list_rooms->end_room->name);

    free(line);
    return 0;
}

int main(int ac, char *av[]){
    (void) ac;
    (void) av;

    double start = get_time_sec();

    char *line;
    
    line = get_next_line(0);
    if(!line)
        return -1;
    ft_printf("%s", line);
    int nb_start_ant = ft_atoi(line);
    free(line);
    if(nb_start_ant <= 0){
        write(2, "Error: invalid number ant\n", 26);
        return -1;
    }
    // ft_printf("Nb ant: %d\n", nb_start_ant);
    
    t_room_list *list_rooms = init_room_list();
    if(list_rooms == NULL){
        write(2, "Error: Init room list\n", 22);
        return -1;
    }

    if(extract_data(list_rooms) == -1){
        write(2, "Error: Extract data\n", 20);
        free_list_room(list_rooms);
        return -1;
    }

    // ft_printf("Data list room: nb room:%d total dispo:%d\n", list_rooms->count, list_rooms->capacity);
    print_data_room(list_rooms, list_rooms->start_room->name);
    print_data_room(list_rooms, list_rooms->end_room->name);

    t_path_list *paths = find_all_path(list_rooms, nb_start_ant);
    if (paths == NULL || paths->count == 0){
        write(2, "Error: no path found\n", 22);
        free_list_room(list_rooms);
        return -1;
    }
    
    ants_repartition(paths, nb_start_ant);
    print_data_path(paths);
    count_data_paths(paths, nb_start_ant, true);
    
    double end = get_time_sec();
    print_execution_time(end - start);

    ants_repartition(paths, nb_start_ant);
    init_window(paths, list_rooms);

    free_path_list(paths);
    free_list_room(list_rooms);

    return 0;
}
