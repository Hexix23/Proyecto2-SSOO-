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
//, 
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
			//printf("INFO\n");
			printf("Bloque %d Bytes\n",SIZE_BLOQUE);
			printf("inodos particion = %d\n",MAX_INODOS);
			printf("inodos libres = %d\n",ext_superblock.s_free_inodes_count);
			printf("Bloques particion = %d\n",MAX_BLOQUES_PARTICION);
			printf("Bloques libres = %d\n",ext_superblock.s_free_blocks_count);
			printf("Primer bloque de datos = %d\n",PRIM_BLOQUE_DATOS);
			continue;
		}
		if(strcmp(orden,"bytemaps")==0){
			//printf("BYTEMAPS\n");
			Printbytemaps(&ext_bytemaps);
			continue;
		}
		if(strcmp(orden,"dir")==0){
			printf("DIR\n");
			Directorio(directorio, &ext_blq_inodos);
			//,ext_blq_inodos
			continue;
		}
		if(strcmp(orden,"rename")==0){
			//printf("RENAME\n");
			if(Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2)==0){
				printf("EL fichero no existe\n");
			}else if(Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2)==1){
				printf("BIEN\n");
			}else if(Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2)==2){
				printf("El nombre ya existe\n");
			}
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
         //Si el comando es salir se habrán escrito todos los metadatos
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
		if((strlen(argumento1)>0) && (strlen(argumento2)>0) ){
			return 0;
		}else{
			printf("Argumentos incorrectos\nComando correcto \"rename nombre_origen.txt nombre_final.txt\"\n");
			return 1;
		}
		
	}else if(strcmp(orden,"imprimir")==0){
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("Argumentos incorrectos\nComando correcto \"imprimir archivo.txt\"\n");
				return 1;
			}
			return 0;
		}else{printf("Argumentos incorrectos\nComando correcto \"remove archivo.txt\"\n");}
	}else if(strcmp(orden,"remove")==0){
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("Argumentos incorrectos\nComando correcto \"remove archivo.txt\"\n");
				return 1;
			}
			return 0;
		}else{printf("Argumentos incorrectos\nComando correcto \"remove archivo.txt\"\n");}
	}else if(strcmp(orden,"copy")==0){
		if((strlen(argumento1)>0) && (strlen(argumento2)>0) ){
			return 0;
		}else{
			printf("Argumentos incorrectos\nComando correcto \"copy nombre_origen.txt nombre_final.txt\"\n");
			return 1;
		}
	}else if(strcmp(orden,"salir")==0){
		return 0;
	}else{
		printf("ERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,salir]\n");
		return 1;	
	}
	
	
}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){
	int i=0;
	
	printf("Inodos: ");
	for(i=0;i<MAX_INODOS;i++){
		printf("%d",ext_bytemaps->bmap_inodos[i]);
	}
	printf("\nBloques [0-25]: ");
	for(i=0;i<25;i++){
		printf("%d", ext_bytemaps->bmap_bloques[i]);
	}
	printf("\n");
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){
    int i,j;
    for(i = 1; i < MAX_FICHEROS; i++){
        if((directorio+i)->dir_inodo != NULL_INODO){
            printf("%s\t tamano: %d\t inodo: %d\t bloques: ", (directorio+i)->dir_nfich, inodos->blq_inodos[(directorio+i)->dir_inodo].size_fichero, (directorio+i)->dir_inodo);
                for(j = 0; inodos->blq_inodos[(directorio+i)->dir_inodo].i_nbloque[j] != NULL_INODO; j++){
                    printf("%d ", inodos->blq_inodos[(directorio+i)->dir_inodo].i_nbloque[j]);
                }
                printf("\n");
        }
    }
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){
	int i, j;
	for(i = 1; i < MAX_FICHEROS; i++){
		if((directorio+i)->dir_inodo != NULL_INODO){
			
			if(strcmp(nombreantiguo,(directorio+i)->dir_nfich) == 0){//Si el archivo existe
			
				//printf("\nNOMBRE inicio BIEN\n");
				for(j = 1; j < MAX_FICHEROS; j++){
					if((directorio+j)->dir_inodo != NULL_INODO){
						
						if(strcmp(nombrenuevo,(directorio+j)->dir_nfich) == 0){
							//printf("\nNOMBRE FINAL MAL\n");
							return 2;
							
						}else{
							//printf("\nNOMBRE FINAL MAL\n");
							continue;
						}
					}
				}
				//printf("\nNOMBRE FINAL BIEn\n");
				strcpy((directorio+i)->dir_nfich, nombrenuevo);
				return 1;
				
				
			}else{
				//printf("\nNOMBRE inicio MAL\n");
			}	
	
		}//Si es NULL
	}
	return 0;

		
}
	
