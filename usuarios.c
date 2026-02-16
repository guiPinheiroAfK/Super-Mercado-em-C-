// usuarios.c
// Este arquivo conterá as implementações das funções de gerenciamento de usuários.

#include "utilidades.h"
#include "usuarios.h" // Inclui o cabeçalho usuarios.h (que já inclui utilidades.h)

// --- DEFINIÇÃO DAS VARIÁVEIS GLOBAIS DE USUÁRIOS ---
// As variáveis globais que foram DECLARADAS como 'extern' em usuarios.h,
// devem ser DEFINIDAS AQUI, com sua inicialização.

Usuario *usuarios_cadastrados = NULL; // Array dinâmico para armazenar os usuários
int total_usuarios = 0;             // Contador de usuários cadastrados
Usuario *usuario_logado = NULL;     // Ponteiro para o usuário atualmente logado (NULL se ninguém logado)

// --- ESQUELETOS DAS FUNÇÕES DE GERENCIAMENTO DE USUÁRIOS ---
// Você preencherá o código dentro destas chaves depois.

// Funções de Persistência
void carregar_usuarios() {
    FILE *arquivo = fopen(ARQUIVO_USUARIOS, "rb"); // Abre o arquivo em modo binário de leitura

    if (arquivo == NULL) {
        // Se o arquivo não existe, significa que não há usuários cadastrados ainda
        usuarios_cadastrados = NULL;
        total_usuarios = 0;
        printf("Nenhum usuario encontrado no arquivo. Iniciando com lista vazia.\n");
        // Sleep(1000); // Opcional: pausar para o usuário ver a mensagem
        return;
    }

    // Lê a quantidade total de usuários
    fread(&total_usuarios, sizeof(int), 1, arquivo);

    if (total_usuarios > 0) {
        // Aloca memória para todos os usuários
        usuarios_cadastrados = (Usuario *)realloc(usuarios_cadastrados, total_usuarios * sizeof(Usuario));
        if (usuarios_cadastrados == NULL) {
            printf("Erro: Falha ao alocar memoria para usuarios.\n");
            exit(1); // Encerra o programa em caso de erro crítico de memória
        }

        // Lê os dados de cada usuário
        fread(usuarios_cadastrados, sizeof(Usuario), total_usuarios, arquivo);
    } else {
        // Se total_usuarios for 0 (arquivo existe, mas está vazio ou foi salvo assim)
        if (usuarios_cadastrados != NULL) {
            free(usuarios_cadastrados);
            usuarios_cadastrados = NULL;
        }
    }

    fclose(arquivo); // Fecha o arquivo
    printf("Usuarios carregados com sucesso! Total: %d\n", total_usuarios);
    // Sleep(1000); // Opcional: pausar para o usuário ver a mensagem

}

void salvar_usuarios() {
    FILE *arquivo = fopen(ARQUIVO_USUARIOS, "wb"); // Abre o arquivo em modo binário de escrita (sobrescreve se existir)

    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo para salvar usuarios.\n");
        return;
    }

    // Escreve a quantidade total de usuários
    fwrite(&total_usuarios, sizeof(int), 1, arquivo);

    if (total_usuarios > 0) {
        // Escreve os dados de todos os usuários
        fwrite(usuarios_cadastrados, sizeof(Usuario), total_usuarios, arquivo);
    }

    fclose(arquivo); // Fecha o arquivo
    printf("Usuarios salvos com sucesso!\n");
    // Sleep(1000); // Opcional: pausar para o usuário ver a mensagem
}

