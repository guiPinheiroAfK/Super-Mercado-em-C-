// utilidades.h
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>     // Para isalpha, isdigit, isspace
#include <windows.h>   

typedef struct CADASTRO {
    int id_cliente; // ID do cliente
    char nomeC[41];
    char nSocial[21];
    char rua[101];
    char bairro[101];
    char cpf_str[101];
    char zap[101];
    int nCasa;
}CADASTRO;



// Protótipos das funções auxiliares
void limpaBuffer();
int validar_string_letras_espacos(const char *str); // Já estava
int validar_string_digitos(const char *str);     // Já estava

int validar_nome(const char * const nome);
int validar_digito(const char * const telefone); // Parece ser uma função genérica para dígitos em string, se for específica para 1 dígito, ajuste.
int validar_telefone(const char * const telefone);
int validar_cpf(const char * const cpf);
void ler_cpf(struct CADASTRO *novo_cadastro);
void validarCadastro(FILE *arquivo, const struct CADASTRO *novo_cadastro);

#endif // UTILIDADES_H