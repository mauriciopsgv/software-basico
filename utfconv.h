#include <stdio.h>
#include <stdlib.h>

void imprime_bom (FILE* arq_saida, unsigned char* bom, char ordem);

char checa_bom (FILE* arq_entrada, int* contador_erro);

int bytes_to_int (unsigned char* bytes);

int determina_n (unsigned int utf32);

void separa_bytes (unsigned char* b_utf32, unsigned int utf32, unsigned int n);

void separa_bytes_6 (unsigned char* b_utf32, unsigned int utf32, int n, unsigned int mascara_separa);

void imprime_bytes (FILE* arq_saida, unsigned char* b_utf32, unsigned int n, char ordem);

void imprime_primeiro (FILE* arq_saida, unsigned char byte, int n);

void imprime_continuacao (FILE* arq_saida, unsigned char* bytes, int n);

int testa_byte_continuacao (unsigned char byte);

int soma_bytes_continuacao (FILE* arq_entrada, unsigned char* utf8, unsigned int n, unsigned int* contador_erro);

int conv8_32(FILE* arq_entrada, FILE* arq_saida, char ordem);

int conv32_8(FILE* arq_entrada, FILE* arq_saida);
