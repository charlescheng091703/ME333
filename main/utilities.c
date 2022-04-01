#include "utilities.h"

int state = 1; 

int get_mode(void) {
    return state;
}

void set_mode(int mode) {
    state = mode;
}
