all : main_exec main_exec_personA main_exec_personB
main_exec: main.o Utilities/Controls/checkInputs.o Utilities/Semaphore/sem_utilities.o Utilities/Random/generateRandom.o Manager/Gestore/manager.o Manager/PeopleAB/people.o Utilities/SharedMem/shm_utilities.o  Utilities/MsgQueue/msg_utilities.o  Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o
	gcc -o main_exec main.o Utilities/Controls/checkInputs.o Utilities/Semaphore/sem_utilities.o Utilities/Random/generateRandom.o Manager/Gestore/manager.o Manager/PeopleAB/people.o Utilities/SharedMem/shm_utilities.o  Utilities/MsgQueue/msg_utilities.o  Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o

main_exec_personA: Manager/PeopleAB/PersonA/personA.o Utilities/Semaphore/sem_utilities.o Manager/PeopleAB/people.o  Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.o Utilities/MsgQueue/msg_utilities.o Utilities/Random/generateRandom.o Utilities/SharedMem/shm_utilities.o Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o
	gcc -o main_exec_personA Manager/PeopleAB/PersonA/personA.o Utilities/Semaphore/sem_utilities.o Manager/PeopleAB/people.o  Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.o Utilities/MsgQueue/msg_utilities.o Utilities/Random/generateRandom.o Utilities/SharedMem/shm_utilities.o Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o

main_exec_personB: Manager/PeopleAB/PersonB/personB.o Utilities/Semaphore/sem_utilities.o Manager/PeopleAB/people.o  Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.o Utilities/MsgQueue/msg_utilities.o Utilities/Random/generateRandom.o Utilities/SharedMem/shm_utilities.o Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o
	gcc -o main_exec_personB Manager/PeopleAB/PersonB/personB.o Utilities/Semaphore/sem_utilities.o Manager/PeopleAB/people.o  Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.o Utilities/MsgQueue/msg_utilities.o Utilities/Random/generateRandom.o Utilities/SharedMem/shm_utilities.o Utilities/ReaderSync/reader_utilities.o Utilities/Math/math_utilities.o
main.o: main.c
	gcc -c main.c

manager.o :  Manager/Gestore/manager.c Manager/Gestore/manager.h
	gcc -c Manager/Gestore/manager.c

people.o :  Manager/PeopleAB/people.c Manager/PeopleAB/people.h
	gcc -c Manager/PeopleAB/people.c

checkInputs.o: Utilities/Controls/checkInputs.c Utilities/Controls/checkInputs.h
	gcc -c Utilities/Controls/checkInputs.c

generateRandom.o : Utilities/Random/generateRandom.c Utilities/Random/generateRandom.h
	gcc -c Utilities/Random/generateRandom.c

sem_utilities.o : Utilities/Semaphore/sem_utilities.c Utilities/Semaphore/sem_utilities.h
	gcc -c Utilities/Semaphore/sem_utilities.c

shm_utilities.o : Utilities/SharedMem/shm_utilities.c Utilities/SharedMem/shm_utilities.h
	gcc -c Utilities/SharedMem/shm_utilities.c


msg_utilities.o: Utilities/MsgQueue/msg_utilities.c Utilities/MsgQueue/msg_utilities.h
	gcc -c Utilities/MsgQueue/msg_utilities.c

reader_utilities.o : Utilities/ReaderSync/reader_utilities.c Utilities/ReaderSync/reader_utilities.h
	gcc -c Utilities/ReaderSync/reader_utilities.c

personA.o : Manager/PeopleAB/PersonA/personA.c Manager/PeopleAB/PersonA/personA.h
	gcc -c Manager/PeopleAB/PersonA/personA.c
personB.o : Manager/PeopleAB/PersonB/personB.c
	gcc -c Manager/PeopleAB/PersonB/personB.c
msgMskA.o : Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.c Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.h
	gcc -c Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.c
msgMskB.o : Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.c Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.h
	gcc -c Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.c
math_utilities.o: Utilities/Math/math_utilities.c Utilities/Math/math_utilities.h
	gcc -c Utilities/Math/math_utilities.c

clean:
	rm -f main_exec main_exec_personA main_exec_personB main.o Manager/Gestore/manager.o Utilities/Controls/checkInputs.o Utilities/Semaphore/sem_utilities.o Utilities/Random/generateRandom.o Utilites/Math/math_utilities.o Manger/Gestore/manager.o Manager/PeopleAB/people.o Utilities/SharedMem/shm_utilities.o Utilities/MsgQueue/msg_utilities.o Utilities/ReaderSync/reader_utilities.o Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.o Manager/PeopleAB/PersonB/personB.o Manager/PeopleAB/PersonA/personA.o Manager/PeopleAB/PersonA/Utilities_A/MsgMsk/msgMskA.o Manager/PeopleAB/PersonB/Utilities_B/MsgMsk/msgMskB.o Utilities/MsgQueue/msg_utilities.o Utilities/Math/math_utilities.o