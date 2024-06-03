#include "living_defs.h"
 
#include "living_combat.c"

/**
 * This routine is called from objects that moves the player.
 * Special: direction "X" means teleport.
 * The argument is "how#where".
 * The second optional argument is an object to move the player to.
 * If the second argument exists, then the first argument is taken
 * as the movement message only.
 * @param {string} dir_dest Movement message
 * @param {object} optional_dest_ob Object to move the player to
 */
varargs void move_player(string dir_dest, object optional_dest_ob)
{
    string dir;
    mixed dest;
    object ob;
    int is_light, i;

    if (!optional_dest_ob) {
	if (sscanf(dir_dest, "%s#%s", dir, dest) != 2) {
	    tell_object(this_object(), "Move to bad dir/dest\n");
	    return;
	}
    } else {
	dir = dir_dest;
	dest = optional_dest_ob;
    }
    hunting_time -= 1;
    if (hunting_time == 0) {
	if (hunter)
	    hunter->stop_hunter();
	hunter = 0;
	hunted = 0;	
    }
    if (attacker_ob && present(attacker_ob)) {
	hunting_time = 10;
	if (!hunter)
	    tell_object(this_object(), "You are now hunted by " +
			attacker_ob->query_name() + ".\n");
        hunter = attacker_ob;
    }
    is_light = set_light(0);
    if(is_light < 0)
	is_light = 0;
    if(is_light) {
	if (!msgout)
	    msgout = "leaves";
	if (ghost)
	    say(NAME_OF_GHOST + " " + msgout + " " + dir + ".\n");
	else if (dir == "X" && !is_invis)
	    say(cap_name + " " + mmsgout + ".\n");
	else if (!is_invis)
	    say(cap_name + " " + msgout + " " + dir + ".\n");
    }
    move_object(this_object(), dest);
    is_light = set_light(0);
    if(is_light < 0)
	is_light = 0;
    if (level >= 20) {
	if (!optional_dest_ob)
	    tell_object(this_object(), "/" + dest + "\n");
    }
    if(is_light) {
	if (!msgin)
	    msgin = "arrives";
	if (ghost)
	    say(NAME_OF_GHOST + " " + msgin + ".\n");
	else if (dir == "X" && !is_invis)
	    say(cap_name + " " + mmsgin + ".\n");
	else if (!is_invis)
	    say(cap_name + " " + msgin + ".\n");
    }
    if (hunted && present(hunted))
        attack_object(hunted);
    if (hunter && present(hunter))
        hunter->attack_object(this_object());
    if (is_npc)
	return;
    if (!is_light) {
	write("A dark room.\n");
	return;
    }
    ob = environment(this_object());
    if (brief)
	write(ob->short() + ".\n");
    else
	ob->long();
    for (i=0, ob=first_inventory(ob); ob; ob = next_inventory(ob)) {
	if (ob != this_object()) {
	    string short_str;
	    short_str = ob->short();
	    if (short_str)
		write(short_str + ".\n");
	}
	if (i++ > 40) {
	    write("*** TRUNCATED\n");
	    break;
	}
    }
}

/**
 * Transfers all items to another object
 * @param {object} dest Object to transfer all items to
 */
void transfer_all_to(object dest)
{
    object ob;
    object next_ob;

    ob = first_inventory(this_object());	
    while(ob) {
	next_ob = next_inventory(ob);
	/* Beware that drop() might destruct the object. */
	if (!ob->drop(1) && ob)
	    move_object(ob, dest);
	ob = next_ob;
    }
    local_weight = 0;
    if (money == 0)
	return;
    ob = clone_object("obj/money");
    ob->set_money(money);
    move_object(ob, dest);
    money = 0;
}

/**
 * Returns the name of the object
 * @return {string} Name of the object
 */
string query_name() {
    if (ghost)
	return NAME_OF_GHOST;
    return cap_name;
}

/**
 * Queries the alignment of the living object
 * @return {int} Alignment
 */
int query_alignment() {
    return alignment;
}

