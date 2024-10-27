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
    char mensaje[1024], IP_text[20];
    int ipValida = 0, puertoValido = 0;

    //Si no se pasaron los argumentos necesarios:
    if(argc < 3) {
        //Bucle que solicita la IP y el puerto hasta que sean correctos
        do {
            printf("Necesitas indicar la IP donde se ejecuta el servidor y también el puerto.\n");
            //Solicitar y validar la IP
            do {
                printf("La IP es: ");
                //scanf que lee máximo 15 caracteres, 16 si añadimos el '\0'
                scanf(" %15s", IP_text);
                //Estructura para comprobar si la IP es válida
                struct sockaddr_in sa;
                //Almacenamos el valor de retorno de inet_pton (1 si es válida, 0 si no lo es)
                ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr));
                //Si la IP no es válida, lo informa
                if(!ipValida) {
                    printf("Error: IP no válida. Inténtalo de nuevo.\n");
                }
            //El bucle no terminará hasta que sea válida la IP
            }while(!ipValida);
            //Hacemos lo mismo con el puerto
            do {
                printf("Introduce ahora el puerto: ");
                scanf(" %d", &Puerto);
                puertoValido = (Puerto >= 5000 && Puerto <= 65535);
                if(!puertoValido) {
                    printf("Error: Puerto no válido. Debe estar entre 5000 y 65535. Inténtalo de nuevo.\n");
                }
            }while(!puertoValido);
        //El bucle general se repetirá hasta que ambos parámetros sean correctos
        }while(!ipValida || !puertoValido);
    } 
    //Si pasa los argumentos correctamente:
    else {
        strncpy(IP_text, argv[1], sizeof(IP_text) - 1);
        //Asegura que IP_text termine en '\0'
        IP_text[sizeof(IP_text) - 1] = '\0'; 
        //Almacenamos el puerto, pasandolo por la función atoi
        Puerto = atoi(argv[2]);
        //Verificamos la IP de formal similar al caso de si no introduce los argumentos
        struct sockaddr_in sa;
        ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr)) == 1;
        //Hacemos lo mismo con el puerto
        puertoValido = (Puerto >= 5000 && Puerto <= 65535);
        if(!ipValida || !puertoValido) {
            do {
                printf("Necesitas indicar la IP donde se ejecuta el servidor y/o el puerto.\n");
                //Si la ip no es válida, entra en el bucle
                while(!ipValida) {
                    printf("La IP es: ");
                    //scanf que lee máximo 15 caracteres, 16 si añadimos el '\0'
                    scanf(" %15s", IP_text);
                    //Estructura para comprobar si la IP es válida
                    struct sockaddr_in sa;
                    //Almacenamos el valor de retorno de inet_pton (1 si es válida, 0 si no lo es)
                    ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr));
                    //Si la IP no es válida, lo informa
                    if(!ipValida) {
                        printf("Error: IP no válida. Inténtalo de nuevo.\n");
                    }
                }
                //Hacemos lo mismo con el puerto
                while(!puertoValido) {
                    printf("Introduce ahora el puerto: ");
                    scanf(" %d", &Puerto);
                    puertoValido = (Puerto >= 5000 && Puerto <= 65535);
                    if(!puertoValido) {
                        printf("Error: Puerto no válido. Debe estar entre 5000 y 65535. Inténtalo de nuevo.\n");
                    }
                }
            //El bucle general se repetirá hasta que ambos parámetros sean correctos
            }while(!ipValida || !puertoValido);
        }
    }
    printf("IP y Puerto válidos: %s:%d\n", IP_text, Puerto);

    //Creamos el socket del cliente
    /**
     * @param AF_INET: Especificamos que usaremos IPv4
     * @param SOCK_STREAM: Indicamos el tipo de la conexión. Con "SOCK_STREAM" nos referimos a un socket orientado a conexión
     * @param 0: Indicamos el protocolo que queremos usar. Al introducir un "0" usaremos el establecido por defecto
     */
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(Socket < 0) {
        perror("Error creando el socket");
        exit(EXIT_FAILURE);
    }

    //Inicializamos la estructura que almacenará la dirección. Especificamos que usamos IPv4 y almacenamos el puerto al que conectarse
    //Es importante comvertir el puerto de formato host a formato network
    direccion.sin_family = AF_INET; 
    direccion.sin_port = htons(Puerto); 

    //Convertimos la IP de texto a formato binario con la función inet_pton
    /**
     * @param AF_INET: Especificamos la familia de la IP; IPv4
     * @param IP_text: Pasamos la variable donde tenemos almacenada la IP en formato texto
     * @param sin_addr: pasamos la dirección de memoria del campo para almacenar la IP de la estructura donde almacenamos la dirección
     */
    if(inet_pton(AF_INET, IP_text, &direccion.sin_addr) <= 0) {
        perror("Fallo al convertir la IP");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    //Tamaño de la estructura sockaddr_in
    tamanho = sizeof(struct sockaddr_in);

    //Solicitamos la conexión con el servidor con la función "connect"
    /**
     * @param Socket: Socket en el que recibiremos datos, en caso de que se nos envíen
     * @param direccion: Pasamos la dir. mem. de la estructura que almacena la dirección a la que queremos conectarnos (la casteamos a struct sockaddr*)
     * @param tamanho: Introducimos el tamaño de la estructura que almacena la dirección
     */
    if(connect(Socket, (struct sockaddr*) &direccion, tamanho) < 0) {
        perror("No se pudo realizar la conexión");
        close(Socket);
        exit(EXIT_FAILURE);
    }   

    /**
     * 1.c
     * sleep(5);
     * 
     * (Resultados en el informe)
     */

    /**
     * 1.d (sustituír este código por el que hay en el intervalo de líneas [172, 184], incluídas)
     * / Recibir el mensaje del servidor
     *   ssize_t numBytes; // Deja espacio para el terminador nulo
     *   size_t totalBytesReceived = 0;
     *   memset(mensaje, 0, sizeof(mensaje)); // Inicializar el buffer
     *                                                           / sizeof(mensaje) - totalBytesReceived - 1
     *   while ((numBytes = recv(Socket, mensaje + totalBytesReceived, 5, 0)) > 0) {
     *       totalBytesReceived += numBytes;
     *       / Si deseas probar con diferentes tamaños, puedes ajustar el tercer argumento de recv()
     *       / numBytes = recv(Socket, mensaje + totalBytesReceived, 5, 0); // Recibe 5 bytes, por ejemplo
     *       mensaje[totalBytesReceived] = '\0';
     *       / Mostrar el mensaje recibido
     *       printf("El mensaje ha sido: %s\n", mensaje);
     *       printf("El número de bytes recibidos es: %zd\n", totalBytesReceived);
     *   }
     */

    //Con la función "recv" recibimos el mensaje que nos envía el servidor
    /**
     * @param Socket: Especificamos el socket donde recibir el mensaje
     * @param mensaje: Indicamos un buffer donde almacenar el mensaje
     * @param sizeof: indicamos número de bits que queremos leer. En nuestro caso, el que tenemos reservado para la variable de almacenamiento del mensaje,
     * menos 1, para añadir el caracter de final de línea.
     * @param 0: Flags de la función. Introducimos el 0 para que funcione con lo predeterminado
     */
    //Almacenamos el número de bytes recibido, ya que es el valor que devuelve la función.
    ssize_t numBytes = recv(Socket, mensaje, sizeof(mensaje) - 1, 0);
    if(numBytes < 0) {
        perror("Error recibiendo el mensaje");
        close(Socket);
        exit(EXIT_FAILURE);
    }

    //Asegura que el mensaje recibido tenga un final y no exceda la memoria reservada para él
    mensaje[numBytes] = '\0';

    //Mostramos el mensaje recibido y el número de bytes
    printf("El mensaje ha sido: %s\n", mensaje);
    printf("El número de bytes es: %zd\n", numBytes);

    //Cerrar el socket
    if(close(Socket) < 0) {
        perror("Error al cerrar el socket");
        exit(EXIT_FAILURE);
    }

    return 0;
}
