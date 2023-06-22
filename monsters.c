#include "monsters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int g_respect;

room *create_room(int st, int n, int s, int e, int w, int *total_num_rooms) {
    room *new_room = malloc(sizeof(room));
    new_room->state = st;
    new_room->north = n;
    new_room->south = s;
    new_room->east = e;
    new_room->west = w;
    new_room->creatures = malloc(10 * sizeof(creature));
    new_room->num_creatures = 0;
    new_room->id = (*total_num_rooms)++;
    return new_room;
}

creature *create_creature(int t, int l, room *room_store, creature *creature_store, int *total_num_creatures) {
    creature *new_creature = malloc(sizeof(creature));
    new_creature->type = t;
    new_creature->location = l;
    new_creature->id = (*total_num_creatures);
    (room_store + l)->creatures[(room_store + l)->num_creatures++] = *new_creature;
    creature_store[(*total_num_creatures)++] = *new_creature;
    return new_creature;
}

void help() {
    printf("On game startup, enter the number of creatures you would like in the world (1 < n < 100).\n");
    printf("Then enter the state of each room and its neighbors when prompted.\n");
    printf("Then enter the number of creatures you would like in the world (1 < m < 100)\n");
    printf("Then enter the creature type and location for each creature when prompted.\n");
    printf("Then enter any commands you would like (see commands below).\n");
    printf("If you would like to see this help menu, just type 'help'. If your respect reaches 100, you win. Good luck keeping the creatures happy.\n");
    printf("\nThe 'clean' command causes the PC to clean whatever room they're currently in (assuming it is not already clean).\n");
    printf("The '2:clean' command causes the PC to make creature 2 clean the PC's current room (assuming the creature is present in the PC's room).\n");
    printf("The 'dirty' command causes the PC to dirty whatever room they're currently in (assuming it is not already dirty).\n");
    printf("The '2:dirty' command causes the PC to make creature 2 dirty the PC's room (assuming the creature is present in the PC's room).\n");
    printf("The 'east' command causes the PC to move to its current room's east neighbor (assuming there is such a neighbor).\n");
    printf("The '2:leave' command causes the PC to tell creature 2 to go to a different room (assuming creature 2 is present in the PC's room).\n");
    printf("The 'look' command gives a description of the room the PC is currently in: its state, its neighbors, its inhabitants, etc.\n");
    printf("The 'exit' command exits you from the simulation.\n");
    printf("The 'help' command brings up this very window giving a brief description of all commands in the game.\n");
}

void look(room *room_store, creature *c) {
    int room_num = c->location;
    char state_str[100];
    if (room_store[c->location].state == 0) {
        strcpy(state_str, "clean, ");
    } else if (room_store[c->location].state == 1) {
        strcpy(state_str, "half-dirty, ");
    } else if (room_store[c->location].state == 2) {
        strcpy(state_str, "dirty, ");
    }
    printf("Room %d, %s", room_num, state_str);
    (room_store + c->location)->north != -1 ? printf("neighbors %d to the north\n", room_store[c->location].north)
                                            : printf("");
    (room_store + c->location)->south != -1 ? printf("neighbors %d to the south\n", room_store[c->location].south)
                                            : printf("");
    (room_store + c->location)->east != -1 ? printf("neighbors %d to the east\n", room_store[c->location].east)
                                           : printf("");
    (room_store + c->location)->west != -1 ? printf("neighbors %d to the west\n", room_store[c->location].west)
                                           : printf("");
    printf("contains:\n");
    for (int i = 0; i < room_store[c->location].num_creatures; ++i) {
        char type_str[100];
        if (room_store[c->location].creatures[i].type == 0) {
            strcpy(type_str, "PC");
            printf("%s\n", type_str);
        } else {
            if (room_store[c->location].creatures[i].type == 1) strcpy(type_str, "NPC");
            else if (room_store[c->location].creatures[i].type == 2) strcpy(type_str, "Animal");
            printf("%s %d\n", type_str, (room_store + c->location)->creatures[i].id);
        }
    }
}

void generic_clean(room *room_store, creature *pc) {
    room *curr_room = room_store + pc->location;
    if (curr_room->state == 0) printf("Room %d is already clean\n", pc->location);
    else {
        --curr_room->state;
        for (int i = 0; i < curr_room->num_creatures; ++i) {
            creature *c = &curr_room->creatures[i];
            if (c->type == 1) {
                --g_respect;
                printf("NPC %d grumbles. Respect is now %d\n", c->id, g_respect);
            } else if (c->type == 2) {
                ++g_respect;
                printf("Animal %d licks your face. Respect is now %d\n", c->id, g_respect);
            }
        }
    }
    for (int j = 0; j < curr_room->num_creatures; ++j) {
        if (curr_room->state == 0 && curr_room->creatures[j].type == 1) {
            state_induced_leave(room_store, &curr_room->creatures[j]);
        }
    }
}

