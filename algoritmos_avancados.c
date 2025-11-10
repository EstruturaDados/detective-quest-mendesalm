/* 
*Desafio nível aventureiro
*AUTOR: André Luiz Mendes
*DATA: 10/11/25
*/

#include <string.h>
#include <strings.h> 

// --- Constantes ---
#define HASH_TABLE_SIZE 10
#define PISTAS_PARA_ACUSAR 2

// --- Estrutura 1: Mapa da Mansão (Árvore Binária Simples) ---
struct Sala {
    char nome[100];
    char* pista; // Ponteiro para a pista (NULL se não houver ou já coletada)
    struct Sala* esquerda;
    struct Sala* direita;
};

// --- Estrutura 2: Inventário de Pistas (Árvore Binária de Busca - BST) ---
struct PistaNode {
    char pista[256];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
};

// --- Estrutura 3: Banco de Dados de Suspeitos (Tabela Hash) ---
// Nó para a lista encadeada (tratamento de colisões)
struct HashNode {
    char pista[256];   // Chave
    char suspeito[100]; // Valor
    struct HashNode* next;
};

// A própria tabela hash
struct HashTable {
    struct HashNode** baldes; // Um array de ponteiros para HashNode
    int tamanho;
};

// --- Protótipos: Mapa (Árvore) ---
struct Sala* criarSala(const char* nome, const char* pistaStr);
void liberarMemoriaMapa(struct Sala* sala);

// --- Protótipos: Pistas (BST) ---
struct PistaNode* criarNoPista(const char* pista);
struct PistaNode* inserirPista(struct PistaNode* no, const char* pista);
void exibirPistas(struct PistaNode* no);
void liberarMemoriaPistas(struct PistaNode* no);

// --- Protótipos: Suspeitos (Tabela Hash) ---
unsigned int hashFunction(const char* str);
struct HashTable* criarTabelaHash(int tamanho);
void inserirNaHash(struct HashTable* tabela, const char* pista, const char* suspeito);
const char* encontrarSuspeito(struct HashTable* tabela, const char* pista);
void liberarTabelaHash(struct HashTable* tabela);

// --- Protótipos: Lógica do Jogo ---
void explorarSalas(struct Sala* salaInicial, struct PistaNode** pRaizPistas);
void verificarSuspeitoFinal(struct PistaNode* raizPistas, struct HashTable* tabelaSuspeitos);
void contarPistasDoSuspeito(struct PistaNode* noPista, struct HashTable* tabela, const char* suspeitoAcusado, int* pContador);


// --- Função Principal (main) ---
int main() {
    printf("--- Detective Quest: O Julgamento Final ---\n");

    // 1. Inicializa as 3 estruturas de dados
    struct Sala* hall = NULL;
    struct PistaNode* raizPistas = NULL;
    struct HashTable* tabelaSuspeitos = criarTabelaHash(HASH_TABLE_SIZE);

    // 2. Popula o Mapa da Mansão (Árvore)
    hall = criarSala("Hall de entrada", NULL);
    hall->esquerda = criarSala("Sala de estar", "Um recibo de alfaiate amassado");
    hall->direita = criarSala("Sala de jantar", "Um cálice de vinho virado");

    hall->esquerda->esquerda = criarSala("Biblioteca", "Livro 'A Arte da Fuga' fora da estante");
    hall->esquerda->direita = criarSala("Cozinha", "Um botão de uniforme caído");
    
    hall->direita->esquerda = criarSala("Jardim de inverno", "Terra úmida no tapete");
    hall->direita->direita = criarSala("Laboratório", "Frasco de veneno vazio");

    // 3. Popula o Banco de Dados (Tabela Hash)
    // Associando Pistas (Chaves) a Suspeitos (Valores)
    // O Mordomo, a Jardineira e a Condessa têm 2 pistas cada.
    inserirNaHash(tabelaSuspeitos, "Um recibo de alfaiate amassado", "Mordomo");
    inserirNaHash(tabelaSuspeitos, "Um botão de uniforme caído", "Mordomo");
    
    inserirNaHash(tabelaSuspeitos, "Um cálice de vinho virado", "Condessa");
    inserirNaHash(tabelaSuspeitos, "Frasco de veneno vazio", "Condessa");

    inserirNaHash(tabelaSuspeitos, "Livro 'A Arte da Fuga' fora da estante", "Jardineira");
    inserirNaHash(tabelaSuspeitos, "Terra úmida no tapete", "Jardineira");

    printf("Mapa da mansão e banco de dados de suspeitos carregados.\n");
    printf("A exploração começa. Você precisa de %d pistas para uma acusação.\n", PISTAS_PARA_ACUSAR);

    // 4. Inicia a Exploração (Coleta pistas na BST 'raizPistas')
    explorarSalas(hall, &raizPistas);

    // 5. Inicia a Fase de Julgamento
    // Usa a BST (pistas coletadas) e a Tabela Hash (suspeitos)
    verificarSuspeitoFinal(raizPistas, tabelaSuspeitos);

    // 6. Libera toda a memória alocada
    liberarMemoriaMapa(hall);
    liberarMemoriaPistas(raizPistas);
    liberarTabelaHash(tabelaSuspeitos);

    printf("\nSistema finalizado.\n");
    return 0;
}


