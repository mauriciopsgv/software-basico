#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#define	NUMERO_BEM_GRANDE 1000

typedef unsigned char * Code;

typedef union num
{
	int i;
	unsigned char b[4];
} N_union;

int initialize_code (Code codigo){

	codigo[0] = 0x55;							// push %ebp
	codigo[1] = 0x89; 	codigo[2] = 0xe5;		// mov %esp, %ebp

	return 3;
}

void finalize_code (Code codigo, int cont_cod){

	codigo[cont_cod] =  0x89;	codigo[++cont_cod] = 0xec;		// mov %ebp, %esp
	codigo[++cont_cod] = 0x5d;									// pop %ebp
	codigo[++cont_cod] = 0xc3;									// ret
}

void read_ret (FILE* arq_fonte, Code codigo, int cont_cod){

	int i;
	char c;
	N_union num;
	fscanf( arq_fonte, "et %c%d", &c, &num.i );

	switch(c)
	{
		case '$':
		{
			codigo[cont_cod] = 0xb8;
			cont_cod++;

			for(i=0 , cont_cod; i<4; i++, cont_cod++)
			{
				codigo[cont_cod]= num.b[i];
			}
			break;
		}

		case 'p':
		{
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x45;
			codigo[cont_cod++] = (0x08 + (num.i)*4 );
			break;
		}

		default:	
			fprintf(stderr, "Simbolo invalido para essa operacao\n");
	}

	finalize_code(codigo, cont_cod);

}

funcp geracod(FILE* arq_fonte){

	int c, cont_cod;
	Code codigo = (Code) malloc(NUMERO_BEM_GRANDE);

	cont_cod = initialize_code(codigo);

	while ((c = fgetc(arq_fonte)) != EOF){

		switch(c)
		{
			case 'r':
			{
				read_ret(arq_fonte, codigo, cont_cod);
			}
		}

		fscanf(arq_fonte, " ");
	}
	

	return (funcp) codigo;
}
