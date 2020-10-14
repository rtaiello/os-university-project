// Realizzato da Taiello R. e Pellegrino M.
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shm_utilities.h"

//----- funzione relativa alla generazione di un nuovo ID  di SM -----
struct shmid_ds buf;

int createShm(int size) {
    int shmId = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666);
    if (shmId != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in createShm #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return shmId;
}

//----- funzione relativa all'attribuzine di un segmento su uno specifico ID di SM ----
void *attachesSeg(int shmid) {
    void * res;
    res = shmat(shmid, NULL, 0);
    if(res !=(void *) -1){
    } else {
        fprintf(stderr, "%s: %d. Errore in attachesSeg #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}

//-----funzione relativa allo staccamento di un segmento su uno specifico ID di SM----
int detachesSeg(void *seg) {
    int tmp = shmdt(seg);
    if (tmp != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in detachesSeg #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return tmp;
}

//-----funzine che elimina una determinata SM----
int removeShm(int shmid) {
    int tmp = shmctl(shmid, IPC_RMID, 0);
    if (tmp != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in removeShm #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return tmp;
}

// -----funzione relativa al numero di processi agganciati a una specifica SM-----
unsigned long nAttaches(int shmid) {
    if (shmctl(shmid, IPC_STAT, &buf)!=-1){
    } else {
        fprintf(stderr, "%s: %d. Errore in nAttaches #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return buf.shm_nattch;
}
int nByteShm(int shmid){ //funzione che restituisce numero di byte della relativa memroia condivisa
if (shmctl(shmid, IPC_STAT, &buf)!=-1){
} else {
    fprintf(stderr, "%s: %d. Errore in nAttaches #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
    exit(EXIT_FAILURE);
}
return buf.shm_segsz;
}
//altre funzione di informazione da aggiungere
