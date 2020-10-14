// Realizzato da Taiello R. e Pellegrino M.

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <memory.h>
#include <time.h>
#include <sys/msg.h>
#include <limits.h>
#include <sys/wait.h>

#include "manager.h"
#include "../PeopleAB/people.h"
#include "../../Utilities/Semaphore/sem_utilities.h" //libreria sui semafori
#include "../../Utilities/SharedMem/shm_utilities.h" // libreria sulla SM
#include "../../Utilities/ReaderSync/reader_utilities.h"
#include "../../Utilities/Math/math_utilities.h"
#include "../../Utilities/MsgQueue/msg_utilities.h"
#include "../../Utilities/Random/generateRandom.h"

// inizio variabili statistiche
int numA;
int numB;
personStruct personNameMax;
personStruct personGenesMax;
int numCoppie;
int bd;
// fine variabili statistiche

// inizio variabili timer
time_t start;
time_t tmpTime;
// fine variabili timer

// inizio variabili di input
int number_people;
unsigned long init_genes;
int sim_time;
int birth_date;
// fine variabili di input

// inizio variabili ID memorie condivise
int shmid;              // memorioa condivisa per le person
int shmdNumLettoriId;   // memoria condivisa per numLettori
// fine variabili ID memorie condivise

// inizio variabili ID semafori
int semSMId;    // semaforo lettori/scrittori
int semWaitSingleInsertion;   // semaforo per aspettare la creazione del nuovo figlio (insertPerson())
int semWaitAllProcId; // semaforo per aspettare tutti i figli nella createPeople()
int semPrintWait;   // semaforo per la print della tabella iniziale (sleep)
int semWaitB;
// fine variabili ID semafori

// inizio variabili ID code di messaggi
int msgIdAtoB;    // ID coda di messaggi A <-> B
int msgIdBtoA;    // ID coda di messaggi A <-> B
int msgIdAB_G;  // ID coda di messaggi A,B -> G
int msgIdG_AB;  // ID coda di messaggi G -> A,B
// fine variabili ID code di messaggi

int status;
int nameEnd;    // flag se si è raggiunto il numero massimo di caratteri nel nome (NAME_LENGTH)

// inizio dichiarazione prototipi
void waitAllChild();

void insertTwoChild(pid_t pidIndividuo1, pid_t pidIndividuo2);

void execChild(char type);

void newInitPerson(personStruct *p, int index, pid_t pid, unsigned long genesOld); // crea una nuova persona

personStruct *checkType(personStruct *p, int index);

int getPidDeath(personStruct *p);

int getIndexPeople(pid_t thisPid, personStruct *p);

void printTableStatus(personStruct *people);

void createChildValues(personStruct *people, int index, pid_t pid, unsigned long x, char *startName);
// fine dichiarazione prototipi