/**
 * Queries a flag that indicates if the living object is a NPC
 * @return {int} 1 if the object is a NPC, 0 otherwise
 */
int query_npc() {
    return is_npc;
}


/**
 * Shows the stats of the living object 
 */
void show_stats() {		
    int i;	
    write(short() + "\nlevel:\t" + level +
	  "\ncoins:\t" + money +
	  "\nhp:\t" + hit_point +
	  "\nmax:\t" + max_hp +
	  "\nspell\t" + spell_points +
	  "\nmax:\t" + max_sp);
    write("\nep:\t"); write(experience);
    write("\nac:\t"); write(armour_class);
    if (head_armour)
	write("\narmour: " + head_armour->rec_short());
    write("\nwc:\t"); write(weapon_class);
    if (name_of_weapon)
	write("\nweapon: " + name_of_weapon->query_name());
    write("\ncarry:\t" + local_weight);
    if (attacker_ob)
	write("\nattack: " + attacker_ob->query_name());
    if (alt_attacker_ob)
	write("\nalt attack: " + alt_attacker_ob->query_name());
    write("\nalign:\t" + alignment + "\n");
    write("gender:\t" + query_gender_string() + "\n");
    if (i = this_object()->query_quests())
	write("Quests:\t" + i + "\n");
    write(query_stats());
    show_age();
}

/**
 * Stops wielding a weapon
 */
void stop_wielding() {
	attacked_by(this_object());

    if (!name_of_weapon) {
	/* This should not happen ! */
	log_file("wield_bug", "Weapon not wielded !\n");
	write("Bug ! The weapon was marked as wielded ! (fixed)\n");
	return;
    }
    name_of_weapon->un_wield(dead);
    name_of_weapon = 0;
    weapon_class = 0;
}

/**
 * Stop wearing an armour
 * @param {string} name Name of the armour to stop wearing 
 */
void stop_wearing(string name) {
    if(!head_armour) {
	/* This should not happen ! */
	log_file("wearing_bug", "armour not worn!\n");
	write("This is a bug, no head_armour\n");
	return;
    }
    head_armour = head_armour->remove_link(name);
    if(head_armour && objectp(head_armour))
	armour_class = head_armour->tot_ac();
    else {
	armour_class = 0;
	head_armour = 0;
    }
    if (!is_npc)
	if(!dead)
	    say(cap_name + " removes " + name + ".\n");
    write("Ok.\n");
}

/**
 * Queries the level of the living object
 * @return {int} Level of the living object
 */
int query_level() {
    return level;
}

/* This object is not worth anything in the shop ! */
int query_value() { return 0; }

/* It is never possible to pick up a player ! */
int get() { return 0; }


/**
 * Drops all money from the living object
 * @param {int} verbose If true, the drop is announced
 */
void drop_all_money(int verbose) {
    object mon;
    if (money == 0)
	return;
    mon = clone_object("obj/money");
    mon->set_money(money);
    move_object(mon, environment());
    if (verbose) {
	say(cap_name + " drops " + money + " gold coins.\n");
	tell_object(this_object(), "You drop " + money + " gold coins.\n");
    }
    money = 0;
}

/**
 * Wield a weapon.
 */
void wield(object w) {
    if (name_of_weapon)
	stop_wielding();
    name_of_weapon = w;
    weapon_class = w->weapon_class();
    say(cap_name + " wields " + w->query_name() + ".\n");
    write("Ok.\n");
}

/* Wear some armour. */
object wear(object a) {
    object old;

    if(head_armour) {
	old = head_armour->test_type(a->query_type());
	if(old)
	    return old;
	old = head_armour;
	a->link(old);
    }
    head_armour = a;
    /* Calculate new ac */
    armour_class = head_armour->tot_ac();
    say(cap_name + " wears " + a->query_name() + ".\n");
    write("Ok.\n");
    return 0;
}

int add_weight(int w) {
    if (w + local_weight > Str + 10 && level < 20)
	return 0;
    local_weight += w;
    return 1;
}