void generic_dirty(room *room_store, creature *pc) {
    room *current_room = room_store + pc->location;
    if (current_room->state == 2) printf("Room %d is already dirty\n", pc->location);
    else {
        ++current_room->state;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            creature curr_creature = current_room->creatures[i];
            if (curr_creature.type == 1) {
                ++g_respect;
                printf("NPC %d smiles. Respect is now %d\n", curr_creature.id, g_respect);
            } else if (curr_creature.type == 2) {
                --g_respect;
                printf("Animal %d growls. Respect is now %d\n", curr_creature.id, g_respect);
                if (current_room->state == 2) {
                    state_induced_leave(room_store, &curr_creature);
                }
            }
        }
    }
}

void specific_dirty(room *room_store, creature *pc, int target_id) {
    room *current_room = room_store + pc->location;
    creature *target_creature = 0;
    int is_present = 0;
    for (int i = 0; i < current_room->num_creatures; ++i) {
        if (current_room->creatures[i].id == target_id) {
            target_creature = current_room->creatures + i;
            is_present = 1;
            break;
        }
    }
    if (!is_present) {
        printf("No such creature in room %d\n", pc->location);
        return;
    } else if (current_room->state == 2) {
        printf("Room is already dirty\n");
        return;
    }
    current_room->state--;
    if (target_creature->type == 1) {
        g_respect += 3;
        printf("NPC %d smiles a lot. Respect is now %d\n", target_creature->id, g_respect);
    } else if (target_creature->type == 2) {
        g_respect -= 3;
        printf("Animal %d growls a lot. Respect is now %d\n", target_creature->id, g_respect);
    }
    for (int j = 0; j < current_room->num_creatures; ++j) {
        creature *curr_creature = &current_room->creatures[j];
        if (curr_creature->type == 1 && curr_creature->id != target_id) {
            ++g_respect;
            printf("NPC %d smiles. Respect is now %d\n", curr_creature->id, g_respect);
        } else if (curr_creature->type == 2 && curr_creature->id != target_id) {
            --g_respect;
            printf("Animal %d growls. Respect is now %d\n", curr_creature->id, g_respect);
        }
    }
    if (current_room->state == 2) {
        for (int l = 0; l < current_room->num_creatures; ++l) {
            if (current_room->creatures[l].type == 2) {
                state_induced_leave(room_store, &current_room->creatures[l]);
            }
        }
    }
}

void specific_clean(room *room_store, creature *pc, int target_id) {
    room *current_room = room_store + pc->location;
    creature *target_creature = 0;
    int is_present = 0;
    for (int i = 0; i < current_room->num_creatures; ++i) {
        if (current_room->creatures[i].id == target_id) {
            target_creature = current_room->creatures + i;
            is_present = 1;
            break;
        }
    }
    if (!is_present) {
        printf("No such creature in room %d\n", pc->location);
        return;
    } else if (current_room->state == 0) {
        printf("Room is already clean\n");
        return;
    }
    current_room->state--;
    if (target_creature->type == 1) {
        g_respect -= 3;
        printf("NPC %d grumbles a lot. Respect is now %d\n", target_creature->id, g_respect);
    } else if (target_creature->type == 2) {
        g_respect += 3;
        printf("Animal %d licks your face a lot. Respect is now %d\n", target_creature->id, g_respect);
    }
    for (int j = 0; j < current_room->num_creatures; ++j) {
        creature *curr_creature = &current_room->creatures[j];
        if (curr_creature->type == 1 && curr_creature->id != target_id) {
            --g_respect;
            printf("NPC %d grumbles. Respect is now %d\n", curr_creature->id, g_respect);
        } else if (curr_creature->type == 2 && curr_creature->id != target_id) {
            ++g_respect;
            printf("Animal %d licks your face. Respect is now %d\n", curr_creature->id, g_respect);
        }
    }
    if (current_room->state == 0) {
        for (int l = 0; l < current_room->num_creatures; ++l) {
            if (current_room->creatures[l].type == 1) {
                state_induced_leave(room_store, current_room->creatures + l);
            }
        }
    }
}

