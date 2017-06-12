/*
 * datosGobalesGenerales.h
 *
 *  Created on: 18/5/2017
 *      Author: utnso
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
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "commons/collections/dictionary.h"
#include "ProcessControlBlock.h"

#ifndef DATOSGOBALESGENERALES_H_
#define DATOSGOBALESGENERALES_H_

enum id_Modulos{
	Kernel = 0,
	CPU = 1,
	Memoria = 2,
	Consola = 3,
	FileSystem = 4
};

typedef struct{
	int size;
	int offset;
	char* path;
}__attribute__((packed)) t_pedidoFS;

typedef struct{
	int size;
	int offset;
	char* path;
	void* buffer;
}__attribute__((packed)) t_escritura;

typedef struct{
	int id;
	t_direccion direccion;
	void* valor;
}__attribute__((packed)) t_escrituraMemoria;

typedef struct{
	int id;
	t_direccion direccion;
}__attribute__((packed)) t_pedidoMemoria;

typedef struct{
	int valor;
	t_nombre_compartida variable;
}__attribute__((packed)) t_asignarValor;

typedef struct{
	int pid;
	int descriptorArchivo;
	char* mensaje;
}__attribute__((packed)) t_mensajeDeProceso;

typedef struct {
	int pid;
	char* flags;
	char* path;
}__attribute__((packed)) t_crearArchivo;

typedef struct {
	int pid;
	int fileDescriptor;
}__attribute__((packed)) t_archivo;

typedef struct {
	int pid;
	int fileDescriptor;
	int posicion;
}__attribute__((packed)) t_moverCursor;

enum tipos_de_Acciones{
	//***Todas las acciones del Kernel al enviar
		envioPCB = 4,
		envioDelPidEnSeco = 1,
		envioCantidadPaginas = 2,
		envioPaginaMemoria = 3,
		pidFinalizadoPorFaltaDeMemoria = 8,
		errorFinalizacionPid = 5,
		pidFinalizado = 6,
		enviarDatosCPU = 7,
		imprimirPorPantalla = 8,
		envioDelFileDescriptor = 9,
		bloquearProceso = 10,
		errorSemaforo = 11,
		respuestaBooleanaKernel=12,


	//***Todas las acciones del CPU
		asignarValor = 101,
		pedirValor = 102,
		pedirPCB = 103,
		enviarPCBaTerminado = 104,
		enviarPCBaReady = 105,
		mensajeParaEscribir = 106,
		waitSemaforo = 107,
		signalSemaforo = 108,
		asignarValorCompartida = 109,
		pedirValorCompartida = 110,
		abrirArchivo = 111,
		cerrarArchivo = 112,
		borrarArchivoCPU = 113,
		leerArchivo = 114,
		moverCursorArchivo = 115,
		reservarMemoria = 116,
		liberarMemoria = 117,
		enviarPCBaBloqueado = 118,


	//***Todas las acciones del Memoria
		enviarTamanoPaginas = 200,
		lineaDeCodigo = 201,
		RespuestaBooleanaDeMemoria = 202,
     	inicializarPrograma = 203,
		solicitarBytes = 204,
		almacenarBytes = 205,
		asignarPaginas = 206, /// asigna paginas a un proceso, esto es mas que nada para el heap
		finalizarPrograma = 207,
		liberarUnaPagina = 208,


	//***Todas las acciones de Consola
		envioScriptAnsisop = 301,
		finalizarCiertoScript = 302,
		desconectarConsola = 303,

	//***Todas las acciones de FileSystem
		validacionDerArchivo = 401,
		creacionDeArchivo = 402,
		borrarArchivo = 403,
		obtenerDatosDeArchivo= 404,
		guardarDatosDeArchivo = 405,
		respuestaBooleanaDeFs = 407,
		respuestaConContenidoDeFs = 408
};


#endif /* DATOSGOBALESGENERALES_H_ */
