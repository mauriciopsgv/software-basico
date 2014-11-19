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

Line * cria_no ( Line * iterador_lista, int wanted_line, int index_to_change){
	
	Line * lista = (Line *) malloc(sizeof(Line));

	if(iterador_lista != NULL)
		iterador_lista->ant = lista;

	lista->wanted_line = wanted_line;
	lista->index_to_change = index_to_change;
	lista->prox = iterador_lista;
	lista->ant = NULL;

	return lista;
}

void printList(Line* l, int line, int no)
{
	if(l==NULL)return;
	printf("No %d-> %d %d\n", no, l->wanted_line,l->index_to_change);
	printList(l->prox, line -1, no +1);
}

Line* destroi_no(Line* iterador_lista, Line * node_to_delete){

	Line* lista = iterador_lista;

	if(node_to_delete->ant != NULL)
		node_to_delete->ant->prox = node_to_delete->prox;
	else
		lista = node_to_delete->prox;


	if(node_to_delete->prox != NULL)
		node_to_delete->prox->ant = node_to_delete->ant;

	free(node_to_delete);
	
	return lista;
}

Line * check_line ( Line * iterador_lista, int line){

	if( iterador_lista == NULL)
		return NULL;

	if( iterador_lista->wanted_line <= line)
	{	
		return iterador_lista;
	}

	return check_line(iterador_lista-> prox, line);
}

int write_number (Code codigo, int index_to_change, N_union number_to_write)
{
	int i;

	for(i=0, index_to_change; i<4; i++, index_to_change++)
		codigo[index_to_change] = number_to_write.b[i];

	return index_to_change;
}

int initialize_code (Code codigo, int * lines){

	codigo[0] = 0x55;							// push %ebp
	codigo[1] = 0x89; 	codigo[2] = 0xe5;		// mov %esp, %ebp
	codigo[3] = 0x83;	codigo[4] = 0xec;	codigo[5] = 0x14;

	return 6;
}

int finalize_code (Code codigo, int cont_cod){

	codigo[cont_cod++] = 0x89;	codigo[cont_cod++] = 0xec;		// mov %ebp, %esp
	codigo[cont_cod++] = 0x5d;									// pop %ebp
	codigo[cont_cod++] = 0xc3;									// ret

	return cont_cod;
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

int read_ret (FILE* arq_fonte, Code codigo, int cont_cod, int* ordem_var_local){

	int i;
	int ordem;
	char c;
	N_union num;
	fscanf( arq_fonte, "et %c%d", &c, &num.i );

	switch(c)
	{
		case '$':
		{
			codigo[cont_cod++] = 0xb8;

//			cont_cod = write_number(codigo, cont_cod , num);

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

	return finalize_code(codigo, cont_cod);
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

//			cont_cod = write_number(codigo, cont_cod , o1);

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

//			cont_cod = write_number(codigo, cont_cod , o2);

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
			break;
		}

		case 'v':
		{
			ordem = gibe_me_my_index(ordem_var_local, o.i);

			if(ordem == -1){
				codigo[cont_cod++] = 0x89;
				codigo[cont_cod++] = 0x4d;
				codigo[cont_cod++] = 0xfc;
				ordem_var_local[(*cont_var_local)++] = o.i;
			}

			else
			{
				codigo[cont_cod++] = 0x89;
				codigo[cont_cod++] = 0x4d;
				codigo[cont_cod++] = 0xfc - 4*(ordem); //precisa mudar isso
			}
			break;
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
	int ordem, wanted_line;
	N_union o1, o2;

	fscanf(arq_fonte, "feq %c%d %c%d %d", &c1, &o1.i, &c2, &o2.i, &wanted_line);

	switch(c1){ //coloco em %ecx

		case '$':
		{
			codigo[cont_cod++] = 0xb9;

			cont_cod = write_number(codigo, cont_cod , o1);

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

			cont_cod = write_number(codigo, cont_cod , o2);

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
	codigo[cont_cod] = 0xFF;
	codigo[cont_cod+1] = 0xFF;
	codigo[cont_cod+2] = 0xFF;
	codigo[cont_cod+3] = 0xFF;



	*iterador_lista = cria_no( *iterador_lista, wanted_line, cont_cod);

	/*Essa parte do código é o número da primeira instrução menos o número
	da instrução imediatamente depois */

	return cont_cod + 4;
}



funcp geracod(FILE* arq_fonte,int* NAO_ESQ_DE_TIRAR_ISSO){

	int c, cont_cod, cont_var_local = 0, line = 1;

	int ordem_var_local[5] = {-1,-1,-1,-1,-1};

	int lines[50];

	Line * structure_to_change = NULL;
	Line * temp;

	Code codigo = (Code) malloc(NUMERO_BEM_GRANDE);

	N_union number_to_write;

	Line * iterador_lista = NULL;

	cont_cod = initialize_code(codigo, lines);

	while ((c = fgetc(arq_fonte)) != EOF){

		lines[line] = cont_cod;

		switch(c)
		{
			case 'r':
			{
				cont_cod = read_ret(arq_fonte, codigo, cont_cod, ordem_var_local);
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
		
		printList(iterador_lista, line, 1);

		printf("\n");
		
		if( iterador_lista != NULL ){

			temp = iterador_lista;
			structure_to_change = check_line(temp, line);
			

			while ( structure_to_change != NULL){

				printf("entrei");
				printf("\n>>>>>>> %d %d\n\n",lines[structure_to_change->wanted_line],((structure_to_change->index_to_change) + 4));


				number_to_write.i = lines[structure_to_change->wanted_line] - ((structure_to_change->index_to_change) + 4);

				write_number(codigo, structure_to_change->index_to_change , number_to_write);

				iterador_lista = destroi_no(iterador_lista, structure_to_change);
			
				temp = iterador_lista;

				structure_to_change = check_line(temp, line);
			}

		}
		printf("%d", line);
		line ++;	
	}


	*NAO_ESQ_DE_TIRAR_ISSO = cont_cod;
	return  codigo;
	//return (funcp) codigo
}

/*
v0 := $0 + $1 
ifeq p0 $0 6
v0 := v0 * p0 
p0 := p0 - $1 
ifeq $1 $1 2
ret v0
*/
