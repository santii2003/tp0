#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	/*assert(!"no implementado!");*/

	int socket_servidor;
	int err_serv;
	errno = 0; //así no indica error previo

	struct addrinfo hints, *servinfo; //*p -> agregada si la funcion no está

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);
	/*Comprobación de error*/
	if (getaddrinfo(NULL, PUERTO, &hints, &servinfo) != 0){
		printf("Error al realizar gettadrrinfo: %s\n", strerror(errno));
		abort();
	}


	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
							servinfo->ai_socktype, 
							servinfo->ai_protocol);

	//permitimos que varios sockets puedan hacer bind a un mismo puerto
	err_serv = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));



	// Asociamos el socket a un puerto
	err_serv = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err_serv == -1) {
		error_show("No se pudo hacer bind");
		abort();
	}

	// Escuchamos las conexiones entrantes		
	err_serv = listen(socket_servidor, SOMAXCONN);
	if (err_serv == -1) {
		error_show("No se pudo hacer listen");
		abort();
	}


	freeaddrinfo(servinfo);
	log_trace(logger, "%s","Listo para escuchar a mi cliente");
	return socket_servidor;
}


// bloqueamos el proceso servidor!!
int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");
	return socket_cliente;
}

// send y recv deben saber cuantos bytes van a recibir
int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
