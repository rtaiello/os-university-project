// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "sem_utilities.h"

int createSemSet(int semNum) {	// crea un set con semNum semafori (OK==0 || -1==errore || se la chiave già esiste-->errore EEXIST)
    int res = semget(IPC_PRIVATE, semNum, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (res != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in createSemSet #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
	return res;
}

int setSem (int semId, int semNum, int semValue) {	// inizializza il semaforo al set setmId, al numero semNum, con il valore semValue (OK==0 || -1==errore)
	union semun arg;
	arg.val=semValue;
    int res = semctl(semId, semNum, SETVAL, arg);
    if (res != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in setSem #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}

int getSemValue (int semId, int semNum) {	// ritorna il valore del semaforo, al set semId, al numero semNum
	return semctl(semId, semNum, GETVAL, NULL);
}	//OK

int setAllSem (int semId, unsigned short* values) {	// inizializza i semafori, al set setmId, con i numeri di values (OK==0 || -1==errore)
	union semun arg;
	arg.array=values;
	return semctl(semId, 0, SETALL, arg);
}	//OK

unsigned short* getAllSemValue (int semId) {	// ritorna i valori del semaforo al set semId
	union semun arg;
	semctl(semId, 0, GETALL, arg);
	return arg.array;
}

int subSem(int semId, int semNum) {	// decrementa di 1 il semaforo, al set semId, al numero semNum (OK==0 || -1==errore)
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    int res = semop(semId, &sops, 1);
    if(res != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in subSem #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}

int addSem(int semId, int semNum) {	// incrementa di 1 il semaforo, al set semId, al numero semNum (OK==0 || -1==errore)
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    int res = semop(semId, &sops, 1);
    if(res != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in addSem #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}

int waitSem(int semId, int semNum) {	// metto in attesa il processo finche' il semaforo non arriva a 0, al set semId, al numero semNum (OK==0 || -1==errore)
	struct sembuf sops;
	sops.sem_num = semNum;
	sops.sem_op = 0;
	sops.sem_flg = 0;
    int res = semop(semId, &sops, 1);
    if (res == -1) {
        if(errno == EINTR){
            printf("errore catturato");
        }
        fprintf(stderr, "%s: %d. Errore in waitSem #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}


int getLastPid(int semId, int semNum) {	// ritorna il PID dell'ultimo processo che ha eseguito una semop() sul semaforo
	return semctl(semId, semNum, GETPID);
}

int getNCNT(int semId, int semNum) {	// ritorna il numero di processi attualmente in attesa di un incremento del valore del semaforo
	return semctl(semId, semNum, GETNCNT);
}

int getZCNT(int semId, int semNum) {	// ritorna il numero di processi attualmente in attesa che il valore del semaforo divenga 0
	return semctl(semId, semNum, GETZCNT);
}
int getSemVal(int semId, int semNum){
    return semctl(semId, semNum, GETVAL);
}
int rmSem(int semId){	// rimuove il set di semafori (OK==0 || -1==errore)
	return semctl(semId, 0, IPC_RMID);
}	//OK

/*
int main () {
	printf("DEBUGGIN sem_utilities.c\n\n");
	int idSemaforo= createSemSet(1);
	printf("Semaforo creato con id: %d \n", idSemaforo);
	printf("setSem=%d\n", setSem(idSemaforo, 0, 3));
	printf("Semaforo con id  %d ora ha valore %d\n", idSemaforo, getSemValue(idSemaforo, 0));
	printf("subSem=%d\n", subSem(idSemaforo, 0));
	printf("Semaforo con id  %d ora ha valore %d\n", idSemaforo, getSemValue(idSemaforo, 0));
	printf("addSem=%d\n", addSem(idSemaforo, 0));
	printf("Semaforo con id  %d ora ha valore %d\n", idSemaforo, getSemValue(idSemaforo, 0));


	int idSemaforo2=createSemSet(2);
	printf("Set di semafori creato con id: %d \n", idSemaforo2);
	unsigned short valori [2];
	valori[0]=7;
	valori[1]=10;
	printf("setAllSem=%d\n", setAllSem(idSemaforo2, valori));
	printf("%d \n", getSemValue(idSemaforo2, 0));
	printf("%d \n", getSemValue(idSemaforo2, 1));
	printf("rimozione1=%d \n", rmSem(idSemaforo));
	printf("rimozione2=%d \n", rmSem(idSemaforo2));
}
 */
