/*
 * funcionesParaTodosYTodas.c
 *
 *  Created on: 22/5/2017
 *      Author: utnso
 */

#include "funcionesParaTodosYTodas.h"

int sum(t_list *lista,int(* funcion) (void*)){
	int i;
	int contador=0;
	for( i = 0; i< list_size(lista);i++){
		contador += funcion(list_get(lista,i));
	}
	return contador;
}

int tamanoMensajeAEscribir(int tamanioContenido){
	return sizeof(int)*3 + tamanioContenido;
}

void* serializarMensajeAEscribir(t_mensajeDeProceso mensaje, int tamanio){
	void* stream = malloc(tamanoMensajeAEscribir(mensaje.tamanio));

	memcpy(stream,&mensaje.pid,sizeof(int));

	memcpy(stream + sizeof(int),&mensaje.descriptorArchivo,sizeof(int));

	memcpy(stream + sizeof(int) * 2,&mensaje.tamanio,sizeof(int));

	memcpy(stream + sizeof(int) * 3,mensaje.mensaje,mensaje.tamanio);

	return stream;
}

t_mensajeDeProceso deserializarMensajeAEscribir(void* stream){
	t_mensajeDeProceso mensaje;

	memcpy(&mensaje.pid,stream,sizeof(int));

	memcpy(&mensaje.descriptorArchivo,stream + sizeof(int),sizeof(int));

	int tamanoContenido;

	memcpy(&tamanoContenido,stream + sizeof(int) * 2,sizeof(int));

	mensaje.tamanio = tamanoContenido;

	mensaje.mensaje = malloc(tamanoContenido);

	memcpy(mensaje.mensaje,stream + sizeof(int) * 3, tamanoContenido);


	return mensaje;
}



