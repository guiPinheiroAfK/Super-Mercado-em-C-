// cadastros.c
// Este arquivo conterá as implementações das funções de cadastro de produtos e categorias.
#include <string.h>
#include <strings.h>  // Essa aqui eu (gui, especificamente) só adicionei por causa de problemas do linux e do MacOS
#include "cadastro.h" // Inclui o cabeçalho cadastros.h (que já inclui utilidades.h)
#include <time.h>      // Incluir se as funções de carregar/salvar ou outras em cadastros.c usarem time_t

// --- Constantes para Nomes de Arquivo ---
#define ARQUIVO_PRODUTOS "produtosCad.txt"
#define ARQUIVO_CATEGORIAS "categoriasCad.txt" // Adicionado para persistência de categorias

// --- DEFINIÇÃO DAS VARIÁVEIS GLOBAIS DE PRODUTOS E CATEGORIAS ---
// As variáveis globais que foram DECLARADAS como 'extern' em cadastros.h,
// devem ser DEFINIDAS AQUI, com sua inicialização.

Categoria *categorias = NULL;
int total_categorias = 0;
int proximo_id_categoria = 1; // Começa em 1 para facilitar IDs sequenciais

Produto *produtos = NULL;
int total_produtos = 0;


void cadastrar_categoria() {
    system("cls");
    printf("--- CADASTRO DE NOVA CATEGORIA ---\n");

    Categoria nova_categoria;
    nova_categoria.id = proximo_id_categoria; // Atribui o próximo ID disponível

    do {
        printf("Digite o nome da categoria (apenas letras e espacos, max 49 caracteres): ");
        if (fgets(nova_categoria.nome, sizeof(nova_categoria.nome), stdin) == NULL) {
            perror("Erro ao ler o nome da categoria");
            return;
        }
        nova_categoria.nome[strcspn(nova_categoria.nome, "\n")] = '\0'; // Remove a quebra de linha

        if (strlen(nova_categoria.nome) == 0) {
            printf("O nome da categoria nao pode ser vazio.\n");
        } else if (!validar_string_letras_espacos(nova_categoria.nome)) {
            printf("Nome invalido! Use apenas letras e espacos.\n");
        } else {
            // Verificar se a categoria já existe (case-insensitive)
            int categoria_existe = 0;
            for (int i = 0; i < total_categorias; i++) {
                if (strcasecmp(categorias[i].nome, nova_categoria.nome) == 0) {
                    printf("Esta categoria ja existe! Por favor, digite outro nome.\n");
                    categoria_existe = 1;
                    break;
                }
            }
            if (!categoria_existe) {
                break; // Sai do loop se o nome for válido e único
            }
        }
    } while (1);

    // Redimensiona o array de categorias
    Categoria *temp = realloc(categorias, (total_categorias + 1) * sizeof(Categoria));
    if (temp == NULL) {
        perror("Erro ao alocar memoria para a categoria");
        return;
    }
    categorias = temp;
    categorias[total_categorias] = nova_categoria;
    total_categorias++;
    proximo_id_categoria++; // Incrementa o próximo ID disponível

    printf("Categoria '%s' (ID: %d) cadastrada com sucesso!\n", nova_categoria.nome, nova_categoria.id);
    salvar_categorias(); // Salva as categorias no arquivo
    Sleep(1500);
}

void listar_categorias() {
    system("cls");
    printf("--- LISTA DE CATEGORIAS ---\n");
    if (total_categorias == 0) {
        printf("Nenhuma categoria cadastrada ainda.\n");
        Sleep(1500);
        return;
    }

    printf("ID\tNome\n");
    printf("---\t----\n");
    for (int i = 0; i < total_categorias; i++) {
        printf("%d\t%s\n", categorias[i].id, categorias[i].nome);
    }
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); // Espera o ENTER
}

