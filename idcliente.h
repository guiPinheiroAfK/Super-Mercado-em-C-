// idcliente.h
#ifndef IDCLIENTE_H
#define IDCLIENTE_H

#include "utilidades.h" // Para a struct CADASTRO e outras funções auxiliares
#include <stdio.h> // Para FILE, fopen, fclose, fscanf, fprintf, perror
#define ARQUIVO_PROXIMO_ID "proximo_id_cliente.dat"
#define ARQUIVO_CLIENTES "cadastros.txt"

// Variável global para armazenar o próximo ID de cliente
// 'extern' indica que a variável é definida em outro lugar (idcliente.c)
extern struct CADASTRO *clientes;
extern int total_clientes;
extern int proximo_id_cliente;

// Protótipos das funções para gerenciar o ID do cliente
void carregar_proximo_id_cliente();
void salvar_proximo_id_cliente();
int obter_proximo_id_cliente();

// Protótipos das funções para gerenciar os DADOS DOS CLIENTES em memória
void carregar_clientes();               // Para carregar todos os clientes do arquivo
void salvar_clientes();                 // Para salvar todos os clientes no arquivo
void adicionar_cliente_em_memoria(const struct CADASTRO *novo_cadastro); // Para adicionar um cliente recém-criado na lista em memória
void liberar_clientes();                // Para liberar a memória alocada para os clientes


#endif // IDCLIENTE_H