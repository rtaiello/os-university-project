// Realizzato da Taiello R. e Pellegrino M.
#ifndef PROJECTPT_MSG_UTILITIES_H
#define PROJECTPT_MSG_UTILITIES_H

#include "../../Manager/PeopleAB/people.h"

struct msgbufAB {
    long int mtype;                 //pid destinatario
    pid_t pidMittente;              //pid mittente (chi invia)
    unsigned long genesMittente;    //genes del mittente
    int accept;                     //(1=accettato, 0=rifiutato)
};

struct msgbufG_AB {
    long int mtype;     //pid destinatario
    int controllato;    //(1=ok, 0=non ok)
    int st;             //(1=sim_time finito, 0=continuare)
};

struct msgbufAB_G {
    long int mtype;     //pid mittente (come flag)
    pid_t pidMittente;  //pid mittente
    pid_t pidSelected;  //pid con chi mi accoppio
};


// inizio funzioni per A <-> B
int sendAB (int msgId, pid_t pidDestinatario, pid_t pidMittente, unsigned long genesMittente, int accept);
int receiveAB (int msgId, pid_t mioPid, pid_t *pidMittente, unsigned long *genesMittente, int *accept, int flag);
// fine funzioni per A <-> B


// inizio funzioni per G -> A,B
int sendGtoAB (int msgId, pid_t pidDestinatario, int controllato, int st);
int receiveABfromG (int msgId, pid_t mioPid, int *controllato, int *st, int flag);
// fine funzioni per G -> A,B


// inizio funzioni per A,B -> G
int sendABtoG (int msgId, pid_t pidMittente, pid_t pidSelected);
int receiveGfromAB (int msgId, pid_t mtype, pid_t *pidMittente, pid_t *pidSelected, int flag);
// fine funzioni per A,B -> G


int createMsg ();

int removeMsg (int msgId);
#endif //PROJECTPT_MSG_UTILITIES_H