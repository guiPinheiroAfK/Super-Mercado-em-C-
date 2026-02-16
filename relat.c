// relat.c

#include "relat.h"
#include "utilidades.h" // Para limpaBuffer() e outras
#include <time.h>       // Para manipulação de tempo (mktime, gmtime, localtime)

// --- FUNÇÕES AUXILIARES DE COMPARAÇÃO PARA QSORT ---

// Compara dois clientes pelo nome para qsort
int comparar_clientes_por_nome(const void *a, const void *b) {
    const CADASTRO *clienteA = (const CADASTRO *)a;
    const CADASTRO *clienteB = (const CADASTRO *)b;
    return strcmp(clienteA->nomeC, clienteB->nomeC);
}

// Compara dois produtos pela descrição para qsort
int comparar_produtos_por_descricao(const void *a, const void *b) {
    const Produto *prodA = (const Produto *)a;
    const Produto *prodB = (const Produto *)b;
    return strcmp(prodA->descricao, prodB->descricao);
}

// Compara dois produtos vendidos pela quantidade (para os mais vendidos)
int comparar_produtos_vendidos_por_quantidade(const void *a, const void *b) {
    const ProdutoVendidoContador *pvA = (const ProdutoVendidoContador *)a;
    const ProdutoVendidoContador *pvB = (const ProdutoVendidoContador *)b;
    // Ordem decrescente (do maior para o menor)
    if (pvA->quantidade_vendida < pvB->quantidade_vendida) return 1;
    if (pvA->quantidade_vendida > pvB->quantidade_vendida) return -1;
    return 0; // Se as quantidades forem iguais
}

// --- FUNÇÕES AUXILIARES PARA MANIPULAÇÃO DE DATAS ---

// Converte uma string de data "DDMMAAAA" para time_t (sem hora)
int parse_data_ddmmyyyy(const char *data_str, time_t *timestamp) {
    struct tm tm_struct = {0};
    int dia, mes, ano;
    // Formato esperado: "DDMMAAAA"
    if (sscanf(data_str, "%2d%2d%4d", &dia, &mes, &ano) == 3) {
        tm_struct.tm_mday = dia;
        tm_struct.tm_mon = mes - 1;   // tm_mon é 0-11
        tm_struct.tm_year = ano - 1900; // tm_year é anos desde 1900
        tm_struct.tm_hour = 0; // Zera a hora para pegar apenas o dia
        tm_struct.tm_min = 0;
        tm_struct.tm_sec = 0;
        *timestamp = mktime(&tm_struct);
        return (*timestamp != (time_t)-1); // Retorna 1 se a conversão for bem-sucedida
    }
    return 0; // Falha na conversão
}

// Solicita e valida um período de datas (início e fim) no formato DDMMAAAA
int obter_data_inicio_fim(time_t *data_inicio, time_t *data_fim) {
    char s_data_inicio[10], s_data_fim[10]; // 8 para DDMMAAAA + 1 para '\0'

    printf("\n--- Definir Período ---\n");
    printf("Digite a data de INÍCIO (DDMMAAAA): ");
    if (fgets(s_data_inicio, sizeof(s_data_inicio), stdin) == NULL) return 0;
    s_data_inicio[strcspn(s_data_inicio, "\n")] = '\0';

    printf("Digite a data de FIM (DDMMAAAA): ");
    if (fgets(s_data_fim, sizeof(s_data_fim), stdin) == NULL) return 0;
    s_data_fim[strcspn(s_data_fim, "\n")] = '\0';

    if (!parse_data_ddmmyyyy(s_data_inicio, data_inicio) || !parse_data_ddmmyyyy(s_data_fim, data_fim)) {
        printf("ERRO: Formato de data inválido. Use DDMMAAAA (ex: 23062025).\n");
        Sleep(2000);
        return 0;
    }

    if (*data_inicio > *data_fim) {
        printf("AVISO: Data de início não pode ser posterior à data de fim. Invertendo.\n");
        time_t temp = *data_inicio;
        *data_inicio = *data_fim;
        *data_fim = temp;
        Sleep(2000);
    }
    return 1;
}

