#ifndef LIVING_DEFS_H
#define LIVING_DEFS_H

#include "log.h"

#define KILL_NEUTRAL_ALIGNMENT		10
#define ADJ_ALIGNMENT(al)		((-al - KILL_NEUTRAL_ALIGNMENT)/4)
#define NAME_OF_GHOST			"some mist"

/*
 * If you are going to copy this file, in the purpose of changing
 * it a little to your own need, beware:
 *
 * First try one of the following:
 *
 * 1. Do clone_object(), and then configur it. This object is specially
 *    prepared for configuration.
 *
 * 2. If you still is not pleased with that, create a new empty
 *    object, and make an inheritance of this objet on the first line.
 *    This will automatically copy all variables and functions from the
 *    original object. Then, add the functions you want to change. The
 *    original function can still be accessed with '::' prepended on the name.
 *
 * The maintainer of this LPmud might become sad with you if you fail
 * to do any of the above. Ask other wizards if you are doubtful.
 *
 * The reason of this, is that the above saves a lot of memory.
 */

/*
 * Include this file in objects that "lives".
 * The following variables are defined here:
 *
 */

int time_to_heal;	/* Count down variable. */
int money;		/* Amount of money on player. */
string name;		/* Name of object. */
string msgin, msgout;	/* Messages when entering or leaving a room. */
int is_npc, brief;	/* Flags. */
int level;		/* Level of monster. */
static int armour_class;	/* What armour class of monster. */
int hit_point;		/* Number of hit points of monster. */
int max_hp, max_sp;
int experience;		/* Experience points of monster. */
string mmsgout;		/* Message when leaving magically. */
string mmsgin;		/* Message when arriving magically. */
static object attacker_ob;	/* Name of player attacking us. */
static object alt_attacker_ob;	/* Name of other player also attacking us. */
static int weapon_class;	/* How good weapon. Used to calculate damage. */
static object name_of_weapon;	/* To see if we are wielding a weapon. */
static object head_armour;	/* What armour we have. */
int ghost;		/* Used for monsters that can leave a ghost. */
static int local_weight;	/* weight of items */
static object hunted, hunter;	/* used in the hunt mode */
static int hunting_time;	/* How long we will stay in hunting mode. */
static string cap_name;	/* Capital version of "name". */
int spell_points;	/* Current spell points. */
static string spell_name;
static int spell_cost, spell_dam;
int age;		/* Number of heart beats of this character. */
int is_invis;		/* True when player is invisible */
int frog;		/* If the player is a frog */
int whimpy;		/* Automatically run when low on HP */
string auto_load;	/* Special automatically loaded objects. */
int dead;		/* Are we alive or dead? */
string flags;		/* Bit field of flags */

/*
 * All characters have an aligment, depending on how good or chaotic
 * they are.
 * This value is updated when killing other players.
 */
int alignment;
int gender;	/* 0 means neuter ("it"), 1 male ("he"),  2 female ("she") */

/*
 * Character stat variables.
 */
int Str, Int, Con, Dex;

/*
 * The following routines are defined for usage:
 * stop_fight		Stop a fight. Good for scroll of taming etc.
 * hit_player		Called when fighting.
 * transfer_all_to:	Transfer all objects to dest.
 * move_player:		Called by the object that moves the monster.
 * query_name:		Gives the name to external objects.
 * attacked_by		Tells us who are attacking us.
 * show_stats		Dump local status.
 * stop_wielding	Called when we drop a weapon.
 * stop_wearing		Called when we drop an armour.
 * query_level		Give our level to external objects.
 * query_value		Always return 0. Can't sell this object.
 * query_npc		Return 1 if npc otherwise 0.
 * get			Always return 0. Can't take this object.
 * attack		Should be called from heart_beat. Will maintain attack.
 * query_attack
 * drop_all_money	Used when the object dies.
 * wield		Called by weapons.
 * wear			Called by armour.
 * add_weight		Used when picking up things.
 * heal_self		Enable wizards to heal this object.
 * can_put_and_get	Can look at inventory, but not take things from it.
 * attack_object	Called when starting to attack an object.
 * test_if_any_here	For monsters. Call this one if you suspect no enemy
 *			is here any more.
 *			Return 1 if anyone there, 0 if none.
 * force_us		Force us to do a command.
 * query_spell_points	Return how much spell points the character has.
 * reduce_hit_point	Reduce hit points, but not below 0.
 */

void attack_object(object ob);
void stop_fight();
void transfer_all_to(object dest);
string short();
string query_gender_string();
string query_stats();
void show_age();

/** Forward Defines */
varargs mixed query_quests(string str);
int second_life();

/* living_combat.c */

/**
 * This function is called from other players when they want to make
 * damage to us. We return how much damage we received, which will
 * change the attackers score. This routine is probably called from
 * heart_beat() from another player.
 * Compare this function to reduce_hit_point(dam).
 * @param {int} dam Amount of damange that will be received
 * @return {int} Adjusted damage amount
 */
int hit_player(int dam);

/**
 * This routine is called when we are attacked by a player.
 * @param {object} ob The living object that is attacking us
 */
void attacked_by(object ob);

/**
 * @returns true if there still is a fight.
 */
int attack();
object query_attack();
void attack_object(object ob);
void run_away();

#endif // LIVING_DEFS_H
