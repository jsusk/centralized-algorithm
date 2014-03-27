/* 
 * File:   main.c
 * Author: joe
 *
 * Created on 19 de marzo de 2014, 10:24 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "socket_lib.h"

/*
 * PROCESO UNO.
 */
int main(int argc, char** argv) {

    struct sockaddr_in direccion;

    char peticion[220];
    char respuesta[220];

    int sock, conecta, envio_peticion, mensaje_r;
    int contadorImagen = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    manejo_de_error(sock, "ERROR AL CREAR SOCKET");


    //llenando los valors de la structura
    iniciar_addr(&direccion);
    printf("PROCESO UNO. INICIADO. - - - - -\n");

    
    int intento=0;
    //ciclo para mandar más imagenes.
    while(contadorImagen < 4){
        
        printf("Intento de conexion: %d\n",intento);
        intento++;
        conecta = connect(sock, (struct sockaddr*) &direccion, sizeof (direccion));
        manejo_de_error(conecta, "ERROR CONNECT: no hubo conexion.");
        if(conecta>0)
            printf("Conexion hecha.\n");
        
        //mandando mensaje a coordinador.
        strcpy(peticion, "transportar_imagen?");
        envio_peticion = send(sock, peticion, strlen(peticion), 0);
        manejo_de_error(envio_peticion, "ERROR SEND: no se envio peticion.");
        if(envio_peticion>0)
            printf("peticion enviada.\n");
        
        //recibiendo respuesta de coordinador
        memset(respuesta,0,strlen(respuesta));
        mensaje_r = recv(sock, respuesta, sizeof (respuesta), 0);
        manejo_de_error(mensaje_r, "ERROR RECV: No se recibio respuesta.");
        
        if(mensaje_r>0)
            printf("Se recibe: %s\n",respuesta);

        if (strcmp(respuesta, "OK") == 0) {

            switch(contadorImagen){
                case 0:
                    printf("Comenzando a enviar imagen a coordinador. \n");
                    send_image(sock,"lagochungara.bmp");
                    contadorImagen++;
                break;
                case 1:
                    printf("Comenzando a enviar imagen a coordinador. \n");
                    send_image(sock,"guepardos.bmp");
                    contadorImagen++;                
                break;
                case 2:
                    printf("Comenzando a enviar imagen a coordinador. \n");
                    send_image(sock,"magia.bmp");    
                    contadorImagen++;
                break;
                case 3:
                    printf("Comenzando a enviar imagen a coordinador. \n");
                    send_image(sock,"win_nine.bmp");
                    contadorImagen++;
                break;                    
            }

            

        }
        sleep(10);
        close(sock);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        manejo_de_error(sock, "ERROR AL CREAR SOCKET");

        //llenando los valors de la structura
        iniciar_addr(&direccion);
    
    }
    

    

    return (EXIT_SUCCESS);
}
