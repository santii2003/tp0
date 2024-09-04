#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
    char *ip = malloc(16 * sizeof(char)); // Para una dirección IP de estilo IPv4
    char *puerto = (char*)malloc(6 * sizeof(char)); // Para un puerto (hasta 5 dígitos más terminador)
    char *valor = NULL; // Valor se inicializará desde la config directamente
	t_log *logger;
	t_config *config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	
	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	config = iniciar_config();


	// Usando el config creado previamente, leemos los valores del config y los
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	if (config_has_property(config, "IP")){
		strcpy(ip,  config_get_string_value(config, "IP"));
	}
	if (config_has_property(config, "PUERTO")){
		strcpy(puerto,  config_get_string_value(config, "PUERTO"));
		
	}
	if (config_has_property(config, "CLAVE")){
		valor = config_get_string_value(config, "CLAVE");	
	}

	// Loggeamos el valor de config
	log_info(logger, valor, true, LOG_LEVEL_INFO);

	/* ---------------- LEER DE CONSOLA ---------------- */
	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	
	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(puerto,conexion);
	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
}

t_log *iniciar_logger(void)
{
	
	t_log *nuevo_logger = log_create("tp0.log", "Hola! Soy un miserable Log", true, LOG_LEVEL_INFO);
	log_info(nuevo_logger,"Ahora si soy un log?");
	return nuevo_logger;
}


t_config *iniciar_config(void) 
{
	t_config *nuevo_config = config_create("cliente.config");
	if (nuevo_config == NULL){
		error_show("No se encontro el archivo en el path");
		abort();
	}
	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	char *leido;
	
	leido = readline("> "); 
	log_info(logger, "%s",leido); 
	

	while(strcmp(leido, "") != 0){
		free(leido);
		leido = readline("> "); 
		log_info(logger, "%s",leido); 
	
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char *leido = NULL;
	t_paquete *paquete = crear_paquete();

	leido = readline("> "); 
	
	while(strcmp(leido, "") != 0){
		agregar_a_paquete(paquete, leido, (strlen(leido)+1));
		free(leido);
		//volvemos a leer una nueva línea
		leido = readline("> "); 
	}

	/*Enviamos paquete al socket conexion*/
	enviar_paquete(paquete, conexion);

	/*Liberamos las líneas leidas y el paquete enviado*/
	free(leido);
	eliminar_paquete(paquete);
	
}




void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	liberar_conexion(conexion);
	config_destroy(config);
	log_destroy(logger);
	/*Mensaje por pantalla que muestra si terminó o no el cliente*/
	printf("Cliente cerrado!!!! \n");
}