void createPeople(int i_pe, unsigned long g, int s_ti, int b_da) {
    // inizio inizializzazione valori input
    number_people = i_pe; // inizializzo a init_people
    init_genes = g;
    sim_time = s_ti;
    birth_date = b_da;
    // fine inizializzazione valori input

    //inizio inizializzazione variabili statistiche
    numA = 0;
    numB = 0;
    strcpy(personNameMax.name, "");
    personGenesMax.genes=1;
    numCoppie = 0;
    //fine inziazilizzazione variabili statistiche

    nameEnd=0;

    // inizio inizializzazione semaforo semWaitB
    semWaitB = createSemSet(1);
    setSem(semWaitB,0,0);
    // fine inizializzazione semaforo semWaitB

    // inizio inizializzazione semaforo semWaitAllProcId
    semWaitAllProcId = createSemSet(2);
    setSem(semWaitAllProcId, 0, (number_people));
    setSem(semWaitAllProcId, 1, (number_people));
    semWaitSingleInsertion = createSemSet(3);
    setSem(semWaitSingleInsertion, 0, 1);
    // fine inizializzazione semaforo semWaitAllProcId

    // inizio inizializzazione semSMId (SEMAFORO DI MUTUA ESCLUSIONE)
    semSMId = createSemSet(2);
    setSem(semSMId, LETTORE, 1);
    setSem(semSMId, SCRITTORE, 1);
    // fine inizializzazione semSMId

    // inizio  inizializzazione semaforo semPrintWait (per far partire tutti i processi insieme dopo la print della tabella)
    semPrintWait = createSemSet(1);
    setSem(semPrintWait, 0, 1);
    // fine inizializzazione semaforo semPrintWait

    // inizio creazione memoria condivisa people
    shmid = createShm((sizeof(personStruct) * number_people));  // ottengo ID
    personStruct *people = (personStruct *) attachesSeg(shmid);
    // fine creazione memoria condivisa people

    // inizio creazione e inizializzazione memoria condivisa numLettori
    shmdNumLettoriId = createShm(sizeof(int));  // ottengo ID
    int *numLettori = (int *) attachesSeg(shmdNumLettoriId);
    *numLettori = 0;
    // fine creazione e inizializzazione memoria condivisa numLettori

    // inizio inizializzazione code di messaggi
    msgIdAtoB = createMsg();
    msgIdBtoA = createMsg();
    msgIdAB_G = createMsg();
    msgIdG_AB = createMsg();
    // fine inizializzazione code di messaggi

    int i=0;
    setSem(semWaitSingleInsertion, 2, (number_people-1));
    for (i = 0; i < number_people; i++) {
        switch (fork()) {
            case -1:
                printf("fork fallita\n");
                exit(EXIT_FAILURE);
            case 0: {
                subSem(semSMId, SCRITTORE); // inizio scrittore
                newInitPerson(people, i, getpid(), init_genes);// creo l'individuo
                addSem(semSMId, SCRITTORE); // inizio scrittore

                if (i == (number_people - 1)) {
                    waitSem(semWaitSingleInsertion, 2);
                    people = checkType(people,i); //controllo che la popolazione non sia costituita interamente da A o B (solo alla fine)
                } else {
                    subSem(semWaitSingleInsertion, 2);
                }

                pReader(semSMId, numLettori);   // inizio lettore
                char type = people[i].tipo; // mi serve per lo switch della execve
                vReader(semSMId, numLettori);   // fine lettore

                subSem(semWaitAllProcId, 1);    //sottraggo -1 al semaforo e quindi libero il padre alla fine quando raggiunge 0
                subSem(semWaitAllProcId, 0);

                waitSem(semWaitAllProcId, 0);   // l'individuo aspetta la creazione di tutti gli altri prima di proseguire

                detachesSeg(numLettori);
                detachesSeg(people);

                waitSem(semPrintWait, 0); // aspetto la tabella del gestore

                execChild(type);    // funzione per la execve
                exit(EXIT_FAILURE);
            }
            default :
                break;  // padre continua sotto
        }
    }
    waitSem(semWaitAllProcId, 1); // metto in attesa il gestore finche' tutti i figli sono stati creati con la fork()

    printf("Tabella della funzione createPeople\n");
    printTableStatus(people);

    int a=0;
    for (a=0; a < number_people; a++){
        pReader(semSMId, numLettori);   // inizio lettore
        switch (people[a].tipo) {
            case 'B' :
                numB++;
                break;
            default:
                numA++;
                break;
        }
        vReader(semSMId, numLettori);   // fine lettore
    }

    detachesSeg(numLettori);
    detachesSeg(people);
    sleep(2);

    subSem(semPrintWait, 0); // faccio partire tutti i processi
    printf("Si inizia!\n");
    start = time(NULL);
    tmpTime = start;
    waitAllChild();
    printf("ERRORE!");
    exit(EXIT_FAILURE);
}