int selecionar_categoria() {
    if (total_categorias == 0) {
        printf("Nenhuma categoria cadastrada. Por favor, cadastre uma categoria primeiro.\n");
        return -1; // Indica que não há categorias para selecionar
    }

    listar_categorias(); // Exibe as categorias disponíveis
    int id_selecionado;
    int categoria_encontrada = 0;

    do {
        printf("Digite o ID da categoria para o produto: ");
        if (scanf("%d", &id_selecionado) != 1) {
            printf("Entrada invalida. Digite um numero.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            for (int i = 0; i < total_categorias; i++) {
                if (categorias[i].id == id_selecionado) {
                    categoria_encontrada = 1;
                    break;
                }
            }
            if (!categoria_encontrada) {
                printf("ID de categoria nao encontrado. Tente novamente.\n");
            }
        }
    } while (!categoria_encontrada);

    return id_selecionado;
}
// Salva as categorias no arquivo
void salvar_categorias() {
    FILE *arquivo = fopen(ARQUIVO_CATEGORIAS, "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo de categorias para escrita");
        return;
    }
    for (int i = 0; i < total_categorias; i++) {
        fprintf(arquivo, "%d;%s\n", categorias[i].id, categorias[i].nome);
    }
    fclose(arquivo);
}
// Carrega as categorias do arquivo
void carregar_categorias() {
    FILE *arquivo = fopen(ARQUIVO_CATEGORIAS, "r");
    if (arquivo == NULL) {
        // Arquivo não existe ou não pode ser aberto, é a primeira execução ou foi apagado
        printf("Arquivo de categorias nao encontrado ou vazio. Iniciando sem categorias pre-existentes.\n");
        return;
    }

    // Libera qualquer categoria carregada anteriormente (para recarregar)
    if (categorias != NULL) {
        free(categorias);
        categorias = NULL;
        total_categorias = 0;
    }

    Categoria temp_cat;
    char line[150]; // Buffer para a linha do arquivo
    int max_id = 0;

    while (fgets(line, sizeof(line), arquivo) != NULL) {
        line[strcspn(line, "\n")] = '\0'; // Remove a quebra de linha

        // Usa sscanf para parsear a linha
        if (sscanf(line, "%d;%49[^\n]", &temp_cat.id, temp_cat.nome) == 2) {
            // Redimensiona o array
            Categoria *temp_ptr = realloc(categorias, (total_categorias + 1) * sizeof(Categoria));
            if (temp_ptr == NULL) {
                perror("Erro ao realocar memoria para carregar categorias");
                // Tenta salvar o que já foi carregado antes de sair
                if (categorias != NULL) free(categorias);
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }
            categorias = temp_ptr;
            categorias[total_categorias] = temp_cat;
            total_categorias++;
            if (temp_cat.id > max_id) {
                max_id = temp_cat.id;
            }
        }
    }
    fclose(arquivo);
    proximo_id_categoria = max_id + 1; // Garante que novos IDs sejam únicos
    printf("%d categorias carregadas.\n", total_categorias);
}

