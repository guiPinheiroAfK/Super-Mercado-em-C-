// cadastros.h
#ifndef CADASTROS_H
#define CADASTROS_H

#include <stdio.h>    // Para funções de entrada/saída (printf, scanf, FILE, etc.)
#include <stdlib.h>   // Para funções de alocação de memória (malloc, free) e utilitários gerais
#include <string.h>   // Para funções de manipulação de strings (strcpy, strcmp, strlen)
#include <ctype.h>    // Para funções de manipulação de caracteres (isalpha, isdigit, toupper)
#include <windows.h>  // CORRIGIDO: Era <windows.h.h> antes. Para Sleep e system("cls")
#include "utilidades.h" // Para usar limpaBuffer, e também para ter acesso a struct CADASTRO (se necessário indiretamente)

// --- DEFINIÇÃO DAS STRUCTS DE PRODUTOS E CATEGORIAS ---
// As structs devem ser definidas AQUI no .h
// REMOVA estas DEFINIÇÕES do seu main.c depois de copiar para cá!

typedef struct Categoria {
    int id;
    char nome[50];
} Categoria;

typedef struct Produto {
    int codigo;
    char descricao[100];
    int id_categoria; // Usará o ID da categoria
    float preco_compra;
    float margem_lucro_percentual; // Em percentual (ex: 20 para 20%)
    int quantidade_estoque;
    int estoque_minimo;
} Produto;

// --- DECLARAÇÕES DE VARIÁVEIS GLOBAIS DE PRODUTOS E CATEGORIAS (usando 'extern') ---
// Estas são DECLARAÇÕES. As DEFINIÇÕES (com o valor inicial) ficarão em cadastros.c
// REMOVA as DEFINIÇÕES dessas variáveis do seu main.c após copiar para cadastros.c!

extern Categoria *categorias;
extern int total_categorias;
extern int proximo_id_categoria;

extern Produto *produtos;
extern int total_produtos;

// --- PROTÓTIPOS DAS FUNÇÕES DE PRODUTOS E CATEGORIAS ---
// Copie estes protótipos para cá.
// REMOVA estes PROTÓTIPOS do seu main.c depois de copiar para cá!

// Funções de Categorias de Produtos
void cadastrar_categoria();
void listar_categorias();
int selecionar_categoria();
void salvar_categorias();
void carregar_categorias();
void menu_cadastro_categorias(); // Menu específico de categorias

// Funções de Produtos
void cadastrar_produto();
void listar_produtos();
Produto *buscar_produto_por_codigo(int codigo); // Retorna um ponteiro para Produto
void alterar_estoque_produto();
void salvar_produtos();
void carregar_produtos();
void menu_cadastro_produtos(); // Menu específico de produtos

// Menu de alto nível para gestão de produtos e categorias
void menu_principal_produtos_cadastro(); // Renomeado do 'menu_principal' do sistema de produtos

#endif // CADASTROS_H