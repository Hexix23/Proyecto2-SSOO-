#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
	 char *comando[LONGITUD_COMANDO];
	 char *orden[LONGITUD_COMANDO];
	 char *argumento1[LONGITUD_COMANDO];
	 char *argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     //...
     
     fent = fopen("particion.bin","r+b");
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);
     printf("");
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Buce de tratamiento de comandos
     for (;;){
		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
		 
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
		 
		
		// printf("\nB%sB\n",orden);
		// printf("\nC%sC\n",argumento1);
		// printf("\nD%sD\n",argumento2);
		 
		if(strcmp(orden,"info")==0){
			printf("INFO\n");
			continue;
		}
		if(strcmp(orden,"bytemaps")==0){
			printf("BYTEMAPS\n");
			continue;
		}
		if(strcmp(orden,"dir")==0){
			printf("DIR\n");
			continue;
		}
		if(strcmp(orden,"rename")==0){
			printf("RENAME\n");
			continue;
		}
		if(strcmp(orden,"imprimir")==0){
			printf("IMPRIMIR\n");
			continue;
		}
		if(strcmp(orden,"remove")==0){
			printf("REMOVE\n");
			continue;
		}
		if(strcmp(orden,"copy")==0){
			printf("COPY\n");
			continue;
		}
		if(orden=="salir"){
			printf("SALIR\n");
			continue;
		}
            
         //...
         // Escritura de metadatos en comandos rename, remove, copy
         /*
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrÃ¡n escrito todos los metadatos
         //faltan los datos y cerrar
         */
        if (strcmp(orden,"salir")==0){
        	//GrabarDatos(&memdatos,fent);
            printf("\nSALIR\n");
            fclose(fent);
            return 0;
        }
     }
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){
	//Limpiamos las variables
	strcpy(orden,"");
	strcpy(argumento1,"");
	strcpy(argumento2,"");	
	
	//Sustituimos el /n por un /0
	char aux[strlen(strcomando)-1];
	int contador=0;
	strncpy(aux,strcomando,strlen(strcomando)-1);
	aux[strlen(strcomando)-1]='\0';
	
	//printf("\n[%s][%d]",aux,strlen(aux));
	//Asignamos la orden y los argumentos
	char *token=strtok(aux, " ");		
		
	while(token!=NULL){		
		if(contador==0){
			strcpy(orden,token);			
		}else if(contador==1){
			strcpy(argumento1,token);			
		}else if(contador==2){
			strcpy(argumento2,token);
		}
		token=strtok(NULL, " ");
		contador++;
	}
	
	//Comprobamos ordenes
	if(strcmp(orden,"info")==0){
		return 0;	//Retorna 0 si la funcion existe
	}else if(strcmp(orden,"bytemaps")==0){
		return 0;
	}else if(strcmp(orden,"dir")==0){
		return 0;
	}else if(strcmp(orden,"rename")==0){
		return 0;
	}else if(strcmp(orden,"imprimir")==0){
		return 0;
	}else if(strcmp(orden,"remove")==0){
		return 0;
	}else if(strcmp(orden,"copy")==0){
		return 0;
	}else if(strcmp(orden,"salir")==0){
		return 0;
	}else{
		printf("\nERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,salir]\n");
		return 1;	
	}
	
	
}
