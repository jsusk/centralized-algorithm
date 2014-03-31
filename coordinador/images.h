/* 
 * File:   images.h
 * Author: sacasher
 *
 * Created on 18 de marzo de 2014, 09:35 PM
 */

#ifndef IMAGES_H
#define	IMAGES_H

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct BMP {
    char bm[2]; //(2 Bytes) BM (Tipo de archivo)
    int tamano; //(4 Bytes) Tamaño del archivo en bytes
    int reservado; //(4 Bytes) Reservado
    int offset; //(4 Bytes) offset, distancia en bytes entre la img y los píxeles
    int tamanoMetadatos; //(4 Bytes) Tamaño de Metadatos (tamaño de esta estructura = 40)
    int alto; //(4 Bytes) Ancho (numero de píxeles horizontales)
    int ancho; //(4 Bytes) Alto (numero de pixeles verticales)
    short int numeroPlanos; //(2 Bytes) Numero de planos de color
    short int profundidadColor; //(2 Bytes) Profundidad de color (debe ser 24 para nuestro caso)
    int tipoCompresion; //(4 Bytes) Tipo de compresión (Vale 0, ya que el bmp es descomprimido)
    int tamanoEstructura; //(4 Bytes) Tamaño de la estructura Imagen (Paleta)
    int pxmh; //(4 Bytes) Píxeles por metro horizontal
    int pxmv; //(4 Bytes) Píxeles por metro vertical
    int coloresUsados; //(4 Bytes) Cantidad de colores usados 
    int coloresImportantes; //(4 Bytes) Cantidad de colores importantes
    unsigned char **pixelR; //Puntero a los pixeles del plano R (Rojo)
    unsigned char **pixelG; //Puntero a los pixeles del plano G (Verde)
    unsigned char **pixelB; //Puntero a los pixeles del plano B (Azul)
} BMP;


//*****************************************************************
//DECLARACIÓN DE FUNCIONES
//*****************************************************************
void abrir_imagen(BMP *imagen, char ruta[]); //Función para abrir la imagen BMP
void crear_imagen(BMP *imagen, char ruta[]); //Función para crear una imagen BMP

/**
 * Guarda en un archivo (ej: contador.log) los datos recibidos
 * @param nameFile path del archivo a crear
 * @param dataWrite Datos a escribir en el archivo
 */
void setFileData(char *nameFile, char *dataWrite);

/**
 * Obtiene los datos guardados en el archivo contador.log
 * @param nameFile Path del archivo a leer
 * @param dataRead Buffer de almacenamiento de la lectura del archivo
 */
void getFileData(char *nameFile, char *dataRead);
/**
 * Procesar la imagen recibida y guardarla
 * @param imageName Path de la imagen a tratar
 * @param contador número de imagen a tratar, sirve como nombre para el archivo
 * de salida
 */
void imageProcessing(char* imageName, int contador);
/**
 * Enviar un mensaje al servidor (TCP)
 * @param socket Descriptor del socket
 * @param message Mensaje a enviar
 * @return -1 si falla el envío
 */
int sendMessage(int socket, char *message);


/**
 * Escribir en archivo el archivo recibido
 * @param nameFile Path del archivo a salvar
 * @param socket Socket Descriptor
 * @param params tamaño del archivo param[0]=bloques a recibir param[1]=tam restante
 */
void writeDataFile(char *nameFile, int socket, int params[]);

/**
 * Envía una imagen 
 * @param socket Socket descriptor
 * @param name Nombre o path de la imagen a enviar
 * @return -1 en caso de error, 1 en caso contrario
 */
int send_image(int socket,char *name);

/**
 * Recibe una imagen y la almacena en disco
 * @param socket Socket descriptor
 * @param name Nombre o path para la imagen
 * @return -1 n caso de error, 1 en caso contrario
 */
int receive_image(int socket,char *name);

int send_image_to_dos(int socket,char *name);

#ifdef	__cplusplus
}
#endif

#endif	/* IMAGES_H */

