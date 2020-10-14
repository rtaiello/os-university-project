#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //libreria per isdigit(string)
#include <string.h> //libreria per strcpy
#include "Utilities/Controls/checkInputs.h"
#include "Manager/Gestore/manager.h"


void insertInt(int *value);

void insertUI(unsigned long *value);

int main() {
    // inserimento init_people
    int init_people;
    do {
        printf("Inserisci init_people\n");
        insertInt(&init_people);
        printf("init_people: %d\n", init_people); //debug
    } while (checkInitPeople(init_people) == 0);
    printf("Ha inserito un init_people valido\n"); //debug

    //inserimento genes
    unsigned long genes;
    printf("Inserisci genes\n");
    insertUI(&genes);
    printf("genes: %lu\n", genes); //debug
    printf("Ha inserito un genes valido\n"); //debug

    // inserimento sim_time
    int sim_time;
    do {
        printf("Inserisci sim_time\n");
        insertInt(&sim_time);
        printf("sim_time: %d\n", sim_time); //debug
    } while (checkSimTime(sim_time) == 0);
    printf("Ha inserito un sim_time valido\n"); //debug

    // inserimento birth_date
    int birth_date;
    do {
        printf("Inserisci birth_date\n");
        insertInt(&birth_date);
        printf("birth_date: %d\n", birth_date); //debug
    } while (checkBirthDate(birth_date, sim_time) == 0);
    printf("Ha inserito un birth_date valido\n"); //debug

    // chiamo il gestore
    createPeople(init_people, genes, sim_time, birth_date);
    exit(EXIT_SUCCESS);
}

void insertInt(int *value) {
    int flag = 1;
    char string[100];
    int err = 0;
    do {
        strcpy(string, ""); //reinizializzo l'array
        if (err)
            printf("Inserisci nuovamente il valore: \n"); //se e' la seconda volta almeno visualizzo il messaggio di reinserire il valore
        fgets(string, sizeof(string), stdin);
        int i = 0;
        flag = 1;
        while (i < 100 && flag && string[i] != '\n') {
            if (!isdigit(string[i])) flag = 0; //interrompo se non e' un numero
            i++;
        }
        err = 1;
    } while (!flag || string[0] == '\n');
    *value = atoi(string);
    // controllo che la scanf sia andata a buon punto, senno' ripeto
}

void insertUI(unsigned long *value) {
    int flag = 1;
    char string[100];
    int err = 0;
    do {
        strcpy(string, ""); //reinizializzo l'array
        if (err)
            printf("Inserisci nuovamente il valore: \n"); //se e' la seconda volta almeno visualizzo il messaggio di reinserire il valore
        fgets(string, sizeof(string), stdin);
        int i = 0;
        flag = 1;
        while (i < 100 && flag && string[i] != '\n') {
            if (!isdigit(string[i])) flag = 0; //interrompo se non e' un numero
            i++;
        }
        err = 1;
    } while (!flag || string[0] == '\n');
    char *ptr;
    *value = strtoul(string, &ptr, 10);
    strcpy(ptr, "");
    // controllo che la scanf sia andata a buon punto, senno' ripeto
}
