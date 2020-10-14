// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include <sys/ipc.h>
#include "msgMskA.h"

// inizio maschere per A <-> B
int sendAtoB (int msgId, pid_t mioPid, pid_t pidDestinatarioB, int accettato) {
    return sendAB(msgId, pidDestinatarioB, mioPid, 0, accettato);
}
int receiveAfromB (int msgId, pid_t mioPid, pid_t *pidMittenteB, unsigned long *genesMittenteB, int flag) {
    int accept;
    return receiveAB(msgId, mioPid, pidMittenteB, genesMittenteB, &accept, flag); //non-bloccante
}
// fine maschere per A <-> B