void waitAllChild() {
    pid_t pidIndividuo1= 0;
    pid_t pidIndividuo2= 0;
    pid_t tmpPid1 = 0;  // per il controllo del gestore per evitare doppia send
    pid_t tmpPid2 = 0;  // per il controllo del gestore per evitare doppia send
    pid_t pidKill = 0;  // PID di chi muore per birth_date
    int scattato = 0;   // flag se birth_date è scattato
    while (((time(NULL) - start) <= sim_time)&& !nameEnd) {
        if (time(NULL) - tmpTime >= birth_date) {
            bd++;
            scattato = 1;
            tmpTime = time(NULL);
            printf("SCATTATO BIRTH_DATE \n");
            personStruct *people = (personStruct *) attachesSeg(shmid);
            int *numLettori = (int *) attachesSeg(shmdNumLettoriId);

            pReader(semSMId,numLettori);    // inizio lettore
            pidKill = getPidDeath(people);
            personStruct tmp = getPersonStruct(pidKill, people, number_people);
            int index = tmp.myIndex;
            vReader(semSMId, numLettori);   // fine lettore

            printf("la persona da uccidere ha pid = %d , nome = (%s) , tipo = %c, gene = %lu, index=%d\n", pidKill, tmp.name, tmp.tipo, tmp.genes, tmp.myIndex);
            kill(pidKill, SIGALRM);
            sendGtoAB(msgIdG_AB, pidKill, 0, 0);

            setSem(semWaitSingleInsertion, 0, 1);
            addSem(semWaitB,0); // alzo il semaforo per evitare starvation del gestore da parte dei B
            switch (fork()) {
                case -1:
                    exit(EXIT_FAILURE);
                case 0: {
                    subSem(semSMId, SCRITTORE); // inizio scrittore
                    newInitPerson(people, index, getpid(), init_genes); // creo la persona in people
                    addSem(semSMId, SCRITTORE); // inizio scrittore

                    int flag = 1;
                    int i = 0;
                    char tipo;

                    pReader(semSMId, numLettori);
                    while (flag && i < (number_people - 1)) {   // controlla che non ci siano tutti individui con lo stesso tipo
                        if (people[i].tipo != people[i + 1].tipo) {
                            flag = 0;
                        }
                        tipo = people[i].tipo;
                        i++;
                    }
                    vReader(semSMId, numLettori);

                    if (flag) {
                        switch (tipo) {
                            case 'A' :
                                subSem(semSMId, SCRITTORE); // inizio scrittore
                                people[index].tipo = 'B';
                                addSem(semSMId, SCRITTORE); // fine scrittore
                                printf("Modificato il tipo di %d da A a B\n", getpid());
                                break;
                            default :
                                subSem(semSMId, SCRITTORE); // inizio scrittore
                                people[index].tipo = 'A';
                                addSem(semSMId, SCRITTORE); // fine scrittore
                                printf("Modificato il tipo di %d da B a A\n", getpid());
                                break;
                        }
                    }

                    pReader(semSMId, numLettori);   // inizio lettore
                    char type = people[index].tipo; // per lo switch del execve
                    vReader(semSMId, numLettori);   // fine lettore
                    subSem(semWaitSingleInsertion, 0);   //LIBERA IL PADRE NELLA SUA ESECUZIONE ORA CHE HO COMPLETATO LA CREAZIONE
                    subSem(semWaitB,0); // abbasso il semaforo per evitare starvation del gestore da parte dei B
                    detachesSeg(numLettori);
                    detachesSeg(people);
                    execChild(type);    //execve
                    exit(EXIT_FAILURE);
                }
                default :
                    break;
            }
            waitSem(semWaitSingleInsertion, 0); // IL PADRE ATTENDE CHE IL FIGLIO COMPLETI IL SUO INSERIMENTO NELLA POPOLAZIONE
            waitpid(pidKill, &status, 0); //Rimuovo l'individuo colpito da birth_date
            printf("Tabella della funzione insertPerson\n");
            printTableStatus(people);

            pReader(semSMId, numLettori);   // inizio lettore
            switch (people[index].tipo) {
                case 'B' :
                    numB++;
                    break;
                default:
                    numA++;
                    break;
            }
            vReader(semSMId, numLettori);   // fine lettore

            detachesSeg(numLettori);
            detachesSeg(people);
        }

        if (receiveGfromAB(msgIdAB_G, 0,  &pidIndividuo1, &pidIndividuo2, IPC_NOWAIT) == 0) {
            printf("l'individuo con pid %d si vuole accopiare con l'individo %d\n", pidIndividuo1, pidIndividuo2);
            pid_t pidIndividuo3;
            pid_t pidIndividuo4;
            personStruct *people = (personStruct *) attachesSeg(shmid);
            int *numLettori = (int *) attachesSeg(shmdNumLettoriId);

            pReader(semSMId,numLettori);    // inizio lettore
            int okPid1 = existPerson(pidIndividuo1, people, number_people); // se la persona esiste in people
            int okPid2 = existPerson(pidIndividuo2, people, number_people); // se la persona esiste in people
            vReader(semSMId, numLettori);   // fine lettore

            detachesSeg(numLettori);
            detachesSeg(people);

            if (okPid1&&okPid2) {   // se entrambi i genitori esistono ancora in people
                receiveGfromAB(msgIdAB_G, pidIndividuo2, &pidIndividuo3, &pidIndividuo4, 0);
                if ((pidIndividuo1 == pidIndividuo4) && (pidIndividuo2 == pidIndividuo3)) {
                    //comunico a entrambi che possono morire tranquilli
                    sendGtoAB(msgIdG_AB, pidIndividuo1, 1, 0);
                    sendGtoAB(msgIdG_AB, pidIndividuo2, 1, 0);
                    waitpid(pidIndividuo1, &status, 0); // aspetto che sia veramente terminato
                    waitpid(pidIndividuo2, &status, 0);
                    insertTwoChild(pidIndividuo1, pidIndividuo2);
                    numCoppie++;
                }
            } else if (okPid2 && scattato && pidIndividuo1!=tmpPid2) {
                tmpPid1 = pidIndividuo1;
                tmpPid2 = pidIndividuo2;
                int m = sendGtoAB(msgIdG_AB, pidIndividuo2, 0, 0);
            } else if (okPid1 && scattato && pidIndividuo2!=tmpPid1) {
                tmpPid1 = pidIndividuo1;
                tmpPid2 = pidIndividuo2;
                int m = sendGtoAB(msgIdG_AB, pidIndividuo1, 0, 0);
            }
        }
    }

    // scattato sim_time o raggiunto numero massimo di caratteri
    if (nameEnd) {
        printf("raggiunto limite dei caratteri (%d) per il campo nome\n", NAME_LENGTH);
    } else {
        printf("sim_time finito\n");
    }

    personStruct *people = (personStruct *) attachesSeg(shmid); // attacco a people
    int *numLettori = (int *) attachesSeg(shmdNumLettoriId);
    pid_t pidTerminate;
    int a=0;
    for (a=0; a<number_people;a++){
        pReader(semSMId, numLettori);   // inizio lettore
        pidTerminate = people[a].myPid;
        vReader(semSMId, numLettori);   // inizio lettore
        sendGtoAB(msgIdG_AB, pidTerminate, 0, 1);
        waitpid(pidTerminate, &status, 0);
    }

    //finche non tutti figli sono terminati non proseguo con il resto del padre
    detachesSeg(people);
    detachesSeg(numLettori);

    // inizio rimozione strutture IPC
    removeShm(shmid);
    removeShm(shmdNumLettoriId);
    removeMsg(msgIdG_AB);
    removeMsg(msgIdAtoB);
    removeMsg(msgIdBtoA);
    removeMsg(msgIdAB_G);
    rmSem(semSMId);
    rmSem(semWaitSingleInsertion);
    rmSem(semWaitAllProcId);
    rmSem(semPrintWait);
    rmSem(semWaitB);
    // fine rimozione strutture IPC

    printf("SIMULAZIONE TERMINATA CON SUCCESSO\n");
    printf("------------------\n");
    printf("Durata totale: %d secondi \n", (int) (time(NULL) - start));
    printf("Numero di morti per birth_date: %d \n", bd);
    printf("Numero di individui vissuti di tipo A: %d\n", numA);
    printf("Numero di individui vissuti di tipo B: %d\n", numB);

    printf("Caratteristiche dell'individuo vissuto con nome più lungo:\n");
    printf("| PID: %d || TIPO: %c || NOME: (%s) || GENE: %lu |\n",
           personNameMax.myPid, personNameMax.tipo, personNameMax.name, personNameMax.genes);
    printf("Caratteristiche dell'individuo vissuto con gene maggiore:\n");
    printf("| PID: %d || TIPO: %c || NOME: (%s) || GENE: %lu |\n",
           personGenesMax.myPid, personGenesMax.tipo, personGenesMax.name, personGenesMax.genes);

    exit(EXIT_SUCCESS);
}

