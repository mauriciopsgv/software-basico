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
			//cont_cod = ultimo indice que imprimiu + 1 quando sai daqui
		}

		case 'p':
		{
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x45;
			codigo[cont_cod++] = (0x08 + (num.i)*4 );
			break;
			//cont_cod = ultimo indice que imprimiu + 1 quando sai daqui
		}

		case 'v':
		{
			break;
		}

		default:	
			fprintf(stderr, "Simbolo invalido\n");
	}

	finalize_code(codigo, cont_cod);

}




int read_att (FILE* arq_fonte, Code codigo, int cont_cod, int c){

	int i;
	char c1,c2, op;
	N_union o, o1, o2;

	fscanf(arq_fonte, "%d := %c%d %c %c%d", &o.i, &c1, &o1.i, &op , &c2, &o2.i);

	switch(c1){

		case '$':
		{
			codigo[cont_cod++] = 0xb9;

			for(i=0 , cont_cod; i<4; i++, cont_cod++)
			{
				codigo[cont_cod]= o1.b[i];
			}
			// quando sair desse for o contador ja estara pronto para ser indice 
			// de um proximo uso no codigo
			break;
		}

		case 'p':
		{
			break;
		}

		case 'v':
		{
			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(c2){


		case '$':
		{
			codigo[cont_cod++] = 0xba;

			for(i=0 , cont_cod; i<4; i++, cont_cod++)
			{
				codigo[cont_cod]= o2.b[i];
			}
			// quando sair desse for o contador ja estara pronto para ser indice 
			// de um proximo uso no codigo
			break;
		}

		case 'p':
		{
			break;
		}

		case 'v':
		{
			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(op)
	{
		case '+':
		{
			codigo[cont_cod++] = 0x01;
			codigo[cont_cod++] = 0xca;
			break;
			//cont_cod ja pronto pra acessar
		}

		case '-':
		{
			break;
		}

		case '*':
		{
			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(c){

		case 'p':
		{
			codigo[cont_cod++] = 0x89;
			codigo[cont_cod++] = 0x55;
			codigo[cont_cod++] = (0x08 + (o.i)*4 );
		}

		case 'v':
		{

		}
	}

	return cont_cod;

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
				break;
			}

			case 'v': case 'p':
			{
				cont_cod = read_att(arq_fonte, codigo, cont_cod, c);
			}
		}

		fscanf(arq_fonte, " ");
	}
	

	return (funcp) codigo;
}
