#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "images.h"

void abrir_imagen(BMP *imagen, char *ruta) {
    FILE *archivo; //Puntero FILE para el archivo de imágen a abrir
    int i, j, k, resto;
    unsigned char R, B, G, var;

    //Abrir el archivo de imágen
    archivo = fopen(ruta, "rb+");
    if (!archivo) {
        //Si la imágen no se encuentra en la ruta dada
        printf("La imágen %s no se encontro\n", ruta);
        exit(1);
    }

    //Leer la cabecera de la imagen y almacenarla en la estructura global img
    fseek(archivo, 0, SEEK_SET);
    fread(&imagen->bm, sizeof (char), 2, archivo);
    fread(&imagen->tamano, sizeof (int), 1, archivo);
    fread(&imagen->reservado, sizeof (int), 1, archivo);
    fread(&imagen->offset, sizeof (int), 1, archivo);
    fread(&imagen->tamanoMetadatos, sizeof (int), 1, archivo);
    fread(&imagen->alto, sizeof (int), 1, archivo);
    fread(&imagen->ancho, sizeof (int), 1, archivo);
    fread(&imagen->numeroPlanos, sizeof (short int), 1, archivo);
    fread(&imagen->profundidadColor, sizeof (short int), 1, archivo);
    fread(&imagen->tipoCompresion, sizeof (int), 1, archivo);
    fread(&imagen->tamanoEstructura, sizeof (int), 1, archivo);
    fread(&imagen->pxmh, sizeof (int), 1, archivo);
    fread(&imagen->pxmv, sizeof (int), 1, archivo);
    fread(&imagen->coloresUsados, sizeof (int), 1, archivo);
    fread(&imagen->coloresImportantes, sizeof (int), 1, archivo);

    //Validar ciertos datos de la cabecera de la imágen	
    if (imagen->bm[0] != 'B' || imagen->bm[1] != 'M') {
        printf("La imagen debe ser un bitmap.\n");
        exit(1);
    }
    if (imagen->profundidadColor != 24) {
        printf("La imagen debe ser de 24 bits.\n");
        exit(1);
    }

    //*********************************************************************************************************
    //Colocar el cursor en el byte (offset) donde comienzan los datos de la imagen
    //*********************************************************************************************************
    fseek(archivo, imagen->offset, SEEK_SET);

    //*********************************************************************************************************
    //Reservar memoria para el arreglo que tendra los planos RGB de la imagen (Arreglo de tamaño "img.ancho X img.alto")
    //*********************************************************************************************************
    imagen->pixelR = malloc(imagen->alto * sizeof (char *));
    imagen->pixelG = malloc(imagen->alto * sizeof (char *));
    imagen->pixelB = malloc(imagen->alto * sizeof (char *));
    for (i = 0; i < imagen->alto; i++) {
        imagen->pixelR[i] = malloc(imagen->ancho * sizeof (char));
        imagen->pixelG[i] = malloc(imagen->ancho * sizeof (char));
        imagen->pixelB[i] = malloc(imagen->ancho * sizeof (char));
    }

    //*********************************************************************************************************
    //Padding (Bytes necesarios para que el Pad row alcance a ser multiplo de 4 Bytes)		
    //*********************************************************************************************************
    resto = (imagen->ancho * 3) % 4;
    if (resto != 0)
        resto = 4 - resto;

    //*********************************************************************************************************
    //Pasar la imágen a el arreglo reservado en escala de grises
    //*********************************************************************************************************
    //Iterar a través de las filas de píxeles, teniendo en cuenta que los datos comienza en la esquina inferior izquierda de la imagen BMP
    for (i = imagen->alto - 1; i >= 0; i--) {
        for (j = 0; j < imagen->ancho; j++) {
            fread(&B, sizeof (char), 1, archivo); //Byte Blue del pixel
            fread(&G, sizeof (char), 1, archivo); //Byte Green del pixel
            fread(&R, sizeof (char), 1, archivo); //Byte Red del pixel
            imagen->pixelR[i][j] = R;
            imagen->pixelG[i][j] = G;
            imagen->pixelB[i][j] = B;
        }

        //Padding (Bytes necesarios para que el Pad row alcance a ser multiplo de 4 Bytes)		
        for (k = 1; k <= resto; k++) {
            fread(&var, sizeof (char), 1, archivo); //Leer los pixeles de relleno (Padding)
        }
    }
    //Cerrrar el archivo
    fclose(archivo);
}

//****************************************************************************************************************************************************
//Función para crear una imagen BMP, a partir de la estructura imagen imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
//Parametros de entrada: Referencia a un BMP (Estructura BMP), Referencia a la cadena ruta char ruta[]=char *ruta
//Parametro que devuelve: Ninguno
//****************************************************************************************************************************************************