// --- FUNÇÕES DE CARREGAMENTO E LIBERAÇÃO DE DADOS ---

void carregar_historico_vendas(RegistroVendaHistorico **historico, int *total_registros) {
    FILE *file = fopen(ARQUIVO_HIST_VENDAS, "r");
    if (file == NULL) {
        printf("AVISO: Arquivo de histórico de vendas '%s' não encontrado. Nenhuma venda anterior para relatar.\n", ARQUIVO_HIST_VENDAS);
        *historico = NULL;
        *total_registros = 0;
        return;
    }

    int count = 0;
    char line_buffer[256];
    // Formato esperado: ID_VENDA;DATA;VALOR_TOTAL;ID_CLIENTE;TIPO_PAGAMENTO
    // Adaptei o sscanf para ler o tipo_pagamento também.
    // Lembre-se que sua função de salvar vendas precisa SALVAR o tipo_pagamento.
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        int id_venda_temp, id_cliente_temp;
        float valor_total_temp;
        char data_temp[9], tipo_pagamento_temp[20]; // Data agora tem 8 caracteres + \0
        if (sscanf(line_buffer, "%d;%8[^;];%f;%d;%19[^\n]", // %8[^;] para a data
                   &id_venda_temp, data_temp, &valor_total_temp, &id_cliente_temp, tipo_pagamento_temp) == 5) {
            count++;
        }
    }
    rewind(file);

    if (count == 0) {
        *historico = NULL;
        *total_registros = 0;
        fclose(file);
        return;
    }

    *historico = (RegistroVendaHistorico *)malloc(count * sizeof(RegistroVendaHistorico));
    if (*historico == NULL) {
        perror("Erro ao alocar memoria para historico de vendas");
        *total_registros = 0;
        fclose(file);
        return;
    }

    *total_registros = 0;
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        if (sscanf(line_buffer, "%d;%8[^;];%f;%d;%19[^\n]", // %8[^;] para a data
                   &(*historico)[*total_registros].id_venda,
                   (*historico)[*total_registros].data, // AQUI USAMOS .data
                   &(*historico)[*total_registros].valor_total,
                   &(*historico)[*total_registros].id_cliente,
                   (*historico)[*total_registros].tipo_pagamento) == 5) {
            (*total_registros)++;
        }
    }

    fclose(file);
}

void liberar_historico_vendas(RegistroVendaHistorico *historico) {
    if (historico != NULL) {
        free(historico);
        historico = NULL;
    }
}


// --- MENUS DE RELATÓRIOS ---

void mRelatorio() {
    int opcao_relatorio;
    do {
        system("cls");
        printf("========================================\n");
        printf("           MENU DE RELATÓRIOS           \n");
        printf("========================================\n");
        printf("1. Relatórios de Clientes\n");        // 5.1
        printf("2. Relatórios de Produtos\n");        // 5.2
        printf("3. Relatórios de Vendas\n");          // 5.3
        printf("4. Relatórios Financeiros (Caixa)\n"); // Extra, do anterior
        printf("5. Retornar ao Menu Principal\n");    // 5.4
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao_relatorio) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            opcao_relatorio = -1;
            continue;
        }
        limpaBuffer();

        switch (opcao_relatorio) {
            case 1:
                menu_relatorios_clientes();
                break;
            case 2:
                menu_relatorios_produtos();
                break;
            case 3:
                menu_relatorios_vendas();
                break;
            case 4: // Este era o antigo "Relatórios Financeiros"
                relatorio_caixa_atual(); // Pode expandir para um submenu se quiser mais opções
                Sleep(1000);
                break;
            case 5:
                printf("\nRetornando ao Menu Principal...\n");
                Sleep(1000);
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (opcao_relatorio != 5);
}

