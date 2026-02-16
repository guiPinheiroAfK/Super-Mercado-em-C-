#include "utilidades.h" // Importa as declarações das funções auxiliares
#include "usuarios.h"  // Importa as declarações das funções de usuários
#include "cadastro.h"  // Importa as declarações das funções de cadastro
#include "venPag.h"  // Importa as declarações das funções de vendas e pagamentos
#include "relat.h"  // Importa as declarações das funções de relatórios
#include "idcliente.h" // Importa as declarações das funções de ID de cliente

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <locale.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TAMANHO_BUFFER 100

// Protótipo dos menus, posteriormente vou declarar eles em outro arquivo.h 
void menu(void); 
void mCadastro(void);
void mCaixa(void);
void mFechamento(void);
void fechamentoP();
void mRelatorio(); 
void vMenuDentro(void); 
void cadastrar_clientes();

struct PRODUTO {
    int codigo;          // Código do produto
    char nome[50];      // Nome do produto
    float preco;        // Preço do produto
    int estoque;      // Quantidade disponível
};

int caixaA = 0;           // Variável para status do caixa: 0=fechado, 1=aberto
float caixaF = 0.0;       // Variável para o valor do caixa final/atual
int caixaC = 0;           // Variável para permissão de retorno (se usada para controle de fluxo)
float caixaI = 0.0;       // Variável para o valor do caixa inicial do dia/sessão

ItemVenda *carrinho_de_compras = NULL; // Ponteiro para o array dinâmico que armazena os itens do carrinho
int total_itens_carrinho = 0;          // Contador do número de itens atualmente no carrinho
float total_venda_atual = 0.0;         // Variável para o valor total da venda atual

CADASTRO *clientes = NULL; // Ponteiro para o array dinâmico que armazena os clientes cadastrados
int total_clientes = 0;    // Contador do número total de clientes cadastrados



// Variáveis de controle de fluxo (não são do caixa/cadastro, mas são globais)
int opcao;
int escolha_pagamento;
int pagamento_concluido = 0;
int i;