void crear_imagen(BMP *imagen, char ruta[]) {
    FILE *archivo; //Puntero FILE para el archivo de imágen a abrir

    int i, j, k, resto, var;

    //Abrir el archivo de imágen
    archivo = fopen(ruta, "wb+");
    if (!archivo) {
        //Si la imágen no se encuentra en la ruta dada
        printf("La imágen %s no se pudo crear\n", ruta);
        exit(1);
    }

    //Leer la cabecera de la imagen y almacenarla en la estructura global img
    fseek(archivo, 0, SEEK_SET);
    fwrite(&imagen->bm, sizeof (char), 2, archivo);
    fwrite(&imagen->tamano, sizeof (int), 1, archivo);
    fwrite(&imagen->reservado, sizeof (int), 1, archivo);
    fwrite(&imagen->offset, sizeof (int), 1, archivo);
    fwrite(&imagen->tamanoMetadatos, sizeof (int), 1, archivo);
    fwrite(&imagen->alto, sizeof (int), 1, archivo);
    fwrite(&imagen->ancho, sizeof (int), 1, archivo);
    fwrite(&imagen->numeroPlanos, sizeof (short int), 1, archivo);
    fwrite(&imagen->profundidadColor, sizeof (short int), 1, archivo);
    fwrite(&imagen->tipoCompresion, sizeof (int), 1, archivo);
    fwrite(&imagen->tamanoEstructura, sizeof (int), 1, archivo);
    fwrite(&imagen->pxmh, sizeof (int), 1, archivo);
    fwrite(&imagen->pxmv, sizeof (int), 1, archivo);
    fwrite(&imagen->coloresUsados, sizeof (int), 1, archivo);
    fwrite(&imagen->coloresImportantes, sizeof (int), 1, archivo);

    //*********************************************************************************************************
    //Colocar el cursor en el byte (offset) donde comienzan los datos de la imagen
    //*********************************************************************************************************
    fseek(archivo, imagen->offset, SEEK_SET);

    //*********************************************************************************************************
    //Pasar la imágen del arreglo reservado en escala de grises a el archivo (Deben escribirse los valores BGR)
    //*********************************************************************************************************
    resto = (imagen->ancho * 3) % 4; //Padding (Bytes necesarios para que el Pad row alcance a ser multiplo de 4 Bytes)		
    if (resto != 0)
        resto = 4 - resto;

    //Iterar a través de las filas de píxeles, teniendo en cuenta que los datos comienza en la esquina inferior izquierda de la imagen BMP
    for (i = imagen->alto - 1; i >= 0; i--) {
        for (j = 0; j < imagen->ancho; j++) {
            //Ecribir los 3 bytes BGR al archivo BMP, en este caso todos se igualan al mismo valor (Valor del pixel en la matriz de la estructura imagen)
            fwrite(&imagen->pixelB[i][j], sizeof (char), 1, archivo); //Escribir el Byte Blue del pixel 
            fwrite(&imagen->pixelG[i][j], sizeof (char), 1, archivo); //Escribir el Byte Green del pixel
            fwrite(&imagen->pixelR[i][j], sizeof (char), 1, archivo); //Escribir el Byte Red del pixel
        }

        //Padding (Bytes necesarios para que el Pad row alcance a ser multiplo de 4 Bytes)		
        for (k = 1; k <= resto; k++)
            fwrite(&var, sizeof (char), 1, archivo); //Escribir los pixeles de relleno		
    }
    //Cerrrar el archivo
    fclose(archivo);
}

/**
 * Escribir datos al contador.log truncando o creando un nuevo archivo
 * @param nameFile Nombre o ruta del archivo 
 * @param dataWrite Los datos a escribir en el archivo
 */
void setFileData(char *nameFile, char *dataWrite) {
    int outputFd, openFlags;
    mode_t filePerms;
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; //rw-rw-rw-
    outputFd = open(nameFile, openFlags, filePerms);
    if (outputFd == -1)
        perror("No se puede crear el archivo");
    write(outputFd, dataWrite, strlen(dataWrite));
    close(outputFd);
}

/**
 * Obtener el contenido del archivo contador.log
 * @param nameFile nombre o ruta del archivo
 * @param dataRead Donde se almacenara el dato leído
 * Regresa en dataRead el dato leído o en caso contrario la cadena "error"
 */
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
        read(inputFd, dataRead, strlen(dataRead));
    }
    close(inputFd);
}