void menu_relatorios_clientes() {
    int opcao;
    do {
        system("cls");
        printf("========================================\n");
        printf("        RELATÓRIOS DE CLIENTES          \n");
        printf("========================================\n");
        printf("1. Listagem de Clientes (Ordenado por Nome)\n"); // 5.1.1
        printf("2. Listagem de Clientes que Compraram (Período)\n"); // 5.1.2
        printf("3. Retornar ao Menu de Relatórios\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            opcao = -1;
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                relatorio_clientes_ordenado_nome();
                break;
            case 2:
                relatorio_clientes_que_compraram_periodo();
                break;
            case 3:
                printf("\nRetornando...\n");
                Sleep(500);
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (opcao != 3);
}

void menu_relatorios_produtos() {
    int opcao;
    do {
        system("cls");
        printf("========================================\n");
        printf("         RELATÓRIOS DE PRODUTOS         \n");
        printf("========================================\n");
        printf("1. Listagem de Produtos (Ordenado por Descrição)\n"); // 5.2.1
        printf("2. Listagem de Produtos com Estoque Zero ou Mínimo\n"); // 5.2.2
        printf("3. Listagem dos Produtos Mais Vendidos (Período)\n"); // 5.2.3
        printf("4. Retornar ao Menu de Relatórios\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            opcao = -1;
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                relatorio_produtos_ordenado_descricao();
                break;
            case 2:
                relatorio_produtos_estoque_zero_ou_minimo();
                break;
            case 3:
                relatorio_produtos_mais_vendidos_periodo();
                break;
            case 4:
                printf("\nRetornando...\n");
                Sleep(500);
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (opcao != 4);
}

void menu_relatorios_vendas() {
    int opcao;
    do {
        system("cls");
        printf("========================================\n");
        printf("          RELATÓRIOS DE VENDAS          \n");
        printf("========================================\n");
        printf("1. Listagem das Vendas (em um determinado período)\n"); // 5.3.1
        printf("2. Faturamento Consolidado – em um período\n");       // 5.3.2
        printf("3. Retornar ao Menu de Relatórios\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            opcao = -1;
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                relatorio_vendas_por_periodo();
                break;
            case 2:
                relatorio_faturamento_consolidado_periodo();
                break;
            case 3:
                printf("\nRetornando...\n");
                Sleep(500);
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }
    } while (opcao != 3);
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DE RELATÓRIOS ESPECÍFICAS ---

// 5.1 Listagem dos Clientes
void relatorio_clientes_ordenado_nome() { // 5.1.1
    printf("\n--- LISTAGEM DE CLIENTES (Ordenada por Nome) ---\n");
    if (total_clientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
    } else {
        // Cria uma cópia do array de clientes para não alterar a ordem original
        CADASTRO *clientes_copia = (CADASTRO *)malloc(total_clientes * sizeof(CADASTRO));
        if (clientes_copia == NULL) {
            perror("Erro ao alocar memoria para copia de clientes");
            printf("\nPressione ENTER para continuar...");
            limpaBuffer(); getchar();
            return;
        }
        memcpy(clientes_copia, clientes, total_clientes * sizeof(CADASTRO));

        // Ordena a cópia
        qsort(clientes_copia, total_clientes, sizeof(CADASTRO), comparar_clientes_por_nome);

        printf("---------------------------------------------------------------------------------------------------\n");
        printf("ID Cliente | Nome Completo              | CPF           | Telefone    | Endereço\n");
        printf("---------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < total_clientes; i++) {
            printf("%-10d | %-26s | %-13s | %-11s | %s, %d - %s\n",
                   clientes_copia[i].id_cliente, clientes_copia[i].nomeC, clientes_copia[i].cpf_str, clientes_copia[i].zap,
                   clientes_copia[i].rua, clientes_copia[i].nCasa, clientes_copia[i].bairro);
        }
        printf("---------------------------------------------------------------------------------------------------\n");
        free(clientes_copia); // Libera a memória da cópia
    }
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}

void relatorio_clientes_que_compraram_periodo() { // 5.1.2
    printf("\n--- LISTAGEM DOS CLIENTES QUE COMPRARAM (em um determinado período) ---\n");
    RegistroVendaHistorico *historico = NULL;
    int total_registros = 0;
    time_t data_inicio, data_fim;

    if (!obter_data_inicio_fim(&data_inicio, &data_fim)) {
        return; // Retorna se a data for inválida
    }

    carregar_historico_vendas(&historico, &total_registros);

    if (total_registros == 0) {
        printf("Nenhum registro de venda encontrado.\n");
    } else {
        // Para formatar a data para exibição
        char s_inicio[9], s_fim[9];
        strftime(s_inicio, sizeof(s_inicio), "%d%m%Y", localtime(&data_inicio));
        strftime(s_fim, sizeof(s_fim), "%d%m%Y", localtime(&data_fim));

        printf("\nClientes que realizaram compras entre %s e %s:\n", s_inicio, s_fim);
        printf("---------------------------------------------------------------------------------------------------\n");
        printf("ID Cliente | Nome Completo              | CPF           | Telefone\n");
        printf("---------------------------------------------------------------------------------------------------\n");

        // Usar um array auxiliar para IDs de clientes que já foram listados
        int *clientes_listados = (int *)calloc(total_clientes, sizeof(int));
        if (clientes_listados == NULL) {
            perror("Erro ao alocar memoria para lista de clientes");
            liberar_historico_vendas(historico);
            printf("\nPressione ENTER para continuar...");
            limpaBuffer(); getchar();
            return;
        }

        for (int i = 0; i < total_registros; i++) {
            time_t data_venda;
            if (parse_data_ddmmyyyy(historico[i].data, &data_venda)) { // Usando historico[i].data
                // A data_venda tem a hora zerada, então a comparação funciona para o dia inteiro
                if (data_venda >= data_inicio && data_venda <= data_fim) {
                    int cliente_index = -1;
                    for (int k = 0; k < total_clientes; k++) {
                        if (clientes[k].id_cliente == historico[i].id_cliente) {
                            cliente_index = k;
                            break;
                        }
                    }

                    if (cliente_index != -1 && clientes_listados[cliente_index] == 0) {
                        printf("%-10d | %-26s | %-13s | %-11s\n",
                               clientes[cliente_index].id_cliente, clientes[cliente_index].nomeC,
                               clientes[cliente_index].cpf_str, clientes[cliente_index].zap);
                        clientes_listados[cliente_index] = 1; // Marca como listado
                    }
                }
            }
        }
        printf("---------------------------------------------------------------------------------------------------\n");
        free(clientes_listados);
    }
    liberar_historico_vendas(historico);
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}

// 5.2 Listagem dos Produtos
void relatorio_produtos_ordenado_descricao() { // 5.2.1
    printf("\n--- LISTAGEM DE PRODUTOS (Ordenada por Descrição) ---\n");
    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado.\n");
    } else {
        // Cria uma cópia do array de produtos
        Produto *produtos_copia = (Produto *)malloc(total_produtos * sizeof(Produto));
        if (produtos_copia == NULL) {
            perror("Erro ao alocar memoria para copia de produtos");
            printf("\nPressione ENTER para continuar...");
            limpaBuffer(); getchar();
            return;
        }
        memcpy(produtos_copia, produtos, total_produtos * sizeof(Produto));

        // Ordena a cópia
        qsort(produtos_copia, total_produtos, sizeof(Produto), comparar_produtos_por_descricao);

        printf("----------------------------------------------------------\n");
        printf("Código | Descrição do Produto         | Preço Venda | Estoque\n");
        printf("----------------------------------------------------------\n");
        for (int i = 0; i < total_produtos; i++) {
            printf("%-6d | %-28s | R$ %-9.2f | %-7d\n",
                   produtos_copia[i].codigo, produtos_copia[i].descricao, produtos_copia[i].preco_compra, produtos_copia[i].quantidade_estoque);
        }
        printf("----------------------------------------------------------\n");
        free(produtos_copia);
    }
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}

void relatorio_produtos_estoque_zero_ou_minimo() { // 5.2.2
    printf("\n--- LISTAGEM DE PRODUTOS COM ESTOQUE ZERO OU MÍNIMO ---\n");
    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado.\n");
        printf("\nPressione ENTER para continuar...");
        limpaBuffer(); getchar();
        return;
    }

    int limite_minimo;
    printf("Digite o limite mínimo de estoque para considerar (0 para apenas zerados): ");
    if (scanf("%d", &limite_minimo) != 1 || limite_minimo < 0) {
        limpaBuffer();
        printf("Entrada inválida. Usando limite 0.\n");
        limite_minimo = 0;
    }
    limpaBuffer();

    printf("----------------------------------------------------------\n");
    printf("Código | Descrição do Produto         | Estoque Atual\n");
    printf("----------------------------------------------------------\n");
    int encontrou = 0;
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].quantidade_estoque <= limite_minimo) {
            printf("%-6d | %-28s | %-13d\n",
                   produtos[i].codigo, produtos[i].descricao, produtos[i].quantidade_estoque);
            encontrou = 1;
        }
    }
    if (!encontrou) {
        printf("Nenhum produto com estoque abaixo ou igual a %d.\n", limite_minimo);
    }
    printf("----------------------------------------------------------\n");
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}

