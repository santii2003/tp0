#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
    char *ip = (char*)malloc(16 * sizeof(char)); // Para una dirección IP de estilo IPv4
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


	if (config == NULL){
		/*Tenemos que terminar el programa */
			
		/*¿Se libera memoria en la funcion terminar_programa o exteriormente? */
		abort();
	}

	
	// Usando el config creado previamente, leemos los valores del config y los
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// segmetation fault
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
	log_create("tp0.log", valor, true, LOG_LEVEL_INFO);
	/* ---------------- LEER DE CONSOLA ---------------- */
	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje("CLAVE",conexion);
	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log *iniciar_logger(void)
{
	
	t_log *nuevo_logger = log_create("tp0.log", "Hola! Soy un miserable Log", true, LOG_LEVEL_INFO);
	log_info(nuevo_logger,"Ahora si soy un log?"); // parece que log_info hace que imprima los resultados por consola, por más que el log_create?
								
	return nuevo_logger;
}


t_config *iniciar_config(void) 
{
	/*
		- Creamos una config que levante el archivo 'cliente.config'
		- Obtenemos el valor de la key CLAVE en formato string
		
	 */

	t_config *nuevo_config = config_create("cliente.config");
	return nuevo_config;
}


// tenemos que loggear las entradas que vayamos pasando por consola
// y si una es vacia acabamos programa

void leer_consola(t_log *logger)
{
	char *leido;
	
/*Bucle que lee la consola e imprime por pantalla 
	Solo falta que lo logee*/
	while (1) 
	{
	    leido = readline("> "); // Muestra el prompt y lee la entrada del usuario

        if (leido) {
            add_history(leido); // Agrega la entrada al historial
        }
		int corte = 0;

        // Condiciones de corte
        if (strcmp(leido, "exit") == corte || strcmp(leido, "") == corte) {
            free(leido); // Libera la memoria de la línea leída
            break; // Sale del bucle
        }

        printf("%s\n", leido); // Imprime la línea leída
		log_info(logger, "%s",leido); // guarda las lineas en tp0.log!!
        free(leido); // Libera la memoria de la línea leída
    }
	// ¡No te olvides de liberar las lineas antes de regresar!
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!

	//con esta variable, debemos ir juntando las lineas que se leen por consola e 
	//e ir agragandolas al paquete con agregar_a_paquete
	char *leido;


	t_paquete *paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	while (1) 
	{	
		int corte = 0;
	    leido = readline("> "); // Muestra el prompt y lee la entrada del usuario
	   // Condiciones de corte
        if (strcmp(leido, "exit") == corte || strcmp(leido, "") == corte) {
            free(leido); // Libera la memoria de la línea leída
            break; // Sale del bucle
        }
	
	
	
		int longitud_linea = strlen(leido)+1;
		agregar_a_paquete(paquete, leido, longitud_linea);



        free(leido); // Libera la memoria de la línea leída
    }
	eliminar_paquete(paquete);
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config)
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	liberar_conexion(conexion);
	config_destroy(config);
	log_destroy(logger);
}
