/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "parser/metadata_program.h"
#include "parser/parser.h"
#include "commons/string.h"
#include "commons/collections/list.h"

#include "../../Nuestras/src/laGranBiblioteca/datosGobalesGenerales.h"
#include "../../Nuestras/src/laGranBiblioteca/ProcessControlBlock.c"
#include "../../Nuestras/src/laGranBiblioteca/sockets.c"
#include "../../Nuestras/src/laGranBiblioteca/config.c"

#include "primitivas.h"
#include "compartidas.h"

void conectarConMemoria();
void conectarConKernel();
void pedidoValido(int*,void*,int);
void recibirInstruccion(void*,char**,int,int*);




int main(void)
{
	AnSISOP_funciones AnSISOP_funciones = {
			.AnSISOP_definirVariable = AnSISOP_definirVariable,
			.AnSISOP_obtenerPosicionVariable = AnSISOP_obtenerPosicionVariable,
			.AnSISOP_dereferenciar = AnSISOP_dereferenciar,
			.AnSISOP_asignar = AnSISOP_asignar,
			.AnSISOP_obtenerValorCompartida = AnSISOP_obtenerValorCompartida,
			.AnSISOP_asignarValorCompartida = AnSISOP_asignarValorCompartida,
			.AnSISOP_irAlLabel = AnSISOP_irAlLabel,
			.AnSISOP_llamarSinRetorno = AnSISOP_llamarSinRetorno,
			.AnSISOP_llamarConRetorno = AnSISOP_llamarConRetorno,
			.AnSISOP_finalizar = AnSISOP_finalizar,
			.AnSISOP_retornar = AnSISOP_retornar
	};
	AnSISOP_kernel AnSISOP_funciones_kernel = {
			.AnSISOP_wait = AnSISOP_wait,
			.AnSISOP_signal = AnSISOP_signal,
			.AnSISOP_reservar = AnSISOP_reservar,
			.AnSISOP_liberar = AnSISOP_liberar,
			.AnSISOP_abrir = AnSISOP_abrir,
			.AnSISOP_borrar = AnSISOP_borrar,
			.AnSISOP_cerrar = AnSISOP_cerrar,
			.AnSISOP_moverCursor = AnSISOP_moverCursor,
			.AnSISOP_escribir = AnSISOP_escribir,
			.AnSISOP_leer = AnSISOP_leer
	};

	printf("Inicializando CPU.....\n\n");


	// ******* Configuracion Inicial de CPU

 	printf("Configuracion Inicial: \n");

 	configuracionInicial("/home/utnso/workspace/tp-2017-1c-While-1-recursar-grupo-/CPU/cpu.config");

 	imprimirConfiguracion();

 	conectarConKernel();

 	conectarConMemoria();



 	if(recibirMensaje(socketKernel,(void*)&datosIniciales) != 7) perror("Kernel que es esta basura? Dame mis datos para comenzar");

 	printf("Datos recibidos de Kernel:\nsize_pag-%d\nquantum-%d\nsize_stack-%d\n", datosIniciales->size_pag, datosIniciales->quantum, datosIniciales->size_stack);

 	//ESTE GRAN WHILE(1) ESTA COMENTADO PORQUE EN REALIDAD ES PARA RECIBIR UN PCB ATRAS DE OTRO Y EJECUTARLOS HASTA QUE EL KERNEL ME DIGA MORITE HIPPIE

	while(1){

 		int quantumRestante = datosIniciales->quantum;

 		terminoPrograma = false;
 		bloqueado = false;

		// Recepcion del pcb

		enviarMensaje(socketKernel,pedirPCB,&quantumRestante,sizeof(int));

		void* pcbSerializado;
		puts("esperando pcb\n");
		if(recibirMensaje(socketKernel,&pcbSerializado) != envioPCB) perror("Error en la accion maquinola");


		pcb=deserializarPCB(pcbSerializado);

		free(pcbSerializado);

		while(!terminoPrograma && quantumRestante != 0 && !bloqueado){

			t_pedidoMemoria pedido;
			pedido.id = pcb->pid;
			pedido.direccion = calcularDireccion(pcb->indiceCodigo[pcb->programCounter].start);
			pedido.direccion.size = pcb->indiceCodigo[pcb->programCounter].offset;


			// Pedido de Codigo
			enviarMensaje(socketMemoria,solicitarBytes,(void *)&pedido, sizeof(pedido));


			//Recepcion del codigo ANSISOP
			void* stream;
			int booleano;

			//Se recibe si tal pedido es valido o rompe por todos lados
			int accion = recibirMensaje(socketMemoria,&stream);

			pedidoValido(&booleano,stream,accion);


			//Si el pedido salio bien se pasa a pedir el codigo concretamente
			char* instruccion;


			if(booleano){
				recibirInstruccion(stream,&instruccion,pedido.direccion.size,&accion);
			}else{
				terminoPrograma = true;
				pcb->exitCode = -5;		//Excepcion de Memoria STACK OVERFLOW
			}

			//Si se recibio una linea de codigo se analiza
			if(accion == lineaDeCodigo){
				printf("\n%s\n\n",instruccion);

				//Magia del Parser para llamar a las primitivas
				analizadorLinea(instruccion,&AnSISOP_funciones,&AnSISOP_funciones_kernel);
				pcb->programCounter++;
			}

			free(instruccion);
			free(stream);
			quantumRestante--;
		}

		//ACA AVISARLE A KERNEL QUE TERMINE QUE CON ESTE PROCESO
		if(terminoPrograma){
			printf("Envie el PCB al Kernel porque termine toda su ejecucion\n");
			enviarMensaje(socketKernel,enviarPCBaTerminado,serializarPCB(pcb),tamanoPCB(pcb) + 4);
		}

/*		if(bloqueado){
			printf("Envie el PCB al Kernel porque se bloqueo el programa\n");
			enviarMensaje(socketKernel,enviarPCBaWaiting,serializarPCB(pcb),tamanoPCB(pcb) + 4);
		}
*/
		if(quantumRestante == 0){
			printf("Envie el PCB al Kernel porque me quede sin quantum\n");
			enviarMensaje(socketKernel,enviarPCBaReady,serializarPCB(pcb),tamanoPCB(pcb) + 4);
		}

		//libera la memoria malloqueada por el PCB
		destruirPCB_Puntero(pcb);

	}


	close(socketKernel);
	liberarConfiguracion();
	free(datosIniciales);

	return 0;
}



