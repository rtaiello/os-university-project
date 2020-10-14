#include <stdio.h>
#include <sys/ipc.h>
#include "msgMskB.h"


// inizio maschere per A <-> B
int sendBtoA (int msgId, pid_t mioPid, pid_t pidDestinatarioA, unsigned long mioGenes) {
    return sendAB(msgId, pidDestinatarioA, mioPid, mioGenes, 0);
}
int receiveBfromA (int msgId, pid_t mioPid, pid_t *pidMittenteA, int *accept) {
    unsigned long genesMittente;
    return receiveAB(msgId, mioPid, pidMittenteA, &genesMittente, accept, IPC_NOWAIT);
}
// fine maschere per A <-> B