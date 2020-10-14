// Realizzato da Taiello R. e Pellegrino M.

#ifndef PROJECTPT_MSGMSKA_H
#define PROJECTPT_MSGMSKA_H
#include "../../../../../Utilities/MsgQueue/msg_utilities.h"

// inizio maschere per A <-> B
int sendAtoB (int msgId, pid_t mioPid, pid_t pidDestinatarioB, int accettato);
int receiveAfromB (int msgId, pid_t mioPid, pid_t *pidMittenteB, unsigned long *genesMittenteB, int flag);
// fine maschere per A <-> B
#endif //PROJECTPT_MSGMSKA_H
