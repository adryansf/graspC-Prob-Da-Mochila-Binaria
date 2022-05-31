#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define iteracoesMaxima 12500
#define limiteInferiorAlpha 0.8
#define limiteSuperiorAlpha 0.95

typedef struct{
  int ganho, peso;
  float custoBeneficio;
} Item;

Item* obterDados(int *capacidadeMaxima, int *quantidadeItens, Item *itens){
  int i;
  FILE *dados = fopen("dadosGerados.txt", "r");
  fscanf(dados, "%d", capacidadeMaxima);
  fscanf(dados, "%d", quantidadeItens);
  itens = (Item*) malloc(*quantidadeItens * sizeof(Item));
  for(i = 0; i < *quantidadeItens; i++){
    fscanf(dados, "%d %d", &itens[i].ganho, &itens[i].peso);
    itens[i].custoBeneficio = (float) itens[i].ganho / itens[i].peso;
  }
  fclose(dados);
  return itens;
}

// Quicksort
int particiona(Item *vetor, int inicio, int fim){
	int i = inicio, j;
  float pivo = vetor[inicio].custoBeneficio;
  Item *aux = malloc(sizeof(Item));

	for(j = inicio+1; j <= fim; j++){
    if(vetor[j].custoBeneficio >= pivo){
	    i++;

      aux->ganho = vetor[i].ganho;
      aux->peso = vetor[i].peso;
      aux->custoBeneficio = vetor[i].custoBeneficio;

      vetor[i].ganho = vetor[j].ganho;
      vetor[i].peso = vetor[j].peso;
      vetor[i].custoBeneficio = vetor[j].custoBeneficio;
	    
	    vetor[j].ganho = aux->ganho;
      vetor[j].peso = aux->peso;
      vetor[j].custoBeneficio = aux->custoBeneficio;
	  }
	}

	aux->ganho = vetor[i].ganho;
  aux->peso = vetor[i].peso;
  aux->custoBeneficio = vetor[i].custoBeneficio;

	vetor[i].ganho = vetor[inicio].ganho;
  vetor[i].peso = vetor[inicio].peso;
  vetor[i].custoBeneficio = vetor[inicio].custoBeneficio;
	
  vetor[inicio].ganho = aux->ganho;
  vetor[inicio].peso = aux->peso;
  vetor[inicio].custoBeneficio = aux->custoBeneficio;

  free(aux);

	return i;
}

void quickSort(Item *vetor, int inicio, int fim){
  int meio;                      
  if (inicio < fim) {                  
    meio = particiona(vetor, inicio, fim);
    quickSort(vetor, inicio, meio-1);    
    quickSort(vetor, meio+1, fim);
  }
}
// Fim do QuickSort

int ganho(int *solucao, Item *itens, int quantidadeItens, int capacidadeMaxima){
  int peso = 0, ganho = 0, i;
  for(i = 0; i < quantidadeItens; i++){
    if(solucao[i] == 1){
      peso += itens[i].peso;
      ganho += itens[i].ganho;
    }
  }

  if(peso > capacidadeMaxima) return -1;
  return ganho;
}

int *buscaLocal(int *solucao, Item *itens, int quantidadeItens, int capacidadeMaxima) {
  int ganhoAntigo = ganho(solucao, itens, quantidadeItens, capacidadeMaxima);
  int ganhoAtual = ganhoAntigo;
  int melhorSolucao[4];
  int i;

  for(i = 0; i < quantidadeItens; i++){
    if(solucao[i] == 0){
      solucao[i] = 1;
      int novoGanho = ganho(solucao, itens, quantidadeItens, capacidadeMaxima);
      if(novoGanho > ganhoAtual){
        ganhoAtual = novoGanho;
        for(int i = 0; i < quantidadeItens; i++) melhorSolucao[i] = solucao[i];
      }
      solucao[i] = 0;
    }
  }

  if(ganhoAtual > ganhoAntigo){
    return buscaLocal(melhorSolucao, itens, quantidadeItens, capacidadeMaxima);
  }
  return solucao;
}