void cadastrar_clientes() {
    struct CADASTRO novo_cadastro;
    FILE *arquivo = fopen(ARQUIVO_CLIENTES, "a");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    printf("CADASTROS\n");
    printf("Ta funcionando?\n");

    // Leitura do CPF como string
    printf("Vamos ver se funciona\n");
    ler_cpf(&novo_cadastro);
    printf("CPF cadastrado: %s\n", novo_cadastro.cpf_str);

    // Entrada do nome completo
    do {
        printf("Digite seu nome completo (até 40 caracteres, somente letras e espaços): \n");
        if (fgets(novo_cadastro.nomeC, sizeof(novo_cadastro.nomeC), stdin) == NULL) {
            perror("Erro ao ler o nome");
            fclose(arquivo);
            return;
        }
        novo_cadastro.nomeC[strcspn(novo_cadastro.nomeC, "\n")] = '\0';
        if (!validar_nome(novo_cadastro.nomeC)) {
            printf("Nome inválido! Use apenas letras e espaços.\n");
        }
    } while (!validar_nome(novo_cadastro.nomeC));

    // Entrada do telefone com validação para apenas números e tamanho 10 ou 11
    do {
        printf("Agora precisamos de um número para contato (apenas números, 11 dígitos): \n");
        if (fgets(novo_cadastro.zap, sizeof(novo_cadastro.zap), stdin) == NULL) {
            perror("Erro ao ler o telefone");
            fclose(arquivo);
            return;
        }
        novo_cadastro.zap[strcspn(novo_cadastro.zap, "\n")] = '\0';
        if (!validar_telefone(novo_cadastro.zap)) {
            printf("Número inválido! Deve conter apenas números e ter 11 dígitos.\n");
        }
    } while (!validar_telefone(novo_cadastro.zap));

    // Entrada do endereço - rua
    printf("Agora preciso cadastrar um endereço.\n");

    Sleep(150);
    printf("Endereço (Rua): \n");
    do{
    if (fgets(novo_cadastro.rua, sizeof(novo_cadastro.rua), stdin) == NULL) {
        perror("Erro ao cadastrar endereço");
        fclose(arquivo);
        return;
    }
    novo_cadastro.rua[strcspn(novo_cadastro.rua, "\n")] = '\0';
    if (!validar_nome(novo_cadastro.rua)) {
            printf("Rua inválido! Use apenas letras e espaços.\n");
            printf("Endeço(Rua): ");
        }
    }while(!validar_nome(novo_cadastro.rua));

    // Entrada do número da casa
char numero_casa_str[10]; // Buffer para armazenar a entrada como string
do {
    printf("Endereço (Número da casa): \n");
    if (fgets(numero_casa_str, sizeof(numero_casa_str), stdin) == NULL) {
        perror("Erro ao ler o nº da casa.");
        fclose(arquivo);
        return;
    }
    numero_casa_str[strcspn(numero_casa_str, "\n")] = '\0'; // Remove o '\n'

    // Verifica se a entrada contém apenas dígitos
    if (!validar_digito(numero_casa_str)) {
        printf("Inválido! Deve conter apenas números.\n");
    } else {
        // Se a entrada for válida, converte para inteiro
        novo_cadastro.nCasa = atoi(numero_casa_str);
        break; // Sai do loop se a entrada for válida
    }
} while (1);

    // Entrada do bairro
    printf("Endereço (Bairro): \n");
    if (fgets(novo_cadastro.bairro, sizeof(novo_cadastro.bairro), stdin) == NULL) {
        perror("Erro ao ler o nome do bairro.");
        fclose(arquivo);
        return;
    }
    novo_cadastro.bairro[strcspn(novo_cadastro.bairro, "\n")] = '\0';

    // Entrada do nome social
    printf("Como deseja ser chamado? (nome social, até 20 caracteres): \n");
    if (fgets(novo_cadastro.nSocial, sizeof(novo_cadastro.nSocial), stdin) == NULL) {
        perror("Erro ao ler o nome social.");
        fclose(arquivo);
        return;
    }
    novo_cadastro.nSocial[strcspn(novo_cadastro.nSocial, "\n")] = '\0';

    int opcao_salvar;
    // Usamos fgets para ler a escolha para evitar problemas de buffer com scanf
    char escolha_str[10]; 

    printf("\nTodos os dados foram preenchidos. Deseja salvar este novo cliente?\n");
    printf("1. Sim\n");
    printf("2. Nao\n");
    printf("Escolha uma opcao: ");

    // Loop para garantir uma entrada válida (1 ou 2)
    do {
        if (fgets(escolha_str, sizeof(escolha_str), stdin) == NULL) {
            perror("Erro ao ler opcao");
            return; // Saia da função se houver erro de leitura grave
        }
        escolha_str[strcspn(escolha_str, "\n")] = '\0'; // Remove o newline

        // Converte a string para inteiro
        opcao_salvar = atoi(escolha_str);

        if (opcao_salvar != 1 && opcao_salvar != 2) {
            printf("Opcao invalida. Por favor, digite 1 para Sim ou 2 para Nao: ");
        }
    } while (opcao_salvar != 1 && opcao_salvar != 2);


    switch (opcao_salvar) {
        case 1:
            // AGORA SIM: OBTÉM O PRÓXIMO ID DISPONÍVEL E O SALVA NO ARQUIVO .DAT
            novo_cadastro.id_cliente = obter_proximo_id_cliente();
            printf("ID do novo cliente: %04d\n", novo_cadastro.id_cliente); // Feedback para o usuário

            // Adiciona o cliente na lista em memória (clientes)
            adicionar_cliente_em_memoria(&novo_cadastro);
            printf("Cliente %s (ID: %04d) cadastrado e adicionado a memoria com sucesso!\n", novo_cadastro.nomeC, novo_cadastro.id_cliente);
            
            // O salvamento para o arquivo cadastros.txt será feito pela funcao salvar_clientes()
            // que é chamada uma unica vez no main() ao final do programa.

            break;
        case 2:
            printf("Cadastro cancelado. O cliente nao sera salvo.\n");
            // Nenhuma ação é necessária aqui, pois o ID não foi gerado e o cliente
            // não foi adicionado à memória.
            break;
    }
}