void relatorio_produtos_mais_vendidos_periodo() { // 5.2.3
    printf("\n--- LISTAGEM DOS PRODUTOS MAIS VENDIDOS (em um determinado período) ---\n");
    RegistroVendaHistorico *historico = NULL;
    int total_registros = 0;
    time_t data_inicio, data_fim;

    if (!obter_data_inicio_fim(&data_inicio, &data_fim)) {
        return;
    }

    carregar_historico_vendas(&historico, &total_registros);

    if (total_registros == 0) {
        printf("Nenhum registro de venda encontrado.\n");
        liberar_historico_vendas(historico);
        printf("\nPressione ENTER para continuar...");
        limpaBuffer(); getchar();
        return;
    }

    // Contadores para produtos vendidos
    // Maximo de produtos igual ao total_produtos global para simplificar
    ProdutoVendidoContador *contadores_produtos = (ProdutoVendidoContador *)
        calloc(total_produtos, sizeof(ProdutoVendidoContador));
    if (contadores_produtos == NULL) {
        perror("Erro ao alocar memoria para contadores de produtos");
        liberar_historico_vendas(historico);
        printf("\nPressione ENTER para continuar...");
        limpaBuffer(); getchar();
        return;
    }

    // A variável 'num_produtos_distintos' foi comentada pois a lógica de contagem
    // de produtos vendidos individualmente ainda precisa ser implementada.
    // int num_produtos_distintos = 0; // Quantos produtos diferentes foram vendidos

    printf("\n(Para implementar este relatório, o 'RegistroVendaHistorico' precisaria detalhar os itens da venda, ");
    printf("ou você precisaria carregar os detalhes de cada venda pelo 'id_venda').\n");
    printf("Sem os itens individuais por venda, não é possível contar os produtos mais vendidos.\n");

    free(contadores_produtos);
    liberar_historico_vendas(historico);
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}