void  heal_self(int h) {
    if (h <= 0)
	return;
    hit_point += h;
    if (hit_point > max_hp)
	hit_point = max_hp;
    spell_points += h;
    if (spell_points > max_sp)
	spell_points = max_sp;
}

void restore_spell_points(int h) {
    spell_points += h;
    if (spell_points > max_sp)
	spell_points = max_sp;
}

int can_put_and_get(string str)
{
    return str != 0;
}

int query_ghost() { return ghost; }

void zap_object(object ob)
{
    ob->attacked_by(this_object());
    say(cap_name + " summons a flash from the sky.\n");
    write("You summon a flash from the sky.\n");
    experience += ob->hit_player(100000);
    write("There is a big clap of thunder.\n\n");
}

void missile_object(object ob)
{
    if (spell_points < 10) {
	write("Too low on power.\n");
	return;
    }
    spell_name = "magic missile";
    spell_dam = random(20);
    spell_cost = 10;
    attacker_ob = ob;
}

void shock_object(object ob)
{
    if (spell_points < 15) {
	write("Too low on power.\n");
	return;
    }
    spell_name = "shock";
    spell_dam = random(30);
    spell_cost = 15;
    attacker_ob = ob;
}

void fire_ball_object(object ob)
{
    if (spell_points < 20) {
	write("Too low on power.\n");
	return;
    }
    spell_name = "fire ball";
    spell_dam = random(40);
    spell_cost = 20;
    attacker_ob = ob;
}

/*
 * If no one is here (except ourself), then turn off the heart beat.
 */

int test_if_any_here()
{
    object ob;
    ob = environment();
    if (!ob)
	return 0;
    ob = first_inventory(environment());
    while(ob) {
	if (ob != this_object() && living(ob) && !ob->query_npc())
	    return 1;
	ob = next_inventory(ob);
    }
    return 0;
}

void show_age() {
    int i;

    write("age:\t");
    i = age;
    if (i/43200) {
	write(i/43200 + " days ");
	i = i - (i/43200)*43200;
    }
    if (i/1800) {
	write(i/1800 + " hours ");
	i = i  - (i/1800)*1800;
    }
    if (i/30) {
	write(i/30 + " minutes ");
	i = i - (i/30)*30;
    }
    write(i*2 + " seconds.\n");
}

void stop_hunter()
{
    hunter = 0;
    tell_object(this_object(), "You are no longer hunted.\n");
}

/*
 * This function remains only because of compatibility, as command() now
 * can be called with an object as argument.
 */
void force_us(string cmd) {
    if (!this_player() || this_player()->query_level() <= level ||
	query_ip_number(this_player()) == 0) {
	tell_object(this_object(), this_player()->query_name() +
	    " failed to force you to " + cmd + "\n");
	return;
    }
    command(cmd);
}

/* This is used by the shop etc. */
void add_money(int m) {
#ifdef LOG_EXP
    if (this_player() && this_player() != this_object() &&
      query_ip_number(this_player()) && query_ip_number(this_object()) &&
      level < 20 && m >= ROOM_EXP_LIMIT)
	log_file("EXPERIENCE", ctime(time()) + " " +name + "(" + level +
		") " + m + " money by " + this_player()->query_real_name() +
		"(" + this_player()->query_level() + ")\n");
#endif
    money = money + m;
    if (level <= 19 && !is_npc)
	add_worth(m);
}

int query_money() {
    return money;
}

int query_exp() {
    return experience;
}

int query_frog() {
    return frog;
}

int frog_curse(string arg) {
    if (arg) {
	if (frog)
	    return 1;
	tell_object(this_object(), "You turn into a frog !\n");
	frog = 1;
	return 1;
    }
    tell_object(this_object(), "You turn HUMAN again.\n");
    frog = 0;
    return 0;
}

int query_hp() {
    return hit_point;
}

int query_wimpy() {
    return whimpy;
}

string query_current_room() {
    return object_name(environment(this_object()));
}

int query_spell_points() {
    return spell_points;
}

void stop_fight() {
    attacker_ob = alt_attacker_ob;
    alt_attacker_ob = 0;
}