//******************************************************************FUNCIONES PARA MODULARIZAR Y QUEDE UN LINDO CODIGO*********************************************************************\\


void conectarConMemoria()
{
	int rta_conexion;
	socketMemoria = conexionConServidor(getConfigString("PUERTO_MEMORIA"),getConfigString("IP_MEMORIA")); // Asignación del socket que se conectara con la memoria
	if (socketMemoria == 1){
		perror("Falla en el protocolo de comunicación");
	}
	if (socketMemoria == 2){
		perror("No se conectado con el Memoria, asegurese de que este abierto el proceso");
	}
	if ( (rta_conexion = handshakeCliente(socketMemoria, CPU)) == -1) {
				perror("Error en el handshake con Memoria");
				close(socketMemoria);
	}
	printf("Conexión exitosa con el Memoria(%i)!!\n",rta_conexion);
}

void conectarConKernel()
{
	int rta_conexion;
	socketKernel = conexionConServidor(getConfigString("PUERTO_KERNEL"),getConfigString("IP_KERNEL")); // Asignación del socket que se conectara con la memoria
	if (socketKernel == 1){
		perror("Falla en el protocolo de comunicación");
	}
	if (socketKernel == 2){
		perror("No se conectado con el Kernel, asegurese de que este abierto el proceso");
	}
	if ( (rta_conexion = handshakeCliente(socketKernel, CPU)) == -1) {
				perror("Error en el handshake con Kernel");
				close(socketMemoria);
	}
	printf("Conexión exitosa con el Kernel(%i)!!\n",rta_conexion);
}

void pedidoValido(int* booleano,void* stream, int accion){
	switch(accion){
		case RespuestaBooleanaDeMemoria:{
			memcpy(booleano,stream,sizeof(int));
		}break;
		default:{
			perror("Error en la accion maquinola");
		}break;
	}
}

void recibirInstruccion(void *stream, char** instruccion, int size, int* accion){
	*accion = recibirMensaje(socketMemoria,&stream);
	switch(*accion){
		case lineaDeCodigo:{
			*instruccion = stream;
			(*instruccion)[size - 1] = '\0';
		}break;
		default:{
			perror("Error en la accion maquinola");
		}break;
	}
}













