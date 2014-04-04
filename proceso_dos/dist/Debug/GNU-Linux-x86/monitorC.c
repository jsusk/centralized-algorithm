
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 20
void getFileData(char *nameFile, char *dataRead);

int main(int argc, char** argv) {

	char dataFile[MAX];
	int aux = 0;

	while(1){
		
		printf("Monitoreando a COORDINADOR\n");
		getFileData("death_note.log", dataFile);
		

		if (strlen(dataFile) > 0) {
			if (strcmp(dataFile, "error") < 0) {
			    aux = atoi(dataFile);
			}
		}

		if(aux!=0){
			printf("Coordinador fallido. Procediendo a reactivar. PID: %d\n",aux);
			sleep(150);
			remove("death_note.log");
			system("./proceso_dos");
		
		}
		sleep(10);
	}

}

void getFileData(char *nameFile, char *dataRead) {
    int inputFd, openFlags;
    mode_t filePerms;
    //memset(dataRead,0,sizeof(dataRead));
    openFlags = O_RDONLY;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; //rw-rw-rw-
    inputFd = open(nameFile, openFlags);
    if (inputFd == -1) {
        strcpy(dataRead, "error");
    } else {
        read(inputFd, dataRead, MAX);
    }
    close(inputFd);
}