// 5.3 Listagem das Vendas
void relatorio_vendas_por_periodo() { // 5.3.1
    printf("\n--- LISTAGEM DAS VENDAS (em um determinado período) ---\n");
    RegistroVendaHistorico *historico = NULL;
    int total_registros = 0;
    time_t data_inicio, data_fim;

    if (!obter_data_inicio_fim(&data_inicio, &data_fim)) {
        return;
    }

    carregar_historico_vendas(&historico, &total_registros);

    if (total_registros == 0) {
        printf("Nenhum registro de venda encontrado.\n");
    } else {
        char s_inicio[9], s_fim[9];
        strftime(s_inicio, sizeof(s_inicio), "%d%m%Y", localtime(&data_inicio));
        strftime(s_fim, sizeof(s_fim), "%d%m%Y", localtime(&data_fim));

        printf("\nVendas realizadas entre %s e %s:\n", s_inicio, s_fim);
        printf("--------------------------------------------------------------------------------------------------\n");
        printf("ID Venda | Data     | Valor Total | ID Cliente | Tipo Pagamento\n"); // Coluna de data ajustada
        printf("--------------------------------------------------------------------------------------------------\n");
        float total_periodo = 0.0;
        int encontrou_venda_no_periodo = 0;

        for (int i = 0; i < total_registros; i++) {
            time_t data_venda;
            if (parse_data_ddmmyyyy(historico[i].data, &data_venda)) { // Usando historico[i].data
                if (data_venda >= data_inicio && data_venda <= data_fim) {
                    printf("%-8d | %-8s | R$ %-8.2f | %-10d | %s\n", // %-8s para a data
                           historico[i].id_venda, historico[i].data, historico[i].valor_total,
                           historico[i].id_cliente, historico[i].tipo_pagamento);
                    total_periodo += historico[i].valor_total;
                    encontrou_venda_no_periodo = 1;
                }
            }
        }
        if (!encontrou_venda_no_periodo) {
            printf("Nenhuma venda encontrada no período especificado.\n");
        } else {
            printf("--------------------------------------------------------------------------------------------------\n");
            printf("TOTAL DE VENDAS NO PERÍODO: R$ %.2f\n", total_periodo);
        }
        printf("---------------------------------------------------------------------------------------------------\n");
    }
    liberar_historico_vendas(historico);
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}


