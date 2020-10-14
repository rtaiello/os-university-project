// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "msg_utilities.h"


int createMsg() {  // crea una coda di messaggi e ne restituisce l'id
    int msgId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);   //VEDI DISPENSE SE SI PUò FARE DI MEGLIO PER LA KEY
    if (msgId != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in createMsg #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return msgId;
}

// inizio funzioni per A <-> B
int sendAB (int msgId, pid_t pidDestinatario, pid_t pidMittente, unsigned long genesMittente, int accept) {
    struct msgbufAB messaggio;
    messaggio.mtype = pidDestinatario;
    messaggio.pidMittente = pidMittente;
    messaggio.genesMittente = genesMittente;
    messaggio.accept = accept;

    size_t data_size = msgsnd(msgId, &messaggio, sizeof(messaggio)- sizeof(long), 0);
    if (data_size != -1) {
        return 0;
    } else {
        fprintf(stderr, "%s: %d. Errore in sendAB #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}

int receiveAB (int msgId, pid_t mioPid, pid_t *pidMittente, unsigned long *genesMittente, int *accept, int flag) {
    struct msgbufAB messaggio;
    size_t data_size = msgrcv(msgId, &messaggio, sizeof(messaggio)-sizeof(long), (long int) mioPid, flag);
    if (data_size!=-1) {
        *pidMittente = messaggio.pidMittente;
        *genesMittente = messaggio.genesMittente;
        *accept = messaggio.accept;
        return 0;
    } else {
        if (errno == ENOMSG) { //non ci sono messaggi
            return 1;
        }
        fprintf(stderr, "%s: %d. Errore in receiveAB #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}
// fine funzioni per A <-> B


// inizio funzioni per G -> A,B
int sendGtoAB (int msgId, pid_t pidDestinatario, int controllato, int st) {
    struct msgbufG_AB messaggio;
    messaggio.mtype = pidDestinatario;
    messaggio.controllato = controllato;
    messaggio.st = st;

    size_t data_size = msgsnd(msgId, &messaggio, sizeof(messaggio)- sizeof(long), 0);
    if (data_size != -1) {
        return 0;
    } else {
        fprintf(stderr, "%s: %d. Errore in sendG #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}

int receiveABfromG (int msgId, pid_t mioPid, int *controllato, int *st, int flag) {
    struct msgbufG_AB messaggio;
    size_t data_size = msgrcv(msgId, &messaggio, sizeof(messaggio)-sizeof(long), (long int) mioPid, flag);
    if (data_size!=-1) {
        *controllato = messaggio.controllato;
        *st = messaggio.st;
        return 0;
    } else {
        if (errno == ENOMSG) { //non ci sono messaggi
            return 1;
        }
        fprintf(stderr, "%s: %d. Errore in receiveG #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}
// fine funzioni per G -> A,B


// inizio funzioni per A,B -> G
int sendABtoG (int msgId, pid_t pidMittente, pid_t pidSelected) {
    struct msgbufAB_G messaggio;
    messaggio.mtype = pidMittente;
    messaggio.pidMittente = pidMittente;
    messaggio.pidSelected = pidSelected;

    size_t data_size = msgsnd(msgId, &messaggio, sizeof(messaggio)- sizeof(long), 0);
    if (data_size != -1) {
        return 0;
    } else {
        fprintf(stderr, "%s: %d. Errore in sendG #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}

int receiveGfromAB (int msgId, pid_t mtype, pid_t *pidMittente, pid_t *pidSelected, int flag) {
    struct msgbufAB_G messaggio;
    size_t data_size = msgrcv(msgId, &messaggio, sizeof(messaggio)-sizeof(long), (long int) mtype, flag);
    if (data_size!=-1) {
        *pidMittente = messaggio.pidMittente;
        *pidSelected = messaggio.pidSelected;
        return 0;
    } else {
        if (errno == ENOMSG) { //non ci sono messaggi
            return 1;
        }
        fprintf(stderr, "%s: %d. Errore in receiveG #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
}
// fine funzioni per A,B -> G
int removeMsg(int msgId) {
    int res = msgctl(msgId, IPC_RMID, NULL);
    if (res != -1) {
    } else {
        fprintf(stderr, "%s: %d. Errore in removeMsg #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}