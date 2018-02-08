/* ASA - Projeto 2
Grupo 29: Miguel Regouga, 83530 | Pedro Caldeira, 83539
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct kruskalResult {
    int custo;
    int aeroportosTot;
    int estradasTot;
    int flagInsuficiente;
};

struct Edge {
    int from;
    int to;
    int custo; /* Custo das estradas (edges) */
    int label; /*1 significa que e do sky. 0 significa que e estradas*/
};
 
struct Graph {
    int numArcos;
    int numVertices;
    struct Edge* edge;
};
 
struct Graph* createNewGraph(int v, int a) {
    struct Graph* g = malloc( sizeof(struct Graph) );
    g->numArcos = a;
    g->numVertices = v;
    g->edge = malloc(g->numArcos * sizeof(struct Edge));
    
    return g;
}
 
struct subset {
    int parent;
    int rank;
};

struct Edge createNewEdge(int f, int t, int c, int l) {
    struct Edge newEdge;
    newEdge.from = f;
    newEdge.to = t;
    newEdge.custo = c;
    newEdge.label = l;
    return newEdge;
}
 
int findSet(struct subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = findSet(subsets, subsets[i].parent);
 
    return subsets[i].parent;
}
 
void Union(struct subset subsets[], int x, int y) {
    int xroot = findSet(subsets, x);
    int yroot = findSet(subsets, y);
 
    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;

    else {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}


int left(int k) {
    return 2*k+1;
}

int right(int k) {
    return 2*(k+1);
} 

int less(struct Edge e1, struct Edge e2){
    if(e1.custo == e2.custo) {
        if(e1.label ==  e2.label)
            return 1;
        if(e1.label && !e2.label)
            return 0; 
        if(e2.label  && !e1.label)
            return 1; 
        return 0;
    }
    if(e1.custo < e2.custo) return 1;
    else return 0;
}

struct Edge* fixDown(struct Edge array[], int l, int r, int k){
    int ileft, iright, largest = k;
    ileft = l + left(k-l);
    iright = l + right(k-l);
    if(ileft <= r && less(array[largest], array[ileft]))
        largest = ileft;
    if(iright <= r && less(array[largest], array[iright]))
        largest = iright;
    if(largest != k){
        struct Edge e;
        e = array[k];
        array[k] = array[largest];
        array[largest] = e;   
        array = fixDown(array, l, r, largest);
    }
    return array;
} 

struct Edge* buildheap(struct Edge array[], int l, int r){
    int k, heapsize;
    heapsize = r-l+1;
    for (k = heapsize/2-1; k >= l; k--)
        array = fixDown(array, l, r, l+k);
    return array;
} 

struct Edge* heapsort2(struct Edge array[], int l, int r){
    array = buildheap(array,l,r);
    while (r-l > 0) {
        struct Edge e = array[l];
        array[l] = array[r];
        array[r] = e; 
        array = fixDown(array, l, --r, l);
    }
    return array;
}

 
struct kruskalResult kruskal(struct Graph* graph, int skyHigh) {
    int V = graph->numVertices;
    int i = 0;  
    int v;
    struct kruskalResult resultado;
    resultado.custo = 0;
    resultado.aeroportosTot = 0;
    resultado.estradasTot = 0;
    resultado.flagInsuficiente = 0;
 
    struct subset *s = malloc(V * sizeof(struct subset));
 
    for (v = 0; v < V; ++v) {
        s[v].parent = v;
        s[v].rank = 0;
    }

    while(i < graph->numArcos) {
        struct Edge proximaEdge = graph->edge[i++];

        if(skyHigh == 0 && (proximaEdge.label || proximaEdge.to == V)) continue;

        int first = findSet(s, proximaEdge.from);
        int second = findSet(s, proximaEdge.to);

        if (first != second) {
            Union(s, first, second);
            resultado.custo += proximaEdge.custo;
            if(proximaEdge.label)
                resultado.aeroportosTot++;
            else
                resultado.estradasTot++;
        }
    }
    return resultado;
}
 
int main() {
    int numeroCidades, numeroEstradas, numeroAeroportos, minEdges;
    int i = 0, j = 0, v, a, cidadeA, cidadeB, cidade, custo, custoAeroporto; 
    struct kruskalResult resultado1, resultado2, resultadofinal;

    scanf("%d", &numeroCidades);
    scanf("%d", &numeroAeroportos);

    struct Edge vetorAeroportos[numeroAeroportos];

    if(numeroAeroportos != 0) {
        while(i != numeroAeroportos) {
            scanf("%d %d", &cidade, &custoAeroporto);
            vetorAeroportos[i] = createNewEdge(cidade - 1, numeroCidades, custoAeroporto, 1);
            i++;
        }
    }
    scanf("%d", &numeroEstradas);

    if(numeroAeroportos != 0) v = numeroCidades + 1;
    else v = numeroCidades;

    a = numeroAeroportos + numeroEstradas;
    struct Graph* graph = createNewGraph(v, a);
    
    
    while (j != numeroEstradas) {
        scanf("%d %d %d", &cidadeA, &cidadeB, &custo);
        graph->edge[j] = createNewEdge(cidadeA - 1, cidadeB - 1, custo, 0);
        j++;
    }

    for(i = 0; i < numeroAeroportos; i++, j++)
        graph->edge[j] = vetorAeroportos[i];

    heapsort2(graph->edge, 0, graph->numArcos);

    resultado1 = kruskal(graph, 0);
    resultado2 = kruskal(graph, numeroAeroportos);

    if (numeroAeroportos) minEdges = graph->numVertices - 2;
    else minEdges = graph->numVertices - 1;

    if(numeroAeroportos == 0 || (resultado1.custo <= resultado2.custo && resultado1.estradasTot == minEdges)) {
        resultadofinal.custo = resultado1.custo;
        resultadofinal.aeroportosTot = 0;
        resultadofinal.estradasTot = resultado1.estradasTot;
    }
    else {
        resultadofinal = resultado2;
    }

    resultadofinal.flagInsuficiente = 0;

    if(resultado1.estradasTot != minEdges && resultadofinal.estradasTot + resultadofinal.aeroportosTot != minEdges + 1){
        resultadofinal.flagInsuficiente = 1;
    }

    if(!resultadofinal.flagInsuficiente) {
        printf("%d\n%d %d\n", resultadofinal.custo, resultadofinal.aeroportosTot, resultadofinal.estradasTot);
    }
    else {
        printf("Insuficiente\n");
    }

    return 0;
}