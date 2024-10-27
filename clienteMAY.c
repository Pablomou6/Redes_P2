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
    int Socket, Puerto, i, archivoValido = 0, ipValida = 0, puertoValido = 0;
    struct sockaddr_in direccion;
    socklen_t tamanho;
    char mensaje[1024], IP_text[20], archivo[20], salida[20];
    
    // Verificación de que se pasen los argumentos necesarios
    if(argc < 4) {
        // Solicitar archivo hasta que sea válido
        do {
            printf("Necesitas leer un archivo de texto EXISTENTE. Introduce su nombre:\n");
            scanf(" %[^\n\r]", archivo);
            // Verificar si el archivo existe y tiene extensión .txt
            FILE *file = fopen(archivo, "r");
            //Al intentar abrirlo, si da error no existe, y con strstr comprobamos que tenga la extensión .txt
            archivoValido = (file != NULL && strstr(archivo, ".txt") != NULL);
            //Si el archivo es correcto, lo cerramos, simplemente.
            if(file) {
                fclose(file);
            }
            //Si no es válido, lo pedimos de nuevo
            if(!archivoValido) {
                printf("Error: El archivo no es válido o no existe. Debe ser un archivo .txt.\n");
            }
        }while(!archivoValido);
        // Solicitar IP hasta que sea válida
        do {
            printf("Necesitas indicar la IP donde se ejecuta el servidor.\nLa IP es: ");
            scanf(" %15s", IP_text);
            //Estructura auxiliar
            struct sockaddr_in sa;
            //Si no pueod convertir la IP, la IP no es válida.
            ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr));
            if(!ipValida) {
                printf("Error: IP no válida. Inténtalo de nuevo.\n");
            }
        }while(!ipValida);
        // Solicitar puerto hasta que sea válido
        do {
            printf("Introduce ahora el puerto (entre 5000 y 65535): ");
            scanf(" %d", &Puerto);
            puertoValido = (Puerto >= 5000 && Puerto <= 65535);
            if(!puertoValido) {
                printf("Error: Puerto no válido. Debe estar entre 5000 y 65535.\n");
            }
        }while(!puertoValido);
    }
    // Si se pasan los argumentos correctamente
    else {
        // Verificar archivo
        //Lo copiamos a la variable
        strncpy(archivo, argv[1], sizeof(archivo) - 1);
        archivo[sizeof(archivo) - 1] = '\0';
        //Lo intentamos abrir
        FILE *file = fopen(archivo, "r");
        //Si el archivo dio error al abrirlo, ya que no existe, o no tiene la extensión .txt, no es válido
        archivoValido = (file != NULL && strstr(archivo, ".txt") != NULL);
        //Si es válido, lo cerramos
        if(file) {
            fclose(file);
        }
        if(!archivoValido) {
            while(!archivoValido) {
                printf("Error con el archivo. Necesitas leer un archivo de texto EXISTENTE. Introduce su nombre:\n");
                scanf(" %[^\n\r]", archivo);
                // Verificar si el archivo existe y tiene extensión .txt
                FILE *file = fopen(archivo, "r");
                //Al intentar abrirlo, si da error no existe, y con strstr comprobamos que tenga la extensión .txt
                archivoValido = (file != NULL && strstr(archivo, ".txt") != NULL);
                //Si el archivo es correcto, lo cerramos, simplemente.
                if(file) {
                    fclose(file);
                }
                //Si no es válido, lo pedimos de nuevo
                if(!archivoValido) {
                    printf("Error: El archivo no es válido o no existe. Debe ser un archivo .txt.\n");
                }
            }
        }
        //Copiamos la IP a una variable
        strncpy(IP_text, argv[2], sizeof(IP_text) - 1);
        IP_text[sizeof(IP_text) - 1] = '\0';
        //Estructura auxikiar
        struct sockaddr_in sa;
        //Si la función pudo convertir la IP, es una Ip válida
        ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr));
        if(!ipValida) {
            //La IP se pedirá constantemente si no es válida
            while(!ipValida) {
                printf("Error con la IP. Necesitas indicar la IP donde se ejecuta el servidor.\nLa IP es: ");
                scanf(" %15s", IP_text);
                //Estructura auxiliar
                struct sockaddr_in sa;
                //Si no pueod convertir la IP, la IP no es válida.
                ipValida = inet_pton(AF_INET, IP_text, &(sa.sin_addr));
                if(!ipValida) {
                    printf("Error: IP no válida. Inténtalo de nuevo.\n");
                }
            }
        }
        // Verificar puerto
        Puerto = atoi(argv[3]);
        puertoValido = (Puerto >= 5000 && Puerto <= 65535);
        if (!puertoValido) {
            while(!puertoValido) {
                printf("Error con el puerto. Introduce ahora el puerto (entre 5000 y 65535): ");
                scanf(" %d", &Puerto);
                puertoValido = (Puerto >= 5000 && Puerto <= 65535);
                if(!puertoValido) {
                    printf("Error: Puerto no válido. Debe estar entre 5000 y 65535.\n");
                }
            }
        }
    }
    printf("Archivo, IP y puerto válidos: %s, %s:%d\n", archivo, IP_text, Puerto);

    //Convertimos el nombre del archivo a mayúsculas
    for (i = 0; i < strlen(archivo); i++) {
        //Bucle que convierte el nombre del archivo a mayúsculas, excepto al punto
        if (archivo[i] != '.') {
            salida[i] = toupper(archivo[i]);
        } 
        //Copiamos el punto, sin usar la función toupper sobre él
        else {
            salida[i] = archivo[i];  
        }
    }
    //Asegurarse de que la cadena tenga el terminador nulo
    salida[i] = '\0';

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

    //Abrimos el archivo de entrada
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

    //Bucle que lee el archivo de entrada línea a línea, mientras tenga líneas para leer
    while(fgets(mensaje, sizeof(mensaje), doc) != NULL) {
        //Con la función send, mandamos el mensaje deseado al cliente. Esta función devuelve el número de bytes enviado
        /**
         * @param SocketConnet: socket al que mandamos el mensaje (el del servidor)
         * @param str: String que almacena el mensaje a enviar
         * @param strlen: Indicamos, en este argumento, la longitud del mensaje a enviar.
         * @param 0: Flags de la función. "0" es el valor por defecto
         */
        ssize_t TamMsg = send(Socket, mensaje, strlen(mensaje), 0);
        if(TamMsg < 0){
            perror("No se pudo enviar el mensaje.\n");         
            close(Socket);
            exit(EXIT_FAILURE);
        }
        //Con la función "recv" recibimos el mensaje que nos envía el servidor
        /**
         * @param Socket: Especificamos el socket donde recibir el mensaje
         * @param mensaje: Indicamos un buffer donde almacenar el mensaje
         * @param sizeof: indicamos número de bits que queremos leer. En nuestro caso, el que tenemos reservado para la variable de almacenamiento del mensaje,
         * menos 1, para añadir el caracter de final de línea.
         * @param 0: Flags de la función. Introducimos el 0 para que funcione con lo predeterminado
         */
        //Almacenamos el número de bytes recibido, ya que es el valor que devuelve la función.
        ssize_t numBytes = recv(Socket, mensaje, sizeof(mensaje) - 1, 0); // Deja espacio para el terminador nulo
        if(numBytes < 0) {
            perror("Error recibiendo el mensaje");
            close(Socket);
            exit(EXIT_FAILURE);
        }
        //Asegura que el mensaje recibido sea una cadena válida
        mensaje[numBytes] = '\0';
        //Imprimir en el archivo de salida el mensaje en mayúsculas
        fprintf(output, "%s", mensaje);
        /**
         * Apartado 3
         * Añadimos un sleep para tener tiempo de crear un cliente en otra terminal
         * sleep(10);
         */
    }

    //Cerrar el socket
    if(close(Socket) < 0) {
        perror("Error al cerrar el socket");
        exit(EXIT_FAILURE);
    }
    //Cerramos ambos archivos acabada la ejecución.
    fclose(doc);
    fclose(output);

    return 0;
}