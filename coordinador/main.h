/* 
 * File:   main.h
 * Author: emmanuel
 *
 * Created on 15 de marzo de 2014, 11:28 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <unistd.h>
    
#define P1 1
#define P2 2
#define P3 3

void recibir_imagen(int socka); 
void get_ip(struct sockaddr_in client,char *ip_cliente);

typedef enum {SIN_IMAGEN,EXISTENCIA_IMAGEN, COPIO_P3, COPIO_P2, P2_BORRAR, P3_BORRAR} ESTADO;

#ifdef	__cplusplus
}
#endif




#endif	/* MAIN_H */

