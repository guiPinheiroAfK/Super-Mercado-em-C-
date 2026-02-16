// utilidades.c - Este arquivo "implementa" as funções declaradas em utilidades.h

#include "utilidades.h"
extern struct CADASTRO *clientes;
extern int total_clientes;



void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int validar_string_letras_espacos(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha((unsigned char)str[i]) && !isspace((unsigned char)str[i])) {
            return 0; // Caractere inválido
        }
    }
    return 1; // Válido
}

int validar_string_digitos(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0; // Caractere não numérico
        }
    }
    return 1; // Válido
}

int validar_nome(const char * const nome) {
    for (int i = 0; nome[i] != '\0'; i++) {
        if (!isalpha((unsigned char)nome[i]) && nome[i] != ' ') {
            return 0; // caractere inválido encontrado
        }
    }
    return 1; // válido
}
// Função para validar que uma string contém apenas letras e espaços
int validar_digito(const char * const telefone){
        int len = strlen(telefone);
        for (int i = 0; i < len; i++) {
            if (!isdigit((unsigned char)telefone[i])) {
                return 0; // caractere não numérico encontrado
            }
        }
        return 1; // válido
    }
// Função para validar que uma string contém apenas dígitos e 11 caracteres
int validar_telefone(const char * const telefone) {
    int len = strlen(telefone);
    if (len != 11) {
        return 0; // tamanho inválido
    }
    return validar_digito(telefone); // Verifica se todos os caracteres são dígitos
}

int validar_cpf(const char * const cpf) {
    if (strlen(cpf) != 11 ) {
        return 0; // CPF inválido se não tiver 11 dígitos
    }
    for (int i = 0; i < 11; i++) {
        if (cpf[i] < '0' || cpf[i] > '9') {
            return 0; // CPF inválido se não for numérico
        }
    }
    return 1; // CPF válido
}
//  Função para validar o CPF
void ler_cpf(struct CADASTRO *novo_cadastro) {
    do {
        printf("Digite seu CPF (11 dígitos): ");
        if (fgets(novo_cadastro->cpf_str, sizeof(novo_cadastro->cpf_str), stdin) == NULL) {
            perror("Erro ao ler o CPF");
            return;
        }
        novo_cadastro->cpf_str[strcspn(novo_cadastro->cpf_str, "\n")] = '\0'; // remove '\n'
        if (strlen(novo_cadastro->cpf_str) != 11 || !validar_cpf(novo_cadastro->cpf_str)) {
            printf("CPF inválido. Deve conter exatamente 11 dígitos numéricos.\n");
        } else {
            printf("CPF válido: %s\n", novo_cadastro->cpf_str);
            break; // CPF válido, sai do loop
        }
    } while (1);
}

