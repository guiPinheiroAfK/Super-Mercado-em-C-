// usuarios.h
#ifndef USUARIOS_H
#define USUARIOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h> // Para Sleep e system("cls") se as funções de usuário usarem
#include "utilidades.h" // Para limpaBuffer e outras validações genéricas

// Enum para definir os tipos de usuário
typedef enum {
    COMUM, // Valor 0
    ADM    // Valor 1 (Administrador)
} TipoUsuario;
// --- DEFINIÇÃO DA STRUCT DE USUÁRIO ---
typedef struct Usuario {
    char login[13];    // Login (username) >= 8 e <= 12 caracteres + terminador nulo
    char senha[9];     // Senha (password) >= 6 e <= 8 caracteres + terminador nulo
    TipoUsuario tipo_usuario; // Tipo de usuário (COMUM ou ADM)
    int ativo;          // 1: Administrador, 2: Usuário Comum
} Usuario;

// --- DEFINIÇÕES DE ARQUIVOS E OUTRAS CONSTANTES ---
#define ARQUIVO_USUARIOS "usuarios.dat" // Nome do arquivo para salvar os usuários

// --- DECLARAÇÕES DE VARIÁVEIS GLOBAIS DE USUÁRIOS (usando 'extern') ---
// Estas variáveis serão definidas em usuarios.c
extern Usuario *usuarios_cadastrados; // Array dinâmico de usuários
extern int total_usuarios;           // Quantidade total de usuários cadastrados
extern Usuario *usuario_logado;      // Ponteiro para o usuário atualmente logado (NULL se ninguém logado)

// --- PROTÓTIPOS DAS FUNÇÕES DE GERENCIAMENTO DE USUÁRIOS ---

// Funções de Persistência
void carregar_usuarios();
void salvar_usuarios();

// Funções de Autenticação e Login
Usuario *autenticar_usuario(const char *login_digitado, const char *senha_digitada);
int menu_login(); // Menu inicial para login ou saída do programa
void logout_usuario();

// Funções de Cadastro de Usuário
void cadastrar_usuario(); // Para cadastrar novos usuários (inclui admins e comuns)

// Funções de Validação de Input para Usuário/Senha
int validar_login_formato(const char *login);
int validar_senha_formato(const char *senha);

// Funções de Permissão e Admin
int verificar_permissao_admin(); // Verifica se o usuário logado é admin; se não, pede login de admin para operação
void checar_primeiro_admin(); // Garante que o primeiro admin seja cadastrado na primeira execução

// Funções auxiliares (se precisar de alguma específica para usuários)
Usuario *buscar_usuario_por_login(const char *login);


#endif // USUARIOS_H