Item visualizarSolucao(Item* itens, int quantidadeItens,int *solucao){
  int contador = 0;
  int ganhoTotal = 0, pesoTotal = 0;

  printf("Itens inclusos:\n");
  for(contador = 0; contador < quantidadeItens; contador++){
    if(solucao[contador]){
      printf("[%d] Ganho = %d\tPeso = %d\n", contador, itens[contador].ganho, itens[contador].peso);
      ganhoTotal += itens[contador].ganho;
      pesoTotal += itens[contador].peso;
    }
  }
  printf("Ganho Total = %d\n", ganhoTotal);
  printf("Peso Total = %d\n", pesoTotal);
}

double geradorDeAlpha(){
  double alpha = limiteInferiorAlpha + (((rand() % 101)/100.0) * (limiteSuperiorAlpha - limiteInferiorAlpha));
  return alpha;
}

int geradorAleatorio(double alpha, int quantidadeItens, int valorPadrao){
  if(alpha == 1) return valorPadrao;
  else if(alpha == 0) return rand() % quantidadeItens;
  return (rand() % (quantidadeItens + 1)) * alpha;
}

void construtorGananciosoAleatorio(int *solucao, Item *itens, int quantidadeItens,int capacidadeMaxima, double alpha){
  int i, random, capacidadeAtual = capacidadeMaxima;
  for(i = 0; i < quantidadeItens; i++){
    random = geradorAleatorio(alpha, quantidadeItens, i);
    while(solucao[random]){
      random = geradorAleatorio(alpha, quantidadeItens, i);
    }

    if(itens[random].peso <= capacidadeAtual){
      solucao[random] = 1;
      capacidadeAtual -= itens[random].peso;
    }
  }
}

int *GRASP(Item *itens, int quantidadeItens, int capacidadeMaxima, double alpha){
  int *solucaoInicial, *melhorSolucao, i;
  solucaoInicial = malloc(quantidadeItens * sizeof(int));
  melhorSolucao = malloc(quantidadeItens * sizeof(int));

  for(i = 0; i < quantidadeItens; i++){
    solucaoInicial[i] = 1;
    melhorSolucao[i] = 0;
  }

  // Decrescente Baseado no custo benefício
  quickSort(itens, 0, (quantidadeItens - 1));

  if(ganho(solucaoInicial, itens, quantidadeItens, capacidadeMaxima) != -1){
    printf("\n\nTodos itens cabem na mochila. Ganho total: %d", ganho(solucaoInicial, itens, quantidadeItens, capacidadeMaxima));
    return 0;
  }

  for(int contador = 0; contador < iteracoesMaxima; contador++){
    for(i = 0; i < quantidadeItens; i++) solucaoInicial[i] = 0;
    construtorGananciosoAleatorio(solucaoInicial, itens, quantidadeItens, capacidadeMaxima, alpha);
    int *novaSolucao = buscaLocal(solucaoInicial, itens, quantidadeItens, capacidadeMaxima);
    if(ganho(melhorSolucao, itens, quantidadeItens, capacidadeMaxima) < ganho(novaSolucao, itens, quantidadeItens, capacidadeMaxima)){
      int j;
      for(j = 0; j < quantidadeItens; j++){
        melhorSolucao[j] = novaSolucao[j];
      }
    }
  }

  return melhorSolucao;
}

int main(){
  srand(time(NULL));
  int capacidadeMaxima, quantidadeItens, *solucao, i;
  double alpha = geradorDeAlpha();
  Item *itens = obterDados(&capacidadeMaxima, &quantidadeItens, itens);
  
  // Teste de desempenho
  clock_t momentos[2];
  double tempo;

  momentos[0] = clock();
  solucao = GRASP(itens, quantidadeItens, capacidadeMaxima, alpha);
  momentos[1] = clock();

  tempo = ((momentos[1] - momentos[0]) * 1000.0 / CLOCKS_PER_SEC) / 1000.0;

  printf("Parametro Alpha: %lf\n", alpha);
  printf("N° Iteracoes: %d\n", iteracoesMaxima);
  
  printf("\n\n");
  visualizarSolucao(itens, quantidadeItens, solucao);

  printf("\nTempo de execucao total: %g s\n", tempo);
  return 0;
}