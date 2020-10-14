// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include "../../../Utilities/SharedMem/shm_utilities.h"
#include "../../../Utilities/Semaphore/sem_utilities.h"
#include "../../../Utilities/MsgQueue/msg_utilities.h"
#include "../../../Utilities/ReaderSync/reader_utilities.h"
#include "Utilities_A/MsgMsk/msgMskA.h"
#include "../../../Utilities/Math/math_utilities.h"

int init_genes;
int number_people;
pid_t pidB; // pid del B che contatto
unsigned long genesMittenteB;
int controllato; // (1=gestore approva coppia, 0 altrimenti)
int st; // (1=sim_time, 0 altrimenti)
int scattato;   // flag per birth_date
unsigned long myGenes;
unsigned long myTarget;
pid_t *contattati;    // array pei PID (B) che mi hanno già contattato

personStruct myPerson;
personStruct *people;
int *numLettori;
int semSMId;    // ID semafori mutua sclusione

// inizio variabili ID code di messaggi
int msgIdAtoB;    // ID coda di messaggi A <-> B
int msgIdBtoA;    // ID coda di messaggi A <-> B
int msgIdAB_G;  // ID coda di messaggi A,B -> G
int msgIdG_AB;  // ID coda di messaggi G -> A,B
// fine variabili ID code di messaggi

struct sigaction sa;
sigset_t stop_mask,old_mask;

// inizio dichiarazione prototipi
void sigHanlderA(int signo);
void terminateProcess();
void searchPersonB();
// fine dichiarazione prototipi

void sigHandlerA(int signo){
    subSem(semSMId, SCRITTORE); // inizio scrittore
    people[myPerson.myIndex].scelto = 1;
    addSem(semSMId, SCRITTORE); // fine scrittore
    printf("Sono A con PID %d , devo morire di birth_date\n",getpid());
    scattato = 1;
    terminateProcess();
}

int main(int argc, char *argv[]) {
    // inizio inizializzione struct sigaction
    sa.sa_handler = (void (*)(int)) sigHandlerA;
    sa.sa_flags =  SA_NODEFER;  // flag che blocca altri segnali nel caso ci si trovi nel sigHandler
    sigemptyset(&stop_mask);    // pulisce maschera dei segnali, nessuno verrà bloccato
    sigaddset(&stop_mask, SIGALRM); // aggiunge SIGALRM alla maschera dei segnali da bloccare
    sigemptyset(&sa.sa_mask);   // pulisce machera segnali

    sigaction(SIGALRM, &sa, NULL);  // setto SIGALRM all'handler
    // fine inizializzione struct sigaction

    // inizio variabili execve
    int shmid = atoi(argv[1]);
    number_people = atoi(argv[2]);
    semSMId = atoi(argv[3]);
    int shmdNumLettoriId = atoi(argv[4]);
    msgIdAtoB = atoi(argv[5]);
    msgIdBtoA = atoi(argv[6]);
    msgIdAB_G = atoi(argv[7]);
    msgIdG_AB = atoi(argv[8]);
    init_genes = atoi(argv[9]);
    // fine variabili execve

    numLettori = (int *)attachesSeg(shmdNumLettoriId);
    people = (personStruct *) attachesSeg(shmid);


    contattati=(pid_t *)malloc(number_people*sizeof(pid_t)); // dò come dimensione dell'array il n di persone
    int i=0;
    for (i=0; i<number_people; i++){
        contattati[i]=0;    // inizializzo l'array di contattati
    }

    scattato = 0;
    st=0;
    controllato=0;

    sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
    pReader(semSMId, numLettori);   // inizio lettore
    myPerson = getPersonStruct(getpid(), people, number_people);
    vReader(semSMId, numLettori);   // fine lettore
    sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);

    myGenes = myPerson.genes;
    myTarget = myGenes; // inizializzo target al mio gene

    searchPersonB();
}


