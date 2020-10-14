
#ifndef PROJECTPT_MSGMSKB_H
#define PROJECTPT_MSGMSKB_H

#include "../../../../../Utilities/MsgQueue/msg_utilities.h"
int sendBtoA (int msgId, pid_t mioPid, pid_t pidDestinatarioA, unsigned long mioGenes);
int receiveBfromA (int msgId, pid_t mioPid, pid_t *pidMittenteA, int *accept);
#endif //PROJECTPT_MSGMSKB_H
