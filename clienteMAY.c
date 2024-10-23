#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>

int main(int argc, char* argv[]) {
    int Socket, Puerto, i;
    struct sockaddr_in direccion;
    socklen_t tamanho;
    char mensaje[512], IP_text[20], archivo[20], salida[20];
    
    // Verificación de argumentos
    if(argc < 4) {
        printf("Necesitas leer un archivo de texto. INtroduce su nombre.\n");
        scanf(" %[^\n\r]", archivo);
        printf("Necesitas indicar la IP donde se ejecuta el servidor y también el puerto.\n");
        printf("La IP es: ");
        scanf(" %s", IP_text);
        printf("\n");
        printf("Introduce ahora el puerto: ");
        scanf(" %d", &Puerto);
        printf("\n");
    }
    else {
        strncpy(archivo, argv[1], sizeof(archivo) - 1);
        archivo[sizeof(archivo) - 1] = '\0';
        strncpy(IP_text, argv[2], sizeof(IP_text) - 1);
        IP_text[sizeof(IP_text) - 1] = '\0'; // Asegura que IP_text termine en null
        Puerto = atoi(argv[3]);
    }

    //Convertimos el nombre del archivo a mayúsculas
    for (i = 0; i < strlen(archivo); i++) {
        if (archivo[i] != '.') {
            salida[i] = toupper(archivo[i]);  // Convertir a mayúsculas
        } else {
            salida[i] = archivo[i];  // Copiar el punto
        }
    }
    // Asegurarse de que la cadena tenga el terminador nulo
    salida[i] = '\0';

    // Crear el socket
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(Socket < 0) {
        perror("Error creando el socket");
        exit(EXIT_FAILURE);
    }

    // Inicialización de la estructura sockaddr_in
    direccion.sin_family = AF_INET; 
    direccion.sin_port = htons(Puerto);  // Convierte el puerto a formato de red

    // Conversión de la IP de texto a formato binario
    if(inet_pton(AF_INET, IP_text, &direccion.sin_addr) <= 0) {
        perror("Fallo al convertir la IP");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    // Tamaño de la estructura sockaddr_in
    tamanho = sizeof(struct sockaddr_in);

    // Solicitar conexión con el servidor
    if(connect(Socket, (struct sockaddr*) &direccion, tamanho) < 0) {
        perror("No se pudo realizar la conexión");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    //lectura del archivo .txt
    FILE* doc = fopen(archivo, "r");
    if(doc == NULL) {
        perror("Error al abrir el archivo de entrada.\n");
        exit(EXIT_FAILURE);
    }
    //Apertura del archivo de salida
    FILE* output = fopen(salida, "w");
    if(output == NULL) {
        perror("Error al abrir el archivo de salida.\n");
        exit(EXIT_FAILURE);
    }

    //Procesamos el archivo línea a línea y se envía
    while(fgets(mensaje, sizeof(mensaje), doc) != EOF) {
        ssize_t TamMsg = send(Socket, mensaje, strlen(mensaje), 0);
        if(TamMsg < 0){
            perror("No se pudo enviar el mensaje.\n");         
            close(Socket);
            exit(EXIT_FAILURE);
        }
        // Recibir el mensaje del servidor
        ssize_t numBytes = recv(Socket, mensaje, sizeof(mensaje) - 1, 0); // Deja espacio para el terminador nulo
        if(numBytes < 0) {
            perror("Error recibiendo el mensaje");
            close(Socket);
            exit(EXIT_FAILURE);
        }
        // Asegura que el mensaje recibido sea una cadena válida
        mensaje[numBytes] = '\0';
        //Imprimir en el archivo de salida el mensaje e mayúsculas
        fprintf(output, "%s\n", mensaje);
    }

    // Cerrar el socket
    if(close(Socket) < 0) {
        perror("Error al cerrar el socket");
        exit(EXIT_FAILURE);
    }

    fclose(doc);
    fclose(output);

    return 0;
}