void state_induced_leave(room *room_store, creature *c) {
    room *current_room = room_store + c->location;
    room *new_room = 0;
    if (current_room->north != -1) {
        c->location = current_room->north;
        new_room = room_store + current_room->north;
        /* shift creatures in current room to keep data store consistent */
        creature *new_store = malloc(10 * sizeof(creature));
        int new_num = 0;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            if (current_room->creatures[i].id != c->id) {
                new_store[new_num++] = current_room->creatures[i];
            }
        }
        current_room->creatures = new_store;
        current_room->num_creatures--;
        new_room->creatures[new_room->num_creatures++] = *c;
        if (c->type == 1) {
            printf("NPC %d leaves north\n", c->id);
        } else if (c->type == 2) {
            printf("Animal %d leaves north\n", c->id);
        }
    } else if (current_room->south != -1 && (room_store + current_room->south)->num_creatures < 10) {
        new_room = room_store + current_room->south;
        /* shift creatures in current room to keep data store consistent */
        creature *new_store = malloc(10 * sizeof(creature));
        int new_num = 0;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            if (current_room->creatures[i].id != c->id) {
                new_store[new_num++] = current_room->creatures[i];
            }
        }
        c->location = new_room->id;
        creature *old = current_room->creatures;
        current_room->creatures = new_store;
        current_room->num_creatures--;
        new_room->creatures[new_room->num_creatures++] = *c;
        if (c->type == 1) {
            printf("NPC %d leaves south\n", c->id);
        } else if (c->type == 2) {
            printf("Animal %d leaves south\n", c->id);
        }
    } else if (current_room->east != -1 && (room_store + current_room->east)->num_creatures < 10) {
        new_room = room_store + current_room->east;
        /* shift creatures in current room to keep data store consistent */
        creature *new_store = malloc(10 * sizeof(creature));
        int new_num = 0;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            if (current_room->creatures[i].id != c->id) {
                new_store[new_num++] = current_room->creatures[i];
            }
        }
        creature *old = current_room->creatures;
        current_room->creatures = new_store;
        current_room->num_creatures--;
        new_room->creatures[new_room->num_creatures++] = *c;
        c->location = new_room->id;
        if (c->type == 1) {
            printf("NPC %d leaves east\n", c->id);
        } else if (c->type == 2){
            printf("Animal %d leaves east\n", c->id);
        }
    } else if (current_room->west != -1 && (room_store + current_room->west)->num_creatures < 10) {
        new_room = room_store + current_room->south;
        /* shift creatures in current room to keep data store consistent */
        creature *new_store = malloc(10 * sizeof(creature));
        int new_num = 0;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            if (current_room->creatures[i].id != c->id) {
                new_store[new_num++] = current_room->creatures[i];
            }
        }
        c->location = new_room->id;
        creature *old = current_room->creatures;
        current_room->creatures = new_store;
        current_room->num_creatures--;
        new_room->creatures[new_room->num_creatures++] = *c;

        if (c->type == 1) {
            printf("NPC %d leaves west\n", c->id);
        } else if (c->type == 2){
            printf("Animal %d leaves west\n", c->id);
        }
    } else {
        if (c->type == 1) {
            printf("NPC %d exits through the ceiling\n", c->id);
        } else if (c->type == 2) {
            printf("Animal %d exits through the ceiling\n", c->id);
        }
        creature *new_store = malloc(10 * sizeof(creature));
        int new_num = 0;
        for (int i = 0; i < current_room->num_creatures; ++i) {
            if (current_room->creatures[i].id != c->id) {
                new_store[new_num++] = current_room->creatures[i];
            }
        }
        creature *old = current_room->creatures;
        current_room->creatures = new_store;
        current_room->num_creatures--;
        for (int k = 0; k < current_room->num_creatures; ++k) {
            if (current_room->creatures[k].type == 1) {
                --g_respect;
                printf("NPC %d grumbles. Respect is now %d\n", current_room->creatures[k].id, g_respect);
            } else if (current_room->creatures[k].type == 2) {
                --g_respect;
                printf("Animal %d growls. Respect is now %d\n", current_room->creatures[k].id, g_respect);
            }
        }
        return;
    }
    if (new_room->state == 0 && c->type == 1) {
        dirty_to_make_fit(room_store, c);
    } else if (new_room->state == 2 && c->type == 2) {
        clean_to_make_fit(room_store, c);
    }
}

