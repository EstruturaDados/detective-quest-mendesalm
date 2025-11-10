/* 
*Desafio nível aventureiro
*AUTOR: André Luiz Mendes
*DATA: 10/11/25
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estrutura 1: Mapa da Mansão (Árvore Binária Simples) ---

/**
 * @brief Define a estrutura de um cômodo (Sala) no mapa.
 * Contém um ponteiro opcional para uma pista.
 */
struct Sala {
    char nome[100];
    char* pista; // Ponteiro para a pista (NULL se não houver pista)
    struct Sala* esquerda;
    struct Sala* direita;
};

// --- Estrutura 2: Inventário de Pistas (Árvore Binária de Busca - BST) ---

/**
 * @brief Define o nó da Árvore Binária de Busca (BST) para pistas.
 * Armazena a pista de forma ordenada.
 */
struct PistaNode {
    char pista[256]; // Buffer fixo para a pista
    struct PistaNode* esquerda;
    struct PistaNode* direita;
};

// --- Protótipos das Funções (Mapa) ---
struct Sala* criarSala(const char* nome, const char* pistaStr);
void liberarMemoriaMapa(struct Sala* sala);

// --- Protótipos das Funções (Pistas - BST) ---
struct PistaNode* criarNoPista(const char* pista);
struct PistaNode* inserirPista(struct PistaNode* no, const char* pista);
void exibirPistas(struct PistaNode* no);
void liberarMemoriaPistas(struct PistaNode* no);

// --- Protótipo da Função (Exploração) ---
void explorarSalasComPistas(struct Sala* salaInicial, struct PistaNode** pRaizPistas);


// --- Função Principal (main) ---

/**
 * @brief Ponto de entrada do programa.
 * 1. Monta o mapa da mansão (com pistas incluídas).
 * 2. Inicializa a árvore de pistas (BST) como vazia.
 * 3. Inicia a exploração.
 * 4. Exibe as pistas coletadas ao final.
 * 5. Libera toda a memória alocada.
 */
int main() {
    printf("--- Detective Quest: O Mistério da Mansão ---\n");

    // 1. Montagem do mapa da mansão (Árvore Binária)
    // Algumas salas agora recebem uma string de pista (o 2º argumento).
    // Se não houver pista, passamos NULL.
    struct Sala* hall = criarSala("Hall de entrada", NULL);

    // Nível 1
    hall->esquerda = criarSala("Sala de estar", "Um recibo de alfaiate amassado.");
    hall->direita = criarSala("Sala de jantar", "Um cálice de vinho virado.");

    // Nível 2
    hall->esquerda->esquerda = criarSala("Biblioteca", "Livro 'A Arte da Fuga' fora da estante.");
    hall->esquerda->direita = criarSala("Cozinha", "Cheiro fraco de charuto no exaustor.");
    hall->direita->direita = criarSala("Jardim de inverno", NULL);

    // Nível 3 (Folhas)
    hall->esquerda->esquerda->esquerda = criarSala("Escritório secreto", "Um cofre aberto e vazio.");
    hall->esquerda->direita->esquerda = criarSala("Despensa", NULL);
    hall->direita->direita->direita = criarSala("Estátua misteriosa", "Uma chave pequena escondida na base.");

    // 2. Inicializa a raiz da BST de Pistas
    struct PistaNode* raizPistas = NULL;
    
    printf("Mapa carregado. A exploração começa agora.\n");

    // 3. Inicia a exploração
    // Passamos o endereço da raizPistas (ponteiro para ponteiro)
    // para que a função possa modificar a árvore.
    explorarSalasComPistas(hall, &raizPistas);

    // 4. Exibe o resultado final
    printf("\n----------------------------------------\n");
    printf("📋 INVENTÁRIO DE PISTAS (Ordem Alfabética):\n");
    
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi encontrada durante a exploração.\n");
    } else {
        // A função 'exibirPistas' faz o percurso em ordem (in-order)
        exibirPistas(raizPistas);
    }
    printf("----------------------------------------\n");

    // 5. Libera a memória das duas árvores
    liberarMemoriaMapa(hall);
    liberarMemoriaPistas(raizPistas);
    printf("Sistema finalizado.\n");

    return 0;
}

// --- Funções do Mapa (Árvore Binária) ---

/**
 * @brief Aloca dinamicamente uma Sala (mapa).
 * Se 'pistaStr' não for NULL, aloca memória para a pista e a copia.
 *
 * @param nome O nome do cômodo.
 * @param pistaStr A string da pista (ou NULL se não houver).
 * @return Ponteiro para a nova Sala criada.
 */
struct Sala* criarSala(const char* nome, const char* pistaStr) {
    struct Sala* novaSala = (struct Sala*)malloc(sizeof(struct Sala));
    if (novaSala == NULL) {
        exit(1); // Falha crítica
    }
    
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    // Alocação dinâmica da pista (se existir)
    if (pistaStr != NULL) {
        // Aloca memória para a string da pista (+1 para o '\0')
        novaSala->pista = (char*)malloc(strlen(pistaStr) + 1);
        if (novaSala->pista != NULL) {
            strcpy(novaSala->pista, pistaStr);
        }
    } else {
        novaSala->pista = NULL; // Marca que não há pista
    }

    return novaSala;
}

/**
 * @brief Libera a memória da árvore do mapa (pós-ordem).
 * Também libera a memória alocada para as pistas (se existirem).
 */