// --- Funções do Mapa (Árvore) ---

/**
 * @brief Cria dinamicamente um cômodo (Sala) com ou sem pista.
 */
struct Sala* criarSala(const char* nome, const char* pistaStr) {
    struct Sala* novaSala = (struct Sala*)malloc(sizeof(struct Sala));
    if (novaSala == NULL) exit(1);
    
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    
    if (pistaStr != NULL) {
        novaSala->pista = (char*)malloc(strlen(pistaStr) + 1);
        if (novaSala->pista != NULL) {
            strcpy(novaSala->pista, pistaStr);
        }
    } else {
        novaSala->pista = NULL;
    }
    return novaSala;
}

void liberarMemoriaMapa(struct Sala* sala) {
    if (sala == NULL) return;
    liberarMemoriaMapa(sala->esquerda);
    liberarMemoriaMapa(sala->direita);
    if (sala->pista != NULL) {
        free(sala->pista);
    }
    free(sala);
}

// --- Funções das Pistas (BST) ---

struct PistaNode* criarNoPista(const char* pista) {
    struct PistaNode* novo = (struct PistaNode*)malloc(sizeof(struct PistaNode));
    if (novo == NULL) exit(1);
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/**
 * @brief Insere a pista coletada na árvore de pistas (BST).
 * Mantém a ordem alfabética e ignora duplicatas.
 */
struct PistaNode* inserirPista(struct PistaNode* no, const char* pista) {
    if (no == NULL) {
        return criarNoPista(pista);
    }
    int cmp = strcmp(pista, no->pista);
    if (cmp < 0) {
        no->esquerda = inserirPista(no->esquerda, pista);
    } else if (cmp > 0) {
        no->direita = inserirPista(no->direita, pista);
    }
    return no;
}

/**
 * @brief Exibe as pistas da BST em ordem alfabética (In-Order).
 */
void exibirPistas(struct PistaNode* no) {
    if (no == NULL) return;
    exibirPistas(no->esquerda);
    printf("  - %s\n", no->pista);
    exibirPistas(no->direita);
}

void liberarMemoriaPistas(struct PistaNode* no) {
    if (no == NULL) return;
    liberarMemoriaPistas(no->esquerda);
    liberarMemoriaPistas(no->direita);
    free(no);
}


// --- Funções da Tabela Hash (Suspeitos) ---

/**
 * @brief Função de hash simples (djb2).
 */
unsigned int hashFunction(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % HASH_TABLE_SIZE;
}

/**
 * @brief Aloca a Tabela Hash e seus 'baldes' (buckets).
 */
struct HashTable* criarTabelaHash(int tamanho) {
    struct HashTable* tabela = (struct HashTable*)malloc(sizeof(struct HashTable));
    if (tabela == NULL) exit(1);
    
    tabela->tamanho = tamanho;
    // Usa calloc para alocar e inicializar todos os ponteiros para NULL
    tabela->baldes = (struct HashNode**)calloc(tamanho, sizeof(struct HashNode*));
    if (tabela->baldes == NULL) exit(1);
    
    return tabela;
}

/**
 * @brief Insere associação pista/suspeito na tabela hash.
 * Usa encadeamento (chaining) para tratar colisões.
 */
void inserirNaHash(struct HashTable* tabela, const char* pista, const char* suspeito) {
    unsigned int indice = hashFunction(pista);
    
    // Cria o novo nó
    struct HashNode* novoNo = (struct HashNode*)malloc(sizeof(struct HashNode));
    if (novoNo == NULL) exit(1);
    strcpy(novoNo->pista, pista);
    strcpy(novoNo->suspeito, suspeito);
    
    // Insere no início da lista encadeada (balde)
    novoNo->next = tabela->baldes[indice];
    tabela->baldes[indice] = novoNo;
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 * Retorna o nome do suspeito ou NULL se a pista não for encontrada.
 */
const char* encontrarSuspeito(struct HashTable* tabela, const char* pista) {
    unsigned int indice = hashFunction(pista);
    struct HashNode* atual = tabela->baldes[indice];
    
    // Percorre a lista encadeada no balde
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito; // Encontrado
        }
        atual = atual->next;
    }
    return NULL; // Não encontrado
}

/**
 * @brief Libera toda a memória da Tabela Hash e suas listas.
 */
void liberarTabelaHash(struct HashTable* tabela) {
    if (tabela == NULL) return;
    for (int i = 0; i < tabela->tamanho; i++) {
        struct HashNode* atual = tabela->baldes[i];
        while (atual != NULL) {
            struct HashNode* temp = atual;
            atual = atual->next;
            free(temp);
        }
    }
    free(tabela->baldes);
    free(tabela);
}


