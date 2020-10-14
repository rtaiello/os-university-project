// Realizzato da Taiello R. e Pellegrino M.
//random char type (a o B)
//random string (solo un carattere) [A-Z]
// random unsigned long [2, 2+genes]

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generateRandom.h"


char generateType(pid_t pid) {
    srand(time(NULL) ^ (pid<<16));
    int randomNumber = rand() % 2;
    switch (randomNumber) {
        case 0 :
            return 'A';
        default :
            return 'B';
    }
}

char *generateName(pid_t pid) {
    srand(time(NULL) ^ (pid<<16));
    int minRange = 65;
    int maxRange = 90;
    int randomAscii = minRange + rand() / (RAND_MAX / (maxRange - minRange + 1) + 1); //[65=A ; Z=90]
    char carattere = (char) randomAscii;
    char *a = (char *) malloc(sizeof(char)*2);
    a[0] = carattere;
    a[1] = '\0';
    return a;
}


unsigned long generateGenes(unsigned long genes, pid_t pid) {
    srand(time(NULL) ^ (pid<<16));
    int minRange = 2;
    int maxRange = 2 + genes;
    return minRange + rand() / (RAND_MAX / (maxRange - minRange + 1) + 1); // [2; 2+genes]
}


/*
int main() {
	int i=0;
	for (i=0; i<100; i++){
			printf("%s \n",generateName());
	}
}
 */