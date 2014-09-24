#include <stdio.h>
#include <stdlib.h>

void imprime_bom (FILE *arq_saida, unsigned char * bom, char ordem);

void separa_bytes (unsigned char * b_utf32, unsigned int utf32, unsigned int n);

void imprime_bytes (FILE *arq_saida, unsigned char * b_utf32, unsigned int n, char ordem);

int testa_byte_continuacao (unsigned char byte);

int soma_bytes_continuacao (FILE *arq_entrada, unsigned char * utf8,unsigned int n, unsigned int *contador_erro);

int conv8_32(FILE *arq_entrada, FILE *arq_saida, char ordem);
