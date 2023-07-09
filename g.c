/* UM TRABALHO POR LUCA DA SILVA ÁVILA E LUIZ PHYLLIP SABADINI BAZONI */
/* MATRÍCULAS: 2021100331 E 2021100353 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// estrutura para arestas
typedef struct {
	int p1, p2;
	double w;
} edg;

// estrutura para congregar a weighted quick-union
typedef struct {
	int size;
	int* tree;
	int* nSize;
} set;

set* initSet(int n);
void freeSet(set* ds);
int find(set* s, int p);
void connect(set* s, int a, int b);
int connected(set* s, int p, int q);

// funções referentes a edg e strings, respectivamente, para ordenação
int cmp(const void* a, const void* b);
int strcmpCallback(const void* a, const void* b);



int main(int argc, char** argv){
	
	/*******************TEMPO TOTAL************************/
	FILE* timeOut = fopen("time.txt", "w");	
	fprintf(timeOut, "Calculando tempos de execução do código\n");
	clock_t startTotal = clock();

	/***********TEMPO PARA LEITURA DE ARQUIVOS************/
	clock_t start = clock();
	

	if(argc<2) printf("arg error\n"), exit(0);

	FILE* in = fopen(argv[1], "r");
	FILE* out = fopen(argv[3], "w");	
	int k = atoi(argv[2]);
	
	if(!in) printf("file error\n"), exit(0);
	
	/*CONTAGEM DE LINHAS 
	 *complexidade: O(N)*/
	unsigned long long n = 0;
	
	while(!feof(in)){
		if(fgetc(in) == '\n'){
			n++;
		}
	}
	
	if(!n) printf("empty error\n"), exit(0);
	fseek(in, 0, SEEK_SET);	

	// importante inicializar o vetor, porque a função getline é usada aqui
	char* key[n];
	for(int i = 0; i < n; i++) key[i] = NULL;

	size_t lc = 0;
    for(int i=0; i<n; i++){
        getline(&key[i], &lc, in);
    }
	
	// a quantidade das m coordenadas dos pontos eh definida pela quantidade de vírgulas nas linhas
	int howManyCoordinates = 0;
	for(int i=0; key[0][i] != '\n'; i++){
		if(key[0][i] == ',') howManyCoordinates++;
	}

	int mCoordinates[n][howManyCoordinates];
	
	// necessario ordenar as entradas todas antes de qualquer coisa
	qsort(key, n, sizeof(char*), strcmpCallback);	
	
	/*LEITURA DOS DADOS
	 * complexidade O(N)*/	
	
	for(int i=0; i<n; i++){

		key[i] = strtok(key[i], ",");
		char* line = key[i];
		for(int j=0; j<howManyCoordinates; j++){
			line = strtok(NULL, ",");
			mCoordinates[i][j] = atoi(line);
		}
	}			
	fclose(in);
	
	/***********LEITURA DE ARQUIVOS PRONTA************/
	clock_t end = clock();
	double time = (double) (end - start) / CLOCKS_PER_SEC;
	fprintf(timeOut, "Leitura de arquivos: %f\n", time);
	
	/***********TEMPO PARA CALCULAR E ORDENAR AS DISTÂNCIAS************/
	start = clock();


	/*CALCULO DAS DISTANCIAS DE COORDENADAS DE DIVERSAS DIMENSÕES
	 * complexidade O(N^2)*/ /*Talvez da pra fazer melhor que isso*/
	unsigned long long m = n*(n-1)/2;
	edg* edgList = malloc(m*sizeof(edg));
		
	edg* currenEdg = edgList;
	for(int i=0; i<n; i++){
		for(int j=i+1; j<n; j++){
			double distance=0;
			for(int l=0; l<howManyCoordinates; l++)
				distance += (mCoordinates[i][l] - mCoordinates[j][l])*(mCoordinates[i][l] - mCoordinates[j][l]);	

			currenEdg-> p1 = i;
			currenEdg-> p2 = j;
			currenEdg-> w = distance;
			
			currenEdg++;
		}
	}
	

	/***********CALCULADAS AS DISTANCIAS*************/
	end = clock();
	time = (double) (end - start) / CLOCKS_PER_SEC;
	fprintf(timeOut, "Cálculo das Distâncias: %f\n", time);
	
	/**********CALCULO DO TEMPO DE ORDENAÇÃO********/
	start = clock();
	// essencial ordenar também as arestas
	qsort(edgList, m, sizeof(edg), cmp);	
	end = clock();
	time = (double) (end - start) / CLOCKS_PER_SEC;
	fprintf(timeOut, "Cálculo da Ordenação das Distâncias: %f\n", time);
	/*********CALCULADO O TEMPO DE ORDENAÇÃO********/

	/***********APLICAÇÃO DO ALGORITMO DE KRUSKAL************/
	start = clock();


	// nesse momento é feita a MST
	// a proposta é entregar uma MST sem k-1 arestas 
	// a formação da MST vai seguir um limite
	// o número de uma MST completa é N-1 arestas (N o número de pontos)
	// como se quer remover k-1 arestas, é mais proveitoso simplesmente não incluir as k-1 arestas de maior peso
	// sendo assim, o número máximo de arestas é (N-1)-(k-1)
	// o que resulta em N - k
	set* uf = initSet(n);
	int count = 0;

	for(int i=0; i<m; i++){	
		int p1 = edgList[i].p1;
		int p2 = edgList[i].p2;

		if(!connected(uf, p1, p2)){
			connect(uf, p1, p2);
			count++;
			if(count == (n-k)){
				// printf("%d\n", i);
				break;
			}
		}
	}

	// para deixar a saída ordenada de forma lexicográfica
	// optamos por formar uma quick-find diferente a partir das weighted quick-unions
	// o que aproveita menos trocas de valores na hora de formar a MST
	// mas é o que permite nesse código que a saída seja ordenada de acordo com a entrada ordenada no começo da execução do código
	int* v = uf->tree;
	int vector[k];
	int found = 0;
	int con = 0;
	for(int j=0; j<k; j++) vector[j] = -1;

	/*complexidade NlogN*/
	for(int i=0; i<n; i++){
		v[i] = find(uf, v[i]);
		for(int j=0; j<k; j++){
			if(v[i] == vector[j]){
				found = 1;
				break;
			}
		}
		if(found == 0){
			vector[con] = v[i];
			con++;
		}
		found = 0;

	}

	// for(int i =0; i<k; i++) printf("%d ", vector[i]);
	// printf("\n");
	
	/***********APLICAÇÃO DO ALGORITMO DE KRUSKAL************************************/
	end = clock();
	time = (double) (end - start) / CLOCKS_PER_SEC;	
	fprintf(timeOut, "Algoritmo de Kruskal e Componentes Conexas: %f\n", time);
	
	// imprime cada componente em ordem	
	for(int j=0; j<k; j++){	
		int current = -1;
		int counted=0;
		for(int i=0; i<=n; i++){	
			if(current == -1) current = v[i];
			else if(v[i-1] == current){
				if(counted == 1) fprintf(out, ",");
				fprintf(out, "%s", key[i-1]);
				v[i-1] = -1;
				counted=1;
			}
		}
		fprintf(out, "\n");
	}
	
	fclose(out);

	
	
	// liberação de memória	
	for(int i=0; i<n; i++){
		free(key[i]);	
	}
	free(edgList);	
	freeSet(uf);

	clock_t endTotal = clock();
	double timeTotal = (double) (endTotal - startTotal) / CLOCKS_PER_SEC;
	fprintf(timeOut, "Tempo Total de Execução do Código: %f\n", timeTotal);
	fclose(timeOut);

	return 0;
}

