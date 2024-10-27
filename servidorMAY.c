#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>

//Función que convierte un str dado, en el mismo str, pero en mayúsculas
void convertToUpperCase(char *str) {
    //Bucle que, mientras la i-ésima posición del str es diferente del caracter de fin de línea, converte la posición en mayúscula
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char* argv[]) {
    int SocketServer, SocketConnect;
    struct sockaddr_in dirserv, dircli;
    socklen_t length;
    char str[1024];
    int PuertoServ = 0, puertoValido;

    //Verificar si se pasó un argumento de puerto
    if(argc < 2) {
        //Si no se pasó como argumento:
        do {
            printf("Introduce ahora el puerto (Introduce uno entre 5000 y 65535): ");
            scanf(" %d", &PuertoServ);
            puertoValido = (PuertoServ >= 5000 && PuertoServ <= 65535);
            if(!puertoValido) {
                printf("Error: Puerto no válido. Debe estar entre 5000 y 65535. Inténtalo de nuevo.\n");
            }
            //El bucle se repitirá hasta que el puerto sea válido
        }while(!puertoValido);
    }
    else {
        PuertoServ = atoi(argv[1]);
        puertoValido = (PuertoServ >= 5000 && PuertoServ <= 65535);
        //Si no es correcto, hacemos un bucle que lo pida constantemente hasta serlo
        if(!puertoValido) {
            while(!puertoValido) {
                printf("Introduce un puerto correcto (Introduce uno entre 5000 y 65535): ");
                scanf(" %d", &PuertoServ);
                puertoValido = (PuertoServ >= 5000 && PuertoServ <= 65535);
                if(!puertoValido) {
                    printf("Error: Puerto no válido. Debe estar entre 5000 y 65535. Inténtalo de nuevo.\n");
                }
            }
        }
    }
    printf("Puerto introducido correcto.\n");

    //Creamos el socket con la función "socket".
    /**
     * @param AF_INET: Especificamos el dominio de la IP, que utilizaremos IPv4
     * @param SOCK_STREAM: Indicamos el tipo de la conexión. Con "SOCK_STREAM" nos referimos a un socket orientado a conexión
     * @param 0: El tercer parámetro se refiere al protocolo a usar, en este caso ponemos "0" para usar los predeterminados.
     */
    SocketServer = socket(AF_INET, SOCK_STREAM, 0);
    if(SocketServer < 0) {
        perror("Error creando el socket.\n");
        exit(EXIT_FAILURE);
    }

    //Inicializamos la estructura de la dirección del servidor. En la familia, indicamos que usaremos IPv4, la dirección utilizamos "INADDR_ANY" ya que 
    //nos interesa aceptar cualquier mensaje de cualquier dirección. Importante usar la función htonl, para cambiar así del formato de bytes de host 
    //al formato de bytes de network. Por último, indicamos un puerto, aplicándole la misma función anterior (pero la versión short, debido a tener 16 bits, no 32)
    dirserv.sin_family = AF_INET;
    dirserv.sin_addr.s_addr = htonl(INADDR_ANY);
    dirserv.sin_port = htons(PuertoServ);

    //Con la función bind, le asignamos dirección y puerto al socket del servidor.
    /**
     * @param SocketServer: Socket la que queremos asignarle dirección y puerto.
     * @param dirserv: Debemos pasar la dirección de memoria de la estructura donde tenemos la diercción a asignar al socket (casteandola como struct sockaddr*)
     * @param sockaddr_in: Le pasamos, con sizeof, el tamaño  de la estructura de tipo sockaddr_in
     */
    if(bind(SocketServer, (struct sockaddr*) &dirserv, sizeof(struct sockaddr_in)) < 0) {
        perror("No se pudo asignar una dirección.\n");
        exit(EXIT_FAILURE);
    }

    //Marcamos el socket como pasivo con la función listen, para recibir peticiones
    /**
     * @param SocketServer: Indicamos el socket que queremos marcar como pasivo
     * @param 6: Indicamos el número máximo de peticiones que guardaremos en la cola.
     */
    if(listen(SocketServer, 6) < 0) {
        perror("El socket no pudo hacerse pasivo.\n");
        exit(EXIT_FAILURE);
    }
    
    //Almacenamos el tamaño de la dirección del cliente
    length = sizeof(dircli);

    //Bucle infinito para aceptar peticiones
    while(1) {
        //Con la función accept aceptaremos las conexiones
        /**
         * @param SocketServer: Indicamos el socket que aceptará las conexiones
         * @param dircli: Como en bind, pasamos la dir. mem. de la estructura que almacena la dirección del cliente (la casteamos a struct sockaddr*)
         * @param length: Pasamos la dir. mem. de la variable que almacena el tamaño de la estructura que guarda la dirección del cliente
         */
        if((SocketConnect = accept(SocketServer, (struct sockaddr*) &dircli, &length)) < 0) {
            perror("No se pudo aceptar la conexión.\n");
            close(SocketServer);
            exit(EXIT_FAILURE);
        }
        //Declaramos una variable de tipo array de chars, con tamaño "INET_ADDRSTRLEN", macro que almacena el tamaño de una IP
        char IP[INET_ADDRSTRLEN];
        //Con la función "inet_ntop" convertimos una IP de formato binario a formato texto
        /**
        * @param AF_INET: Indicamos que usaremos una IPv4
        * @param sin_addr: Campo de la estructura que almacena la dirección del cliente. Le pasamos la dirección de memoria
        * @param IP: Variable donde queremos almacenar la IP convertida
        * @param INET_ADDSTRLEN: Macro que indica el tamaño disponible para almacenar la conversión de la IP (el tamaño de la variable donde almacenarla)
        */
        inet_ntop(AF_INET, &(dircli.sin_addr), IP, INET_ADDRSTRLEN);
        printf("Se ha conectado la IP %s . Puerto: %d.\n", IP, ntohs(dircli.sin_port));

        //Bucle para modificar las líneas enviadas por el cliente y devolverlas
        while(1) {
            //Con la función "recv" recibimos el mensaje que nos envía el cliente
            /**
             * @param SocketConnect: Especificamos el socket donde recibir el mensaje
             * @param str: Indicamos un buffer donde almacenar el mensaje
             * @param 1023: indicamos número de bits que queremos leer. En nuestro caso, el que tenemos reservado para la variable de almacenamiento del mensaje,
             * menos 1, para añadir el caracter de final de línea.
             * @param 0: Flags de la función. Introducimos el 0 para que funcione con lo predeterminado
             */
            //Almacenamos el número de bytes recibido, ya que es el valor que devuelve la función.
            ssize_t bytes_received = recv(SocketConnect, str, 1024 - 1, 0);
            //Si es 0 o igual a 0, hubo error al transmitir el mensaje.
            if(bytes_received <= 0) {
                break;
            }
            //Asegurar que termina el mensaje
            str[bytes_received] = '\0';  
            //Convertir a mayúsculas
            convertToUpperCase(str);
            //Con la función send, mandamos el mensaje deseado al cliente. Esta función devuelve el número de bytes enviado
            /**
             * @param SocketConnet: socket al que mandamos el mensaje (el del cliente)
             * @param str: String que almacena el mensaje a enviar
             * @param strlen: Indicamos, en este argumento, la longitud del mensaje a enviar.
             * @param 0: Flags de la función. "0" es el valor por defecto
             */
            if(send(SocketConnect, str, strlen(str), 0) < 0) {
                perror("Error al enviar datos al cliente");
                break;
            }
        }
        //Cerramos la conexión con el cliente.
        close(SocketConnect); 
    }
}