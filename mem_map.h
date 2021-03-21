#include <sys/types.h>

typedef struct mem_node Mem_node;
struct mem_node {
    Mem_node *next;
    long first;
    long last;
};

typedef struct {
    Mem_node *node;
} Mem_map;

Mem_map* load_map(pid_t pid);

long get_offset(Mem_map *map, long address);

void add_node(Mem_map *map, Mem_node *node);

void free_map(Mem_map *map);

void print_map(Mem_map *map);