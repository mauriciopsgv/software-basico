#include "utfconv.h"
#include <stdio.h>
#include <stdlib.h>
#define  mascara_bc_up 0xBF
#define	 mascara_bc_down 0x80
#define  mascara_continuacao 0x3F


void separa_bytes (unsigned char * b_utf32, unsigned int utf32, unsigned int n){
	
	int i, aux, mascara_separa = 0x000000FF;

	for(i=0; i<n; i++){
		aux = utf32 & mascara_separa;

		b_utf32[i] = aux >> (i*8);

		mascara_separa = mascara_separa << 8;
	}
}

void imprime_bom (FILE *arq_saida, unsigned char * bom, char ordem){

	int i;
	
	if(ordem == 'L'){
		for( i=3 ; i>-1; i--)
			fprintf(arq_saida, "%c", bom[i]);
	}
	else{
		for( i=0 ; i<4 ; i++)
			fprintf(arq_saida, "%c", bom[i]);
	}

}

void imprime_bytes (FILE *arq_saida, unsigned char * b_utf32, unsigned int n, char ordem){

	int i;
	char c = 0;

	if(ordem == 'L'){
		for(i= 0 ; i < n ; i++)
			fprintf(arq_saida, "%c", b_utf32[i]);

		for(i= (4-n); i>0 ; i--)
			fprintf(arq_saida, "%c", c);

	}
	else if (ordem == 'B'){
		for(i= (4-n); i>0 ; i--)
			fprintf(arq_saida, "%c", c);

		for(i=0; i<n; i++)
			fprintf(arq_saida, "%c", b_utf32[i]);
	}
	else
		fprintf(stderr, "Erro de codificacao na passagem do parametro\n");

}

int testa_byte_continuacao ( unsigned char byte){

	if( byte <= mascara_bc_up && byte >= mascara_bc_down)
		return 1;
	return 0;
}

int soma_bytes_continuacao (FILE *arq_entrada, unsigned char * utf8, unsigned int n, unsigned int * contador_erro){
	int i, soma, aux;
	soma = 0;

	for(i=1; i<n; i++){

		fscanf(arq_entrada, "%c", &utf8[i]);
		*contador_erro += 1;

		if( testa_byte_continuacao(utf8[i]) ){
			
			utf8[i] = utf8[i] & mascara_continuacao;
			aux = (unsigned int) utf8[i];
			aux = aux << ( (n-i-1)*6 );
			soma += aux;

		}
		else{

			fprintf(stderr, "Erro de codificação incorreta no arquivo de entrada, no byte numero: %d", *contador_erro);
			return -1;
		}

	}
	return soma;
}


int conv8_32 (FILE *arq_entrada, FILE *arq_saida, char ordem){

	unsigned char utf8[4], b_utf32[4];
	unsigned int utf32, soma, n_bytes, contador_erro = 0;
	unsigned char bom[4] = { 0x00, 0x00, 0xFE, 0xFF};

	imprime_bom(arq_saida, bom, ordem);

	while( fscanf(arq_entrada, "%c", &utf8[0]) == 1)
	{
		if(utf8[0] <= 0x7F){
			n_bytes = 1;
			imprime_bytes(arq_saida, utf8, n_bytes, ordem);

		}


		else if( utf8[0] <= 0xDF && utf8[0] >= 0xC0)
		{
			n_bytes = 2;
			utf8[0] = utf8[0] & 0x1F; // 00011111
			utf32 = (unsigned int) utf8[0];
			utf32 = utf32 << 6;	// 00000000 00011111 - > 00000111 11000000

			soma = soma_bytes_continuacao(arq_entrada, utf8, n_bytes, &contador_erro);
			if(soma == -1)
				return -1;

			utf32 += soma;
			separa_bytes(b_utf32, utf32, n_bytes);
			imprime_bytes(arq_saida, b_utf32, n_bytes, ordem);

		}

		else if( utf8[0] <= 0xEF && utf8[0] >= 0xE0)
		{
			n_bytes = 3;
			utf8[0] = utf8[0] & 0x0F;
			utf32 = (unsigned int) utf8[0];
			utf32 = utf32 << 12;

			soma = soma_bytes_continuacao(arq_entrada, utf8, n_bytes, &contador_erro);
			if(soma == -1)
				return -1;

			utf32 += soma;
			separa_bytes(b_utf32, utf32, n_bytes);
			imprime_bytes(arq_saida, b_utf32, n_bytes, ordem);

		}

		else if( utf8[0] <= 0xF7 && utf8[0] >= 0xF0)
		{
			n_bytes = 4;
			utf8[0] = utf8[0] & 0x07;
			utf32 = (unsigned int) utf8[0];
			utf32 = utf32 << 18;

			soma = soma_bytes_continuacao(arq_entrada, utf8, n_bytes, &contador_erro);
			if(soma == -1)
				return -1;

			utf32 += soma;
			separa_bytes(b_utf32, utf32, n_bytes);
			imprime_bytes(arq_saida, b_utf32, n_bytes, ordem);

		}

		else{
			fprintf(stderr, "Erro de codificação incorreta no arquivo de entrada, no byte numero: %d", contador_erro);
			return -1;
		}

		contador_erro++;
	}

	return 0;
}
