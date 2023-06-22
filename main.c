#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monsters.h"

int g_respect = 40;

int main() {
    room *room_store = malloc(100 * sizeof(room));
    creature *creature_store = malloc(10000 * sizeof(creature)), *pc = 0;
    int num_rooms, total_num_rooms = 0;
    scanf("%d", &num_rooms);
    for (int i = 0; i < num_rooms; ++i) {
        int st, n, s, e, w;
        scanf("%d %d %d %d %d", &st, &n, &s, &e, &w);
        room_store[i] = *create_room(st, n, s, e, w, &total_num_rooms);
    }
    int num_creatures, total_num_creatures = 0;
    scanf("%d", &num_creatures);
    for (int j = 0; j < num_creatures; ++j) {
        int t, l;
        scanf("%d %d", &t, &l);
        creature *new_creature = create_creature(t, l, room_store, creature_store, &total_num_creatures);
        if (t == 0) pc = new_creature;
    }
    char input[100];
    fgets(input, 100, stdin);
    fgets(input, 100, stdin);
    while (strcmp(input, "exit\n") != 0) {
        get_command(room_store, creature_store, pc, &total_num_rooms, &total_num_creatures, input);
        if (g_respect >= 100) {
            printf("You win!\n");
            break;
        } else if (g_respect <= 0) {
            printf("You lose!\n");
        }
        fgets(input, 100, stdin);
    }
    printf("Goodbye!\n");
    free(creature_store);
    free(room_store);
    return 0;
}