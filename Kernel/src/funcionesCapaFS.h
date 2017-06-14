/*
 * funcionesCapaFS.h
 *
 *  Created on: 13/6/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "commons/collections/list.h"
#include "commons/collections/queue.h"

#include "datosGlobales.h"
#include "funcionesSemaforosYCompartidas.h"

#include "../../Nuestras/src/laGranBiblioteca/datosGobalesGenerales.h"


#ifndef FUNCIONESCAPAFS_H_
#define FUNCIONESCAPAFS_H_

bool archivoExiste(char* path);

ENTRADA_DE_TABLA_GLOBAL_DE_PROCESO * encontrarElDeIgualPid(int pid);

void agregarATablaDeProceso(int df, char* flags, t_list* tablaProceso);



#endif /* FUNCIONESCAPAFS_H_ */