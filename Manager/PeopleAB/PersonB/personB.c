// Realizzato da Taiello R. e Pellegrino M.

#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/stat.h>
#include "../../../Utilities/SharedMem/shm_utilities.h"
#include "../../../Utilities/Semaphore/sem_utilities.h"
#include "../../../Utilities/ReaderSync/reader_utilities.h"
#include "Utilities_B/MsgMsk/msgMskB.h"

// dichiarazione varaibili globali
int scattato;   // flag per birth_date
int controllato;    // flag dal gestore per sapere se posso terminare tranquillamente
int st; // flag daò gestore se sim_time è scattato
int number_people;
personStruct myPerson;

struct sigaction sa;
sigset_t stop_mask, old_mask;
int *numLettori;
personStruct *people;
unsigned long myGenes;

// inizio variabili ID memorie condivise
int shmid;  // ID memoria condivisa people
int shmdNumLettoriId;   // ID memoria condivisa numLettori
// fine variabili ID memorie condivise

// inizio variabili ID semafori
int semSMId;    // ID semafori mutua sclusione
int semWaitB;   // ID semaforo di prelazione da parte del gestore
// fine variabili ID semafori

// inizio variabili ID code di messaggi
int msgIdAtoB;    // ID coda di messaggi A <-> B
int msgIdBtoA;    // ID coda di messaggi A <-> B
int msgIdAB_G;  // ID coda di messaggi A,B -> G
int msgIdG_AB;  // ID coda di messaggi G -> A,B
// fine variabili ID code di messaggi

// inizio dichiarazione prototipi
void sigHandlerB(int signo);
void terminateProcess();
void searchPersonA();
//fine dichiarazione prototipi

void sigHandlerB(int signo) {
    printf("Sono B con PID %d , devo morire di birth_date\n", getpid());
    scattato = 1;
    terminateProcess();
}

int main(int argc, char *argv[]) {
    // inizio inizializzione struct sigaction
    sa.sa_handler = (void (*)(int)) sigHandlerB;
    sa.sa_flags = SA_NODEFER;   // flag che blocca altri segnali nel caso ci si trovi nel sigHandler
    sigemptyset(&stop_mask); // pulisce maschera dei segnali, nessuno verrà bloccato
    sigaddset(&stop_mask, SIGALRM); // aggiunge SIGALRM alla maschera dei segnali da bloccare
    sigemptyset(&sa.sa_mask);   // pulisce machera segnali

    sigaction(SIGALRM, &sa, NULL);  // setto SIGALRM all'handler
    //fine inizializzazione struct sigaction

    // inizio variabili execve
    int shmid = atoi(argv[1]);
    number_people = atoi(argv[2]);
    semSMId = atoi(argv[3]);
    int shmdNumLettoriId = atoi(argv[4]);
    msgIdAtoB = atoi(argv[5]);
    msgIdBtoA = atoi(argv[6]);
    msgIdAB_G = atoi(argv[7]);
    msgIdG_AB = atoi(argv[8]);
    semWaitB = atoi(argv[9]);
    // fine variabili execve

    numLettori = (int *) attachesSeg(shmdNumLettoriId); // attaco memoria condivisa numLettori
    people = (personStruct *) attachesSeg(shmid);  // attacco memoria condivisa people

    scattato = 0;
    st=0;

    sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
    pReader(semSMId, numLettori);   // inizio lettore
    myPerson = getPersonStruct(getpid(), people, number_people);
    myGenes = myPerson.genes;
    vReader(semSMId, numLettori);   // fine lettore
    sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);

    searchPersonA();
}

void searchPersonA() {
    int acceptByA = 0;  // flag se A mi ha accettato
    pid_t pidA = 0; // per il while, per sapere se G mi ha contatto (causa=sim_time)

    while (st != 1 && !acceptByA) {
        int found = 0; // flag per sapere se ne ho trovato uno da contattare
        unsigned long genesMax = 1; // gene massimo
        pid_t pidGenesMax = 0; // pid di quello da contattare

        int i=0;
        waitSem(semWaitB,0);
        for (i = 0; i < number_people; i++) {
            sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
            pReader(semSMId, numLettori);         // inizio lettore
            if ((people[i].tipo == 'A') && (people[i].scelto==0) && (people[i].genes > genesMax)) {
                genesMax = people[i].genes;
                pidGenesMax = people[i].myPid;
                found = 1;
            }
            vReader(semSMId, numLettori);   // fine lettore
            sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);
        }

        if (found) {    // se ho trovato un A che sembra ok
            sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
            pReader(semSMId, numLettori);   // inizio lettore
            int existpidGenesMax = existPerson(pidGenesMax,people,number_people);   // flag se la persona A contattata esiste in people
            personStruct personA = getPersonStruct(pidGenesMax, people, number_people);
            vReader(semSMId, numLettori);   // fine lettore
            sigprocmask(SIG_UNBLOCK, &stop_mask, &old_mask);

            if (existpidGenesMax && personA.tipo=='A') {
                sendBtoA(msgIdBtoA, getpid(), pidGenesMax, myGenes);    // scrivo al A
                int res = 1;    // flag per la receive dal gestore
                do {
                    receiveABfromG(msgIdG_AB, getpid(), &controllato, &st, IPC_NOWAIT);
                    res = receiveBfromA(msgIdAtoB, getpid(), &pidA, &acceptByA);
                } while (res && !st);
            }
        }
        if (!st) {
            receiveABfromG(msgIdG_AB, getpid(), &controllato, &st, IPC_NOWAIT);
        }
    }
    if (acceptByA) {
        sendABtoG(msgIdAB_G, getpid(), pidA);   // dico al gestore con chi vorrei accoppiarmi
    }
    terminateProcess();
}

void terminateProcess() {
    if (!scattato && !st) {
        receiveABfromG(msgIdG_AB, getpid(), &controllato, &st, 0);
        if (st!=1){
            if (controllato==0) {
                printf("Sono il B %d e mi è morto l'A con cui volevo accoppiarmi\n", getpid());
                searchPersonA();
            }
        } else {
            printf("Sono il B %d e devo terminare per fine programma\n", getpid());
        }
    }
    if (st) {
        printf("Sono il B %d e devo terminare per fine programma\n", getpid());
    }
    printf("Sono il B %d e ora muoio\n", getpid());
    detachesSeg(people); //sgancio la SM al processo figlio i-esimo
    detachesSeg(numLettori);
    exit(EXIT_SUCCESS);
}