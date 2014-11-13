#include <stdlib.h>
#include "compiler.h"
#define	NUMERO_BEM_GRANDE 1000

typedef unsigned char * Code;

typedef union num
{
	int i;
	unsigned char b[4];
} N_union;

void initialize_code (Code codigo){

	codigo[0] = 0x55;							// push %ebp
	codigo[1] = 0x89; 	codigo[2] = 0xe5;		// mov %esp, %ebp
}

void finalize_code (Code codigo){

	codigo[8] =0x89;	codigo[9] = 0xe5;		// mov %ebp, %esp
	codigo[10] = 0x5d;							// pop %ebp
	codigo[11] = 0xc3;							// ret
}

void read_ret (FILE* arq_fonte, Code codigo){

	char c;
	N_union num;
	fscanf( arq_fonte, "et %c%d", &c, &num.i );

	switch(c)
	{
		case '$':
		{
			codigo[3] = 0xb8;
			codigo[4] = num.b[0];
			codigo[5] = num.b[1];
			codigo[6] = num.b[2];
			codigo[7] = num.b[3];
		}
		case 'p':
		{

		}
	}

	finalize_code(codigo);

}

funcp geracod(FILE* arq_fonte){

	int c;
	Code codigo = (Code) malloc(NUMERO_BEM_GRANDE);

	initialize_code(codigo);

	while ((c = fgetc(arq_fonte)) != EOF){

		switch(c)
		{
			case 'r':
			{
				read_ret(arq_fonte, codigo);
			}
		}

		fscanf(arq_fonte, " ");
	}
	

	return (funcp) codigo;
}
