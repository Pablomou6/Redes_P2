#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
    int Socket, Puerto;
    struct sockaddr_in direccion;
    socklen_t tamanho;
    char mensaje[512], IP_text[20];
    
    // Verificación de argumentos
    if(argc < 3) {
        printf("Necesitas indicar la IP donde se ejecuta el servidor y también el puerto.\n");
        printf("La IP es: ");
        scanf(" %s", IP_text);
        printf("\n");
        printf("Introduce ahora el puerto: ");
        scanf(" %d", &Puerto);
        printf("\n");
    }
    else {
        strncpy(IP_text, argv[1], sizeof(IP_text) - 1);
        IP_text[sizeof(IP_text) - 1] = '\0'; // Asegura que IP_text termine en null
        Puerto = atoi(argv[2]);
    }

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

    //Hacemos un sleep para que el servidor envíe ambos mensajes.
    sleep(5);
    // Recibir el mensaje del servidor
    ssize_t numBytes = recv(Socket, mensaje, sizeof(mensaje) - 1, 0); // Deja espacio para el terminador nulo
    if(numBytes < 0) {
        perror("Error recibiendo el mensaje");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    // Asegura que el mensaje recibido sea una cadena válida
    mensaje[numBytes] = '\0';

    // Mostrar el mensaje recibido
    printf("El mensaje ha sido: %s\n", mensaje);
    printf("El número de bytes es: %zd\n", numBytes);

    // Cerrar el socket
    if(close(Socket) < 0) {
        perror("Error al cerrar el socket");
        exit(EXIT_FAILURE);
    }

    return 0;
}