void searchPersonB() {
    sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
    subSem(semSMId, SCRITTORE); // inizio scrittore
    people[myPerson.myIndex].scelto=0;
    addSem(semSMId, SCRITTORE); // fine scrittore
    sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);
    int acceptB = 0;    // se il B contattato mi ha accettato
    pidB = 0;
    int res=1;
    int msgB = 1;
    while (!acceptB && res!=0) {
        do {
            res = receiveABfromG(msgIdG_AB, getpid(), &controllato, &st, IPC_NOWAIT);
            msgB = receiveAfromB(msgIdBtoA, getpid(), &pidB, &genesMittenteB, IPC_NOWAIT);
        } while (res ==1 && msgB == 1);
        if (res!=0){    // se il gestore non mi ha scritto
            int j=0;
            int devoAbbassare=0;
            while (j<number_people && !devoAbbassare){
                sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
                pReader(semSMId, numLettori);
                if (people[j].tipo=='B' && people[j].myPid==contattati[j]){
                    devoAbbassare=1;
                }
                vReader(semSMId, numLettori);   // fine lettore
                sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);
                j++;
            }

            if(devoAbbassare && myTarget>=2){
                myTarget=myTarget+1;
            }

            unsigned long mcd = getMCD(genesMittenteB, myGenes);
            int differenza = myTarget - genesMittenteB;
            if (differenza < 0) differenza = differenza * (-1);    // faccio valore assoluto
            if (mcd == myGenes || ((unsigned long) differenza <= mcd <= (init_genes + 2))) {
                acceptB = 1;
                sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
                subSem(semSMId, SCRITTORE); // inizio scrittore
                people[myPerson.myIndex].scelto=1;
                addSem(semSMId, SCRITTORE); // fine scrittore
                sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);
                printf("Sono A %d e ho accettato il B %d\n", getpid(), pidB);
                sendAtoB(msgIdAtoB, getpid(), pidB, 1);   // accetto il B
            } else {
                acceptB = 0;
                sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
                pReader(semSMId, numLettori);   // inizio lettore
                personStruct personContacted = getPersonStruct(pidB, people, number_people);
                vReader(semSMId, numLettori);   // fine lettore
                sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);
                contattati[personContacted.myIndex] = personContacted.myPid;  // mi salvo il pid di chi ho rifiutato
                printf("Sono A %d e ho rifiutato il B %d\n", getpid(), pidB);
                sendAtoB(msgIdAtoB, getpid(), pidB, 0);   // rifiuto il B
            }

        }
    }

    if(pidB!=0) {
        sendABtoG(msgIdAB_G, getpid(), pidB);   // dico al gestore con chi vorrei accoppiarmi
        terminateProcess();
    } else{
        while (receiveAfromB(msgIdBtoA, getpid(), &pidB, &genesMittenteB, IPC_NOWAIT)==0){
            sendAtoB(msgIdAtoB, getpid(), pidB, 0); // rifiuto e libero tutti gli A in attesa di una mia risposta
        }
        printf("Sono il A %d e ora muoio\n", getpid());
        detachesSeg(numLettori);
        detachesSeg(people);
        exit(EXIT_SUCCESS);
    }
}

void terminateProcess() {
    if (!scattato && !st) {
        receiveABfromG(msgIdG_AB, getpid(), &controllato, &st, 0);
        if (st!=1){
            if (controllato==0) {
               printf("Sono il A %d e mi è morto il B con cui volevo accoppiarmi\n", getpid());
                while (receiveAfromB(msgIdBtoA, getpid(), &pidB, &genesMittenteB, IPC_NOWAIT)==0){
                    sendAtoB(msgIdAtoB, getpid(), pidB, 0); // rifiuto e libero tutti gli A in attesa di una mia risposta
                }
                searchPersonB();
            }
        } else {

        }
    }
    if (st) {
        printf("Sono il A %d e devo terminare per fine programma\n", getpid());
    }
    while (receiveAfromB(msgIdBtoA, getpid(), &pidB, &genesMittenteB, IPC_NOWAIT)==0){
        sendAtoB(msgIdAtoB, getpid(), pidB, 0); // rifiuto e libero tutti gli A in attesa di una mia risposta
    }
    printf("Sono il A %d e ora muoio\n", getpid());
    free (contattati);
    detachesSeg(numLettori);
    detachesSeg(people);
    exit(EXIT_SUCCESS);
}