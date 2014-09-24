#include <stdio.h>
#include "utfconv.h"

int main (void) {

	FILE *arq_entrada, *arq_saida;
	arq_entrada = fopen("utf8_peq", "r");
	if(arq_entrada == NULL) {
		printf("Erro na abertura do arquivo!\n");
		exit(1);
	}
	arq_saida = fopen("utf32", "w");
	if(arq_saida == NULL) {
		printf("Erro na abertura do arquivo!\n");
		exit(1);
	}
	
	conv8_32(arq_entrada, arq_saida, 'L');

	fclose(arq_saida);
	fclose(arq_entrada);
	return 0;
}
