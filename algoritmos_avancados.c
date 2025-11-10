/* 
*Desafio nível novato
*AUTOR: André Luiz Mendes
*DATA: 10/11/25
*/

#include <stdio.h>
#include <stdlib.h> // Para malloc e free
#include <string.h> // Para strcpy

// --- Estrutura de Dados da Sala (Nó da Árvore) ---

/**
 * @brief Define a estrutura de um cômodo (Sala) no mapa.
 * Cada sala é um nó na árvore binária.
 */


struct Sala {
    char nome[100];           // Nome do cômodo
    struct Sala* esquerda;  // Ponteiro para o cômodo à esquerda
    struct Sala* direita;   // Ponteiro para o cômodo à direita
};

// --- Protótipos das Funções ---

struct Sala* criarSala(const char* nome);
void explorarSalas(struct Sala* salaInicial);
void liberarMemoria(struct Sala* sala);

// --- Função Principal (main) ---

/**
 * @brief Ponto de entrada do programa.
 * Monta o mapa inicial da mansão (a árvore binária) de forma automática
 * e, em seguida, inicia a função de exploração do jogador.
 */
int main() {
    printf("--- Bem-vindo ao Detective Quest ---\n");
    printf("Iniciando o sistema de mapa...\n");

    // 1. Montagem automática do mapa da mansão (Árvore Binária)
    // O "Hall de entrada" é a raiz (root) da árvore.
    struct Sala* hall = criarSala("Hall de entrada");

    // Nível 1
    hall->esquerda = criarSala("Sala de estar");
    hall->direita = criarSala("Sala de jantar");

    // Nível 2
    hall->esquerda->esquerda = criarSala("Biblioteca");
    hall->esquerda->direita = criarSala("Cozinha");
    hall->direita->direita = criarSala("Jardim de inverno"); // Nó com apenas um filho (à direita)

    // Nível 3 (Folhas / Becos sem saída)
    hall->esquerda->esquerda->esquerda = criarSala("Escritório secreto"); // Folha
    hall->esquerda->direita->esquerda = criarSala("Despensa");          // Folha
    hall->direita->direita->direita = criarSala("Estátua misteriosa"); // Folha

    printf("Mapa da mansão criado. Você está pronto para explorar.\n");

    // 2. Inicia a exploração a partir da raiz (Hall)
    explorarSalas(hall);

    // 3. Libera toda a memória alocada antes de fechar
    liberarMemoria(hall);
    printf("Sistema de mapa finalizado.\n");

    return 0;
}

// --- Implementação das Funções ---

/**
 * @brief Aloca dinamicamente (cria) uma nova sala (nó) na memória.
 * Inicializa a sala com o nome fornecido e define os caminhos
 * (esquerda e direita) como nulos (NULL).
 *
 * @param nome O nome que será atribuído à sala.
 * @return Um ponteiro para a nova Sala criada.
 */
struct Sala* criarSala(const char* nome) {
    // Aloca memória para a estrutura da Sala
    struct Sala* novaSala = (struct Sala*)malloc(sizeof(struct Sala));

    // Verifica se a alocação de memória falhou
    if (novaSala == NULL) {
        printf("Erro crítico: Falha ao alocar memória para o mapa!\n");
        exit(1); // Encerra o programa se não houver memória
    }

    // Copia o nome para a estrutura
    strcpy(novaSala->nome, nome);

    // Inicializa os ponteiros (filhos) como nulos
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    return novaSala;
}

/**
 * @brief Controla a navegação interativa do jogador pela mansão (árvore).
 * O jogador começa na 'salaInicial' (raiz) e pode escolher 'e' (esquerda)
 * ou 'd' (direita) para se mover. O loop continua até que o jogador
 * chegue a um nó-folha (sem saídas) ou escolha 's' para sair.
 *
 * @param salaInicial A sala de onde a exploração começa (geralmente a raiz).
 */
void explorarSalas(struct Sala* salaInicial) {
    struct Sala* salaAtual = salaInicial; // O jogador começa no Hall
    char escolha;

    // O loop continua enquanto o jogador estiver em uma sala válida
    while (salaAtual != NULL) {
        printf("\n----------------------------------------\n");
        printf("🕵️ Você está em: %s\n", salaAtual->nome);

        // Verifica se é um nó-folha (fim da linha)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este cômodo não tem mais saídas. Fim da exploração neste caminho!\n");
            break; // Sai do loop 'while'
        }

        // Apresenta as opções de caminho
        printf("Escolha seu caminho:\n");
        if (salaAtual->esquerda != NULL) {
            printf("  (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf("  (d) Direita: %s\n", salaAtual->direita->nome);
        }
        printf("  (s) Sair da exploração\n");
        printf("Sua escolha: ");

        // Lê a escolha do jogador (o espaço antes de %c ignora quebras de linha anteriores)
        scanf(" %c", &escolha);

        // Processa a escolha
        switch (escolha) {
            case 'e':
            case 'E':
                if (salaAtual->esquerda != NULL) {
                    salaAtual = salaAtual->esquerda; // Move o jogador para a esquerda
                } else {
                    printf("Ops! Não há caminho para a esquerda aqui.\n");
                }
                break;

            case 'd':
            case 'D':
                if (salaAtual->direita != NULL) {
                    salaAtual = salaAtual->direita; // Move o jogador para a direita
                } else {
                    printf("Ops! Não há caminho para a direita aqui.\n");
                }
                break;

            case 's':
            case 'S':
                printf("Você decidiu parar a exploração por enquanto.\n");
                return; // Sai da função (e consequentemente do loop)

            default:
                printf("Comando inválido. Tente 'e', 'd' ou 's'.\n");
                break;
        }
    }
}

/**
 * @brief Libera recursivamente toda a memória alocada para a árvore.
 * Usa um percurso em pós-ordem para garantir que os filhos sejam
 * liberados antes dos pais.
 *
 * @param sala O nó raiz da árvore (ou sub-árvore) a ser liberada.
 */
void liberarMemoria(struct Sala* sala) {
    if (sala == NULL) {
        return; // Caso base da recursão
    }

    // Libera primeiro os filhos
    liberarMemoria(sala->esquerda);
    liberarMemoria(sala->direita);

    // Libera o nó atual (pai)
    // printf("Liberando: %s\n", sala->nome); // (Descomente para depuração)
    free(sala);
}
