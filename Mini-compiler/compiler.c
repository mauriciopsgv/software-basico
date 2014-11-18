#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#define	NUMERO_BEM_GRANDE 1000
#define NUM_VAR_LOCAIS 5

typedef unsigned char * Code;

typedef union num
{
	int i;
	unsigned char b[4];
} N_union;

typedef struct line_if{

	int wanted_line;
	int index_to_change;

	struct line_if * ant;
	struct line_if * prox;

} Line;

Line * cria_no ( Line * iterador_lista, int wanted_line, int index_to_change)
{
	Line * lista = (Line *) malloc(sizeof(Line));

	if(iterador_lista != NULL)
		iterador_lista->ant = lista;

	lista->wanted_line = wanted_line;
	lista->index_to_change = index_to_change;
	lista->prox = iterador_lista;

	return lista;
}

Line* destroi_no (Line * iterador_lista){

	Line * temp;

	if( iterador_lista->ant == NULL)
	{
		temp = iterador_lista->prox;
		temp->ant = NULL;
		free(iterador_lista)
	}
	else
	{
		temp = iterador_lista->ant;
		temp->prox = iterador_lista->prox;
		free(iterador_lista);
	}

	return temp;
}

int check_line ( Line * iterador_lista, int line){

	int i;

	if( iterador_lista == NULL)
		return 0;

	if( iterador_lista->wanted_line == line)
	{	
		i = iterador_lista->index_to_change;
		iterador_lista = destroi_no(iterador_lista);
		return i;
	}

	check_line( iterador_lista-> prox, line);
}



int initialize_code (Code codigo, int * lines){

	codigo[0] = 0x55;							// push %ebp
	lines[0] = 0;
	
	codigo[1] = 0x89; 	codigo[2] = 0xe5;		// mov %esp, %ebp
	lines[1] = 1;

	codigo[3] = 0x83;	codigo[4] = 0xec;	codigo[5] = 0x14;
	lines[2] = 3;

	return 6;
}

void finalize_code (Code codigo, int cont_cod){

	codigo[cont_cod] = 0x5a;
	codigo[++cont_cod] =  0x89;	codigo[++cont_cod] = 0xec;		// mov %ebp, %esp
	codigo[++cont_cod] = 0x5d;									// pop %ebp
	codigo[++cont_cod] = 0xc3;									// ret
}

int gibe_me_my_index ( int * ordem_var_local, int n){

	int i;
	for(i=0; i < NUM_VAR_LOCAIS ; i++)
	{
		if( ordem_var_local[i] == n)
			return i;		
	}

	return -1;
} 

void read_ret (FILE* arq_fonte, Code codigo, int cont_cod, int* ordem_var_local){

	int i, ordem;
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
			ordem = gibe_me_my_index(ordem_var_local, num.i);

			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x45;
			codigo[cont_cod++] = (0xfc - (ordem)*4 );

			break;
		}

		default:	
			fprintf(stderr, "Simbolo invalido\n");
	}

	finalize_code(codigo, cont_cod);
}

