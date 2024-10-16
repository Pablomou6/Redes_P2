#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
    int Socket;
    struct sockaddr_in direccion;
    char mensaje[1024]; // Asegúrate de que el tamaño sea suficiente
    int Puerto;
    char IP_text[20];

    if(argc < 3) {
        printf("Necesitas indicar la IP donde se ejecuta el servidor y el puerto.\n");
        printf("La IP es: ");
        scanf(" %s", IP_text);
        printf("Introduce el puerto: ");
        scanf(" %d", &Puerto);
    } else {
        strncpy(IP_text, argv[1], sizeof(IP_text) - 1);
        IP_text[sizeof(IP_text) - 1] = '\0';
        Puerto = atoi(argv[2]);
    }

    // Crear el socket
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(Socket < 0) {
        perror("Error creando el socket.\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar la estructura sockaddr_in
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(Puerto);
    if(inet_pton(AF_INET, IP_text, &direccion.sin_addr) <= 0) {
        perror("Fallo al convertir la IP.\n");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    // Solicitar conexión
    if(connect(Socket, (struct sockaddr*) &direccion, sizeof(direccion)) < 0) {
        perror("No se pudo realizar la conexión.\n");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    // Esperar un segundo para dar tiempo al servidor a enviar ambos mensajes
    sleep(1);

    // Recibir el mensaje del servidor
    ssize_t numBytes = recv(Socket, mensaje, sizeof(mensaje) - 1, 0); // Deja espacio para el terminador nulo
    if(numBytes < 0) {
        perror("El mensaje no se ha podido recibir.\n");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    mensaje[numBytes] = '\0'; // Asegura que el mensaje sea una cadena válida

    // Mostrar el mensaje recibido
    printf("El mensaje ha sido: %s\n", mensaje);
    printf("El número de bytes recibidos es: %zd\n", numBytes);

    // Cerrar el socket
    if(close(Socket) < 0) {
        perror("Error al cerrar el socket.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