void liberarMemoriaMapa(struct Sala* sala) {
    if (sala == NULL) {
        return;
    }
    liberarMemoriaMapa(sala->esquerda);
    liberarMemoriaMapa(sala->direita);

    // Libera a string da pista antes de liberar a sala
    if (sala->pista != NULL) {
        free(sala->pista);
    }
    free(sala);
}


// --- Funções das Pistas (BST) ---

/**
 * @brief (Função auxiliar) Cria um nó para a BST de Pistas.
 */
struct PistaNode* criarNoPista(const char* pista) {
    struct PistaNode* novo = (struct PistaNode*)malloc(sizeof(struct PistaNode));
    if (novo == NULL) {
        exit(1); // Falha crítica
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/**
 * @brief Insere uma nova pista na Árvore Binária de Busca (BST).
 * A inserção é recursiva e mantém a ordem alfabética.
 * Ignora pistas duplicadas.
 *
 * @param no O nó raiz da BST (ou sub-árvore).
 * @param pista A string da pista a ser inserida.
 * @return O ponteiro para a raiz da árvore (pode ser a nova ou a antiga).
 */
struct PistaNode* inserirPista(struct PistaNode* no, const char* pista) {
    // Caso base: Se a árvore/sub-árvore estiver vazia, cria o nó aqui
    if (no == NULL) {
        return criarNoPista(pista);
    }

    // Compara as strings para decidir onde inserir
    int cmp = strcmp(pista, no->pista);

    if (cmp < 0) {
        // Se a nova pista vem antes (alfabeticamente), insere à esquerda
        no->esquerda = inserirPista(no->esquerda, pista);
    } else if (cmp > 0) {
        // Se a nova pista vem depois (alfabeticamente), insere à direita
        no->direita = inserirPista(no->direita, pista);
    }
    // Se cmp == 0, a pista já existe (duplicada), não faz nada.

    return no; // Retorna a raiz (modificada ou não)
}

/**
 * @brief Exibe todas as pistas da BST em ordem alfabética.
 * Utiliza um percurso recursivo "em ordem" (In-Order Traversal):
 * Esquerda -> Raiz -> Direita.
 *
 * @param no O nó raiz da BST de pistas.
 */
void exibirPistas(struct PistaNode* no) {
    if (no == NULL) {
        return; // Caso base da recursão
    }

    // 1. Visita a sub-árvore esquerda (menores)
    exibirPistas(no->esquerda);

    // 2. Visita o nó atual (Raiz)
    printf("  - %s\n", no->pista);

    // 3. Visita a sub-árvore direita (maiores)
    exibirPistas(no->direita);
}

/**
 * @brief Libera a memória da BST de pistas (pós-ordem).
 */
void liberarMemoriaPistas(struct PistaNode* no) {
    if (no == NULL) {
        return;
    }
    liberarMemoriaPistas(no->esquerda);
    liberarMemoriaPistas(no->direita);
    free(no);
}


// --- Função de Exploração ---

/**
 * @brief Controla a navegação do jogador e a coleta de pistas.
 *
 * @param salaInicial A sala de onde a exploração começa (raiz do mapa).
 * @param pRaizPistas Ponteiro para o ponteiro da raiz da BST (para poder modificá-la).
 */
void explorarSalasComPistas(struct Sala* salaInicial, struct PistaNode** pRaizPistas) {
    struct Sala* salaAtual = salaInicial;
    char escolha;

    while (salaAtual != NULL) {
        printf("\n----------------------------------------\n");
        printf("🕵️ Você está em: %s\n", salaAtual->nome);

        // --- LÓGICA DE COLETA DE PISTA ---
        // Verifica se há uma pista (ponteiro não-nulo)
        if (salaAtual->pista != NULL) {
            printf("\n[!] PISTA ENCONTRADA: %s\n", salaAtual->pista);
            
            // Insere a pista na BST.
            // *pRaizPistas acessa o ponteiro da raiz (em main) e o atualiza.
            *pRaizPistas = inserirPista(*pRaizPistas, salaAtual->pista);
            
            // Marca a pista como "coletada" para não ser pega novamente
            free(salaAtual->pista); // Libera a string da pista
            salaAtual->pista = NULL;  // Define o ponteiro como NULL
        }
        
        // Verifica se é um beco sem saída
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este cômodo não tem mais saídas. Você deve retornar (ou sair).\n");
        }

        // Apresenta as opções de caminho
        printf("\nEscolha seu caminho:\n");
        if (salaAtual->esquerda != NULL) {
            printf("  (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf("  (d) Direita: %s\n", salaAtual->direita->nome);
        }
        printf("  (s) Sair e ver inventário\n");
        printf("Sua escolha: ");

        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e':
            case 'E':
                if (salaAtual->esquerda != NULL) {
                    salaAtual = salaAtual->esquerda;
                } else {
                    printf("Ops! Não há caminho para a esquerda aqui.\n");
                }
                break;

            case 'd':
            case 'D':
                if (salaAtual->direita != NULL) {
                    salaAtual = salaAtual->direita;
                } else {
                    printf("Ops! Não há caminho para a direita aqui.\n");
                }
                break;

            case 's':
            case 'S':
                printf("Você decide encerrar a exploração por agora.\n");
                return; // Sai da função e encerra o loop

            default:
                printf("Comando inválido. Tente 'e', 'd' ou 's'.\n");
                break;
        }
    }
}
