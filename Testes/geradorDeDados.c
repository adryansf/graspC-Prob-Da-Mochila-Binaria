#include <stdio.h>

#define MOCHILA 50
#define ITENSMAX 1000 

int main(){
  FILE *arq;

  arq = fopen("dadosGerados.txt", "w");
  fprintf(arq,"%d\n%d\n", MOCHILA, ITENSMAX);

  for(int i = 0; i < ITENSMAX; i++){
    fprintf(arq,"%d %d\n",(1+(rand() % 30)), (1+ (rand() % 27)));
  }  
  
  fclose(arq);

  printf("\nfim\n");

  return 0;
}