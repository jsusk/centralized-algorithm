/* 
 * File:   main.c
 * Author: sacasher
 *
 * Created on 18 de marzo de 2014, 12:55 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

/*
 * Proceso 3
 */
int main(int argc, char** argv) {
    struct sockaddr_in direccion;
    FILE *filenew;
    char bufferr[1024];
    char peticion[220];
    char nameImage[20];
    int contadorImagen;
    unsigned long fileLen;
    int params[2];
    int rec, rest, kl;
    int sock, i, envio, bytes;

    char toWriteFile[BUF_SIZE];
    char nameFile[20];
    char dataFile[20];
    int aux = 0; //variable donde se almacenará el valor del archivo contador.log
    strcpy(nameFile, "contador.log");
    getFileData(nameFile, dataFile);

    if (strlen(dataFile) > 0) {
        if (strcmp(dataFile, "error") < 0) {
            aux = atoi(dataFile);
        }
    }

    contadorImagen = aux;




    while (contadorImagen < 5) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            printf("ERROR: Socket no construido.");
        else
            printf("Socket creado.");

        sprintf(nameImage, "im%d.bmp", contadorImagen);
        bzero((char*) &direccion, sizeof (direccion));

        direccion.sin_family = AF_INET;
        direccion.sin_port = htons(3393);
        direccion.sin_addr.s_addr = inet_addr("192.168.1.69");
        printf("--------CLIENTE QUE PIDE IMAGEN----------------\n");

        if (connect(sock, (struct sockaddr*) &direccion, sizeof (direccion)) < 0) {//conectando con servidor
            printf("\n ERROR CONNECT: no connect para pedir imagen.");
            sleep(1);
        } else {
            strcpy(peticion, "get_imagen");
            if (send(sock, peticion, strlen(peticion), 0) == -1)
                printf("ERROR SEND: Error al enviar solicitud.\n");
            //COMENZANDO A RECIBIR IMAGEN
            char mensaje_r[10];
            if (recv(sock, mensaje_r, sizeof (mensaje_r), 0) == -1) {
                printf("Error recibiendo respuesta");
            }

            //Mientras no recivamos si preguntamos.
            if (strcmp(mensaje_r, "SI") != 0) {
                sleep(1);
                continue;
            }

            char mensaje_e[] = "LISTO";
            send(sock, mensaje_e, sizeof (mensaje_e), 0);
            //recibiendo imagen
            receive_image(sock,nameImage);
            
/*
            bytes = recv(sock, (char *) &params, 2 * sizeof (int), 0);
            if (bytes < 0)
                printf("ERROR RECV: no se recibieron caracteristicas de la imagen.\n");
            else
                printf("Caracteristicas recibidas en RECV\n");

            rec = params[0]; //recivido
            rest = params[1]; //restante

            printf("Recorrer archivo %d veces \n", rec);
            printf("Resta de b %d \n", rest);
            //writeDataFile(nameImage,sock,params);

            filenew = fopen(nameImage, "w");
            for (kl = 0; kl < rec; kl++) {
                recv(sock, (char *) &bufferr, sizeof (bufferr), 0);
                fwrite(bufferr, 1024, sizeof (char), filenew);
                memset(bufferr, 0, strlen(bufferr));
                printf("Escrito \n");
            }

            if (rest > 0) {
                recv(sock, (char *) &bufferr, rest * sizeof (char), 0);
                fwrite(bufferr, rest, sizeof (char), filenew);
                memset(bufferr, 0, strlen(bufferr));
            }

            fclose(filenew);
*/
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
            direccion.sin_addr.s_addr = inet_addr("192.168.1.69");
            printf("--------CLIENTE QUE PIDE BORRAR IMAGEN----------------\n");

            if (connect(sock, (struct sockaddr*) &direccion, sizeof (direccion)) < 0) {//conectando con servidor
                printf("\n ERROR CONNECT: no connect para pedir imagen.");
                sleep(1);
            } else {
                memset(peticion, 0, sizeof (peticion));
                strcpy(peticion, "borrar_imagen");
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
    }


    return (EXIT_SUCCESS);
}

