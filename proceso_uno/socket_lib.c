#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_lib.h"
#include <strings.h>

void manejo_de_error(int code, char *mensaje){
    if(code < 0)
        printf("%s \n", mensaje);
    else
    	printf("Evento exitoso. \n");
}

void iniciar_addr(struct sockaddr_in *direccion){
    bzero((char *) direccion, sizeof(direccion));
    direccion->sin_family = AF_INET;
    direccion->sin_port = htons(3393);
    direccion->sin_addr.s_addr = inet_addr("127.0.0.1");
    //direccion->sin_addr.s_addr = inet_addr("127.0.0.1");
}