// Funções de Autenticação e Login
Usuario *autenticar_usuario(const char *login_digitado, const char *senha_digitada) {
    // Primeiro, verifica se os formatos de login e senha digitados são válidos.
    // Se não forem válidos, simplesmente retorna NULL sem uma mensagem de erro específica de formato.
    // A interface do usuário (ex: menu_login) deve tratar a mensagem "Login ou senha inválidos".
    if (!validar_login_formato(login_digitado)) {
        return NULL; // Formato de login inválido
    }
    if (!validar_senha_formato(senha_digitada)) {
        return NULL; // Formato de senha inválido
    }

    // Busca o usuário pelo login digitado
    Usuario *usuario_encontrado = buscar_usuario_por_login(login_digitado);

    if (usuario_encontrado != NULL) {
        // Se o usuário foi encontrado, compara a senha digitada com a senha armazenada
        if (strcmp(usuario_encontrado->senha, senha_digitada) == 0) {
            // Verifica se o usuário está ativo antes de autenticar.
            // Se o campo 'ativo' existe e você quer usá-lo para desativar contas.
            if (usuario_encontrado->ativo == 0) { // Supondo 0 para inativo
                printf("Conta inativa. Por favor, contate o administrador.\n");
                return NULL;
            }
            return usuario_encontrado; // Login e senha corretos, e usuário ativo
        } else {
            // Não dê dicas se é login ou senha errada.
            // A mensagem "Login ou senha invalidos" deve ser exibida no menu_login.
            // printf("Senha incorreta.\n"); // <-- REMOVA ESTA LINHA
            return NULL; // Senha incorreta
        }
    } else {
        // Não dê dicas se o login não foi encontrado.
        // A mensagem "Login ou senha invalidos" deve ser exibida no menu_login.
        // printf("Login nao encontrado.\n"); // <-- REMOVA ESTA LINHA
        return NULL; // Login não encontrado
    }
}

int menu_login() {
    int opcao;
    char login_digitado[13];
    char senha_digitada[9];

    // Loop que continua até que um usuário logue com sucesso ou escolha sair do programa
    while (usuario_logado == NULL) {
        system("cls");
        printf("======================================\n");
        printf("    BEM-VINDO AO SISTEMA DONA BERE    \n");
        printf("======================================\n");
        printf("1. Login                              \n");
        printf("2. Cadastrar Novo Usuario             \n");
        printf("3. Sair do Programa                   \n");
        printf("--------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida! Digite um numero.\n");
            Sleep(1500);
            continue; // Volta para o início do loop (sem retornar)
        }
        limpaBuffer(); // Limpa o buffer de entrada

        switch (opcao) {
            case 1:
                system("cls");
                printf("--- TELA DE LOGIN ---\n");
                printf("Login: ");
                fgets(login_digitado, sizeof(login_digitado), stdin);
                login_digitado[strcspn(login_digitado, "\n")] = 0;

                printf("Senha: ");
                fgets(senha_digitada, sizeof(senha_digitada), stdin);
                senha_digitada[strcspn(senha_digitada, "\n")] = 0;

                usuario_logado = autenticar_usuario(login_digitado, senha_digitada);

                if (usuario_logado != NULL) {
                    printf("\nLogin bem-sucedido! Bem-vindo, %s!\n", usuario_logado->login);
                    Sleep(2000);
                    return 1; // Logou com sucesso, retorna 1 para o main.c
                } else {
                    printf("\nFalha no login. Verifique seu login e senha.\n");
                    Sleep(2000);
                    // O loop 'while' continuará, não precisamos de 'break' aqui para ir para o próximo 'case'.
                    // Ele simplesmente vai mostrar o menu_login novamente.
                }
                break; // Adicionado break para sair do switch e voltar ao while.

            case 2:
                // Chama a função de cadastro de usuário.
                // Esta função já contém a lógica para exigir permissão de admin
                // caso se tente cadastrar um novo administrador.
                cadastrar_usuario();
                // Após o cadastro, o loop 'while' continuará no menu_login
                // até que um usuário seja autenticado ou a opção de sair seja escolhida.
                break;

            case 3:
                printf("Saindo do programa. Ate mais!\n");
                Sleep(1500);
                return 0; // Usuário quer sair, retorna 0 para o main.c
            
            default:
                printf("Opcao invalida. Tente novamente.\n");
                Sleep(1500);
                // O loop 'while' continuará, não precisamos de 'break' aqui para ir para o próximo 'case'.
                // Ele simplesmente vai mostrar o menu_login novamente.
                break; // Adicionado break para sair do switch e voltar ao while.
        }
    }
    // Este ponto só será alcançado se 'usuario_logado' for diferente de NULL
    // (ou seja, o login foi bem-sucedido e a função já retornou 1 antes)
    // ou se o loop for de alguma forma quebrado de forma inesperada.
    // Para garantir, podemos retornar um valor padrão aqui.
    return 1; // Se o loop foi quebrado porque usuario_logado não é NULL, significa sucesso.
}
void logout_usuario() {
    if (usuario_logado != NULL) {
        printf("Realizando logout do usuario '%s'...", usuario_logado->login);
        usuario_logado = NULL; // Limpa o ponteiro para o usuário logado
        Sleep(1500);
        printf("\nLogout realizado com sucesso!\n");
    } else {
        printf("Nenhum usuario logado no momento.\n");
    }
    Sleep(1000);
}