void menu_cadastro_categorias() {
    int opcao;
    do {
        system("cls");
        printf("--- MENU DE CATEGORIAS ---\n");
        printf("1. Cadastrar Nova Categoria\n");
        printf("2. Listar Categorias\n");
        printf("3. Voltar ao Menu Principal\n"); // Alterado para voltar ao menu principal
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida! Digite um numero.\n");
            limpaBuffer();
            Sleep(1000);
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                cadastrar_categoria();
                break;
            case 2:
                listar_categorias();
                break;
            case 3:
                printf("Voltando ao Menu Principal...\n");
                Sleep(1000);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                Sleep(1000);
                break;
        }
    } while (opcao != 3);
}
// Função para cadastrar um novo produto
void cadastrar_produto() {
    system("cls");
    printf("--- CADASTRO DE NOVO PRODUTO ---\n");

    Produto novo_produto;
    char codigo_str[10];
    int codigo_existe;

    // Solicita e valida o código do produto
    do {
        printf("Digite o codigo do produto (entre 1001 e 9999): ");
        if (fgets(codigo_str, sizeof(codigo_str), stdin) == NULL) {
            perror("Erro ao ler o codigo");
            return;
        }
        codigo_str[strcspn(codigo_str, "\n")] = '\0';

        if (!validar_string_digitos(codigo_str)) {
            printf("Codigo invalido! Digite apenas numeros.\n");
            continue;
        }

        novo_produto.codigo = atoi(codigo_str);

        if (novo_produto.codigo < 1001 || novo_produto.codigo > 9999) {
            printf("O codigo deve estar entre 1001 e 9999.\n");
            continue;
        }

        // Verifica se o código já existe
        codigo_existe = 0;
        for (int i = 0; i < total_produtos; i++) {
            if (produtos[i].codigo == novo_produto.codigo) {
                printf("Codigo ja existente! Por favor, escolha outro.\n");
                codigo_existe = 1;
                break;
            }
        }
    } while (codigo_existe || novo_produto.codigo < 1001 || novo_produto.codigo > 9999);


    // Descrição do produto
    do {
        printf("Digite a descricao do produto (max 99 caracteres): ");
        if (fgets(novo_produto.descricao, sizeof(novo_produto.descricao), stdin) == NULL) {
            perror("Erro ao ler a descricao");
            return;
        }
        novo_produto.descricao[strcspn(novo_produto.descricao, "\n")] = '\0';
        if (strlen(novo_produto.descricao) == 0) {
            printf("A descricao nao pode ser vazia.\n");
        } else {
            break;
        }
    } while (1);

    // Seleção de Categoria
    int id_cat = selecionar_categoria();
    if (id_cat == -1) { // Se não houver categorias para selecionar, aborta o cadastro do produto
        Sleep(2000); // Dá um tempo para o usuário ler a mensagem
        return;
    }
    novo_produto.id_categoria = id_cat;

    // Preço de Compra
    do {
        printf("Digite o preco de compra (ex: 10.50): ");
        if (scanf("%f", &novo_produto.preco_compra) != 1) {
            printf("Entrada invalida. Digite um numero para o preco.\n");
            limpaBuffer();
        } else if (novo_produto.preco_compra <= 0) {
            printf("O preco de compra deve ser maior que zero.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            break;
        }
    } while (1);

    // Margem de Lucro
    do {
        printf("Digite a margem de lucro (em percentual, ex: 20 para 20%%): ");
        if (scanf("%f", &novo_produto.margem_lucro_percentual) != 1) {
            printf("Entrada invalida. Digite um numero para a margem de lucro.\n");
            limpaBuffer();
        } else if (novo_produto.margem_lucro_percentual < 0) { // Permitir 0% de lucro, mas não negativo
            printf("A margem de lucro nao pode ser negativa.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            break;
        }
    } while (1);

    // Quantidade em Estoque
    do {
        printf("Digite a quantidade em estoque: ");
        if (scanf("%d", &novo_produto.quantidade_estoque) != 1) {
            printf("Entrada invalida. Digite um numero inteiro para a quantidade.\n");
            limpaBuffer();
        } else if (novo_produto.quantidade_estoque < 0) {
            printf("A quantidade em estoque nao pode ser negativa.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            break;
        }
    } while (1);

    // Estoque Mínimo
    do {
        printf("Digite o estoque minimo: ");
        if (scanf("%d", &novo_produto.estoque_minimo) != 1) {
            printf("Entrada invalida. Digite um numero inteiro para o estoque minimo.\n");
            limpaBuffer();
        } else if (novo_produto.estoque_minimo < 0) {
            printf("O estoque minimo nao pode ser negativo.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            break;
        }
    } while (1);

    // Redimensiona o array de produtos
    Produto *temp = realloc(produtos, (total_produtos + 1) * sizeof(Produto));
    if (temp == NULL) {
        perror("Erro ao alocar memoria para o produto");
        return;
    }
    produtos = temp;
    produtos[total_produtos] = novo_produto;
    total_produtos++;

    printf("Produto '%s' (Codigo: %d) cadastrado com sucesso!\n", novo_produto.descricao, novo_produto.codigo);
    salvar_produtos(); // Salva os produtos no arquivo após o cadastro
    Sleep(1500);
}
// Função para listar todos os produtos
void listar_produtos() {
    system("cls");
    printf("--- LISTA DE PRODUTOS ---\n");
    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado ainda.\n");
        Sleep(1500);
        return;
    }

    printf("Cod.\tDescricao\tCategoria\tPreco Compra\tMargem Lucro\tPreco Venda\tEstoque\tEstoque Minimo\n");
    printf("-----\t----------\t---------\t------------\t------------\t-----------\t-------\t--------------\n");

    for (int i = 0; i < total_produtos; i++) {
        // Encontrar o nome da categoria pelo ID
        char nome_categoria[50] = "Desconhecida";
        for (int j = 0; j < total_categorias; j++) {
            if (categorias[j].id == produtos[i].id_categoria) {
                strcpy(nome_categoria, categorias[j].nome);
                break;
            }
        }
        float preco_venda = produtos[i].preco_compra * (1 + produtos[i].margem_lucro_percentual / 100.0);

        printf("%d\t%s\t\t%s\t\t%.2f\t\t%.1f%%\t\t%.2f\t\t%d\t%d\n",
               produtos[i].codigo,
               produtos[i].descricao,
               nome_categoria,
               produtos[i].preco_compra,
               produtos[i].margem_lucro_percentual,
               preco_venda,
               produtos[i].quantidade_estoque,
               produtos[i].estoque_minimo);
    }
    printf("\nPressione ENTER para continuar...");
    limpaBuffer(); // Espera o ENTER
}


Produto *buscar_produto_por_codigo(int codigo) {
    // COLE AQUI O CÓDIGO COMPLETO DA FUNÇÃO 'buscar_produto_por_codigo'
    // Esta função itera sobre o array global 'produtos' para encontrar um produto pelo seu 'codigo'.

    // Verifica se há produtos para buscar
    if (total_produtos == 0 || produtos == NULL) {
        return NULL; // Não há produtos cadastrados
    }

    // Percorre todos os produtos cadastrados
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].codigo == codigo) {
            return &produtos[i]; // Retorna um ponteiro para o produto encontrado
        }
    }

    return NULL; // Se o produto não for encontrado, retorna NULL
}
// Função para alterar o estoque de um produto
void alterar_estoque_produto() {
    system("cls");
    printf("--- ALTERAR ESTOQUE DE PRODUTO ---\n");

    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado para alterar o estoque.\n");
        Sleep(1500);
        return;
    }

    listar_produtos(); // Ajuda o usuário a ver os códigos existentes

    int codigo_busca;
    printf("\nDigite o codigo do produto cujo estoque deseja alterar: ");
    if (scanf("%d", &codigo_busca) != 1) {
        printf("Entrada invalida. Digite um numero.\n");
        limpaBuffer();
        Sleep(1500);
        return;
    }
    limpaBuffer();

    int indice_encontrado = -1;
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].codigo == codigo_busca) {
            indice_encontrado = i;
            break;
        }
    }

    if (indice_encontrado == -1) {
        printf("Produto com codigo %d nao encontrado.\n", codigo_busca);
        Sleep(1500);
        return;
    }

    // Produto encontrado, agora altera o estoque
    int novo_estoque;
    printf("Produto encontrado: %s (Estoque atual: %d)\n",
           produtos[indice_encontrado].descricao,
           produtos[indice_encontrado].quantidade_estoque);

    do {
        printf("Digite a nova quantidade em estoque: ");
        if (scanf("%d", &novo_estoque) != 1) {
            printf("Entrada invalida. Digite um numero inteiro.\n");
            limpaBuffer();
        } else if (novo_estoque < 0) {
            printf("O estoque nao pode ser negativo.\n");
            limpaBuffer();
        } else {
            limpaBuffer();
            break;
        }
    } while (1);

    produtos[indice_encontrado].quantidade_estoque = novo_estoque;
    printf("Estoque do produto '%s' atualizado para %d.\n",
           produtos[indice_encontrado].descricao,
           produtos[indice_encontrado].quantidade_estoque);

    salvar_produtos(); // Salva as alterações no arquivo
    Sleep(1500);
}
// Salva os produtos no arquivo
void salvar_produtos() {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo de produtos para escrita");
        return;
    }
    for (int i = 0; i < total_produtos; i++) {
        fprintf(arquivo, "%d;%s;%d;%.2f;%.2f;%d;%d\n",
                produtos[i].codigo,
                produtos[i].descricao,
                produtos[i].id_categoria,
                produtos[i].preco_compra,
                produtos[i].margem_lucro_percentual,
                produtos[i].quantidade_estoque,
                produtos[i].estoque_minimo);
    }
    fclose(arquivo);
}
// Carrega os produtos do arquivo
void carregar_produtos() {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "r");
    if (arquivo == NULL) {
        printf("Arquivo de produtos nao encontrado ou vazio. Iniciando sem produtos pre-existentes.\n");
        return;
    }

    // Libera qualquer produto carregado anteriormente (para recarregar)
    if (produtos != NULL) {
        free(produtos);
        produtos = NULL;
        total_produtos = 0;
    }

    Produto temp_prod;
    char line[256]; // Buffer para a linha do arquivo

    while (fgets(line, sizeof(line), arquivo) != NULL) {
        line[strcspn(line, "\n")] = '\0'; // Remove a quebra de linha

        if (sscanf(line, "%d;%99[^;];%d;%f;%f;%d;%d",
                   &temp_prod.codigo,
                   temp_prod.descricao,
                   &temp_prod.id_categoria,
                   &temp_prod.preco_compra,
                   &temp_prod.margem_lucro_percentual,
                   &temp_prod.quantidade_estoque,
                   &temp_prod.estoque_minimo) == 7) {

            // Redimensiona o array
            Produto *temp_ptr = realloc(produtos, (total_produtos + 1) * sizeof(Produto));
            if (temp_ptr == NULL) {
                perror("Erro ao realocar memoria para carregar produtos");
                // Tenta salvar o que já foi carregado antes de sair
                if (produtos != NULL) free(produtos);
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }
            produtos = temp_ptr;
            produtos[total_produtos] = temp_prod;
            total_produtos++;
        } else {
            fprintf(stderr, "Aviso: Linha mal formatada no arquivo %s: %s\n", ARQUIVO_PRODUTOS, line);
        }
    }
    fclose(arquivo);
    printf("%d produtos carregados.\n", total_produtos);
}

