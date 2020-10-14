// Realizzato da Taiello R. e Pellegrino M.
#ifndef PROJECTPT_SEM_UTILITIES_H
#define PROJECTPT_SEM_UTILITIES_H
#define LETTORE 0
#define SCRITTORE 1

int createSemSet(int semNum);	// crea un set con semNum semafori (OK==0 || -1==errore || se la chiave già esiste-->errore EEXIST)

int setSem (int semId, int semNum, int semValue);	// inizializza il semaforo al set setmId, al numero semNum, con il valore semValue (OK==0 || -1==errore)

int getSemValue (int semId, int semNum);	// ritorna il valore del semaforo, al set semId, al numero semNum

int setAllSem (int semId, unsigned short* values);	// inizializza i semafori, al set setmId, con i numeri di values (OK==0 || -1==errore)

unsigned short* getAllSemValue (int semId);	// ritorna i valori del semaforo al set semId

int subSem(int semId, int semNum);	// decrementa di 1 il semaforo, al set semId, al numero semNum (OK==0 || -1==errore)

int addSem(int semId, int semNum);	// incrementa di 1 il semaforo, al set semId, al numero semNum (OK==0 || -1==errore)

int waitSem(int semId, int semNum);

int getLastPid(int semId, int semNum);	// ritorna il PID dell'ultimo processo che ha eseguito una semop() sul semaforo

int getNCNT(int semId, int semNum); // ritorna il numero di processi attualmente in attesa di un incremento del valore del semaforo

int getZCNT(int semId, int semNum); // ritorna il numero di processi attualmente in attesa che il valore del semaforo divenga 0

int getSemVal(int semId, int semNum);

int rmSem(int semId); // rimuove il set di semafori (OK==0 || -1==errore)

#ifndef _SYS_SEM_H_

union semun {
    // value for SETVAL
    int val;
    // buffer for IPC_STAT, IPC_SET
    struct semid_ds* buf;
    // array for GETALL, SETALL
    unsigned short* array; // Linux specific part

    // buffer for IPC_INFO
    struct seminfo* __buf;
};
#endif

#endif //PROJECTPT_SEM_UTILITIES_H