// Funções de Cadastro de Usuário
void cadastrar_usuario() {
    system("cls");
    printf("CADASTRO DE NOVO USUARIO\n");
    printf("--------------------------\n");

    char novo_login[13];
    char nova_senha[9];
    int tipo_usuario_int;
    Usuario *usuario_existente;

    // Solicita e valida o login
    do {
        printf("Digite o login (8 a 12 caracteres): ");
        fgets(novo_login, sizeof(novo_login), stdin);
        novo_login[strcspn(novo_login, "\n")] = 0;

        usuario_existente = buscar_usuario_por_login(novo_login);

        if (!validar_login_formato(novo_login)) {
            printf("Login invalido! Deve ter entre 8 e 12 caracteres.\n");
        } else if (usuario_existente != NULL) {
            printf("Erro: Login ja existe. Por favor, escolha outro.\n");
        }
    } while (!validar_login_formato(novo_login) || usuario_existente != NULL);

    // Solicita e valida a senha
    do {
        printf("Digite a senha (6 a 8 caracteres): ");
        fgets(nova_senha, sizeof(nova_senha), stdin);
        nova_senha[strcspn(nova_senha, "\n")] = 0;

        if (!validar_senha_formato(nova_senha)) {
            printf("Senha invalida! Deve ter entre 6 e 8 caracteres.\n");
        }
    } while (!validar_senha_formato(nova_senha));

    // Solicita e valida o tipo de usuário
    do {
        printf("Tipo de Usuario (1-Administrador, 2-Usuario Comum): ");
        if (scanf("%d", &tipo_usuario_int) != 1 || (tipo_usuario_int != 1 && tipo_usuario_int != 2)) {
            printf("Opcao de tipo invalida! Digite 1 ou 2.\n");
            limpaBuffer();
        } else {
            limpaBuffer();

            // AQUI ESTÁ O PONTO CRÍTICO:
            // A LÓGICA DE 'if (total_usuarios == 0)' FOI REMOVIDA DAQUI.
            // Ela agora só existe na checar_first_admin().
            // Se esta parte está sendo executada, significa que total_usuarios > 0
            // ou que estamos em um fluxo errado.

            // Lógica de permissão para cadastrar ADMINISTRADOR (agora SÓ se já há usuários)
            // Verificar se o usuário logado NÃO é ADM e está tentando cadastrar um ADM
            if (tipo_usuario_int == ADM) { // Se o tipo escolhido é Administrador
                // E o usuário logado NÃO TIVER permissão de administrador (ou seja, não é ADM ou é NULL)
                if (usuario_logado == NULL || usuario_logado->tipo_usuario != ADM) {
                     printf("Permissao negada: Apenas administradores podem cadastrar outros administradores.\n");
                     printf("Por favor, digite o login e senha de um administrador para continuar.\n");
                     char admin_login[13];
                     char admin_senha[9];

                     printf("Login Admin: ");
                     fgets(admin_login, sizeof(admin_login), stdin);
                     admin_login[strcspn(admin_login, "\n")] = 0;

                     printf("Senha Admin: ");
                     fgets(admin_senha, sizeof(admin_senha), stdin);
                     admin_senha[strcspn(admin_senha, "\n")] = 0;

                     Usuario *admin_temp = autenticar_usuario(admin_login, admin_senha);
                     if (admin_temp != NULL && admin_temp->tipo_usuario == ADM) {
                         printf("Permissao de administrador concedida temporariamente.\n");
                         Sleep(1500);
                         break; // Sai do do-while de tipo_usuario
                     } else {
                         printf("Autenticacao de administrador falhou. Nao e possivel cadastrar administrador.\n");
                         Sleep(2000);
                         return; // Aborta o cadastro se a autenticação de admin falhar
                     }
                } else { // Se o usuário logado JÁ É ADM
                    break; // Pode cadastrar outro ADM
                }
            } else { // Se o tipo escolhido é COMUM
                break; // Pode cadastrar usuário comum sem checagem extra de admin
            }
        }
    } while (1); // Loop infinito, sai com break

    // Aloca ou realoca memória para o novo usuário
    usuarios_cadastrados = (Usuario *)realloc(usuarios_cadastrados, (total_usuarios + 1) * sizeof(Usuario));
    if (usuarios_cadastrados == NULL) {
        printf("Erro: Falha ao alocar memoria para novo usuario.\n");
        exit(1);
    }

    // Copia os dados para o novo usuário
    strcpy(usuarios_cadastrados[total_usuarios].login, novo_login);
    strcpy(usuarios_cadastrados[total_usuarios].senha, nova_senha);
    usuarios_cadastrados[total_usuarios].tipo_usuario = (TipoUsuario)(tipo_usuario_int == 1 ? ADM : COMUM);
    usuarios_cadastrados[total_usuarios].ativo = 1;

    total_usuarios++;

    salvar_usuarios();
    printf("\nUsuario cadastrado com sucesso!\n");
    Sleep(1500);
}

