#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

bool bfs(t_room_list *list_rooms, int **capacity, int *parent, bool *visited, int *queue)
{
    int count = list_rooms->count;
    int total_nodes = count * 2;

    for (int i = 0; i < total_nodes; i++) {
        visited[i] = false;
    }

    int head = 0;
    int tail = 0;

    int start_out = list_rooms->start_room->id + count;
    int end_in = list_rooms->end_room->id;
    bool found = false;

    visited[start_out] = true;
    queue[tail++] = start_out;
    parent[start_out] = -1;

    while(head < tail){
        int current = queue[head++];
        if(current == end_in){
            found = true;
            break;
        }

        for(int next = 0; next < total_nodes; next++){
            if(!visited[next] && capacity[current][next] > 0){
                visited[next] = true;
                parent[next] = current;
                queue[tail++] = next;
            }
        }
    }
    return found;
}

void extract_paths(t_room_list *list_rooms, t_path_list *paths, int **capacity)
{
    t_room *start = list_rooms->start_room;
    t_room *end = list_rooms->end_room;

    for (int i = 0; i < start->nb_connections; i++)
    {
        t_room *first = start->connections[i];

        if (capacity[start->id + list_rooms->count][first->id] == 0)
        {
            t_path *path = init_path();
            if (!path)
                return;

            add_room_in_path(path, start);
            t_room *current = first;

            while (current != end)
            {
                add_room_in_path(path, current);
                t_room *next = NULL;

                for (int j = 0; j < current->nb_connections; j++)
                {
                    t_room *neighbor = current->connections[j];
                    if (capacity[current->id + list_rooms->count][neighbor->id] == 0)
                    {
                        next = neighbor;
                        break;
                    }
                }
                if (next == NULL)
                    break;
                current = next;
            }

            if (current == end)
            {
                add_room_in_path(path, end);
                add_path_in_path_list(paths, path);
            }
        }
    }
}

t_path_list *find_all_path(t_room_list *list_rooms, int nb_start_ant)
{
    int count = list_rooms->count;
    int total_nodes = count * 2;

    int **capacity = malloc(total_nodes * sizeof(int *));
    if (!capacity)
    {
        free_capacity(capacity, total_nodes);
        return NULL;
    }

    for (int i = 0; i < total_nodes; i++) {
        capacity[i] = ft_calloc(total_nodes, sizeof(int));
        if (!capacity[i]) {
            free_capacity(capacity, i + 1);
            return NULL; 
        }
    }

    int *parent = malloc(total_nodes * sizeof(int));
    if (!parent){
        free_capacity(capacity, total_nodes);
        free(parent);
        return NULL;
    }

    bool *visited = malloc(total_nodes * sizeof(bool));
    int *queue = malloc(total_nodes * sizeof(int));
    if (!visited || !queue) {
        free(parent);
        if (visited)
            free(visited);
        if (queue)
            free(queue);
        free_capacity(capacity, total_nodes);
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        if (i == list_rooms->start_room->id || i == list_rooms->end_room->id)
            capacity[i][i + count] = 2147483647;
        else
            capacity[i][i + count] = 1;
    }

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < list_rooms->rooms[i]->nb_connections; j++) {
            int neighbor_id = list_rooms->rooms[i]->connections[j]->id;
        
            capacity[i + count][neighbor_id] = 1;
        }
    }

    t_path_list *best_paths = NULL;
    int min_lines = 1000000;

    while (bfs(list_rooms, capacity, parent, visited, queue)) {
        int current = list_rooms->end_room->id;

        while(parent[current] != -1)
        {
            int prev = parent[current];
            capacity[prev][current] -= 1;
            capacity[current][prev] += 1;
            current = prev;
        }

        t_path_list *current_paths = init_path_list();
        if(current_paths == NULL)
        {
            free(visited);
            free(queue);
            free(parent);
            free_capacity(capacity, total_nodes);
            free_path_list(current_paths);
            if (best_paths)
                free_path_list(best_paths);
            return NULL;
        }
        extract_paths(list_rooms, current_paths, capacity);

        ants_repartition(current_paths, nb_start_ant);
        int nb_lines = count_data_paths(current_paths, nb_start_ant, false);

        if (nb_lines < min_lines) {
            min_lines = nb_lines;

            if (best_paths != NULL)
            {
                free_path_list(best_paths);
            }
            best_paths = current_paths;
        }
        else {
            free_path_list(current_paths);
            // break;
        }
    }

    free(visited);
    free(queue);
    free(parent);
    free_capacity(capacity, total_nodes);
    
    return best_paths;
}

void ants_repartition(t_path_list *paths, int nb_start_ant)
{
    paths->max_ants = 0;
    for (int i = 0; i < nb_start_ant; i++){
        t_path *choosen_path = NULL;

        for (int j = 0; j < paths->count; j++){
            t_path *current_path = paths->paths[j];

            if (choosen_path == NULL || (current_path->count + current_path->ants) < (choosen_path->count + choosen_path->ants)){
                choosen_path = current_path;
            }
        }
        choosen_path->ants += 1;
        if (choosen_path->ants > paths->max_ants)
            paths->max_ants = choosen_path->ants;
    }
}

int count_data_paths(t_path_list *paths, int nb_start_ant, bool is_print)
{
    int ant_count = 1;
    int finished = 0;
    int count_line = 0;
    while (finished != nb_start_ant){
        for (int j = 0; j < paths->count; j++){
            t_path *current_path = paths->paths[j];
            for (int i = current_path->count - 1; i > 0; i--){
                t_room *current_room = current_path->rooms[i];
                t_room *prev_room = current_path->rooms[i - 1];
                if(prev_room->id == current_path->rooms[0]->id && ant_count <= nb_start_ant && current_path->ants > 0){
                    current_path->ants--;
                    current_room->ant = ant_count;
                    if(is_print)
                        ft_printf("L%d-%s ", current_room->ant, current_room->name);
                    ant_count++;
                }
                else if (prev_room->ant != -1) {
                    current_room->ant = prev_room->ant;
                    prev_room->ant = -1;
                    if(is_print)
                        ft_printf("L%d-%s ", current_room->ant, current_room->name);
                    if (current_room->id == current_path->rooms[current_path->count - 1]->id)
                        finished++;
                }
            }
        }
        count_line++;
        if(is_print)
            ft_printf("\n");
    }
    if(is_print)
        ft_printf("Nb line: %d\n", count_line);
    return count_line;
}