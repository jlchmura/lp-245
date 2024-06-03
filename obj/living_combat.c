#include "living_defs.h"

int hit_player(int dam) {
    if (!attacker_ob)
	set_heart_beat(1);
    if (!attacker_ob && this_player() != this_object())
	attacker_ob = this_player();
    else if (!alt_attacker_ob && attacker_ob != this_player() &&
	     this_player() != this_object())
	alt_attacker_ob = this_player();
    /* Don't damage wizards too much ! */
    if (level >= 20 && !is_npc && dam >= hit_point) {
	tell_object(this_object(),
		    "Your wizardhood protects you from death.\n");
	return 0;
    }
    if(dead)
	return 0;	/* Or someone who is dead */
    dam -= random(armour_class + 1);
    if (dam <= 0)
	return 0;
    if (dam > hit_point+1)
	dam = hit_point+1;
    hit_point = hit_point - dam;
    if (hit_point<0) {
	object corpse;
	/* We died ! */
	
	if (!is_npc && !query_ip_number(this_object())) {
	    /* This player is linkdead. */
	    write(cap_name + " is not here. You cannot kill a player who is not logged in.\n");
	    hit_point = 20;
	    stop_fight();
	    if (this_player())
	        this_player()->stop_fight();
            return 0;
	}

	dead = 1;
	if (hunter)
	    hunter->stop_hunter();
	hunter = 0;
	hunted = 0;
	say(cap_name + " died.\n");
	experience = 2 * experience / 3;	/* Nice, isn't it ? */
	hit_point = 10;
	/* The player killing us will update his alignment ! */
	/* If he exist */
	if(attacker_ob) {
	    attacker_ob->add_alignment(ADJ_ALIGNMENT(alignment));
	    attacker_ob->add_exp(experience / 35);
	}
	corpse = clone_object("obj/corpse");
	corpse->set_name(name);
	transfer_all_to(corpse);
	move_object(corpse, environment(this_object()));
	if (!this_object()->second_life())
	    destruct(this_object());
	if (!is_npc)
	    save_object("players/" + name);
    }
    return dam;
}



/**
 * This routine is called when we are attacked by a player.
 * @param {object} ob The living object that is attacking us
 */
void attacked_by(object ob) {
    if (!attacker_ob) {
	attacker_ob = ob;
	set_heart_beat(1);
	return;
    }
    if (!alt_attacker_ob) {
	alt_attacker_ob = ob;
	return;
    }
}


/**
 * @returns true if there still is a fight.
 */
