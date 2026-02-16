#ifndef RELAT_H
#define RELAT_H

// --- INCLUDES NECESSÁRIOS PARA O MÓDULO DE RELATÓRIOS ---
#include <stdio.h>    // Para operações de entrada/saída (printf, fprintf, FILE)
#include <stdlib.h>   // Para alocação de memória (malloc, free) e outras utilidades
#include <string.h>   // Para manipulação de strings (strcpy, strcmp, strlen)
#include <time.h>     // Para funções relacionadas a tempo
#include <windows.h>  // Para Sleep() ou outras funções específicas do Windows
#include <ctype.h>    // Para funções de caracteres (tolower, toupper)

// Inclua seus próprios cabeçalhos com as definições das estruturas
#include "cadastro.h"   // Assume que contém as structs Produto, CADASTRO, Categoria
#include "venPag.h"     // Assume que contém a struct ItemVenda (se você tiver histórico de vendas)
#include "utilidades.h" // Para funções auxiliares como limpaBuffer()

// --- DEFINIÇÕES DE ARQUIVOS ---
#define ARQUIVO_HIST_VENDAS "histVendas.txt" // Arquivo onde o histórico de vendas é salvo
#define ARQUIVO_CAIXA_SALDO "caixa_saldo.dat" // Arquivo para o saldo do caixa
#define ARQUIVO_PRODUTOS "produtos.txt"     // Arquivo de produtos
#define ARQUIVO_CLIENTES "cadastros.txt"    // Arquivo de clientes
#define ARQUIVO_CATEGORIAS "categorias.txt" // Arquivo de categorias

// --- DECLARAÇÕES DE VARIÁVEIS GLOBAIS (extern) ---
extern Produto *produtos;
extern int total_produtos;
extern CADASTRO *clientes;
extern int total_clientes;
extern Categoria *categorias;
extern int total_categorias;
extern float caixaF; // Saldo final do caixa para relatório
extern float caixaI; // Saldo inicial do caixa para relatório

// --- ESTRUTURAS DE DADOS ESPECÍFICAS DE RELATÓRIOS ---
// Estrutura para um registro de venda no histórico: APENAS DATA (DDMMAAAA)
typedef struct {
    int id_venda;
    char data[9]; // DDMMYYYY + \0 (8 caracteres para a data, 1 para o nulo)
    float valor_total;
    int id_cliente; // Pode ser 0 se a venda não tiver cliente associado
    char tipo_pagamento[20]; // Ex: "Dinheiro", "Cartao"
} RegistroVendaHistorico;

// Estrutura para contar produtos vendidos (para os mais vendidos)
typedef struct {
    int codigo_produto;
    char nome_produto[50];
    int quantidade_vendida;
    float total_arrecadado;
} ProdutoVendidoContador;


// --- PROTÓTIPOS DOS MENUS DE RELATÓRIOS ---
void mRelatorio(); // Menu principal de relatórios
void menu_relatorios_clientes();
void menu_relatorios_produtos();
void menu_relatorios_vendas();

// --- PROTÓTIPOS DAS FUNÇÕES DE RELATÓRIOS ---

// 5.1 Listagem dos Clientes
void relatorio_clientes_ordenado_nome(); // 5.1.1
void relatorio_clientes_que_compraram_periodo(); // 5.1.2

// 5.2 Listagem dos Produtos
void relatorio_produtos_ordenado_descricao(); // 5.2.1
void relatorio_produtos_estoque_zero_ou_minimo(); // 5.2.2
void relatorio_produtos_mais_vendidos_periodo(); // 5.2.3

// 5.3 Listagem das Vendas
void relatorio_vendas_por_periodo(); // 5.3.1
void relatorio_faturamento_consolidado_periodo(); // 5.3.2

// Funções de Relatório Financeiro (Caixa) - Mantidas do anterior, mas podemos encaixar
void relatorio_caixa_atual();
void relatorio_movimentacao_caixa();

// Funções Auxiliares para Relatórios (internas ao módulo, mas se forem chamadas de fora, prototipe aqui)
// Funções para carregar e liberar o histórico de vendas
void carregar_historico_vendas(RegistroVendaHistorico **historico, int *total_registros);
void liberar_historico_vendas(RegistroVendaHistorico *historico);

// Funções para entrada e validação de datas (para relatórios por período)
int obter_data_inicio_fim(time_t *data_inicio, time_t *data_fim);
int parse_data_ddmmyyyy(const char *data_str, time_t *timestamp); // NOVA FUNÇÃO DE PARSE

// Funções de comparação para qsort (para ordenação)
int comparar_clientes_por_nome(const void *a, const void *b);
int comparar_produtos_por_descricao(const void *a, const void *b);
int comparar_produtos_vendidos_por_quantidade(const void *a, const void *b);

#endif // RELAT_H