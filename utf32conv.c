#include "utfconv.h"
#include <stdio.h>
#include <stdlib.h>
#define TAM_BOM 4
#define ID_CONTINUACAO 0x80
#define LIM_UTF32 0x3FFFFF

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

void separa_bytes_6 (unsigned char* b_utf32, unsigned int utf32, int n, unsigned int mascara_separa){
	
	int i, j, aux;

	for(i=0, j=3 ; i<n; i++, j--){
		aux = utf32 & mascara_separa;

		b_utf32[j] = aux >> (i*6);

		mascara_separa = mascara_separa << 6;
	}
}

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


int conv32_8(FILE *arq_entrada, FILE *arq_saida){

	char ordem;
	unsigned char b_utf32[4];
	int contador_erro=0, n_bytes, n_leitura;
	unsigned int utf32;

	ordem = checa_bom( arq_entrada, &contador_erro );
	if(ordem == -1)
		return -1;
	

	if(ordem == 'L'){

		n_leitura = fscanf(arq_entrada, "%c%c%c%c", &b_utf32[3], &b_utf32[2], &b_utf32[1], &b_utf32[0]);	

		while( n_leitura == 4){

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
			n_leitura = fscanf(arq_entrada, "%c%c%c%c", &b_utf32[3], &b_utf32[2], &b_utf32[1], &b_utf32[0]);

		}
		if(	n_leitura > 0){
			fprintf(stderr, "O numero de bytes do arquivo excede em %d bytes o devido para a leitura correta do arquivo", n_leitura);

		}
	}




	else
	{	
		n_leitura = fscanf(arq_entrada, "%c%c%c%c", &b_utf32[0], &b_utf32[1], &b_utf32[2], &b_utf32[3]);

		while( n_leitura == 4){

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
			n_leitura = fscanf(arq_entrada, "%c%c%c%c", &b_utf32[0], &b_utf32[1], &b_utf32[2], &b_utf32[3]);

		}

		if(	n_leitura > 0){
		fprintf(stderr, "O numero de bytes do arquivo excede em %d bytes o devido para a leitura correta do arquivo", n_leitura);

		}
	}



	return 0;
}