void dirty_to_make_fit(room *room_store, creature *c) {
    room *current_room = room_store + c->location;
    ++current_room->state;
    for (int i = 0; i < current_room->num_creatures; ++i) {
        creature curr_creature = current_room->creatures[i];
        if (curr_creature.id == c->id) continue;
        if (curr_creature.type == 1) {
            ++g_respect;
            printf("NPC %d smiles. Respect is now %d\n", curr_creature.id, g_respect);
        } else if (curr_creature.type == 2) {
            --g_respect;
            printf("Animal %d growls. Respect is now %d\n", curr_creature.id, g_respect);
        }
    }
}

void clean_to_make_fit(room *room_store, creature *c) {
    room *current_room = room_store + c->location;
    --current_room->state;
    for (int i = 0; i < current_room->num_creatures; ++i) {
        creature curr_creature = current_room->creatures[i];
        if (curr_creature.id == c->id) continue;
        if (curr_creature.type == 1) {
            --g_respect;
            printf("NPC %d grumbles. Respect is now %d\n", curr_creature.id, g_respect);
        } else if (curr_creature.type == 2) {
            ++g_respect;
            printf("Animal %d licks your face. Respect is now %d\n", curr_creature.id, g_respect);
        }
    }
}

void pc_leave(room *room_store, creature *pc, int direction) {
    room *current_room = room_store + pc->location;
    int target_room_id;
    if (direction == 0 || direction == 1 || direction == 2 || direction == 3) {
        if (direction == 0) target_room_id = current_room->north;
        else if (direction == 1) target_room_id = current_room->south;
        else if (direction == 2) target_room_id = current_room->east;
        else target_room_id = current_room->west;
        if (target_room_id != -1) {
            creature *old_store = current_room->creatures;
            creature *new_store = malloc(10 * sizeof(creature));
            int new_num = 0;
            for (int i = 0; i < current_room->num_creatures; ++i) {
                if (current_room->creatures[i].id != pc->id) {
                    new_store[new_num++] = current_room->creatures[i];
                }
            }
            current_room->creatures = new_store;
            --current_room->num_creatures;
            room *new_room = room_store + target_room_id;
            new_room->creatures[new_room->num_creatures++] = *pc;
            pc->location = target_room_id;
            if (direction == 0) printf("You leave north\n");
            else if (direction == 1) printf("You leave south\n");
            else if (direction == 2) printf("You leave east\n");
            else printf("You leave west\n");
            return;
        }
    }
    printf("No such room\n");
}

void force_leave(room *room_store, creature *c, creature *pc) {
 room *current_room = &room_store[c->location], *target_room = &room_store[current_room->north];
 target_room->creatures[target_room->num_creatures++] = *c;
 creature *new_store = malloc(10 * sizeof(creature));
 int j = 0;
 for (int i = 0; i < current_room->num_creatures; ++i) {
     if (current_room->creatures[i].id != c->id && current_room->creatures[i].id != pc->id) {
         new_store[j++] = current_room->creatures[i];
     }
 }
 creature *old = current_room->creatures;
 current_room->creatures = new_store;
 current_room->num_creatures--;
}

void get_command(room *room_store, creature *creature_store, creature *pc, int *total_num_rooms,
                 int *total_num_creatures, char input[]) {
    int colon_count = 0;
    for (int i = 0; i < 100; ++i) {
        if (input[i] == ':') {
            ++colon_count;
        }
    }
    if (colon_count == 0) {
        if (strcmp(input, "look\n") == 0) {
            look(room_store, pc);
            return;
        } else if (strcmp(input, "help\n") == 0) {
            help();
            return;
        } else if (strcmp(input, "clean\n") == 0) {
            generic_clean(room_store, pc);
            return;
        } else if (strcmp(input, "dirty\n") == 0) {
            generic_dirty(room_store, pc);
            return;
        } else if (strcmp(input, "north\n") == 0) {
            pc_leave(room_store, pc, 0);
            return;
        } else if (strcmp(input, "south\n") == 0) {
            pc_leave(room_store, pc, 1);
            return;
        } else if (strcmp(input, "east\n") == 0) {
            pc_leave(room_store, pc, 2);
            return; 
        } else if (strcmp(input, "west\n") == 0) {
            pc_leave(room_store, pc, 3); 
            return; 
        }
    } else if (colon_count == 1) {
        char *token = strtok(input, ":");
        int id = atoi(token);
        char *command = strtok(NULL, "\n");
        if (strcmp(command, "clean") == 0) {
            specific_clean(room_store, pc, id);
            return;
        } else if (strcmp(command, "dirty") == 0) {
            specific_dirty(room_store, pc, id);
            return;
        } else if (strcmp(command, "leave") == 0) {
            force_leave(room_store, &creature_store[id], pc);
            return;
        }
    }
    else printf("Invalid command\n");
}
