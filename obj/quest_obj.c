/*
 * This is a standard quest object.
 * Configure it to make it look the way you want.
 */

string hint_string, name;

/**
 * Sets a hint for the quest
 * @param {string} h Quest hint string
 */
void set_hint(string h) {
    hint_string = h;
}

/**
 * Sets the name of the quest
 * @param {string} n name of the quest
 */
void set_name(string n) {
    name = n;
}

int id(string str) { return str == name || str == "quest"; }

string short() {
    return name;
}

void long() {
    write("This is the quest '" + name + "':\n");
    write(hint_string);
}

string hint() { return hint_string; }