// --- Funções da Lógica do Jogo ---

/**
 * @brief Navega pela árvore e ativa o sistema de coleta de pistas.
 * A raiz da BST de pistas (pRaizPistas) é modificada.
 */
void explorarSalas(struct Sala* salaInicial, struct PistaNode** pRaizPistas) {
    struct Sala* salaAtual = salaInicial;
    char escolha;

    while (1) { // Loop infinito, controlado pelo 'return' (escolha 's')
        printf("\n----------------------------------------\n");
        printf("🕵️ Você está em: %s\n", salaAtual->nome);

        // Lógica de Coleta:
        if (salaAtual->pista != NULL) {
            printf("[!] PISTA ENCONTRADA: %s\n", salaAtual->pista);
            
            // Adiciona a pista à BST (Inventário)
            *pRaizPistas = inserirPista(*pRaizPistas, salaAtual->pista);
            
            // "Coleta" a pista (remove da sala para não coletar de novo)
            free(salaAtual->pista);
            salaAtual->pista = NULL;
        }

        // Opções de Navegação
        printf("\nEscolha seu caminho:\n");
        if (salaAtual->esquerda != NULL) {
            printf("  (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf("  (d) Direita: %s\n", salaAtual->direita->nome);
        }
        printf("  (s) Sair e fazer acusação final\n");
        printf("Sua escolha: ");

        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e': case 'E':
                if (salaAtual->esquerda != NULL) {
                    salaAtual = salaAtual->esquerda;
                } else {
                    printf("Ops! Não há caminho para a esquerda aqui.\n");
                }
                break;
            case 'd': case 'D':
                if (salaAtual->direita != NULL) {
                    salaAtual = salaAtual->direita;
                } else {
                    printf("Ops! Não há caminho para a direita aqui.\n");
                }
                break;
            case 's': case 'S':
                printf("Você vai para a sala de interrogatório finalizar o caso...\n");
                return; // Encerra a exploração
            default:
                printf("Comando inválido. Tente 'e', 'd' ou 's'.\n");
        }
    }
}

/**
 * @brief (Função auxiliar recursiva) Conta quantas pistas na BST
 * apontam para o suspeito acusado.
 */
void contarPistasDoSuspeito(struct PistaNode* noPista, struct HashTable* tabela, 
                            const char* suspeitoAcusado, int* pContador) {
    if (noPista == NULL) {
        return; // Caso base
    }

    // Percurso In-Order (Esquerda, Raiz, Direita)
    contarPistasDoSuspeito(noPista->esquerda, tabela, suspeitoAcusado, pContador);

    // Processa o nó atual (Raiz)
    // 1. Pega a pista da BST
    // 2. Procura o suspeito daquela pista na Tabela Hash
    const char* suspeitoDaPista = encontrarSuspeito(tabela, noPista->pista);

    if (suspeitoDaPista != NULL) {
        // 3. Compara (sem case) com o suspeito acusado pelo jogador
        if (strcasecmp(suspeitoDaPista, suspeitoAcusado) == 0) {
            (*pContador)++; // Incrementa o contador
        }
    }
    
    contarPistasDoSuspeito(noPista->direita, tabela, suspeitoAcusado, pContador);
}

/**
 * @brief Conduz à fase de julgamento final.
 * Pede a acusação e verifica na Tabela Hash usando as pistas da BST.
 */
void verificarSuspeitoFinal(struct PistaNode* raizPistas, struct HashTable* tabelaSuspeitos) {
    printf("\n\n--- FASE DE JULGAMENTO ---\n");

    if (raizPistas == NULL) {
        printf("Você não coletou nenhuma pista! O caso está perdido.\n");
        return;
    }

    // 1. Lista as pistas coletadas (da BST)
    printf("Pistas coletadas (em ordem alfabética):\n");
    exibirPistas(raizPistas);
    
    // 2. Pergunta ao jogador
    char acusado[100];
    printf("\nOs suspeitos são: Mordomo, Jardineira, Condessa.\n");
    printf("Quem você acusa? ");
    scanf("%99s", acusado); // Lê o nome do acusado

    // 3. Verifica as evidências
    int contadorDePistas = 0;
    
    // Chama a função recursiva para contar as pistas
    // Passa o endereço do contador (&contadorDePistas)
    contarPistasDoSuspeito(raizPistas, tabelaSuspeitos, acusado, &contadorDePistas);

    printf("\n--- O VEREDITO ---\n");
    printf("Verificando... %d pista(s) que você encontrou apontam para: %s.\n", 
           contadorDePistas, acusado);

    // 4. Conclusão Final
    if (contadorDePistas >= PISTAS_PARA_ACUSAR) {
        printf("Evidências suficientes! Você desvendou o mistério!\n");
        printf("CASO ENCERRADO.\n");
    } else {
        printf("Acusação falhou! As pistas não são fortes o suficiente.\n");
        printf("O verdadeiro culpado escapou...\n");
        printf("CASO ARQUIVADO.\n");
    }
}