void relatorio_faturamento_consolidado_periodo() { // 5.3.2
    printf("\n--- FATURAMENTO CONSOLIDADO – em um período ---\n");
    RegistroVendaHistorico *historico = NULL;
    int total_registros = 0;
    time_t data_inicio, data_fim;

    if (!obter_data_inicio_fim(&data_inicio, &data_fim)) {
        return;
    }

    carregar_historico_vendas(&historico, &total_registros);

    if (total_registros == 0) {
        printf("Nenhum registro de venda encontrado.\n");
    } else {
        float total_dinheiro = 0.0;
        float total_cartao = 0.0;
        float total_consolidado = 0.0;
        int encontrou_venda_no_periodo = 0;

        char s_inicio[9], s_fim[9];
        strftime(s_inicio, sizeof(s_inicio), "%d%m%Y", localtime(&data_inicio));
        strftime(s_fim, sizeof(s_fim), "%d%m%Y", localtime(&data_fim));

        printf("\nFaturamento consolidado entre %s e %s:\n", s_inicio, s_fim);

        for (int i = 0; i < total_registros; i++) {
            time_t data_venda;
            if (parse_data_ddmmyyyy(historico[i].data, &data_venda)) { // Usando historico[i].data
                if (data_venda >= data_inicio && data_venda <= data_fim) {
                    if (strcmp(historico[i].tipo_pagamento, "Dinheiro") == 0) {
                        total_dinheiro += historico[i].valor_total;
                    } else if (strcmp(historico[i].tipo_pagamento, "Cartao") == 0) {
                        total_cartao += historico[i].valor_total;
                    }
                    total_consolidado += historico[i].valor_total;
                    encontrou_venda_no_periodo = 1;
                }
            }
        }

        if (!encontrou_venda_no_periodo) {
            printf("Nenhuma venda encontrada no período especificado.\n");
        } else {
            printf("------------------------------------------------\n");
            printf("Consolidado de valores recebidos em Dinheiro: R$ %.2f\n", total_dinheiro);
            printf("Consolidado de valores recebidos em Cartão:   R$ %.2f\n", total_cartao);
            printf("------------------------------------------------\n");
            printf("FATURAMENTO TOTAL NO PERÍODO:                 R$ %.2f\n", total_consolidado);
            printf("------------------------------------------------\n");
        }
    }
    liberar_historico_vendas(historico);
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); getchar();
}


// --- Funções de Relatório Financeiro (Caixa) - Mantidas do anterior ---
void relatorio_caixa_atual() {
    printf("\n--- RELATÓRIO DE CAIXA ATUAL ---\n");
    printf("Saldo Inicial do Caixa: R$ %.2f\n", caixaI);
    printf("Saldo Atual do Caixa:   R$ %.2f\n", caixaF);
    printf("--------------------------------\n");
    printf("\nPressione ENTER para continuar...");
    limpaBuffer();
    getchar();
}

void relatorio_movimentacao_caixa() {
    printf("\n--- RELATÓRIO DE MOVIMENTAÇÃO DE CAIXA (Ainda não implementado) ---\n");
    printf("\nPressione ENTER para continuar...");
    limpaBuffer();
    getchar();
}