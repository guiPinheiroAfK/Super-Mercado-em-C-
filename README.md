# 🛒 Sistema de Gestão de Vendas e PDV (C)

Este é um sistema de Ponto de Venda (PDV) e gestão empresarial desenvolvido em **C**. O projeto foi estruturado de forma modular para facilitar a manutenção e organização das funcionalidades, que incluem desde o cadastro de produtos até o fechamento de caixa e geração de relatórios financeiros.

## 🚀 Funcionalidades

O sistema é dividido em diversos módulos que cobrem as necessidades básicas de um comércio:

* **Gestão de Usuários:** Sistema de login com diferentes níveis de acesso (ADM/Usuário), controle de status (ativo/inativo) e persistência de dados.
* **Controle de Estoque:** Cadastro, edição e exclusão de categorias e produtos, com armazenamento em arquivos.
* **Gestão de Clientes:** Cadastro completo de clientes com validação de CPF e busca por ID.
* **Módulo de Vendas:** Interface de carrinho de compras, processamento de pagamentos (Dinheiro/Cartão) e baixa automática no estoque.
* **Financeiro (Caixa):** Controle de abertura/fechamento de caixa, sangrias (retiradas) e depósitos manuais.
* **Relatórios:** Visualização de movimentação diária, faturamento por período e listagem de produtos mais vendidos.

## 📂 Estrutura do Projeto

O projeto utiliza uma arquitetura modularizada:

* `main.c`: Ponto de entrada do programa e fluxo principal dos menus.
* `usuarios.c`: Lógica de autenticação e gerenciamento de perfis.
* `cadastro.c`: Implementação das funções de CRUD para produtos e categorias.
* `venPag.c`: Processamento de vendas e lógica do caixa financeiro.
* `relat.c`: Geração de estatísticas e listagens de dados.
* `utilidades.c`: Funções auxiliares (limpeza de buffer, validações de strings e CPF).
* `idcliente.c`: Gerenciamento e persistência de IDs incrementais para clientes.
* `*.h`: (Headers) Contêm as definições de structs e protótipos das funções.

## 🛠️ Como Executar

### Pré-requisitos
* Compilador C (GCC, MinGW, Clang, etc.)
* Ambiente Windows (devido ao uso de bibliotecas como `windows.h` para funções de `Sleep` e `system("cls")`).

### Compilação
No terminal, dentro da pasta do projeto, execute:
```bash
gcc main.c usuarios.c cadastro.c venPag.c relat.c utilidades.c idcliente.c -o sistema_gestao.exe
```
### Execuçãp
./sistema_gestao.exe
