#include "checkInputs.h"

int checkInitPeople(int init_people) {
    if (init_people >= 2) {
        return 1;
    } else {
        return 0;
    }
}

int checkSimTime(int sim_time) {
    if (sim_time >= 0) {
        return 1;
    } else {
        return 0;
    }
}

int checkBirthDate(int birth_date, int sim_time) {
    if ((birth_date > 0) && (birth_date < sim_time)) {
        return 1;
    } else {
        return 0;
    }

}