void insertTwoChild(pid_t pidIndividuo1, pid_t pidIndividuo2) {
    int *numLettori = (int *) attachesSeg(shmdNumLettoriId);
    personStruct *people = (personStruct *) attachesSeg(shmid);

    pReader(semSMId, numLettori);   // inizio lettore
    personStruct individuo1 = getPersonStruct(pidIndividuo1, people, number_people);
    personStruct individuo2 = getPersonStruct(pidIndividuo2, people, number_people);
    vReader(semSMId, numLettori);   // fine lettore

    if (strlen(individuo1.name)+1<NAME_LENGTH && strlen(individuo2.name)+1<NAME_LENGTH) {   // se il nome ha lunghezza accettabile
        int index[2] = {individuo1.myIndex, individuo2.myIndex};
        unsigned long newX = getMCD(individuo1.genes, individuo2.genes);

        setSem(semWaitSingleInsertion, 1, 2);
        setSem(semWaitSingleInsertion, 2, 1);
        addSem(semWaitB,0); // alzo il semaforo per evitare starvation del gestore da parte dei B
        int j=0;
        for (j = 0; j < 2; j++) {
            switch (fork()) {
                case -1:
                    exit(EXIT_FAILURE);
                case 0: {
                    if (j==0){
                        createChildValues(people, index[j], getpid(), newX, individuo1.name);
                        subSem(semWaitSingleInsertion, 2);
                    } else {
                        waitSem(semWaitSingleInsertion, 2);
                        createChildValues(people, index[j], getpid(), newX, individuo2.name);
                        people = checkType(people, index[j]);     //controllo che la popolazione non sia costituita interamente da A o B (solo il secondo figlio creato)
                    }

                    pReader(semSMId, numLettori);   // inizio lettore
                    personStruct myPerson = getPersonStruct(getpid(), people, number_people);
                    char type = myPerson.tipo;
                    vReader(semSMId, numLettori);   // fine lettore
                    detachesSeg(numLettori);
                    detachesSeg(people);
                    subSem(semWaitSingleInsertion, 1);   // libero il gestore
                    execChild(type);    //execve
                    exit(EXIT_FAILURE);
                }
                default :
                    break;
            }
        }
        waitSem(semWaitSingleInsertion, 1); // il gestore aspetta la creazione dela nuova coppia
        subSem(semWaitB,0); // abbasso il semaforo per evitare starvation del gestore da parte dei B
        printTableStatus(people);

        int a=0;
        for (a=0; a < 2; a++){
            pReader(semSMId, numLettori);   // inizio lettore
            switch (people[index[a]].tipo) {
                case 'B' :
                    numB++;
                    break;
                default:
                    numA++;
                    break;
            }
            vReader(semSMId, numLettori);   // fine lettore
        }

        detachesSeg(people);
        detachesSeg(numLettori);
    } else {
        nameEnd=1;
    }
}