int strcmpCallback(const void* a, const void* b) {
    const char** string1 = (const char**) a;
    const char** string2 = (const char**) b;
    return strcmp(*string1, *string2);
}

set* initSet(int n){
	set* new = malloc(sizeof(set));
	new->size = n;
		
	new->tree = malloc(n*sizeof(int));	

	// útil para saber quem vai ser o pai de cada nó da árvore
	new->nSize = malloc(n*sizeof(int));

	for(int i=0; i<n; i++){
		new->tree[i] = i;
		new->nSize[i] = 0;
	}
	return new;	
}

void freeSet(set* ds){
	if(ds == NULL) return;
	
	free(ds->nSize);
	free(ds->tree);
	free(ds);
}

int find(set* s, int p){
	int* v = s->tree;

	while(p != v[p]){
		p = v[p];
	}
	
	return p;
}

void connect(set* s, int a, int b){
	
	int i = find(s, a);
	int j = find(s, b);
	
	if(i == j) return;
	
	if(s->nSize[i] < s->nSize[j]) {
		s->tree[i] = j;
		s->nSize[j] += s->nSize[i];
	} else {
			
		s->tree[j] = i;
		s->nSize[i] += s->nSize[j];
	}

}

int connected(set* s, int p, int q){
	return find(s, p) == find(s, q);
}

int cmp(const void* a, const void* b){

	return (((edg*)a)->w) > (((edg*)b)->w);
}