int attack()
 {
     int tmp;
     mixed whit;
     string name_of_attacker;
 
     if (!attacker_ob) {
     spell_cost = 0;
     return 0;
     }
     name_of_attacker = attacker_ob->query_name();
     if (!name_of_attacker || name_of_attacker == NAME_OF_GHOST ||
     environment(attacker_ob) != environment(this_object())) {
     if (!hunter && name_of_attacker &&
         !attacker_ob->query_ghost())
     {
         tell_object(this_object(), "You are now hunting " +
             attacker_ob->query_name() + ".\n");
         hunted = attacker_ob;
         hunting_time = 10;
     }
     attacker_ob = 0;
     if (!alt_attacker_ob)
         return 0;
     attacker_ob = alt_attacker_ob;
     alt_attacker_ob = 0;
     if (attack()) {
         if (attacker_ob)
         tell_object(this_object(),
                 "You turn to attack " +
                 attacker_ob->query_name() + ".\n");
         return 1;
     }
     return 0;
     }
     if (spell_cost) {
     spell_points -= spell_cost;
     tell_object(attacker_ob, "You are hit by a " + spell_name + ".\n");
     write("You cast a " + spell_name + ".\n");
     }
     if(name_of_weapon) {
     whit = name_of_weapon->hit(attacker_ob);
     if (!attacker_ob) {
         tell_object(this_object(), "CRACK!\nYour weapon broke!\n");
         log_file("BAD_SWORD", name_of_weapon->short() + ", " +
              creator(name_of_weapon) + " XX !\n");
         spell_cost = 0;
         spell_dam = 0;
         destruct(name_of_weapon);
         weapon_class = 0;
         return 1;
     }
     }
     if(whit != "miss") {
     tmp = ((weapon_class + whit) * 2 + Dex) / 3;
     if (tmp == 0)
         tmp = 1;
     tmp = attacker_ob->hit_player(random(tmp) + spell_dam);
     } else
     tmp = 0;
     tmp -= spell_dam;
     if (!is_npc && name_of_weapon && tmp > 20 &&
       random(100) < weapon_class - level * 2 / 3 - 14) {
     tell_object(this_object(), "CRACK!\nYour weapon broke!\n");
     tell_object(this_object(),
             "You are too inexperienced for such a weapon.\n");
     log_file("BAD_SWORD", name_of_weapon->short() + ", " +
          creator(name_of_weapon) + "\n");
     spell_cost = 0;
     spell_dam = 0;
     destruct(name_of_weapon);
     weapon_class = 0;
     return 1;
     }
     tmp += spell_dam;
     if (tmp == 0) {
     tell_object(this_object(), "You missed.\n");
     say(cap_name + " missed " + name_of_attacker + ".\n");
     spell_cost = 0;
     spell_dam = 0;
     return 1;
     }
     experience += tmp;
     tmp -= spell_dam;
     spell_cost = 0;
     spell_dam = 0;
     /* Does the enemy still live ? */
     if (attacker_ob &&
       attacker_ob->query_name() != NAME_OF_GHOST) {
     string how, what;
     how = " to small fragments";
     what = "massacre";
     if (tmp < 30) {
         how = " with a bone crushing sound";
         what = "smash";
     }
     if (tmp < 20) {
         how = " very hard";
         what = "hit";
     }
     if (tmp < 10) {
         how = " hard";
         what = "hit";
     }
     if (tmp < 5) {
         how = "";
         what = "hit";
     }
     if (tmp < 3) {
         how = "";
         what = "grazed";
     }
     if (tmp == 1) {
         how = " in the stomach";
         what = "tickled";
     }
     tell_object(this_object(), "You " + what + " " + name_of_attacker +
             how + ".\n");
     tell_object(attacker_ob, cap_name + " " + what + " you" + how +
             ".\n");
     say(cap_name + " " + what + " " + name_of_attacker + how +
             ".\n", attacker_ob);
     return 1;
     }
     tell_object(this_object(), "You killed " + name_of_attacker + ".\n");
     attacker_ob = alt_attacker_ob;
     alt_attacker_ob = 0;
     if (attacker_ob)
     return 1;
 
     return 0;
 }
 
object query_attack() {
     /* Changed by Herder */
     return attacker_ob;
     /* OLD
     if (attacker_ob)
     return 1;
     return 0;
     */
 }


void attack_object(object ob)
{
   if (ob->query_ghost())
       return;
   set_heart_beat(1);	/* For monsters, start the heart beat */
   if (attacker_ob == ob) {
       attack();
       return;
   }
   if (alt_attacker_ob == ob) {
       alt_attacker_ob = attacker_ob;
       attacker_ob = ob;
       attack();
       return;
   }
   if (!alt_attacker_ob)
       alt_attacker_ob = attacker_ob;
   attacker_ob = ob;
   attacker_ob->attacked_by(this_object());
   attack();
}


void run_away() {
    object here;
    int i, j;

    here = environment();
    i = 0;
    j = random(6);
    while(i<6 && here == environment()) {
	i += 1;
	j += 1;
	if (j > 6)
	    j = 1;
	if (j == 1) command("east");
	if (j == 2) command("west");
	if (j == 3) command("north");
	if (j == 4) command("south");
	if (j == 5) command("up");
	if (j == 6) command("down");
    }
    if (here == environment()) {
	say(cap_name + " tried, but failed to run away.\n", this_object());
	tell_object(this_object(),
	    "Your legs tried to run away, but failed.\n");
    } else {
	tell_object(this_object(), "Your legs run away with you!\n");
    }
}
