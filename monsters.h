#ifndef MONSTERS_H
#define MONSTERS_H

typedef struct creature {
    int id, type, location;
} creature;

typedef struct room {
    int id, state, north, south, east, west, num_creatures;
    creature *creatures;
} room;

/**
 * Creates a room
 * @param st state of the room
 * @param n id of its north neighbor
 * @param s id of its south neighbor
 * @param e id of its east neighbor
 * @param w id of its west neighbor
 * @param total_num_rooms total number of rooms in the world (used for its id)
 * @return the newly created room
 */
room *create_room(int st, int n, int s, int e, int w, int *total_num_rooms);

/**
 * Creates a new creature
 * @param t the creature's type
 * @param l id of the room the creature is located
 * @param room_store data store for all of the rooms in the world
 * @param creature_store data store for all of the creatures in the world
 * @param total_num_creatures total number of creatures in the world (used for its id)
 * @return the newly created creature
 */
creature *create_creature(int t, int l, room *room_store, creature *creature_store, int *total_num_creatures);

/**
 * Gives a description of the room the PC is currently in (its state, its inhabitants, its neighbors, etc.)
 * @param room_store data store for all of the rooms in the world
 * @param c the pc
 */
void look(room *room_store, creature *c);

/**
 * Retrieves the inputted command and performs the proper action
 * @param room_store data store for all of the rooms in the world
 * @param creature_store data store for all of the creatures in the world
 * @param pc the pc
 * @param total_num_rooms total number of rooms in the world
 * @param total_num_creatures total number of creatures in the world
 * @param input the user inputted command
 */
void
get_command(room *room_store, creature *creature_store, creature *pc, int *total_num_rooms, int *total_num_creatures,
            char input[]);

/**
 * Prints a brief description of the game including all usable commands
 */
void help();

/**
 * Called when a creature is caused to leave their current room b/c of a state change
 * @param room_store the data store of all rooms in the world
 * @param c the creature leaving their current room
 */
void state_induced_leave(room *room_store, creature *c);

/**
 * Called when a creature has to dirty a room to make it fit after leaving its previous room
 * @param room_store data store for all of the rooms in the world
 * @param c the creature dirtying its room
 */
void dirty_to_make_fit(room *room_store, creature *c);

/**
 * Called when a creature has to clean its new room after being forced to leave its previous
 * @param room_store data store for all rooms in the world
 * @param c the creature cleaning its new room
 */
void clean_to_make_fit(room *room_store, creature *c);

/**
 * Causes the PC to clean its current room
 * @param room_store data store of all the rooms in the world
 * @param pc the PC
 */
void generic_clean(room *room_store, creature *pc);

/**
 * Causes the PC to dirty the room its currently in
 * @param room_store the data store for all the rooms in the world
 * @param pc the PC
 */
void generic_dirty(room *room_store, creature *pc);

/**
 * Called when PC orders a creature to clean its room
 * @param room_store data store for all the rooms in the world
 * @param pc the PC
 * @param target_id the ID of the creature being forced to clean its room
 */
void specific_clean(room *room_store, creature *pc, int target_id);

/**
 * Called when PC orders a creature to dirty its room
 * @param room_store data store for all the rooms in the world
 * @param pc the PC
 * @param target_id the ID of the creature being forced to dirty its room
 */
void specific_dirty(room *room_store, creature *pc, int target_id);

/**
 * Called when the PC leaves its current room
 * @param room_store data store for all the rooms in the world
 * @param pc the PC
 * @param direction the direction the PC wishes to move to (north, south, east, west)
 */
void pc_leave(room *room_store, creature *pc, int direction);

/**
 * Called when the PC forces a creature to leave the room
 * @param room_store the data store for all of the rooms in the world
 * @param c the creature being forced to leave its current room
 */
void force_leave(room *room_store, creature *c, creature *pc);

#endif