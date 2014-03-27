/* 
 * File:   socket_lib.h
 * Author: emmanuel
 *
 * Created on 15 de marzo de 2014, 11:48 AM
 */

#ifndef SOCKET_LIB_H
#define	SOCKET_LIB_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <netinet/in.h>
    
void manejo_de_error(int code, char *mensaje);
void iniciar_addr(struct sockaddr_in *direccion) ;



#ifdef	__cplusplus
}
#endif

#endif	/* SOCKET_LIB_H */