// Funções de Validação de Input para Usuário/Senha
int validar_login_formato(const char *login) {
    size_t len = strlen(login); // size_t é o tipo de retorno de strlen

    if (len >= 8 && len <= 12) {
        return 1; // Válido
    } else {
        return 0; // Inválido
    }
}

int validar_senha_formato(const char *senha) {
    size_t len = strlen(senha); // size_t é o tipo de retorno de strlen

    if (len >= 6 && len <= 8) {
        return 1; // Válida
    } else {
        return 0; // Inválida
    }
}

// Funções de Permissão e Admin
int verificar_permissao_admin() {
    // Se já houver um usuário logado E ele for um administrador, a permissão é concedida.
    if (usuario_logado != NULL && usuario_logado->ativo == 1) {
        printf("Permissao de administrador concedida (usuario atual e administrador).\n");
        return 1; // Permissão concedida
    }

    // Se não houver usuário logado ou o usuário logado não for administrador,
    // solicita um login e senha de administrador para validar a permissão pontualmente.
    system("cls");
    printf("ACESSO RESTRITO: E NECESSARIO PERMISSAO DE ADMINISTRADOR\n");
    printf("-------------------------------------------------------\n");
    printf("Para prosseguir, por favor, autentique-se como administrador.\n");

    char temp_login[13];
    char temp_senha[9];
    Usuario *admin_temporario = NULL;
    int tentativas = 0;
    const int MAX_TENTATIVAS = 3;

    do {
        printf("Login de Administrador: ");
        limpaBuffer();
        fgets(temp_login, sizeof(temp_login), stdin);
        temp_login[strcspn(temp_login, "\n")] = 0;

        printf("Senha de Administrador: ");
        limpaBuffer();
        fgets(temp_senha, sizeof(temp_senha), stdin);
        temp_senha[strcspn(temp_senha, "\n")] = 0;

        admin_temporario = autenticar_usuario(temp_login, temp_senha);

        if (admin_temporario != NULL && admin_temporario->ativo == 1) {
            printf("\\nAutenticacao de administrador bem-sucedida!\n");
            Sleep(1500);
            return 1; // Permissão concedida temporariamente
        } else {
            tentativas++;
            printf("Login ou senha de administrador incorretos. Tentativas restantes: %d\n", MAX_TENTATIVAS - tentativas);
            Sleep(1500);
            system("cls");
            if (tentativas < MAX_TENTATIVAS) {
                 printf("ACESSO RESTRITO: E NECESSARIO PERMISSAO DE ADMINISTRADOR\n");
                 printf("-------------------------------------------------------\n");
            }
        }
    } while (tentativas < MAX_TENTATIVAS);

    printf("\nAutenticacao de administrador falhou apos %d tentativas. Permissao negada.\n", MAX_TENTATIVAS);
    Sleep(2000);
    return 0; // Permissão negada
}

// Em usuarios.c

