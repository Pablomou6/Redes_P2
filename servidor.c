#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
    int SocketServer, SocketConnect;
    struct sockaddr_in dirserv, dircli;
    socklen_t length;
    char str[] = "Hola! ¿Qué tal?\n";
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
        //! Non fijo que se poida usar a función, pero as que están restringidas son as 
        //! de conversión de host a network e viceversa
        inet_ntop(AF_INET, &(dircli.sin_addr), IP, INET_ADDRSTRLEN);
        printf("Se ha conectado la IP %s . Puerto: %d.\n", IP, ntohs(dircli.sin_port));

        ssize_t TamMsg = send(SocketConnect, str, strlen(str), 0);
        if(TamMsg < 0){
            perror("No se pudo enviar el mensaje.\n");         
            close(SocketConnect);
            exit(EXIT_FAILURE);
        }
        printf("El número de bytes que fueron transmitidos es: %zd\n    ", TamMsg);
        close(SocketConnect); 
    }
}
