// Realizzato da Taiello R. e Pellegrino M.

#ifndef PROJECTPT_SHM_UTILITIES_H
#define PROJECTPT_SHM_UTILITIES_H
int createShm(int size);
void * attachesSeg(int shmid);
int detachesSeg(void *seg);
int removeShm(int shmid);
unsigned long nAttaches(int shmid);
int nByteShm(int shmid);
#endif //PROJECTPT_SHM_UTILITIES_H
