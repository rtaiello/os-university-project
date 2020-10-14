// Realizzato da Taiello R. e Pellegrino M.

#ifndef PROJECTPT_PEOPLE_H
#define PROJECTPT_PEOPLE_H
#include <stdlib.h>
#define NAME_LENGTH 255
typedef struct _personStruct {
    int myIndex; //inserito per comodità, sono a che cella iesima aggioranre la struttura quando creo una nuova persona dopo birth_date
    pid_t myPid;
    char tipo;
    char name[NAME_LENGTH]; // N.B USO L'ARRAY PERCHÈ ALTRIMENTI CON LA SUCCESSIVA MALLOC SI SAREBBE POTUTO COPIARE
    unsigned long genes;
    int scelto;
} personStruct;

personStruct getPersonStruct(pid_t thisPid,personStruct *p,int length);
int existPerson(pid_t thisPid,personStruct *p,int length);

#endif //PROJECTPT_PEOPLE_H