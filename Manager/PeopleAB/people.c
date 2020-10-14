// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include "people.h"

personStruct getPersonStruct(pid_t thisPid, personStruct *p, int length){
  int i = 0;
  int flag = 0;
  while(i<length&&!(flag)){
    flag = p[i].myPid == thisPid;
    //  printf("pid %d del processo %d\n",p[i].myPid,i);
    if(!flag){
      i++;
    }
  }
  return p[i];
}
int existPerson(pid_t thisPid,personStruct *p,int length){
  int i = 0;
  int flag = 0;
  while(i<length&&!(flag)){
    flag = p[i].myPid == thisPid;
    if(!flag){
      i++;
    }
  }
  return flag;
}