void imageProcessing(char* imageName, int contador) {
    int i, j; //Variables auxiliares para loops
    BMP img; //Estructura de tipo imágen
    unsigned char pixel; //Pixel

    char IMAGEN[45];
    char IMAGEN_TRATADA[45];
    strcpy(IMAGEN, imageName);
    sprintf(IMAGEN_TRATADA, "imTratada_%d.bmp", contador);
    //*****************************************************************************************************************
    //Abrir la imágen BMP de 24 bits, almacenar su cabecera en img y colocar sus pixeles en el arreglo img.pixel[][]
    //*****************************************************************************************************************
    abrir_imagen(&img, IMAGEN);
    printf("\n*************************************************************************");
    printf("\nIMAGEN: %s", IMAGEN);
    printf("\n*************************************************************************");
    printf("\nDimensiones de la imágen:\tAlto=%d\tAncho=%d\n", img.alto, img.ancho);
    //***********************************************************************************
    //Tratamiento de los pixeles
    //***********************************************************************************
    //Ejemplo donde se entercambia el plano Rojo con el Azul de la mitad de la imagen
    for (i = 0; i < img.alto; i++)
        for (j = 0; j < img.ancho; j++) {
            pixel = img.pixelR[i][j];
            img.pixelR[i][j] = img.pixelB[i][j];
            img.pixelB[i][j] = img.pixelG[i][j];
            img.pixelG[i][j] = pixel;
        }

    //*****************************************************************
    //1 Crear la imágen BMP a partir del arreglo img.pixel[][]
    //*****************************************************************
    crear_imagen(&img, IMAGEN_TRATADA);
    printf("\nImágen BMP tratada en el archivo: %s\n", IMAGEN_TRATADA);

    //Terminar programa normalmente	
}

int sendMessage(int socket, char *message) {
    return send(socket, message, sizeof (message), 0);
}

void writeDataFile(char *nameFile, int socket, int params[]) {
    int blocks = params[0];
    int restante = params[1];
    int outputFd, openFlags;
    mode_t filePerms;
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; //rw-rw-rw-
    outputFd = open(nameFile, openFlags, filePerms);
    if (outputFd == -1)
        perror("No se puede crear el archivo");
    else {
        char buffer[1024];
        int i;
        for (i = 0; i < blocks; i++) {
            recv(socket, (char *) &buffer, sizeof (buffer), 0);
            write(outputFd, buffer, strlen(buffer));
            memset(buffer, 0, strlen(buffer));
        }

        if (restante > 0) {
            recv(socket, (char *) &buffer, restante * sizeof (char), 0);
            write(outputFd, buffer, strlen(buffer));
        }
        close(outputFd);
    }
}

int receive_image(int socket,char *name) { // Start function 

    int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

    char imagearray[10241], verify = '1';
    FILE *image;

    //Find the size of the image
    do {
        stat = read(socket, &size, sizeof (int));
    } while (stat < 0);

    printf("Packet received.\n");
    printf("Packet size: %i\n", stat);
    printf("Image size: %i\n", size);
    printf(" \n");

    char buffer[] = "Got it";

    //Send our verification signal
    do {
        stat = write(socket, &buffer, sizeof (int));
    } while (stat < 0);

    printf("Reply sent\n");
    printf(" \n");

    image = fopen(name, "w");

    if (image == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1;
    }

    //Loop while we have not received the entire file yet


    int need_exit = 0;
    struct timeval timeout = {10, 0};

    fd_set fds;
    int buffer_fd, buffer_out;

    while (recv_size < size) {
        //while(packet_index < 2){

        FD_ZERO(&fds);
        FD_SET(socket, &fds);

        buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

        if (buffer_fd < 0)
            printf("error: bad file descriptor set.\n");

        if (buffer_fd == 0)
            printf("error: buffer read timeout expired.\n");

        if (buffer_fd > 0) {
            do {
                read_size = read(socket, imagearray, 10241);
            } while (read_size < 0);

            printf("Packet number received: %i\n", packet_index);
            printf("Packet size: %i\n", read_size);


            //Write the currently read data into our image file
            write_size = fwrite(imagearray, 1, read_size, image);
            printf("Written image size: %i\n", write_size);

            if (read_size != write_size) {
                printf("error in read write\n");
            }


            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n", recv_size);
            printf(" \n");
            printf(" \n");
        }

    }


    fclose(image);
    printf("Image successfully Received!\n");
    return 1;
}

int send_image(int socket,char *name) {

    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;

    picture = fopen(name, "r");
    printf("Getting Picture Size\n");

    if (picture == NULL) {
        printf("Error Opening Image File");
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n", size);
    
    //send SI
    char mensaje_e[] = "SI";
    send(socket, mensaje_e, sizeof (mensaje_e), 0);
    
    char mensaje_r[10];
    if (recv(socket, mensaje_r, sizeof (mensaje_r), 0) == -1)
        return;

    printf("%s", mensaje_r);
    
    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *) &size, sizeof (int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do { //Read while we get errors that are due to signals.
        stat = read(socket, &read_buffer, 255);
        printf("Bytes read: %i\n", stat);
    } while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", read_buffer);

    while (!feof(picture)) {
        //while(packet_index = 1){
        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof (send_buffer) - 1, picture);

        //Send data through our socket 
        do {
            stat = write(socket, send_buffer, read_size);
        } while (stat < 0);

        printf("Packet Number: %i\n", packet_index);
        printf("Packet Size Sent: %i\n", read_size);
        printf(" \n");
        printf(" \n");


        packet_index++;

        //Zero out our send buffer
        bzero(send_buffer, sizeof (send_buffer));
    }
}