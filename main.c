#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
//#include <unistd.h> linux

int distribuirCartas = 0;

typedef struct Carta{
    int valor;
    char naipe;
    int v;
    struct Carta *prox;
}Carta;

typedef struct Pilha{
    Carta *topo;
}Pilha;

Pilha* criaPilha(Pilha *p){
    p = (Pilha*) malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

void insereTopo(int valor, char naipe, int v, Pilha *p){
    Carta *novo = (Carta*) malloc (sizeof(Carta));
    novo->valor = valor;
    novo->naipe = naipe;
    novo->v = v;
    if(!(p->topo)){
        novo->prox = NULL;
        p->topo = novo;
    }else{
        novo->prox = p->topo;
        p->topo = novo;
    }
}

// Insere na base da pilha
void insereBase(int valor, char naipe, int v, Carta *base){
    Carta *novo = (Carta*) malloc (sizeof(Carta));
    novo->valor = valor;
    novo->naipe = naipe;
    novo->v = v;
    base->prox = novo;
    novo->prox = NULL;
}

void removeTopo(Pilha *p){
    if(!(p->topo)){
        printf("Pilha vazia\n");
        return ;
    }
    Carta *removido;
    removido = p->topo;
    p->topo = p->topo->prox;
    free(removido);
}

char geraValor(int valor){
    // Converte um valor num�rico char.
    if(valor == 1)
        return 'E';
    if(valor == 2)
        return 'C';
    if(valor == 3)
        return 'P';
    if(valor == 4)
        return 'O';
    return 0;
}

// Verifica se existe a mesma carta no baralho.
int existe(int v, int n, int indice, Carta *baralho){
    int i;
    for(i = 0; i < indice; i++){
        if(baralho[i].valor == v && baralho[i].naipe == n){
            return 1;
        }
    }
    return 0;
}

// Gera todas as cartas do baralho.
Pilha* criaBaralho(){
    Carta *baralho = (Carta*)malloc(sizeof(Carta) * 52);
    int i;
    for(i = 0; i < 52; i++){
        //baralho[i].valor = geraValor(1 + (rand() % 13), "Valor");
        //baralho[i].naipe = geraValor(1 + (rand() % 4), "Naipe");
        do{
            baralho[i].valor = 1 + (rand() % 13);
            baralho[i].naipe = geraValor(1 + (rand() % 4));
        }while(existe(baralho[i].valor, baralho[i].naipe, i, baralho));
    }
    return baralho;
}

void imprimeTodasCartas(Carta *baralho){
    int i;
    for(i = 0; i < 52; i++){
        printf("%d -> %c %i\n", i+1, baralho[i].naipe, baralho[i].valor);
    }
    puts("");
}

Pilha* criaEstoque(Pilha *p, Carta *baralho){
    p = (Pilha*) malloc ((sizeof(Pilha)) * 52);

    int i;
    for(i = 0; i < 52; i++){
        insereTopo(baralho[i].valor, baralho[i].naipe, 0, p);
    }
    Carta *temp = p->topo;
    for(i = 0; i < 51; i++){
        temp = temp->prox;
    }
    temp->prox = NULL;
    return p;
}

void montaPilha(Pilha *p[7], Pilha *estoque){
    Carta *temp = estoque->topo;
    Carta *temp2 = (Carta*)malloc(sizeof(Carta));
    // Distribui as cartas do estoque para todas as pilhas
    int i, j, k = 1;
    for(j = 0; j < 7; j++){
        for(i = 0; i < k; i++){
            if(i == 0){
                temp2 = temp;
                p[j]->topo = temp2;
                // Vira a carta do topo
                p[j]->topo->v = 1;
            }else{
                temp2->prox = temp;
                temp2 = temp2->prox;
            }
            temp = temp->prox;
        }
        temp2->prox = NULL;
        k++;
    }
    // Novo estoque com a quantidade de cartas que restaram.
    estoque->topo = temp;
    return ;
}

void imprimePilha(Pilha *p){
    Carta *temp = p->topo;
    if(!temp){
        printf("|\n");
        return ;
    }
    int i = 0;
    while(temp && i < 52){
    	// Cartas viradas para cima
    	// Se a carta aparece no topo de uma pilha, ela permanece virada
    	if(i == 0)
            temp->v = 1;
    	if(i == 0 || temp->v == 1){
	    	if(temp->naipe == 'C' || temp->naipe == 'O')
	        	printf("{%i %c} ", temp->valor, temp->naipe);
	        else
	        	printf("(%i %c) ", temp->valor, temp->naipe);
    	}else
    		printf("[- -] ");
		temp = temp->prox;
		i++;
		if(distribuirCartas == 1)
            Sleep(100);
    }
    puts("");
}

Carta *ProcuraFim(Pilha *p){
    if(!(p->topo)){
        return NULL;
    }
    Carta *atual = p->topo;
    while(atual->prox)
        atual = atual->prox;
    return atual;
}

void estoqueParaNaipes(Pilha *e, Pilha *n){
    Carta *ultima = ProcuraFim(n); // Guarda a �ltima carta da pilha de naipes
    if(!ultima && e->topo->valor == 1){
        insereTopo(e->topo->valor, e->topo->naipe, 1, n);
        removeTopo(e);
    }else if(!ultima){
        printf("Movimento errado\n");
        return;
    }
    // Verifica se a ultima carta da pilha de naipe possui uma unidade a menos do que a carta a ser inserida
    else if(e->topo->valor == ultima->valor + 1){
        insereBase(e->topo->valor, e->topo->naipe, 1, ultima);
        removeTopo(e);
    }else
        printf("Movimento errado\n");
}

void fileiraParaNaipes(Pilha *f, Pilha *n){
    Carta *ultima = ProcuraFim(n); // Guarda a �ltima carta da pilha de naipes
    if(!ultima && f->topo->valor == 1){
        insereTopo(f->topo->valor, f->topo->naipe, 1, n);
        removeTopo(f);
    }else if(!ultima){
        printf("Movimento errado\n");
        return;
    }
    // Verifica se a ultima carta da pilha de naipe possui uma unidade a menos do que a carta a ser inserida
    else if(f->topo->valor == ultima->valor + 1){
        insereBase(f->topo->valor, f->topo->naipe, 1, ultima);
        removeTopo(f);
    }else
        printf("Movimento errado\n");
}

void FileiraParaFileira(Pilha *f, Pilha *n){
	if(!(n->topo)){
		if(f->topo->valor == 13){
			insereTopo(13, f->topo->naipe, 1, n);
			removeTopo(f);
		}
	}else if(f->topo->naipe == 'C' || f->topo->naipe == 'O'){
		if(n->topo->naipe == 'E' || n->topo->naipe == 'P'){
			if(f->topo->valor == n->topo->valor - 1){
        		insereTopo(f->topo->valor, f->topo->naipe, 1, n);
        		removeTopo(f);
        	}
		}
	}else if(f->topo->naipe == 'E' || f->topo->naipe == 'P'){
		if(n->topo->naipe == 'C' || n->topo->naipe == 'O'){
			if(f->topo->valor == n->topo->valor - 1){
        		insereTopo(f->topo->valor, f->topo->naipe, 1, n);
        		removeTopo(f);
        	}
		}
	}else printf("Movimento errado!\n");
}

// Busca a posi��o selecionada e move a pilha da posi��o escolhida para a base da outra pilha.
void PosFileiraParaFileira(Pilha *f, int pos, Pilha *n){
    Carta *atual = f->topo;
    // Verifica tamanho da pilha e remove o possivel erro de fora do intervalo.
    int tam;
    for(tam = 1; atual->prox; tam++){
        atual = atual->prox;
    }
    atual = f->topo;
    if(pos > tam || pos < 1){
        printf("Fora de intervalo\n");
        return;
    }
    int i;
    for(i = 1; i < pos; i++)
        atual = atual->prox;
    // Variavel que s� � ativada quando uma das condi��es para mover as cartas for verdadeira.
    int aux = 0;
	if(!(n->topo)){
		if(atual->valor == 13){
            aux = 1;
		}
	}else if(atual->naipe == 'C' || atual->naipe == 'O'){
		if(n->topo->naipe == 'E' || n->topo->naipe == 'P'){
			if(atual->valor == n->topo->valor - 1){
        		aux = 1;
        	}
		}
	}else if(atual->naipe == 'E' || atual->naipe == 'P'){
		if(n->topo->naipe == 'C' || n->topo->naipe == 'O'){
			if(atual->valor == n->topo->valor - 1){
        		aux = 1;
        	}
		}
	}else{
        printf("Nao e possivel fazer esse movimento\n");
        return;
	}
    if(aux == 1){
        Carta *temp = atual->prox;
        atual->prox = n->topo;
        n->topo = f->topo;
        f->topo = temp;
    }
}

void ProximaCarta(Pilha *estoque){
	Carta *temp = estoque->topo,
		  *aux = estoque->topo;
	// Procura o fim
	while(temp->prox)
		temp = temp->prox;
	aux = estoque->topo;
	estoque->topo = estoque->topo->prox;
	temp->prox = aux;
	aux->prox = NULL;
}

void EstoqueParaFileira(Pilha *e, Pilha *f){
    if(!(f->topo)){
		if(e->topo->valor == 13){
			insereTopo(13, e->topo->naipe, 1, f);
			removeTopo(e);
		}
	}else if(e->topo->naipe == 'C' || e->topo->naipe == 'O'){
		if(f->topo->naipe == 'E' || f->topo->naipe == 'P'){
			if(e->topo->valor == f->topo->valor - 1){
        		insereTopo(e->topo->valor, e->topo->naipe, 1, f);
        		removeTopo(e);
        	}
		}
	}else if(e->topo->naipe == 'E' || e->topo->naipe == 'P'){
		if(f->topo->naipe == 'C' || f->topo->naipe == 'O'){
			if(e->topo->valor == f->topo->valor - 1){
        		insereTopo(e->topo->valor, e->topo->naipe, 1, f);
        		removeTopo(e);
        	}
		}
	}else printf("Movimento errado\n");
}

void verificarVitoria(Pilha *n[4]){
    Carta *n1 = ProcuraFim(n[0]);
    Carta *n2 = ProcuraFim(n[1]);
    Carta *n3 = ProcuraFim(n[2]);
    Carta *n4 = ProcuraFim(n[3]);
    if(n1 && n1->valor == 13
    && n2 && n2->valor == 13
    && n3 && n3->valor == 13
    && n4 && n4->valor == 13){
        printf("\n");
        printf("|    |' |   /     '    |\n");
        printf(" \\  / |~|~/~\\|/~\\|/~~|| \n");
        printf("  \\/  | | \\_/|   |\\__|.\n");
        exit(1);
    }
}

void imprime(Pilha *estoque, Pilha *naipes[4], Pilha *fileiras[7]){
    printf("--- Estoque ---\n");
    imprimePilha(estoque);
    puts("");

    printf("--- Naipes ---\n");
    printf("1 > Copas   > ");
    imprimePilha(naipes[0]);
    printf("2 > Espadas > ");
    imprimePilha(naipes[1]);
    printf("3 > Paus    > ");
    imprimePilha(naipes[2]);
    printf("4 > Ouros   > ");
    imprimePilha(naipes[3]);
    puts("");

    printf("--- Fileiras ---\n");
    printf("1 > ");
    imprimePilha(fileiras[0]);
    printf("2 > ");
    imprimePilha(fileiras[1]);
    printf("3 > ");
    imprimePilha(fileiras[2]);
    printf("4 > ");
    imprimePilha(fileiras[3]);
    printf("5 > ");
    imprimePilha(fileiras[4]);
    printf("6 > ");
    imprimePilha(fileiras[5]);
    printf("7 > ");
    imprimePilha(fileiras[6]);

    verificarVitoria(naipes);
    distribuirCartas = 0;
}

void menu(Pilha *estoque, Pilha *naipes[4], Pilha *fileiras[7]){
    int opcao, f, n, pos;
    while(1){
    	printf("\nEscolha a opcao: \n");
	    printf("Mover cartas:\n");
	    printf("1 > De uma fileira para outra fileira\n");
	    printf("2 > De uma fileira para a pilha de naipes.\n");
	    printf("3 > Da posicaoo de uma fileira para outra fileira\n");
	    printf("4 > Do estoque para uma fileira\n");
	    printf("5 > Do estoque para a pilha de naipes.\n");
	    printf("6 > Proxima carta do estoque\n");
	    scanf("%d", &opcao);
        switch(opcao){
        case 1:
            printf("Fileira 1:\n");
            scanf("%d", &f);
            printf("Fileira 2\n");
            scanf("%d", &n);
            system("cls");
            if(!(fileiras[f-1]->topo)){
                printf("N�o existe cartas nessa fileira\n");
                break;
            }
            FileiraParaFileira(fileiras[f-1], fileiras[n-1]);
            system("cls");
            break;
        case 2:
            printf("Fileira:\n");
            scanf("%d", &f);
            printf("Naipes\n");
            scanf("%d", &n);
            system("cls");
            if(!(fileiras[f-1]->topo)){
                printf("N�o existe cartas nessa fileira\n");
                break;
            }
            else if(n == 1 && fileiras[f-1]->topo->naipe == 'C')
                fileiraParaNaipes(fileiras[f-1], naipes[0]);
            else if(n == 2 && fileiras[f-1]->topo->naipe == 'E')
                fileiraParaNaipes(fileiras[f-1], naipes[1]);
            else if(n == 3 && fileiras[f-1]->topo->naipe == 'P')
                fileiraParaNaipes(fileiras[f-1], naipes[2]);
            else if(n == 4 && fileiras[f-1]->topo->naipe == 'O')
                fileiraParaNaipes(fileiras[f-1], naipes[3]);
            else{
                system("cls");
                printf("Movimento errado.\n\n");
            }
            break;
        case 3:
            printf("Fileira 1 / Posi��o 1:\n");
            scanf("%d%d", &f, &pos);
            printf("Fileira 2\n");
            scanf("%d", &n);
            if(!(fileiras[f-1]->topo)){
                printf("N�o existe cartas nessa fileira\n");
                break;
            }
            PosFileiraParaFileira(fileiras[f-1], pos, fileiras[n-1]);
            system("cls");
            break;
        case 4:
            printf("Fileira:\n");
            scanf("%d", &n);
            system("cls");
            if(!(estoque->topo)){
                printf("N�o existe cartas no estoque\n");
                break;
            }
            EstoqueParaFileira(estoque, fileiras[n-1]);
            system("cls");
            break;
        case 5:
            printf("Naipes:\n");
            scanf("%d", &n);
            if(n == 1 && estoque->topo->naipe == 'C')
                estoqueParaNaipes(estoque, naipes[0]);
            else if(n == 2 && estoque->topo->naipe == 'E')
                estoqueParaNaipes(estoque, naipes[1]);
            else if(n == 3 && estoque->topo->naipe == 'P')
                estoqueParaNaipes(estoque, naipes[2]);
            else if(n == 4 && estoque->topo->naipe == 'O')
                estoqueParaNaipes(estoque, naipes[3]);
            else{
                system("cls");
                printf("Movimento errado\n");
                break;
            }
            system("cls");
            break;
        case 6:
            ProximaCarta(estoque);
            system("cls");
        	break;
        default:
            break;
        }
        imprime(estoque, naipes, fileiras);
    }
}

int main()
{
    // Ajustes técnicos
    system("color 2f");
    srand(time(NULL));
    setlocale(LC_ALL, "Portuguese");
    system("cls");

    // Inicializa pilhas de estoque, de naipes e de fileiras
    Pilha *estoque = criaPilha(estoque);
    Pilha *naipes[4];
    int i;
    for(i = 0; i < 4; i++){
        naipes[i] = criaPilha(naipes[i]);
    }
    Pilha *fileiras[7];
    for(i = 0; i < 7; i++){
        fileiras[i] = criaPilha(fileiras[i]);
    }

    // Cria um baralho contendo 52 cartas aleatoriamente sorteadas na pilha de estoque
    Carta *baralho = criaBaralho();
    imprimeTodasCartas(baralho);
    estoque = criaEstoque(estoque, baralho);
    imprimePilha(estoque);
    printf("Distribuir cartas...\n");
    distribuirCartas = 1;
    getch();
    system("cls");

    // Inicio do jogo
    montaPilha(fileiras, estoque);
    imprime(estoque, naipes, fileiras);
    menu(estoque, naipes, fileiras);

    return 0;
}
