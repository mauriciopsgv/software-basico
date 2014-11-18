// Compiler Header
#include <stdio.h>

typedef int (*funcp) ();

funcp geracod(FILE* arq_fonte);

// gcc -Wall -m32 -Wa,--execstack -o ...
