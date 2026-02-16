#ifndef VENPAG_H
#define VENPAG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h> // Para Sleep()

// --- ESTRUTURAS DE DADOS ---
#include "cadastro.h"
#include "idcliente.h"
#include "utilidades.h" // Para limpaBuffer() e outras se você as usa globalmente

// --- CONSTANTES ---
#define ARQUIVO_HIST_VENDAS "histVendas.txt"
#define ARQUIVO_CAIXA_SALDO "caixa_saldo.dat" // <<--- Definição da constante aqui

// --- ESTRUTURA PARA ITENS DO CARRINHO ---
typedef struct {
    int codigo_produto;
    char descricao_produto[50];
    float preco_unitario;
    int quantidade;
    float subtotal;
} ItemVenda;

// --- DECLARAÇÕES DAS VARIÁVEIS GLOBAIS DE VENDA ---
extern ItemVenda *carrinho_de_compras;
extern int total_itens_carrinho;
extern float total_venda_atual;

extern CADASTRO *clientes;
extern int total_clientes;
extern Produto *produtos;
extern int total_produtos;

// --- SUAS VARIÁVEIS GLOBAIS DO CAIXA (ACESSO VIA extern) ---
extern int caixaA;   // Status de caixa aberto (0=fechado, 1=aberto)
extern float caixaF; // Valor do caixa final/atual
extern float caixaI; // Valor do caixa inicial

// --- PROTÓTIPOS DAS FUNÇÕES DE VENDA ---
void mVendas();
void limpar_carrinho();
void adicionar_produto_ao_carrinho(int codigo_produto, int quantidade);
void listar_carrinho();
void vMenuDentro();
void finalizar_venda();
void realizar_pagamento();
void registrar_venda_no_historico(float total_da_venda, CADASTRO *cliente_comprador);
void retirada_caixa();

// --- PROTÓTIPOS DAS FUNÇÕES DE CAIXA (AGORA EM VENPAG.H) ---
void caixaAbertura(float *caixaI_ptr, int *caixaA_ptr); // Função de abertura (mantém ponteiros)
void carregar_saldo_caixa();                             // Carregar saldo
void salvar_saldo_caixa();                               // Salvar saldo
void aumentar_caixa();                                   // Aumentar caixa (manual)

// --- PROTÓTIPOS DE FUNÇÕES AUXILIARES ---
void obter_data_hora_atual(char *buffer, size_t tamanho);

#endif