void checar_primeiro_admin() { 
    carregar_usuarios(); // Tenta carregar usuários existentes

    if (total_usuarios == 0) {
        system("cls");
        printf("========================================================\n");
        printf("  PRIMEIRA EXECUCAO DO SISTEMA: CADASTRO DE ADMINISTRADOR\n");
        printf("========================================================\n");
        printf("Para garantir a seguranca, e necessario cadastrar o\n");
        printf("primeiro usuario administrador do sistema.\n");
        printf("Este usuario tera acesso total.\n\n");
        Sleep(3000);

        char admin_login[13];
        char admin_senha[9];
        Usuario *usuario_existente; // Para verificar se o login já existe

        // --- CADASTRANDO O PRIMEIRO ADMINISTRADOR DIRETAMENTE AQUI ---

        // Loop para solicitar e validar o LOGIN do primeiro administrador
        do {
            printf("Digite o LOGIN para o primeiro ADMINISTRADOR (8 a 12 caracteres): ");
            fgets(admin_login, sizeof(admin_login), stdin);
            admin_login[strcspn(admin_login, "\n")] = 0; // Remove a quebra de linha

            usuario_existente = buscar_usuario_por_login(admin_login); // Verifica se o login já existe

            if (!validar_login_formato(admin_login)) { // Supondo que validar_login_formato está em utilidades.c
                printf("Login invalido! Deve ter entre 8 e 12 caracteres.\n");
            } else if (usuario_existente != NULL) {
                printf("Erro: Login ja existe. Por favor, escolha outro.\n");
            }
        } while (!validar_login_formato(admin_login) || usuario_existente != NULL);

        // Loop para solicitar e validar a SENHA do primeiro administrador
        do {
            printf("Digite a SENHA para o primeiro ADMINISTRADOR (6 a 8 caracteres): ");
            fgets(admin_senha, sizeof(admin_senha), stdin);
            admin_senha[strcspn(admin_senha, "\n")] = 0; // Remove a quebra de linha

            if (!validar_senha_formato(admin_senha)) { // Supondo que validar_senha_formato está em utilidades.c
                printf("Senha invalida! Deve ter entre 6 e 8 caracteres.\n");
            }
        } while (!validar_senha_formato(admin_senha));

        // Aloca memória para o primeiro usuário (apenas 1 novo slot)
        usuarios_cadastrados = (Usuario *)realloc(usuarios_cadastrados, (total_usuarios + 1) * sizeof(Usuario));
        if (usuarios_cadastrados == NULL) {
            printf("Erro: Falha ao alocar memoria para o primeiro administrador.\n");
            exit(1);
        }

        // Preenche os dados do primeiro administrador
        strcpy(usuarios_cadastrados[total_usuarios].login, admin_login);
        strcpy(usuarios_cadastrados[total_usuarios].senha, admin_senha);
        usuarios_cadastrados[total_usuarios].tipo_usuario = ADM; // <-- FORÇA PARA ADM
        usuarios_cadastrados[total_usuarios].ativo = 1;

        total_usuarios++; // Incrementa o contador de usuários

        salvar_usuarios(); // Salva o novo administrador no arquivo

        printf("\nAdministrador principal cadastrado com sucesso!\n");
        printf("Por favor, realize o login para acessar o sistema.\n");
        Sleep(2000);

    } else {
        printf("Usuarios existentes carregados. Total: %d\n", total_usuarios);
        Sleep(1500);
    }
}

// Funções auxiliares
Usuario *buscar_usuario_por_login(const char *login) {
    if (total_usuarios == 0 || usuarios_cadastrados == NULL) {
        return NULL; // Não há usuários cadastrados
    }

    for (int i = 0; i < total_usuarios; i++) {
        // Compara o login ignorando maiúsculas/minúsculas para flexibilidade,
        // mas você pode usar strcmp se quiser que seja case-sensitive.
        // Para este exemplo, usaremos strcmp para uma comparação exata.
        if (strcmp(usuarios_cadastrados[i].login, login) == 0) {
            return &usuarios_cadastrados[i]; // Retorna um ponteiro para o usuário encontrado
        }
    }

    return NULL; // Se o usuário não for encontrado, retorna NULL
}