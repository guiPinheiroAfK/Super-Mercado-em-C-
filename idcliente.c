// idcliente.c
#include "idcliente.h" // Inclui o cabeçalho idcliente.h
#include "utilidades.h" // Para a struct CADASTRO e outras funções auxiliares
#include <stdio.h>     // Para FILE, fopen, fclose, fscanf, fprintf, perror
#include <windows.h>   // Para Sleep, se ainda estiver usando (opcional, pode remover se não usar)

// DEFINIÇÃO da variável global proximo_id_cliente
// Esta é a única vez que ela é inicializada fora de uma função.
int proximo_id_cliente = 1;

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DE GERENCIAMENTO DE ID ---

void carregar_proximo_id_cliente() {
    // MODIFICADO: Usando ARQUIVO_PROXIMO_ID (definido em idcliente.h) e modo BINÁRIO "rb"
    FILE *arquivo = fopen(ARQUIVO_PROXIMO_ID, "rb");

    if (arquivo != NULL) {
        // MODIFICADO: Usando fread para ler um inteiro binário
        if (fread(&proximo_id_cliente, sizeof(int), 1, arquivo) != 1) {
            // Se falhar a leitura, ou arquivo vazio/corrompido
            printf("Erro ao ler proximo ID de cliente do arquivo binario. Resetando para 0001.\n");
            proximo_id_cliente = 1; // Reseta para 1
            // Opcional: tentar salvar o valor corrigido imediatamente para não repetir o erro
            salvar_proximo_id_cliente();
        }
        fclose(arquivo);
    } else {
        // Se o arquivo NÃO EXISTE, ele é a primeira execução.
        printf("Arquivo '%s' nao encontrado. Criando e inicializando com ID 0001.\n", ARQUIVO_PROXIMO_ID);
        proximo_id_cliente = 1; // Define o ID para 1

        // MODIFICADO: Abre em modo de escrita binária ("wb") para CRIAR o arquivo
        arquivo = fopen(ARQUIVO_PROXIMO_ID, "wb");
        if (arquivo != NULL) {
            // MODIFICADO: Escreve o ID inicial (1) no novo arquivo binário
            fwrite(&proximo_id_cliente, sizeof(int), 1, arquivo);
            fclose(arquivo);
        } else {
            perror("Erro fatal: Nao foi possivel criar o arquivo de ID binario");
            // Em caso de erro grave, você pode querer sair do programa aqui: exit(EXIT_FAILURE);
        }
    }
    // Sleep(500); // REMOVER: Atrasos de Sleep em funções de carregamento/salvamento não são ideais
                    // porque travam o programa. Use apenas na interface se realmente necessário.
}

void salvar_proximo_id_cliente() {
    // MODIFICADO: Usando ARQUIVO_PROXIMO_ID e modo BINÁRIO "wb"
    FILE *arquivo = fopen(ARQUIVO_PROXIMO_ID, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir ou criar 'proximo_id_cliente.dat' para escrita");
        return;
    }
    // MODIFICADO: Usando fwrite para salvar o inteiro binário
    fwrite(&proximo_id_cliente, sizeof(int), 1, arquivo);
    fclose(arquivo);
}

// A função obter_proximo_id_cliente() está perfeita como está para a lógica do ID.
int obter_proximo_id_cliente() {
    int id_atual = proximo_id_cliente;
    proximo_id_cliente++;
    salvar_proximo_id_cliente();
    return id_atual;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DE GERENCIAMENTO DE DADOS DOS CLIENTES ---
// As funções abaixo (carregar_clientes, salvar_clientes, adicionar_cliente_em_memoria, liberar_clientes)
// estão blz para trabalhar com 'cadastros.txt' (arquivo de texto).

void carregar_clientes() {
    FILE *arquivo = fopen(ARQUIVO_CLIENTES, "r"); // ARQUIVO_CLIENTES definido em idcliente.h
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado. Iniciando com 0 clientes.\n", ARQUIVO_CLIENTES);
        clientes = NULL;
        total_clientes = 0;
        return;
    }

    int count = 0;
    char linha[512];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) > 5) { // Heurística para evitar linhas vazias
            count++;
        }
    }
    rewind(arquivo);

    if (count > 0) {
        // Usando CADASTRO em vez de struct CADASTRO no cast, já que typedef foi feito
        clientes = (CADASTRO *) realloc(clientes, count * sizeof(CADASTRO));
        if (clientes == NULL) {
            perror("Erro ao alocar memoria para clientes");
            fclose(arquivo);
            total_clientes = 0;
            return;
        }
        total_clientes = 0; // Reinicia contador para preenchimento

        for (int i = 0; i < count; i++) {
            if (fgets(linha, sizeof(linha), arquivo) == NULL) {
                fprintf(stderr, "Erro ao ler linha %d do arquivo de clientes.\n", i + 1);
                break;
            }

            int resultado_sscanf = sscanf(linha, "ID: %d; CPF: %[^;]; Nome: %[^;]; Zap: %[^;]; Rua: %[^;]; NCasa: %d; Bairro: %[^;]; NomeSocial: %[^;\n];\n",
                                          &clientes[total_clientes].id_cliente,
                                          clientes[total_clientes].cpf_str,
                                          clientes[total_clientes].nomeC,
                                          clientes[total_clientes].zap,
                                          clientes[total_clientes].rua,
                                          &clientes[total_clientes].nCasa,
                                          clientes[total_clientes].bairro,
                                          clientes[total_clientes].nSocial);

            if (resultado_sscanf == 8) {
                total_clientes++;
            } else {
                fprintf(stderr, "Erro de formato na linha %d do arquivo '%s'. Itens lidos: %d\n", i + 1, ARQUIVO_CLIENTES, resultado_sscanf);
            }
        }
    } else {
        clientes = NULL;
        total_clientes = 0;
    }
    fclose(arquivo);
}

void salvar_clientes() {
    FILE *arquivo = fopen(ARQUIVO_CLIENTES, "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo de clientes para escrita");
        return;
    }

    for (int i = 0; i < total_clientes; i++) {
        fprintf(arquivo, "ID: %04d; CPF: %s; Nome: %s; Zap: %s; Rua: %s; NCasa: %d; Bairro: %s; NomeSocial: %s;\n",
                clientes[i].id_cliente, clientes[i].cpf_str, clientes[i].nomeC, clientes[i].zap,
                clientes[i].rua, clientes[i].nCasa, clientes[i].bairro, clientes[i].nSocial);
    }
    fclose(arquivo);
}

void adicionar_cliente_em_memoria(const CADASTRO *novo_cadastro) { // Usando CADASTRO no parâmetro
    // Usando CADASTRO em vez de struct CADASTRO no cast
    CADASTRO *temp = (CADASTRO *) realloc(clientes, (total_clientes + 1) * sizeof(CADASTRO));
    if (temp == NULL) {
        perror("Erro ao realocar memoria para adicionar novo cliente");
        return;
    }
    clientes = temp;
    clientes[total_clientes] = *novo_cadastro;
    total_clientes++;
}

void liberar_clientes() {
    if (clientes != NULL) {
        free(clientes);
        clientes = NULL;
        total_clientes = 0;
    }

    if (clientes != NULL) {
        free(clientes);
        clientes = NULL;
        total_clientes = 0;
    }
}