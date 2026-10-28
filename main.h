#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "printf/ft_printf.h"

#define BUFFER_SIZE 1024

typedef struct s_room {
    char *name;
    int id;
    int x;
    int y;
    struct s_room **connections;
    int nb_connections;
    bool has_ant;
    int ant;
} t_room;

typedef struct s_room_list {
    t_room **rooms;
    int count;
    int capacity;
    t_room *start_room;
    t_room *end_room;
} t_room_list;

typedef struct s_path {
    t_room **rooms;
    int count;
    int capacity;
    int ants;
} t_path;

typedef struct s_path_list {
    t_path **paths;
    int count;
    int capacity;
    int max_ants;
} t_path_list;

// utils_rooms
t_room *init_room(char *line);
void free_room(t_room *room);

t_room_list *init_room_list();
int add_room(t_room_list *list_rooms, t_room *room);
void print_data_room(t_room_list *list_rooms, char *room_name);
void free_list_room(t_room_list *list_rooms);

t_path *init_path();
int add_room_in_path(t_path *path, t_room *room);
void free_path(t_path *path);

t_path_list *init_path_list();
int add_path_in_path_list(t_path_list *list_path, t_path *path);
void print_data_path(t_path_list *path_list);
void free_path_list(t_path_list *path_list);

void free_capacity(int **capacity, int count);

bool is_link_line(char *line);
double get_time_sec();
void print_execution_time(double time);

//algo
bool bfs(t_room_list *list_rooms, int **capacity, int *parent, bool *visited, int *queue);
void extract_paths(t_room_list *list_rooms, t_path_list *paths, int **capacity);
t_path_list *find_all_path(t_room_list *list_rooms, int nb_start_ant);
void ants_repartition(t_path_list *paths, int nb_start_ant);
int count_data_paths(t_path_list *paths, int nb_start_ant, bool is_print);

void ants_repartition(t_path_list *paths, int nb_start_ant);
int count_data_paths(t_path_list *paths, int nb_start_ant, bool is_print);

//interface
int init_window(t_path_list *list_path, t_room_list *list_rooms);

//utils_libft
int	ft_atoi(const char *nptr);
void *ft_calloc(size_t nmemb, size_t size);
void *ft_memchr(const void *s, int c, size_t n);
void *ft_memcpy(void *dest, const void *src, size_t n);
void *ft_memmove(void *dest, const void *src, size_t n);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strchr(const char *s, int c);
size_t ft_strlcpy(char	*dest, const char *src, size_t size);
void free_split(char **split);
char **ft_split(char const *s, char c);
char *ft_strdup(const char *s);
char *get_next_line(int fd);
int	ft_strcmp(const char *s1, const char *s2);

#include "raylib.h"
#include "raymath.h"

typedef struct s_anim {
    int     **routes;
    int     *route_len;
    int     *first_ant;
    int     nb_paths;
    int     total_turns;
    float   turn;
    float   speed;
    bool    playing;
} t_anim;

//animation
int get_index_room(t_room_list *list, t_room *target);
int init_anmation(t_anim *anim, t_room_list *rooms, t_path_list *paths);
void anim_update(t_anim *anim);
void anim_draw(t_anim *anim, t_path_list *paths, Vector2 *positions, float zoom);
void anim_draw_ids(t_anim *anim, t_path_list *paths, Vector2 *positions, Camera2D camera);
void anim_draw_hud(t_anim *anim, t_path_list *paths);
void anim_free(t_anim *anim);

#endif