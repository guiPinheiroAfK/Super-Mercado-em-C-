// venPag.c
#include "venPag.h"
#include "cadastro.h"   // Para acessar produtos e categorias
#include "idcliente.h" // Para acessar a função de buscar produto por código
#include "utilidades.h" // Para limpaBuffer()

// --- DEFINIÇÕES EXTERNAS DAS VARIÁVEIS GLOBAIS ---
extern ItemVenda *carrinho_de_compras;
extern int total_itens_carrinho;
extern float total_venda_atual;

extern CADASTRO *clientes;
extern int total_clientes;
extern Produto *produtos;
extern int total_produtos;
extern Categoria *categorias; // Se você tiver categoria global aqui também
extern int total_categorias;

// --- SUAS VARIÁVEIS GLOBAIS DO CAIXA (ACESSO AQUI VIA 'extern') ---
extern int caixaA;
extern float caixaF;
extern float caixaI;

// --- Implementação do Menu Principal de Vendas ---
// Dentro de mVendas()
// --- Implementação do Menu Principal de Vendas ---
void mVendas() { // Sem parâmetros de caixa
    int opcao = 0;

    do {
        system("cls");
        printf("======================================\n");
        printf("            MENU VENDAS           \n");
        printf("======================================\n");
        printf("1. Nova Venda  \n");
        printf("2. Retirada de Caixa (Sangria) \n");
        printf("3. Pagamento\n");
        printf("4. Retornar ao Menu Principal  \n");
        printf("--------------------------------------\n");
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
                if (caixaA == 0) { // Usa caixaA GLOBAL
                    printf("ERRO: O caixa nao foi aberto. Por favor, abra o caixa antes de iniciar uma venda.\n");
                    Sleep(2500);
                    break;
                }
                if (total_itens_carrinho > 0) {
                    printf("======================================\n");
                    printf("  ATENCAO: CARRINHO COM ITENS!        \n");
                    printf("======================================\n");
                    printf("Primeiro finalize a compra anterior antes de comecar outra.\n");
                    printf("Se deseja cancelar a compra atual, selecione 'Pagamento' e depois 'Cancelar'.\n");
                    printf("Pressione ENTER para retornar ao menu de Vendas...");
                    limpaBuffer();
                } else {
                    printf("Iniciando nova venda...\n");
                    Sleep(1000);
                    vMenuDentro();
                }
                break;
            case 2:
                retirada_caixa(); // Sem parâmetros
                break;
            case 3:
                if (caixaA == 0) { // Usa caixaA GLOBAL
                    printf("ERRO: O caixa nao foi aberto. Nao e possivel processar pagamento.\n");
                    Sleep(2500);
                    break;
                }
                finalizar_venda(); // Sem parâmetros
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

// Função para abrir o caixa
void caixaAbertura(float *caixaI_ptr, int *caixaA_ptr) {
    int opcao;

    if (*caixaA_ptr == 0) { // Verifica se o caixa ainda não foi aberto
        Sleep(1000);
        printf("Depois de aberto uma vez, o caixa inicial nao podera ser mudado. \n");
        Sleep(1500);
        printf("Deseja continuar com a abertura do caixa? \n");
        printf("1. Sim \n");
        printf("2. Nao. \n");
        scanf("%d", &opcao);
        limpaBuffer();

        if (opcao == 1) {
            system("cls");
            printf("Abertura de Caixa: \n\n");
            Sleep(1250);
            printf("Digite o caixa inicial do dia: ");
            scanf("%f", caixaI_ptr);
            limpaBuffer();
            printf("O caixa inicial e %.2f.\n", *caixaI_ptr);
            Sleep(1500);
            *caixaA_ptr = 1;
            caixaF = *caixaI_ptr; // <<--- Inicializa caixaF aqui
        } else if (opcao == 2) {
            // Se o usuário escolher não abrir o caixa, não faz nada
        }
    } else if (*caixaA_ptr == 1) {
        printf("O caixa ja foi inicializado uma vez! \n");
        printf("Pressione ENTER para continuar...");
        limpaBuffer();
    }
}

// --- Funções de Carrinho de Compras ---

void limpar_carrinho() {
    if (carrinho_de_compras != NULL) {
        free(carrinho_de_compras);
        carrinho_de_compras = NULL;
    }
    total_itens_carrinho = 0;
    total_venda_atual = 0.0;
}

void adicionar_produto_ao_carrinho(int codigo_produto, int quantidade) {
    Produto *prod = buscar_produto_por_codigo(codigo_produto);
    if (prod == NULL) {
        printf("Produto com codigo %d nao encontrado.\n", codigo_produto);
        Sleep(1500);
        return;
    }

    if (quantidade <= 0) {
        printf("Quantidade invalida. Deve ser maior que zero.\n");
        Sleep(1500);
        return;
    }

    // --- Tenta encontrar o produto no carrinho para ATUALIZAR a quantidade ---
    for (int i = 0; i < total_itens_carrinho; i++) {
        if (carrinho_de_compras[i].codigo_produto == codigo_produto) {
            // Verifica se há estoque suficiente para adicionar a QUANTIDADE SOLICITADA AGORA
            // (considerando o que já está no carrinho e o que está disponível no estoque geral)
            if (prod->quantidade_estoque < quantidade) { // Se o estoque na loja é menor que a quantidade que queremos adicionar
                printf("Estoque insuficiente para adicionar mais essa quantidade! Apenas %d unidades disponiveis de '%s'.\n", prod->quantidade_estoque, prod->descricao);
                Sleep(3000);
                return;
            }

            // Atualiza a quantidade e o subtotal no carrinho
            carrinho_de_compras[i].quantidade += quantidade;
            carrinho_de_compras[i].subtotal = carrinho_de_compras[i].quantidade * carrinho_de_compras[i].preco_unitario;
            total_venda_atual += (quantidade * carrinho_de_compras[i].preco_unitario); // Usa preco_unitario do carrinho que ja eh o de venda

            // DEDUZ O ESTOQUE NA MEMÓRIA e SALVA a mudança no arquivo imediatamente
            prod->quantidade_estoque -= quantidade;
            salvar_produtos(); // Persiste a mudança de estoque no arquivo

            printf("Quantidade de '%s' atualizada no carrinho. Nova quantidade: %d. Estoque restante: %d. Subtotal: R$ %.2f\n",
                   prod->descricao,              // %s
                   carrinho_de_compras[i].quantidade, // %d (Nova quantidade no carrinho)
                   prod->quantidade_estoque,     // %d (Estoque restante na loja)
                   carrinho_de_compras[i].subtotal); // %.2f
            Sleep(2000);
            return; // Termina a função, pois o produto foi atualizado no carrinho
        }
    }

    // --- Se o produto NÃO ESTÁ no carrinho, adiciona como um NOVO item ---

    // Verifica se há estoque suficiente para a adição inicial
    if (quantidade > prod->quantidade_estoque) {
        printf("Estoque insuficiente para a compra inicial! Apenas %d unidades disponiveis de '%s'.\n", prod->quantidade_estoque, prod->descricao);
        Sleep(3000);
        return;
    }

    ItemVenda novo_item;
    novo_item.codigo_produto = prod->codigo;
    strcpy(novo_item.descricao_produto, prod->descricao);
    // Calcula o preço de venda para o item do carrinho
    novo_item.preco_unitario = prod->preco_compra * (1 + prod->margem_lucro_percentual / 100.0);
    novo_item.quantidade = quantidade;
    novo_item.subtotal = novo_item.preco_unitario * quantidade;

    // Redimensiona o carrinho para adicionar o novo item
    carrinho_de_compras = (ItemVenda *)realloc(carrinho_de_compras, (total_itens_carrinho + 1) * sizeof(ItemVenda));
    if (carrinho_de_compras == NULL) {
        perror("Erro ao alocar memoria para item do carrinho");
        exit(EXIT_FAILURE);
    }

    carrinho_de_compras[total_itens_carrinho] = novo_item;
    total_itens_carrinho++;
    total_venda_atual += novo_item.subtotal;

    // DEDUZ O ESTOQUE NA MEMÓRIA e SALVA a mudança no arquivo imediatamente
    prod->quantidade_estoque -= quantidade;
    salvar_produtos(); // Persiste a mudança de estoque no arquivo

    printf("Produto '%s' (R$ %.2f x %d) adicionado ao carrinho. Estoque restante: %d. Subtotal: R$ %.2f\n",
           novo_item.descricao_produto, // %s
           novo_item.preco_unitario,    // R$ %.2f
           novo_item.quantidade,        // x %d
           prod->quantidade_estoque,    // %d
           novo_item.subtotal);         // %.2f
    Sleep(2000);
}

void listar_carrinho() {
    printf("======================================\n");
    printf("          CARRINHO DE COMPRAS         \n");
    printf("======================================\n");
    if (total_itens_carrinho == 0) {
        printf("Carrinho vazio.\n");
    } else {
        printf("%-6s %-30s %-10s %-10s %-10s\n", "Cod.", "Descricao", "Preco Un.", "Qtd.", "Subtotal");
        printf("------ ------------------------------ ---------- ---------- ----------\n");
        for (int i = 0; i < total_itens_carrinho; i++) {
            printf("%-6d %-30s %-10.2f %-10d %-10.2f\n",
                   carrinho_de_compras[i].codigo_produto,
                   carrinho_de_compras[i].descricao_produto,
                   carrinho_de_compras[i].preco_unitario,
                   carrinho_de_compras[i].quantidade,
                   carrinho_de_compras[i].subtotal);
        }
        printf("--------------------------------------------------------------------\n");
        printf("TOTAL DA VENDA: R$ %.2f\n", total_venda_atual);
    }
    printf("--------------------------------------------------------------------\n");
    printf("Pressione ENTER para continuar...");
    limpaBuffer();
}

// --- Menu de Navegação e Adição ao Carrinho ---
void vMenuDentro() {
    int opcao_categoria;
    int codigo_produto_add;
    int quantidade_add;
    int encontrou_categoria;

    // Loop externo para selecionar categorias ou voltar ao menu Vendas
    do {
        system("cls"); // Limpa a tela UMA VEZ ao entrar no menu de seleção de categorias
        printf("======================================\n");
        printf("       SELECAO DE PRODUTOS            \n");
        printf("======================================\n");
        listar_categorias(); // Lista todas as categorias disponíveis

        printf("\nDigite o ID da CATEGORIA para ver os produtos (0 para voltar ao menu Vendas): ");
        if (scanf("%d", &opcao_categoria) != 1) {
            limpaBuffer();
            printf("Opcao invalida. Digite um numero.\n");
            Sleep(1500);
            continue; // Volta para o início do loop externo (pedir categoria novamente)
        }
        limpaBuffer();

        if (opcao_categoria == 0) {
            printf("Voltando ao Menu Vendas...\n");
            Sleep(1000);
            return; // Sai da vMenuDentro() e retorna para mVendas()
        }

        // Verifica se a categoria existe
        encontrou_categoria = 0;
        for (int i = 0; i < total_categorias; i++) {
            if (categorias[i].id == opcao_categoria) {
                encontrou_categoria = 1;
                break;
            }
        }

        if (!encontrou_categoria) {
            printf("Categoria com ID %d nao encontrada. Tente novamente.\n", opcao_categoria);
            Sleep(1500);
            continue; // Volta para o início do loop externo (pedir categoria novamente)
        }

        // ====================================================================
        // LOOP INTERNO: EXIBIÇÃO E ADIÇÃO DE PRODUTOS
        // ====================================================================
        do {
            system("cls"); // Limpa a tela APENAS ao entrar ou recarregar a lista de produtos de UMA CATEGORIA
            printf("======================================\n");
            printf("  PRODUTOS DA CATEGORIA: %s           \n", categorias[opcao_categoria - 1].nome);
            printf("======================================\n");

            int produtos_na_categoria = 0;
            printf("%-6s %-30s %-10s %-8s %-10s\n", "Cod.", "Descricao", "Preco", "Estoque", "Minimo");
            printf("------ ------------------------------ ---------- -------- ----------\n");
            for (int i = 0; i < total_produtos; i++) {
                if (produtos[i].id_categoria == opcao_categoria) {
                    float preco_venda = produtos[i].preco_compra * (1 + produtos[i].margem_lucro_percentual / 100.0);
                    printf("%-6d %-30s %-10.2f %-8d %-10d\n",
                           produtos[i].codigo, produtos[i].descricao, preco_venda,
                           produtos[i].quantidade_estoque, produtos[i].estoque_minimo);
                    produtos_na_categoria = 1;
                }
            }
            if (!produtos_na_categoria) {
                printf("Nenhum produto cadastrado para esta categoria.\n");
            }
            printf("--------------------------------------------------------------------\n");

            // --- AQUI NÃO VAMOS MAIS CHAMAR listar_carrinho() DIRETAMENTE ---
            // Ela será chamada APENAS na saída ou na finalização.

            printf("\nO que deseja fazer?\n");
            printf("Digite o CODIGO do produto para adicionar ao carrinho.\n");
            printf("Digite 0 para VOLTAR para a selecao de CATEGORIAS.\n");
            printf("Sua opcao: ");

            if (scanf("%d", &codigo_produto_add) != 1) {
                limpaBuffer();
                printf("Entrada invalida. Digite um numero.\n");
                Sleep(1500);
                continue; // Volta para o início deste loop interno (mostra produtos novamente)
            }
            limpaBuffer();

            if (codigo_produto_add == 0) {
                printf("Voltando para a selecao de categorias...\n");
                Sleep(1000);
                // Antes de voltar, se o carrinho tiver itens, podemos mostrá-lo aqui
                if (total_itens_carrinho > 0) {
                    listar_carrinho(); // Mostra o carrinho ao SAIR da categoria
                }
                break; // Sai DESTE loop interno (volta para o loop externo de categorias)
            }

            printf("Qual a QUANTIDADE deseja comprar? ");
            if (scanf("%d", &quantidade_add) != 1) {
                limpaBuffer();
                printf("Entrada invalida. Digite um numero.\n");
                Sleep(1500);
                continue; // Volta para o início deste loop interno (mostra produtos novamente)
            }
            limpaBuffer();

            adicionar_produto_ao_carrinho(codigo_produto_add, quantidade_add);
            // Após adicionar, o loop se repete e a tela será limpa novamente para mostrar a lista de produtos
            // Isso evita que a tela fique cheia de mensagens de adição, e foca na lista de produtos.

        } while (1); // Este loop interno continua indefinidamente até que um 'break' seja acionado.

    } while (1); // Este loop externo também continua indefinidamente até que um 'return' seja acionado.
}

// --- Funções de Pagamento ---
void finalizar_venda() { // Sem parâmetros de caixa
    system("cls");
    printf("--- FINALIZANDO VENDA ---\n");
    listar_carrinho();

    if (total_itens_carrinho == 0) {
        printf("Nenhum item no carrinho para finalizar ou cancelar.\n");
        printf("Pressione ENTER para voltar ao menu Vendas.\n");
        limpaBuffer();
        return;
    }

    int opcao_finalizacao;
    printf("\nO que deseja fazer?\n");
    printf("1. Processar Pagamento e Concluir Venda\n");
    printf("2. Cancelar Venda e Limpar Carrinho\n");
    printf("Escolha uma opcao: ");

    if (scanf("%d", &opcao_finalizacao) != 1) {
        limpaBuffer();
        printf("Opcao invalida. Digite um numero.\n");
        Sleep(1500);
        return;
    }
    limpaBuffer();

    switch (opcao_finalizacao) {
        case 1:
            salvar_produtos();
            realizar_pagamento(); // Sem parâmetros
            limpar_carrinho();
            printf("\nVenda concluida com sucesso! Estoque atualizado.\n");
            break;
        case 2:
            printf("Devolvendo itens do carrinho ao estoque...\n");
            for (int i = 0; i < total_itens_carrinho; i++) {
                Produto *prod = buscar_produto_por_codigo(carrinho_de_compras[i].codigo_produto);
                if (prod != NULL) {
                    prod->quantidade_estoque += carrinho_de_compras[i].quantidade;
                }
            }
            salvar_produtos();
            limpar_carrinho();
            printf("\nVenda cancelada. Carrinho limpo e estoque restaurado.\n");
            break;
        default:
            printf("Opcao invalida. Nenhuma acao realizada.\n");
            break;
    }
    printf("Pressione ENTER para continuar...");
    limpaBuffer();
}

void realizar_pagamento() { // Sem parâmetros de caixa
    system("cls");
    printf("======================================\n");
    printf("          REALIZAR PAGAMENTO          \n");
    printf("======================================\n");

    if (total_itens_carrinho == 0) {
        printf("Carrinho vazio. Nao ha venda para pagar.\n");
        printf("Pressione ENTER para voltar...");
        limpaBuffer();
        return;
    }

    float valor_a_pagar = total_venda_atual;
    int forma_pagamento;
    float valor_recebido;
    float troco;
    float percentual_desconto = 0.0f;
    float valor_desconto = 0.0f;

    // --- SEÇÃO: IDENTIFICAR O CLIENTE (mantida como está) ---
    int id_cliente_selecionado = 0;
    char escolha_id_str[10];
    CADASTRO *cliente_comprador = NULL;

    printf("\n--- Identificacao do Cliente da Compra ---\n");
    if (total_clientes == 0) {
        printf("Nenhum cliente cadastrado no momento. A venda nao sera associada a um cliente.\n");
        printf("Pressione ENTER para continuar...\n");
        limpaBuffer();
    } else {
        printf("Qual cliente fez esta compra?\n");
        printf("ID   | Nome Social\n");
        printf("-----|-----------------------\n");
        for (int i = 0; i < total_clientes; i++) {
            printf("%04d | %s\n", clientes[i].id_cliente, clientes[i].nSocial);
        }
        printf("--------------------------------------\n");
        printf("Digite o ID do cliente (0 para nao associar): ");

        do {
            fgets(escolha_id_str, sizeof(escolha_id_str), stdin);
            escolha_id_str[strcspn(escolha_id_str, "\n")] = '\0';
            id_cliente_selecionado = atoi(escolha_id_str);

            if (id_cliente_selecionado == 0) {
                printf("Venda nao associada a nenhum cliente.\n");
                break;
            }

            for (int i = 0; i < total_clientes; i++) {
                if (clientes[i].id_cliente == id_cliente_selecionado) {
                    cliente_comprador = &clientes[i];
                    break;
                }
            }

            if (cliente_comprador == NULL && id_cliente_selecionado != 0) {
                printf("ID de cliente nao encontrado. Por favor, digite um ID valido ou 0: ");
            }
        } while (cliente_comprador == NULL && id_cliente_selecionado != 0);
    }
    // --- FIM DA SEÇÃO: IDENTIFICAR O CLIENTE ---

    printf("\nValor total da venda: R$ %.2f\n", valor_a_pagar);
    printf("\nEscolha a forma de pagamento:\n");
    printf("1. Dinheiro\n");
    printf("2. Cartao de Credito/Debito\n");
    printf("Sua opcao: ");

    if (scanf("%d", &forma_pagamento) != 1) {
        limpaBuffer();
        printf("Opcao invalida. Pagamento cancelado.\n");
        Sleep(1500);
        return;
    }
    limpaBuffer();

    switch (forma_pagamento) {
        case 1: // Dinheiro
            printf("\nPagamento em Dinheiro.\n");
            char dar_desconto_opcao;
            printf("Deseja dar um desconto? (S/N): ");
            scanf(" %c", &dar_desconto_opcao);
            limpaBuffer();

            if (toupper(dar_desconto_opcao) == 'S') {
                do {
                    printf("Qual o percentual de desconto (0 a 60)? ");
                    if (scanf("%f", &percentual_desconto) != 1) {
                        limpaBuffer();
                        printf("Entrada invalida. Digite um numero.\n");
                        percentual_desconto = -1;
                        continue;
                    }
                    limpaBuffer();

                    if (percentual_desconto < 0 || percentual_desconto > 60) {
                        printf("Percentual invalido! Deve ser entre 0 e 60.\n");
                        Sleep(1500);
                    }
                } while (percentual_desconto < 0 || percentual_desconto > 60);

                valor_desconto = valor_a_pagar * (percentual_desconto / 100.0f);
                valor_a_pagar -= valor_desconto;
                printf("Desconto aplicado: R$ %.2f (%.0f%%)\n", valor_desconto, percentual_desconto);
                printf("Novo valor a pagar: R$ %.2f\n", valor_a_pagar);
            }

            do {
                printf("Valor recebido: R$ ");
                if (scanf("%f", &valor_recebido) != 1) {
                    limpaBuffer();
                    printf("Valor invalido. Digite um numero.\n");
                    Sleep(1500);
                    valor_recebido = -1.0f;
                    continue;
                }
                limpaBuffer();

                if (valor_recebido < valor_a_pagar) {
                    printf("Valor recebido insuficiente. Faltam R$ %.2f\n", valor_a_pagar - valor_recebido);
                    Sleep(1500);
                }
            } while (valor_recebido < valor_a_pagar);

            troco = valor_recebido - valor_a_pagar;
            printf("Troco: R$ %.2f\n", troco);

            // --- SEÇÃO: VALIDAÇÃO E AUMENTO DO CAIXA (USANDO caixaF GLOBAL) ---
            if (troco > caixaF) {
                printf("\nATENCAO: O troco (R$ %.2f) e maior que o saldo atual do caixa (R$ %.2f).\n", troco, caixaF);
                char aumentar_caixa_str[20];
                float valor_adicional_para_troco;
                printf("Por favor, adicione dinheiro ao caixa para cobrir o troco.\n");
                printf("Quanto deseja adicionar ao caixa? R$ ");
                fgets(aumentar_caixa_str, sizeof(aumentar_caixa_str), stdin);
                valor_adicional_para_troco = atof(aumentar_caixa_str);

                if (valor_adicional_para_troco > 0) {
                    caixaF += valor_adicional_para_troco;
                    printf("R$ %.2f adicionado ao caixa. Novo saldo do caixa: R$ %.2f\n", valor_adicional_para_troco, caixaF);
                } else {
                    printf("Nenhum valor valido foi adicionado. O troco pode nao ser coberto.\n");
                }
            }
            // Atualiza o saldo do caixaF: adiciona o valor da venda e subtrai o troco
            caixaF += valor_a_pagar;
            caixaF -= troco;
            printf("Saldo do Caixa Atualizado: R$ %.2f\n", caixaF);
            // --- FIM DA SEÇÃO: VALIDAÇÃO E AUMENTO DO CAIXA ---

            printf("Pagamento em dinheiro concluido.\n");
            break;

        case 2: // Cartão
            printf("\nPagamento em Cartao de Credito/Debito.\n");
            printf("Aguardando aprovacao da operadora...\n");
            Sleep(2000);
            printf("Pagamento via cartao aprovado.\n");
            caixaF += valor_a_pagar;
            printf("Saldo do Caixa Atualizado (apos cartao): R$ %.2f\n", caixaF);
            break;

        default:
            printf("Opcao de pagamento invalida. Pagamento nao processado.\n");
            Sleep(1500);
            return;
    }

    printf("Pressione ENTER para continuar...");
    limpaBuffer();

    registrar_venda_no_historico(total_venda_atual, cliente_comprador);
}

void registrar_venda_no_historico(float total_da_venda, CADASTRO *cliente_comprador) {
    printf("Registrando venda no historico...\n");

    FILE *hist_arquivo = fopen(ARQUIVO_HIST_VENDAS, "a");
    if (hist_arquivo == NULL) {
        perror("Erro ao abrir o arquivo de historico de vendas para escrita");
        Sleep(2000);
        return;
    }

    char data_hora_str[30];
    obter_data_hora_atual(data_hora_str, sizeof(data_hora_str));

    fprintf(hist_arquivo, "---------------------------------------------------\n");
    fprintf(hist_arquivo, "VENDA REGISTRADA EM: %s\n", data_hora_str);
    fprintf(hist_arquivo, "---------------------------------------------------\n");
    fprintf(hist_arquivo, "ITENS DA COMPRA:\n");

    if (total_itens_carrinho == 0) {
        fprintf(hist_arquivo, "  (Carrinho vazio - erro ou venda cancelada?)\n");
    } else {
        for (int i = 0; i < total_itens_carrinho; i++) {
            fprintf(hist_arquivo, "- Produto: %s (Cod: %d)\n",
                    carrinho_de_compras[i].descricao_produto,
                    carrinho_de_compras[i].codigo_produto);
            fprintf(hist_arquivo, "  Preco Unitario: R$ %.2f\n", carrinho_de_compras[i].preco_unitario);
            fprintf(hist_arquivo, "  Quantidade Comprada: %d\n", carrinho_de_compras[i].quantidade);
            fprintf(hist_arquivo, "  Subtotal do Item: R$ %.2f\n", carrinho_de_compras[i].subtotal);
            fprintf(hist_arquivo, "  ---\n");
        }
    }

    fprintf(hist_arquivo, "---------------------------------------------------\n");
    fprintf(hist_arquivo, "TOTAL DA VENDA: R$ %.2f\n", total_da_venda);
    fprintf(hist_arquivo, "SALDO FINAL DO CAIXA: R$ %.2f\n", caixaF);
    if (cliente_comprador != NULL) {
        fprintf(hist_arquivo, "\nCLIENTE ASSOCIADO:\n");
        fprintf(hist_arquivo, "  ID: %04d\n", cliente_comprador->id_cliente);
        fprintf(hist_arquivo, "  Nome: %s\n", cliente_comprador->nomeC);
        fprintf(hist_arquivo, "  CPF: %s\n", cliente_comprador->cpf_str);
    } else {
        fprintf(hist_arquivo, "\nCLIENTE ASSOCIADO: Nenhum cliente selecionado.\n");
    }

    fprintf(hist_arquivo, "\n===================================================\n\n");
    fflush(hist_arquivo);
    fclose(hist_arquivo);
    printf("Historico de venda registrado em '%s' com sucesso!\n", ARQUIVO_HIST_VENDAS);
    Sleep(2000);
}

// --- Funções de Retirada de Caixa (Sangria) ---
void retirada_caixa() { // <<--- Sem parâmetros de caixa
    system("cls");
    printf("--- RETIRADA DE CAIXA (SANGRIA) ---\n");

    if (caixaF <= 0) { // <<--- Usa caixaF GLOBAL
        printf("Caixa vazio. Nao ha dinheiro para retirada.\n");
        printf("Pressione ENTER para voltar...");
        limpaBuffer();
        return;
    }

    float valor_retirada;
    printf("Saldo atual do caixa: R$ %.2f\n", caixaF); // <<--- Usa caixaF GLOBAL
    printf("Quanto deseja retirar do caixa? R$ ");
    if (scanf("%f", &valor_retirada) != 1) {
        limpaBuffer();
        printf("Valor invalido. Retirada cancelada.\n");
        Sleep(1500);
        return;
    }
    limpaBuffer();

    if (valor_retirada <= 0) {
        printf("Valor de retirada deve ser positivo. Retirada cancelada.\n");
        Sleep(1500);
        return;
    }

    if (valor_retirada > caixaF) { // <<--- Usa caixaF GLOBAL
        printf("Valor de retirada maior que o saldo do caixa! Saldo atual: R$ %.2f\n", caixaF);
        printf("Retirada cancelada.\n");
        Sleep(2000);
        return;
    }

    caixaF -= valor_retirada; // <<--- Modifica caixaF GLOBAL
    printf("R$ %.2f retirados do caixa. Novo saldo: R$ %.2f\n", valor_retirada, caixaF);
    printf("Pressione ENTER para voltar...");
    limpaBuffer();
}

// --- Funções Auxiliares (mantidas como estão ou movidas para onde se encaixem melhor) ---
void obter_data_hora_atual(char *buffer, size_t tamanho) {
    time_t tempo_atual;
    struct tm *info_tempo;

    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);

    strftime(buffer, tamanho, "%d%m%Y / %H:%M:%S", info_tempo);
}

// Função para carregar o saldo do caixa
void carregar_saldo_caixa() {
    FILE *f = fopen(ARQUIVO_CAIXA_SALDO, "rb"); // Modo binário para float
    if (f == NULL) {
        caixaF = 0.0;
        caixaA = 0;
        printf("Arquivo de saldo de caixa nao encontrado. Caixa iniciando sem saldo previo.\n");
    } else {
        fread(&caixaF, sizeof(float), 1, f);
        fclose(f);
        printf("Saldo do caixa carregado: R$ %.2f.\n", caixaF);
        caixaA = 1; // Assume que o caixa estava "aberto" se o saldo foi carregado.
    }
}

// Função para salvar o saldo do caixa
void salvar_saldo_caixa() {
    FILE *f = fopen(ARQUIVO_CAIXA_SALDO, "wb");
    if (f == NULL) {
        perror("Erro ao salvar saldo do caixa.");
        return;
    }
    fwrite(&caixaF, sizeof(float), 1, f);
    fflush(f);
    fclose(f);
    printf("Saldo do caixa salvo: R$ %.2f.\n", caixaF);
}

// Em venPag.c, se preferir que esta função esteja lá, ou pode ser inline
void aumentar_caixa() {
    system("cls");
    printf("--- ADICIONAR VALOR AO CAIXA ---\n");

    if (caixaA == 0) {
        printf("ERRO: O caixa nao esta aberto. Nao e possivel adicionar dinheiro.\n");
        printf("Pressione ENTER para voltar...");
        limpaBuffer();
        return;
    }

    float valor_adicional;
    printf("Saldo atual do caixa: R$ %.2f\n", caixaF);
    printf("Quanto deseja adicionar ao caixa? R$ ");

    if (scanf("%f", &valor_adicional) != 1) {
        limpaBuffer();
        printf("Valor invalido. Nenhuma quantia adicionada.\n");
        Sleep(1500);
        return;
    }
    limpaBuffer();

    if (valor_adicional <= 0) {
        printf("O valor a ser adicionado deve ser positivo. Nenhuma quantia adicionada.\n");
        Sleep(1500);
        return;
    }

    caixaF += valor_adicional;
    printf("R$ %.2f adicionados ao caixa. Novo saldo: R$ %.2f\n", valor_adicional, caixaF);
    printf("Pressione ENTER para voltar...");
    limpaBuffer();
}