void menu_cadastro_produtos() {
    int opcao;
    do {
        system("cls");
        printf("--- MENU DE PRODUTOS ---\n");
        printf("1. Cadastrar Novo Produto\n");
        printf("2. Listar Produtos\n");
        printf("3. Alterar Estoque de Produto\n"); // Nova opção
        printf("4. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida! Digite um numero.\n");
            limpaBuffer();
            Sleep(1000);
            continue;
        }
        limpaBuffer();

        switch (opcao) {
            case 1:
                cadastrar_produto();
                break;
            case 2:
                listar_produtos();
                break;
            case 3:
                alterar_estoque_produto(); // Chama a nova função
                break;
            case 4:
                printf("Voltando ao Menu Principal...\n");
                Sleep(1000);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                Sleep(1000);
                break;
        }
    } while (opcao != 4); // Ajustado para a nova opção
}
// Menu de alto nível para gestão de produtos e categorias
void menu_principal_produtos_cadastro() {
    
    // Este menu coordena as chamadas para menu_cadastro_categorias e menu_cadastro_produtos
    // Implementação básica já sugerida anteriormente
    int opcao;
    do {
        system("cls");
        printf("MENU CADASTRO DE PRODUTOS E CATEGORIAS\\n");
        printf("1. Gerenciar Categorias\\n");
        printf("2. Gerenciar Produtos\\n");
        printf("3. Retornar ao Menu Principal\\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\\n");
            Sleep(1000);
            continue;
        }
        limpaBuffer();
        switch (opcao) {
            case 1:
                menu_cadastro_categorias();
                break;
            case 2:
                menu_cadastro_produtos();
                break;
            case 3:
                printf("Retornando ao menu principal...\\n");
                Sleep(1000);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\\n");
                Sleep(1000);
                break;
        }
    } while (opcao != 3);
}