void execChild(char type) {
    char shmidChar[10]; // ID memoria condivisa people
    sprintf(shmidChar, "%d", shmid);
    char number_peopleChar[10]; // number_people
    sprintf(number_peopleChar, "%d", number_people);
    char semSMIdChar[10]; // semSMId
    sprintf(semSMIdChar, "%d", semSMId);
    char semWaitBChar[10]; // semWaitB
    sprintf(semWaitBChar, "%d", semWaitB);
    char msgIdAtoBChar[10]; // msgIdAtoB
    sprintf(msgIdAtoBChar, "%d", msgIdAtoB);
    char msgIdBtoAChar[10]; // msgIdBtoA
    sprintf(msgIdBtoAChar, "%d", msgIdBtoA);
    char shmdNumLettoriIdChar[10]; // ID memoria condivisa numLettori
    sprintf(shmdNumLettoriIdChar, "%d", shmdNumLettoriId);
    char msgIdG_ABChar[10]; // msgIdG_AB
    sprintf(msgIdG_ABChar, "%d", msgIdG_AB);
    char msgIdAB_GChar[10]; // msgIdAB_G
    sprintf(msgIdAB_GChar, "%d", msgIdAB_G);
    char init_genesChar[10]; // init_genes
    sprintf(init_genesChar, "%lu", init_genes);

    switch (type) {
        case 'A' :
            ;
            char *args1[] = {
                    "/main_exec_personA",
                    shmidChar,
                    number_peopleChar,
                    semSMIdChar,
                    shmdNumLettoriIdChar,
                    msgIdAtoBChar,
                    msgIdBtoAChar,
                    msgIdAB_GChar,
                    msgIdG_ABChar,
                    init_genesChar,
                    NULL
            };
            execve("./main_exec_personA", args1, NULL);
            fprintf(stderr, "%s: %d. Error #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
            exit(EXIT_FAILURE);
        default:
            ;
            char *args2[] = {
                    "/main_exec_personB",
                    shmidChar,
                    number_peopleChar,
                    semSMIdChar,
                    shmdNumLettoriIdChar,
                    msgIdAtoBChar,
                    msgIdBtoAChar,
                    msgIdAB_GChar,
                    msgIdG_ABChar,
                    semWaitBChar,
                    NULL
            };
            execve("./main_exec_personB", args2, NULL);
            fprintf(stderr, "%s: %d. Error #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
            exit(EXIT_FAILURE);
    }
}

void newInitPerson(personStruct *people, int index, pid_t pid, unsigned long genesOld) {
    people[index].myIndex = index;
    people[index].myPid = pid;
    people[index].tipo = generateType(pid); //chiamo random
    strcpy(people[index].name, generateName(pid)); //chiamo random
    people[index].genes = generateGenes(genesOld, pid);//chiamo random
    people[index].scelto = 0;
}

personStruct * checkType(personStruct *p, int index) { //controllo che la popolazione non sia formata interamente da A oppure tutti B
    int flag = 1;
    int i = 0;
    char tipo;
    int *numLettori;
    numLettori = (int *) attachesSeg(shmdNumLettoriId);
    pReader(semSMId, numLettori);   // inizio lettore
    while (flag && i < (number_people - 1)) {
        if (p[i].tipo != p[i + 1].tipo) {
            flag = 0;
        }
        tipo = p[i].tipo;
        i++;
    }
    vReader(semSMId, numLettori);   // fine lettore
    detachesSeg(numLettori);
    if (flag) {
        switch (tipo) {
            case 'A' :
                subSem(semSMId, SCRITTORE); // inizio scrittore
                p[index].tipo = 'B';
                addSem(semSMId, SCRITTORE); // fine scrittore
                printf("Modificato il tipo di %d da A a B\n", getpid());
                break;
            default :
                subSem(semSMId, SCRITTORE); // inizio scrittore
                p[index].tipo = 'A';
                addSem(semSMId, SCRITTORE); // fine scrittore
                printf("Modificato il tipo di %d da B a A\n", getpid());
                break;
        }
    }
    return p;
}

int getPidDeath(personStruct *p) {  // restituisco il pid di chi uccidere
    int minPid = 0;
    unsigned long minGenes = ULONG_MAX;
    int i = 0;
    while (i < number_people) {
        if (p[i].genes < minGenes) {
            minPid = p[i].myPid;
            minGenes = p[i].genes;
        }
        if (p[i].genes == 2) {
            return p[i].myPid;
        }
        i++;
    }
    return minPid;
}

void printTableStatus(personStruct *people) {
    int *numLettori = (int *) attachesSeg(shmdNumLettoriId);
    printf("-----------------------------------------------------------------------\n");
    pReader(semSMId, numLettori);   // inizio lettore
    int j=0;
    for (j=0; j < number_people; j++) {
                printf("| INDICE: %d || PID: %d || TIPO: %c || NOME: (%s) || GENE: %lu |\n", people[j].myIndex,
                       people[j].myPid, people[j].tipo, people[j].name, people[j].genes);

                if (people[j].genes > personGenesMax.genes) personGenesMax = people[j]; // nel caso aggiorno quello con il gene maggiore
                if (strlen(people[j].name) > strlen(personNameMax.name)) personNameMax = people[j]; // nel caso aggiorno quello con il nome più lungo
    }
    vReader(semSMId, numLettori); // fine lettore
    printf("-----------------------------------------------------------------------\n");
    detachesSeg(numLettori);
}

void createChildValues(personStruct *people, int index, pid_t pid, unsigned long x, char *startName) {
    subSem(semSMId, SCRITTORE); // inizio scrittore
    people[index].myPid = pid;
    people[index].tipo = generateType(pid);
    char nameTmp[NAME_LENGTH];
    nameTmp[0] = '\0';
    strcat(nameTmp, startName);
    strcat(nameTmp, generateName(pid));
    strcpy(people[index].name, nameTmp);
    people[index].genes = generateGenes(x, pid);
    people[index].scelto = 0;
    addSem(semSMId, SCRITTORE); // fine scrittore
}