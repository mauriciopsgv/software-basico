#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"



int main (void){
	int retorno;
	funcp f;

	FILE* arq_fonte = fopen("codigo.min", "r");
	if(arq_fonte == NULL){
		fprintf(stderr, "Ńao conseguiu abrir o arquivo de entrada\n");
		exit(1);
	}

	f = geracod(arq_fonte);

	retorno = f();

	printf("O retorno do seu array foi : %d\n",retorno);

	return 0;

}
