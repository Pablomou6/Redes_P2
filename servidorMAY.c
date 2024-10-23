#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>

void convertToUpperCase(char *str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char* argv[]) {
    int SocketServer, SocketConnect;
    struct sockaddr_in dirserv, dircli;
    socklen_t length;
    char str[512];
    int PuertoServ = 0;

    // Verificar si se pasó un argumento de puerto
    if(argc < 2) {
        printf("Por favor, proporciona un puerto.\n");
        return -1;
    }

    //Línea para probar o commit
    // Convertir el argumento a número
    PuertoServ = atoi(argv[1]);

    // Validar el rango del puerto
    if(PuertoServ < 5000 || PuertoServ > 49151) {
        do {
            printf("Puerto no válido. Introduce uno entre 5000 y 49151.\n");
            scanf(" %d", &PuertoServ);
        } while(PuertoServ < 5000 || PuertoServ > 49151);
    }

    //Creamos el socket
    SocketServer = socket(AF_INET, SOCK_STREAM, 0);
    if(SocketServer < 0) {
        perror("Error creando el socket.\n");
        exit(EXIT_FAILURE);
    }

    //Asignamos dirección y puerto al socket
    dirserv.sin_family = AF_INET;
    dirserv.sin_addr.s_addr = htonl(INADDR_ANY);
    dirserv.sin_port = htons(PuertoServ);
    if(bind(SocketServer, (struct sockaddr*) &dirserv, sizeof(struct sockaddr_in)) < 0) {
        perror("No se pudo asignar una dirección.\n");
        exit(EXIT_FAILURE);
    }

    //Marcamos el socket como pasivo para recibir peticiones
    if(listen(SocketServer, 6) < 0) {
        perror("El socket no pudo hacerse pasivo.\n");
        exit(EXIT_FAILURE);
    }
    
    length = sizeof(dircli);

    while(1) {
        if((SocketConnect = accept(SocketServer, (struct sockaddr*) &dircli, &length)) < 0) {
            perror("No se pudo aceptar la conexión.\n");
            close(SocketServer);
            exit(EXIT_FAILURE);
        }
        char IP[INET_ADDRSTRLEN];
        
        inet_ntop(AF_INET, &(dircli.sin_addr), IP, INET_ADDRSTRLEN);
        printf("Se ha conectado la IP %s . Puerto: %d.\n", IP, ntohs(dircli.sin_port));

        // Procesar líneas del cliente
        while(1) {
            ssize_t bytes_received = recv(SocketConnect, str, 512 - 1, 0);
            if(bytes_received <= 0) {
                break;  // Cliente cerró la conexión o error
            }
            str[bytes_received] = '\0';  // Asegurar que termina en null
            // Convertir a mayúsculas
            convertToUpperCase(str);
            // Enviar línea convertida de vuelta al cliente
            if(send(SocketConnect, str, strlen(str), 0) < 0) {
                perror("Error al enviar datos al cliente");
                break;
            }
        }
        close(SocketConnect); 
    }
}