int read_att (FILE* arq_fonte, Code codigo, int cont_cod, int c, int* ordem_var_local, int* cont_var_local){

	int i;
	int ordem;
	char c1,c2, op;
	N_union o, o1, o2;


	fscanf(arq_fonte, "%d := %c%d %c %c%d", &o.i, &c1, &o1.i, &op , &c2, &o2.i);

	switch(c1){ //coloco em ecx

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
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = (0x08 + (o1.i)*4 );
			//cont_cod pronto pra acesso (valor do primeiro indice em branco)

			break;
		}

		case 'v':
		{
			ordem = gibe_me_my_index(ordem_var_local, o1.i);

			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = (0xfc - (ordem)*4 );

			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(c2){ // coloco em edx


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
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x55;
			codigo[cont_cod++] = (0x08 + (o2.i)*4 );
			//cont_cod pronto pra acesso (valor do primeiro indice em branco)
			
			break;
		}

		case 'v':
		{
			ordem = gibe_me_my_index(ordem_var_local, o2.i);

			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x55;
			codigo[cont_cod++] = (0xfc - (ordem)*4 );
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
			codigo[cont_cod++] = 0xd1;
			break;
			//cont_cod ja pronto pra acessar
			//resultado da soma sempre em edx
		}

		case '-':
		{
			codigo[cont_cod++] = 0x29;
			codigo[cont_cod++] = 0xd1;
			break;
		}

		case '*':
		{
			codigo[cont_cod++] = 0x0f;
			codigo[cont_cod++] = 0xaf;
			codigo[cont_cod++] = 0xca;
			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(c){

		case 'p':
		{
			codigo[cont_cod++] = 0x89;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = (0x08 + (o.i)*4 );
		}

		case 'v':
		{
			codigo[cont_cod++] = 0x89;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = 0xfc - 4*(*cont_var_local); //precisa mudar isso
			ordem_var_local[(*cont_var_local)++] = o.i;
		}
	}

	return cont_cod;
}

int add_cmpl (Code codigo, int cont_cod){

	codigo[cont_cod++] = 0x39;
	codigo[cont_cod++] = 0xca;

	return cont_cod;
}

int read_if( FILE* arq_fonte, Code codigo, int cont_cod, int * ordem_var_local, int * cont_var_local, Line** iterador_lista){

	char c1, c2;
	int i, ordem, wanted_line;
	N_union o1, o2;

	fscanf(arq_fonte, "feq %c%d %c%d %d", &c1, &o1.i, &c2, &o2.i, &wanted_line);

	switch(c1){ //coloco em %ecx

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
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = (0x08 + (o1.i)*4 );
			//cont_cod pronto pra acesso (valor do primeiro indice em branco)

			break;
		}

		case 'v':
		{
			ordem = gibe_me_my_index(ordem_var_local, o1.i);

			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x4d;
			codigo[cont_cod++] = (0xfc - (ordem)*4 );

			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	switch(c2){ // coloco em %edx


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
			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x55;
			codigo[cont_cod++] = (0x08 + (o2.i)*4 );
			//cont_cod pronto pra acesso (valor do primeiro indice em branco)
			
			break;
		}

		case 'v':
		{
			ordem = gibe_me_my_index(ordem_var_local, o2.i);

			codigo[cont_cod++] = 0x8b;
			codigo[cont_cod++] = 0x55;
			codigo[cont_cod++] = (0xfc - (ordem)*4 );
			break;
		}

		default:
			fprintf(stderr, "Simbolo invalido\n");
	}

	cont_cod = add_cmpl(codigo,cont_cod);

	codigo[cont_cod++] = 0x0f;
	codigo[cont_cod++] = 0x84;
	//codigo[cont_cod++] = 00;
	//codigo[cont_cod++] = 00;
	//codigo[cont_cod++] = 00;
	//codigo[cont_cod++] = 00;



	*iterador_lista = cria_no( *iterador_lista, wanted_line, cont_cod);

	/*Essa parte do código é o número da primeira instrução menos o número
	da instrução imediatamente depois */

	return cont_cod;
}



funcp geracod(FILE* arq_fonte){

	int i, c, cont_cod,  ordem_var_local[5], cont_var_local = 0, line = 3, index_to_change;

	int lines[NUMERO_BEM_GRANDE];

	Code codigo = (Code) malloc(NUMERO_BEM_GRANDE);

	N_union number_to_write;

	Line * iterador_lista = NULL;

	iterador_lista = &ultimo;

	cont_cod = initialize_code(codigo, lines);

	while ((c = fgetc(arq_fonte)) != EOF){

		lines[line] = cont_cod;

		switch(c)
		{
			case 'r':
			{
				read_ret(arq_fonte, codigo, cont_cod, ordem_var_local);
				break;
			}

			case 'v': case 'p':
			{
				cont_cod = read_att(arq_fonte, codigo, cont_cod, c, ordem_var_local, &cont_var_local);
				break;
			}

			case 'i':
			{
				cont_cod = read_if(arq_fonte, codigo, cont_cod, ordem_var_local, & cont_var_local, &iterador_lista);
				break;
			}
		}

		fscanf(arq_fonte, " ");

		if( iterador_lista == NULL ){

			if( iterador_lista->wanted_line <= line){

				number_to_write.i = lines[line-1] - (iterador_lista->index_to_change) + 4;

			//esse for vai virar função
				for(i=0, index_to_change; i<4; i++, index_to_change++)
					codigo[index_to_change] = number_to_write.b[i];
			}


			if( iterador_lista->wanted_line > line )
			{
				index_to_change = check_line( iterador_lista, line);

				number_to_write.i = lines[line-1] - (index_to_change + 4);

				for(i=0, index_to_change; i<4; i++, index_to_change++)
					codigo[index_to_change] = number_to_write.b[i];
			}
		}

		line ++;	
	}

	return (funcp) codigo;
}

/*
ifs_pendentes = { line_if_1, line_if_2 }

struct line_if{
	wanted_line
	index_to_change
}
*/
