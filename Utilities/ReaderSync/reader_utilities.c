// Realizzato da Taiello R. e Pellegrino M.
#include "../Semaphore/sem_utilities.h"
#include "reader_utilities.h"
#include <stdio.h>

void pReader (int semId, int *numLettori) {
   subSem(semId, LETTORE);
    (*numLettori)++;
   if ((*numLettori)==1) subSem(semId, SCRITTORE);
   addSem(semId, LETTORE);
}

void vReader (int semId, int *numLettori) {
    subSem(semId, LETTORE);
    (*numLettori)--;
    if ((*numLettori)==0) addSem(semId, SCRITTORE);
    addSem(semId, LETTORE);
}