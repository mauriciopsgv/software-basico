/* Mauricio Pedro Silva Gonçalves Vieira - 1311101 - 3WB */
/* Victor Augusto Lima Lins de Souza - 1310784 - 3WB */

#include "utfconv.h"
#include <stdio.h>
#include <stdlib.h>
#define  mascara_bc_up 0xBF
#define	 mascara_bc_down 0x80
#define  mascara_continuacao 0x3F


char checa_bom (FILE* arq_entrada, int* contador_erro){
	int i, flag_L = 0, flag_B = 0;
	char bom[TAM_BOM];
	char bom_B[TAM_BOM] = {0x00, 0x00, 0xFE, 0xFF};
	char bom_L[TAM_BOM] = {0xFF, 0xFE, 0x00, 0x00};

	for(i=0; i<TAM_BOM; i++){
		fscanf(arq_entrada, "%c", &bom[i]);
	
		if( bom[i] == bom_L[i] )
			flag_L = 1;

		if(	bom[i] == bom_B[i] )
			flag_B = 1;

		if( !(flag_B + flag_L) ){
			fprintf(stderr, "Erro no Byte Order Mark (BOM) do arquivo de entrada no byte numero: %d\n", *contador_erro);
			return -1;
		}

		*contador_erro += 1;
	}

	if(flag_L)
		return 'L';
	return 'B';

}

void imprime_bom (FILE* arq_saida, unsigned char* bom, char ordem){

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

int bytes_to_int (unsigned char* bytes, int contador_erro){

	int i, utf32=0;

	for(i = 0; i<4; i++){

		utf32 += bytes[i] << 8*(3-i);
	}

	if(utf32 > 0x10FFFF){
		fprintf(stderr, "Erro de codificacao do arquivo de entrada no byte numero %d\n", contador_erro);
		return -1;
	}

	return utf32;
}

int determina_n (unsigned int utf32){

	if(utf32 <= 0x7F)
		return 1;
	if(utf32 <= 0x7FF)
		return 2;
	if(utf32 <= 0xFFFF)
		return 3;
	else
		return 4;
}

int testa_byte_continuacao ( unsigned char byte){

	if( byte <= mascara_bc_up && byte >= mascara_bc_down)
		return 1;
	return 0;
}

int soma_bytes_continuacao (FILE* arq_entrada, unsigned char* utf8, unsigned int n, unsigned int* contador_erro){
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

void separa_bytes (unsigned char* b_utf32, unsigned int utf32, unsigned int n){
	
	// no vetor o numero fica armazenado em little endian
	int i, aux, mascara_separa = 0x000000FF;

	for(i=0; i<n; i++){
		aux = utf32 & mascara_separa;

		b_utf32[i] = aux >> (i*8);

		mascara_separa = mascara_separa << 8;
	}
}

void separa_bytes_6 (unsigned char* b_utf32, unsigned int utf32, int n, unsigned int mascara_separa){
	
	int i, j, aux;

	for(i=0, j=3 ; i<n; i++, j--){
		aux = utf32 & mascara_separa;

		b_utf32[j] = aux >> (i*6);

		mascara_separa = mascara_separa << 6;
	}
}

void imprime_bytes (FILE* arq_saida, unsigned char* b_utf32, unsigned int n, char ordem){

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

		for(i= n-1; i>-1; i--)
			fprintf(arq_saida, "%c", b_utf32[i]);
	}
	else{
		fprintf(stderr, "Erro de codificacao na passagem do parametro\n");
		return;
	}

}

void imprime_primeiro (FILE* arq_saida, unsigned char byte, int n){

	unsigned int mascara_p = 0x1E;
	if(n==1)
		fprintf(arq_saida, "%c", byte);
	
	else{
		mascara_p = mascara_p << (7-n);
		fprintf(arq_saida, "%c", byte + mascara_p);

	}

}

void imprime_continuacao (FILE* arq_saida, unsigned char* bytes, int n){

	int i;

	for( i=5-n; i<4; i++)
		fprintf(arq_saida, "%c", bytes[i] + ID_CONTINUACAO);
}


int conv8_32 (FILE* arq_entrada, FILE* arq_saida, char ordem){

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

int conv32_8(FILE *arq_entrada, FILE *arq_saida){

	char ordem;
	unsigned char b_utf32[4];
	int contador_erro=0, n_bytes;
	unsigned int utf32;

	ordem = checa_bom( arq_entrada, &contador_erro );
	if(ordem == -1)
		return -1;
	
	if(ordem == 'L'){
		while( fscanf(arq_entrada, "%c%c%c%c", &b_utf32[3], &b_utf32[2], &b_utf32[1], &b_utf32[0]) == 4){

			utf32 = bytes_to_int( b_utf32 , contador_erro);
			if(utf32 == -1)
				return -1;
			n_bytes = determina_n(utf32);

			if(n_bytes == 1)
				separa_bytes_6( b_utf32, utf32, 4, 0x7F);
			else
				separa_bytes_6( b_utf32, utf32, 4, 0x3F);

			imprime_primeiro(arq_saida, b_utf32[4 - n_bytes], n_bytes);

			if(n_bytes > 1)
				imprime_continuacao(arq_saida, b_utf32, n_bytes);

			contador_erro += 4;
		}
	}
	else
	{
		while( fscanf(arq_entrada, "%c%c%c%c", &b_utf32[0], &b_utf32[1], &b_utf32[2], &b_utf32[3]) == 4){

			utf32 = bytes_to_int( b_utf32 , contador_erro);
			if(utf32 == -1)
				return -1;

			n_bytes = determina_n(utf32);

			if(n_bytes == 1)
				separa_bytes_6( b_utf32, utf32, 4, 0x7F);
			else
				separa_bytes_6( b_utf32, utf32, 4, 0x3F);
		
			imprime_primeiro(arq_saida, b_utf32[4 - n_bytes], n_bytes);

			if(n_bytes > 1)
				imprime_continuacao(arq_saida, b_utf32, n_bytes);
			
			contador_erro += 4;
		}
	}



	return 0;
}
