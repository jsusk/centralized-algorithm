/* 
 * File:   main.c
 * Author: emmanuel
 * Ultima modificación: 19 marzo 2014 21:38
 * Created on 15 de marzo de 2014, 11:24 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "main.h"
#include "socket_lib.h"
#include <unistd.h>
/*
 * 
 */
ESTADO actual;
char nameFile[20]; //nobre para el archivo log

static void handlerInterrup(int sig) {
    printf("Estado actual: %d\n", actual);
    char toWriteFile[20];
    sprintf(toWriteFile, "%d", actual);
    setFileData(nameFile, toWriteFile);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sock; //socket principal
    int addrlen; //longitud de la estructura addr 
    int socka; //socket por cliente
    int codigo_error = 0; //codigo de error
    int keep_servidor = 0;
    int keep_canal = 0;

    char dataFile[20]; //datos a escribir
    char mensaje_r[120];
    char mensaje_e[120];
    char ip_cliente[120];

    if (signal(SIGINT, handlerInterrup) == SIG_ERR) {
        printf("Error al asignar la señal");
    }

    int aux = SIN_IMAGEN; //variable donde se almacenará el valor del archivo contador.log
    strcpy(nameFile, "coordinador.log");
    getFileData(nameFile, dataFile);

    if (strlen(dataFile) > 0) {
        if (strcmp(dataFile, "error") < 0) {
            aux = atoi(dataFile);
        }
    }

    actual = aux;

    printf("-------------COORDINADOR INICIADO-----------\n");
    printf("Estado actual: %d \n",actual);
       
    sock = socket(AF_INET, SOCK_STREAM, 0);
    manejo_de_error(sock, "ERROR AL CREAR SOCKET");

    //llenando los valors de la structura
    iniciar_addr(&server);
    do {
        //Inicializamos el servidor
        codigo_error = bind(sock, (struct sockaddr *) &server, sizeof (server));
        manejo_de_error(codigo_error, "ERROR INICIANDO EL SERVIDOR");

        //habilitando para recibir conexiones con listen, encola hasta 5 solicitudes.
        codigo_error = listen(sock, 5);
        manejo_de_error(codigo_error, "ERROR AL ESCUCHAR");

        printf("Listo para recibir conexion. \n");
        do {
            addrlen = sizeof (struct sockaddr_in); //tama;o de la estructura
            printf("Esperando conexion con algun cliente.. \n");
            
            socka = accept(sock, (struct sockaddr*) &client, &addrlen); //se acepta la comunicacion con cliente.
            manejo_de_error(socka, "ERROR ACCEPT: no se acepto comunicacion con cliente.");

            memset(mensaje_r, 0, sizeof (mensaje_r));
            codigo_error = recv(socka, mensaje_r, sizeof (mensaje_r), 0);
            manejo_de_error(codigo_error, "Error recibiendo datos");
            
            
            printf(" * * * * * * NUEVO CLIENTE  * * * * * * *\n");
            get_ip(client, ip_cliente);

            printf("Cliente con ip: %s \n", ip_cliente);
            printf("Mensaje Recibido: %s \n", mensaje_r);
            
            printf("- - - - - DETECTANDO ESTADO - - - - - -\n");

            switch (actual) {
                case SIN_IMAGEN:
                    printf("ESTADO ACTUAL DEL COORDINADOR: sin imagen. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (strcmp(mensaje_r, "transportar_imagen?") == 0) {
                        strcpy(mensaje_e, "OK");
                        codigo_error = send(socka, mensaje_e, strlen(mensaje_e), 0);
                        manejo_de_error(codigo_error, "Error de envio");
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                        printf("Comenzando a recibir imagen...\n");

                        receive_image(socka,"imagen_recibida.bmp");

                        actual = EXISTENCIA_IMAGEN;
                        memset(mensaje_r, 0, sizeof (mensaje_r));
                    } else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }

                    break;
                case EXISTENCIA_IMAGEN:
                    printf("ESTADO ACTUAL DEL COORDINADOR: existencia imagen. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (strcmp(mensaje_r, "get_imagen0") == 0) {
                        sleep(7);
                        printf("Enviado imagen...a P2\n");
                        send_image(socka,"imagen_recibida.bmp");                        
                        printf("Imagen enviada. \n");
                        //if (get_proceso(client, argv[1], argv[2]) == P2)
                        actual = COPIO_P2;
                        //else
                            //actual = COPIO_P3;

                    } else if(strcmp(mensaje_r, "get_imagen1") == 0) {
                        printf("Enviado imagen a P3...\n");
                        send_image(socka,"imagen_recibida.bmp");                        
                        printf("Imagen enviada. \n");
                        //if (get_proceso(client, argv[1], argv[2]) == P2)
                        actual = COPIO_P3;
                    }else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }
                    break;
                case COPIO_P2:
                    printf("ESTADO ACTUAL DEL COORDINADOR: copio p2. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (strcmp(mensaje_r, "get_imagen1") == 0) {
                        printf("Enviado imagen a P3...\n");

                        send_image(socka,"imagen_recibida.bmp");

                        actual = P3_BORRAR;
                        memset(mensaje_r, 0, sizeof (mensaje_r));

                    } else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }

                    break;
                case COPIO_P3:
                    printf("ESTADO ACTUAL DEL COORDINADOR: copio p3. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (strcmp(mensaje_r, "get_imagen0") == 0) {
                        printf("Enviado imagen a P2...\n");

                        send_image(socka,"imagen_recibida.bmp");

                        actual = P2_BORRAR;
                        memset(mensaje_r, 0, sizeof (mensaje_r));

                    } else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }
                    break;
                case P2_BORRAR:
                    printf("ESTADO ACTUAL DEL COORDINADOR: p2_borrar. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (/*get_proceso(client, argv[1], argv[2]) == P2 && */strcmp(mensaje_r, "borrar_imagen0") == 0) {
                        printf("Borrando imagen...");
                        codigo_error = borrar_imagen();
                        actual = SIN_IMAGEN;
                        manejo_de_error(codigo_error, "Fallo al borrar imagen");
                    } else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }
                    break;
                case P3_BORRAR:
                    printf("ESTADO ACTUAL DEL COORDINADOR: p3 borrar. \n");
                    memset(mensaje_e, 0, strlen(mensaje_e));
                    if (/*get_proceso(client, argv[1], argv[2]) == P3 &&*/ strcmp(mensaje_r, "borrar_imagen1") == 0) {
                        printf("Borrando imagen...");
                        codigo_error = borrar_imagen();
                        actual = SIN_IMAGEN;
                        manejo_de_error(codigo_error, "Fallo al borrar imagen");

                    } else {
                        strcpy(mensaje_e, "NO");
                        send(socka, mensaje_e, strlen(mensaje_e), 0);
                        memset(mensaje_e, 0, sizeof (mensaje_e));
                    }
                    break;
            }

            close(socka);
        } while (keep_canal == 0);

    } while (keep_servidor == 0);

    return (EXIT_SUCCESS);
}

int get_proceso(struct sockaddr_in client, char *ip1, char *ip2) {
    printf("Obteniendo proceso. \n");
    int ip;
    ip = client.sin_addr.s_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip, str, INET_ADDRSTRLEN);

    if (strcmp(ip1, str) == 0)
        return P2;
    else if (strcmp(ip2, str) == 0)
        return P3;
    else
        return P1;
}

void get_ip(struct sockaddr_in client, char *ip_cliente) {
    printf("Obteniendo IP de cliente.\n");
    int ip;
    ip = client.sin_addr.s_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip, str, INET_ADDRSTRLEN);

    strcpy(ip_cliente, str);
    printf("La ip de cliente es: %s \n", ip_cliente);

}
//TODO

int borrar_imagen() {
    return remove("imagen_recibida.bmp");
}