/* 
 * File:   main.c
 * Author: sacasher
 *
 * Created on 18 de marzo de 2014, 12:55 PM p3
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

/*
 * Proceso 3
 */

int actual;
char nameFile[20]; //nobre para el archivo log

static void handlerInterrup(int sig) {
    int pid;
    pid = getpid();
        
    char pidChar[20];    
    //REGISTRANDO QUE MURIO PROCESO.
    sprintf(pidChar,"%d",pid);
    setFileData("death_note_p3.log",pidChar);
    exit(EXIT_SUCCESS);
    
}

int main(int argc, char** argv) {
    if (!(argc > 1)) {
        printf("Modo de uso: %s uri",argv[0]);
        return(EXIT_FAILURE);
    }
    struct sockaddr_in direccion;    
    char bufferr[1024];
    char peticion[220];
    char nameImage[20];
    int contadorImagen;
    int sock;
    
    char toWriteFile[BUF_SIZE];
    char nameFile[20];
    char dataFile[20];
    int aux = 0; //variable donde se almacenará el valor del archivo contador.log
    
    int pid;
    pid = getpid();
    if (signal(SIGINT, handlerInterrup) == SIG_ERR) {
        printf("Error al asignar la señal");
    }
    
    
    strcpy(nameFile, "contador.log");
    getFileData(nameFile, dataFile);

    if (strlen(dataFile) > 0) {
        if (strcmp(dataFile, "error") < 0) {
            aux = atoi(dataFile);
        }
    }

    contadorImagen = aux;




    while (contadorImagen < 4) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            printf("ERROR: Socket no construido.");
        else
            printf("Socket creado.");

        sprintf(nameImage, "im%d.bmp", contadorImagen);
        bzero((char*) &direccion, sizeof (direccion));

        direccion.sin_family = AF_INET;
        direccion.sin_port = htons(3393);
         direccion.sin_addr.s_addr = inet_addr(argv[1]);
        printf("--------CLIENTE QUE PIDE IMAGEN-p3---------------\n");
        setFileData("death_note_p3.log","0");

        if (connect(sock, (struct sockaddr*) &direccion, sizeof (direccion)) < 0) {//conectando con servidor
            printf("\n ERROR CONNECT: no connect para pedir imagen.");
            sleep(1);
        } else {
            strcpy(peticion, "get_imagen1");
            if (send(sock, peticion, strlen(peticion), 0) == -1)
                printf("ERROR SEND: Error al enviar solicitud.\n");
            //COMENZANDO A RECIBIR IMAGEN
            char mensaje_r[10];
            if (recv(sock, mensaje_r, sizeof (mensaje_r), 0) == -1) {
                printf("Error recibiendo respuesta");
            }

            //Mientras no recivamos si preguntamos.
            /*if (strcmp(mensaje_r, "SI") != 0) {
                sleep(1);
                //continue;
            }*/

            char mensaje_e[] = "LISTO";
            send(sock, mensaje_e, sizeof (mensaje_e), 0);
            //recibiendo imagen
            receive_image(sock,nameImage);
            //sleep(2);

            printf("\nEscrito totalmente \n");
            contadorImagen++;
            close(sock);
            sprintf(toWriteFile, "%d", contadorImagen);
            setFileData(nameFile, toWriteFile);
            imageProcessing(nameImage, contadorImagen);

            //Enviar petición para borrar la imagen al servidor


            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
                printf("ERROR: Socket no construido.");
            else
                printf("Socket creado.");

            bzero((char*) &direccion, sizeof (direccion));

            direccion.sin_family = AF_INET;
            direccion.sin_port = htons(3393);
            direccion.sin_addr.s_addr = inet_addr(argv[1]);
            printf("--------CLIENTE QUE PIDE BORRAR IMAGEN----------------\n");

            if (connect(sock, (struct sockaddr*) &direccion, sizeof (direccion)) < 0) {//conectando con servidor
                printf("\n ERROR CONNECT: no connect para pedir imagen.");
                sleep(1);
            } else {
                memset(peticion, 0, sizeof (peticion));
                strcpy(peticion, "borrar_imagen1");
                if (send(sock, peticion, strlen(peticion), 0) == -1)
                    printf("ERROR SEND: Error al enviar solicitud.\n");
                if (recv(sock, (char *) &bufferr, sizeof (bufferr), 0) < 0) {
                    printf("ERROR RECV: no se recibieron caracteristicas de la imagen.\n");
                }
                if (strcmp(bufferr, "OK") == 0) {
                    printf("Imagen borrada\n");
                } else if (strcmp(bufferr, "NO") == 0) {
                    printf("Permiso denegado\n");
                }
                close(sock);
            }


        }
        sleep(10);
    }


    return (EXIT_SUCCESS);
}