void fechamentoP(){
    printf("FECHAMENTO DE CAIXA\n");
}

void menu(void) {
    int escolha;
    // O loop 'do-while' interno da main() já garante que o menu só aparece
    // se o usuário estiver logado. Este loop aqui garante que o menu principal
    // continue aparecendo até que o usuário escolha sair/fazer logout.
    do {
        system("cls"); 
        printf("======================================\n");
        printf("          MENU PRINCIPAL              \n");
        printf("======================================\n");
        printf("1. Cadastros                         \n");
        printf("2. Vendas                            \n");
        printf("3. Abertura de Caixa                 \n");
        printf("4. Fechamento de Caixa               \n");
        printf("5. Relatorios                        \n");
        printf("6. Sair / Logout                     \n"); 
        printf("--------------------------------------\n");
        printf("Usuario logado: %s (%s)\n", usuario_logado->login,
                                            (usuario_logado->tipo_usuario == ADM) ? "Administrador" : "Comum");
        printf("--------------------------------------\n");
        printf("Escolha uma opcao: ");

        // Loop para garantir entrada válida
        while (scanf("%d", &escolha) != 1) {
            limpaBuffer();
            system("cls");
            printf("Opcao invalida! Tente novamente.\n\n");
            Sleep(1500);
            printf("Escolha uma opcao: "); // Reexibe a pergunta
        }
        limpaBuffer(); // Limpa o buffer após a leitura

        switch (escolha) {
            case 1: // Cadastros
                system("cls");
                mCadastro();
                break;
            case 2: // Vendas
                system("cls");
                mVendas(&caixaI, &caixaA); // Passa os ponteiros do caixa inicial e aberto
                break;
            case 3: // Abertura de Caixa - REQUER PERMISSÃO DE ADMIN
                system("cls");
                if (verificar_permissao_admin()) { // <-- Checagem de permissão
                    mCaixa();
                } else {
                    printf("\nACESSO NEGADO: Voce nao tem permissao para abrir o caixa.\n");
                    Sleep(2000);
                }
                break;
            case 4: // Fechamento de Caixa - REQUER PERMISSÃO DE ADMIN
                system("cls");
                if (verificar_permissao_admin()) { // <-- Checagem de permissão
                    mFechamento();
                } else {
                    printf("\nACESSO NEGADO: Voce nao tem permissao para fechar o caixa.\n");
                    Sleep(2000);
                }
                break;
            case 5: // Relatórios
                system("cls");
                mRelatorio(); 
                break;
            case 6: // Sair / Logout
                logout_usuario(); // Chama a função de logout
                // O loop 'while (usuario_logado != NULL)' na main() cuidará de sair deste menu.
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (escolha != 6); // O loop continua até que o usuário escolha Sair/Logout
}
void mCadastro() {
    int opcao;
    do {
        system("cls");
        printf("======================================\n");
        printf("          MENU DE CADASTROS           \n");
        printf("======================================\n");
        printf("1. Cadastro de Clientes               \n"); 
        printf("2. Cadastro de Produtos               \n");
        printf("3. Cadastro de Categorias             \n");
        printf("4. Retornar ao Menu Principal         \n");
        printf("--------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                printf("Entrando em Cadastro de Clientes...\n");
                cadastrar_clientes();
                Sleep(1000);
                // Exemplo: menu_gerenciar_usuarios();
                break;
            case 2:
                menu_cadastro_produtos();
                break;
            case 3:
                menu_cadastro_categorias();
                break;
            case 4:
                printf("Voltando ao Menu Principal...\n");
                Sleep(1000);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (opcao != 4);
}
void mCaixa(){
    do{
    printf("CAIXA\n");
    printf("3- 1.Abrir o caixa\n");
    printf("3- 2.Retornar ao Menu Principal\n");
    scanf("%d", &opcao);
    limpaBuffer();
    switch(opcao){
    case 1:
        system("cls");
        caixaAbertura(&caixaI, &caixaA);
        system("cls");
        break;
    case 2:
        system("cls");
        break;
        }
    }while(opcao != 2);
}
void mFechamento(){
    do{
    printf("FECHAMENTO\n");
    printf("4- 1.Fechar o caixa\n");
    printf("4- 2.Retornar ao Menu Principal\n");
    scanf("%d", &opcao);
    limpaBuffer();
    switch(opcao){
    case 1:
        system("cls");
        fechamentoP();
        break;
    case 2:
        system("cls");
        break;
        }
    }while(opcao != 2);
}
int main() {
    setlocale(LC_ALL, "Portuguese");
    system("chcp 1252");
    system("cls");

    // 1. Verificação do primeiro administrador e carregamento de dados
    // Esta função garante que pelo menos um admin exista no sistema de usuários
    checar_primeiro_admin();


    // Dados iniciais do sistema
    carregar_usuarios();
    carregar_categorias();
    carregar_produtos();

    // Carrega o próximo ID de cliente para garantir que novos clientes recebam IDs únicos
    carregar_proximo_id_cliente(); 
    carregar_clientes();
    // Loop principal do programa: gerencia o fluxo de login e acesso aos menus
    int programa_ativo = 1; // Flag para controlar se o programa deve continuar executando

    while (programa_ativo) {
        // Exibe o menu de login e tenta autenticar o usuário ou permite cadastro/saída.
        // menu_login() retorna 1 se um usuário logou com sucesso.
        // menu_login() retorna 0 se o usuário escolheu "Sair do Programa".
        if (menu_login() == 1) { // Se o login foi bem-sucedido...
            // O 'usuario_logado' agora aponta para o usuário que fez login.
            // Entra no loop do menu principal do sistema.
            while (usuario_logado != NULL) {
                system("cls"); // Limpa a tela antes de exibir o menu principal
                // Chama a função do seu menu principal do sistema.
                // Esta função (menu()) deve ter uma opção de logout que chama 'logout_usuario()'.
                // Quando 'logout_usuario()' é chamado, ele define 'usuario_logado = NULL',
                // o que fará com que este loop interno seja encerrado.
                menu(); // Chamada para o seu menu principal
            }
            // Quando sai deste loop interno, é porque o usuário fez logout.
            // O loop externo 'programa_ativo' continua, voltando para o 'menu_login()'
            // para que outro usuário possa logar ou para permitir o cadastro/saída.

        } else { // Se 'menu_login()' retornou 0, significa que o usuário escolheu "Sair do Programa"
            programa_ativo = 0; // Define a flag para encerrar o loop principal 'while (programa_ativo)'
        }
    }

    // 2. Salvamento de dados e liberação de memória ao finalizar o programa
    // Estas funções devem salvar o estado atual dos dados em arquivos.
    salvar_clientes(); // Salva os dados dos clientes
    salvar_usuarios(); // Salva os dados dos usuários
    salvar_categorias(); // Salva os dados de categorias
    salvar_produtos();   // Salva os dados de produtos

    // Libera a memória alocada dinamicamente para evitar vazamentos de memória.
    // Lembre-se de liberar apenas o que foi alocado com malloc/realloc.
    if (usuarios_cadastrados != NULL) {
        free(usuarios_cadastrados);
        usuarios_cadastrados = NULL;
    }
    if (categorias != NULL) { // Se você tem a variável global 'categorias'
        free(categorias);
        categorias = NULL;
    }
    if (produtos != NULL) { // Se você tem a variável global 'produtos'
        free(produtos);
        produtos = NULL;
    }

    return 0; // Finaliza o programa com sucesso
}