int query_wc() {
    return weapon_class;
}

int  query_ac() {
    return armour_class;
}

int reduce_hit_point(int dam) {
    object o;
    if(this_player()!=this_object()) {
	log_file("REDUCE_HP",query_name()+" by ");
	if(!this_player()) log_file("REDUCE_HP","?\n");
	else {
	    log_file("REDUCE_HP",this_player()->query_name());
	    o=previous_object();
	    if (o)
		log_file("REDUCE_HP", " " + object_name(o) + ", " +
			 o->short() + " (" + creator(o) + ")\n");
	    else
		log_file("REDUCE_HP", " ??\n");
	}
    }
    /* this will detect illegal use of reduce_hit_point in weapons */
    hit_point -= dam;
    if (hit_point <= 0)
	hit_point = 1;	
    return hit_point;
}

int query_age() {
    return age;
}

/*----------- Most of the gender handling here: ------------*/

int query_gender() { return gender; }
int query_neuter() { return !gender; }
int query_male() { return gender == 1; }
int query_female() { return gender == 2; }

void set_gender(int g) {
    if (g == 0 || g == 1 || g == 2)
        gender = g;
}
int set_neuter() { return gender = 0; }
int set_male() { return gender = 1; }
int set_female() { return gender = 2; }

string query_gender_string() {
    if (!gender)
	return "neuter";
    else if (gender == 1)
	return "male";
    else
	return "female";
}

string query_pronoun() {
    if (!gender)
	return "it";
    else if (gender == 1)
	return "he";
    else
	return "she";
}

string query_possessive() {
    if (!gender)
	return "its";
    else if (gender == 1)
	return "his";
    else
	return "her";
}

string query_objective() {
    if (!gender)
	return "it";
    else if (gender == 1)
	return "him";
    else
	return "her";
}

/*
 * Flags manipulations. You are not supposed to do this arbitrarily.
 * Every wizard can allocate a few bits from the administrator, which
 * he then may use. If you mainpulate bits that you don't know what they
 * are used for, unexpected things can happen.
 */
void set_flag(int n) {
    if (flags == 0)
	flags = "";
#ifdef LOG_FLAGS
    log_file("FLAGS", name + " bit " + n + " set\n");
    if (previous_object()) {
	if (this_player() && this_player() != this_object() &&
	  query_ip_number(this_player()))
	    log_file("FLAGS", "Done by " +
		     this_player()->query_real_name() + " using " +
		     object_name(previous_object()) + ".\n");
    }
#endif
    flags = set_bit(flags, n);
}

int test_flag(int n) {
    if (flags == 0)
	flags = "";
    return test_bit(flags, n);
}

int clear_flag(int n) {
    if (flags == 0)
	flags = "";
#ifdef LOG_FLAGS
    log_file("FLAGS", name + " bit " + n + " cleared\n");
    if (previous_object()) {
	if (this_player() && this_player() != this_object() &&
	  query_ip_number(this_player()))
	    log_file("FLAGS", "Done by " +
		     this_player()->query_real_name() + " using " +
		     object_name(previous_object()) + ".\n");
    }
#endif

    flags = clear_bit(flags, n);
    return 1;
}

string query_stats() {
    return "str:\t" + Str +
	  "\nint:\t" + Int +
	  "\ncon:\t" + Con +
	  "\ndex:\t" + Dex + "\n";
}

int query_str() { return Str; }
int query_int() { return Int; }
int query_con() { return Con; }
int query_dex() { return Dex; }

/* Note that previous object is 0 if called from ourselves. */
void set_str(int i) {
    if (i<1 || i > 20)
	return;
    Str = i;
}

void set_int(int i) {
    if (i<1 || i > 20)
	return;
    Int = i;
    max_sp = 42 + Int * 8;
}

void set_con(int i) {
    if (i<1 || i > 20)
	return;
    Con = i;
    max_hp = 42 + Con * 8;
}

void  set_dex(int i) {
    if (i<1 || i > 20)
	return;
    Dex = i;
}
