#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Analise lexica

typedef enum {
    TK_NUMERO,
    TK_IDENT,
    TK_SOMA,
    TK_SUB,
    TK_MULT,
    TK_DIV,
    TK_IGUAL,
    TK_PONTO_VIRGULA,
    TK_PAREN_ESQ,
    TK_PAREN_DIR,
    TK_EOF,
    TK_ERRO
} TipoToken;

typedef struct {
    TipoToken tipo;
    char *lexema;
    int posicao;
} Token;

static const char *nome_tipo(TipoToken t) {
    switch (t) {
        case TK_NUMERO:         return "Numero";
        case TK_IDENT:          return "Identificador";
        case TK_SOMA:           return "Soma";
        case TK_SUB:            return "Sub";
        case TK_MULT:           return "Mult";
        case TK_DIV:            return "Div";
        case TK_IGUAL:          return "Igual";
        case TK_PONTO_VIRGULA:  return "PontoVirgula";
        case TK_PAREN_ESQ:      return "ParenEsq";
        case TK_PAREN_DIR:      return "ParenDir";
        case TK_EOF:            return "EOF";
        default:                return "ErroLexico";
    }
}

static char *copia_lexema(const char *src, int inicio, int fim) {
    int n = fim - inicio;
    char *s = malloc(n + 1);

    if (!s) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    memcpy(s, src + inicio, n);
    s[n] = '\0';

    return s;
}

/*
   Produz a sequencia de tokens do programa de entrada.
   Identificadores comecam com letra e podem conter letras
   e digitos depois dela.
*/

static Token *analisa_lexico(const char *src, int *out_n) {
    int cap = 16;
    int n = 0;
    Token *toks = malloc(cap * sizeof(Token));
    int i = 0;

    if (!toks) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    while (src[i] != '\0') {
        char c = src[i];

        if (isspace((unsigned char)c)) {
            i++;
            continue;
        }

        if (n >= cap) {
            cap *= 2;
            Token *novo = realloc(toks, cap * sizeof(Token));

            if (!novo) {
                fprintf(stderr, "Erro de memoria.\n");
                exit(1);
            }

            toks = novo;
        }

        int pos = i;

        if (isalpha((unsigned char)c)) {
            int j = i + 1;

            while (isalnum((unsigned char)src[j])) {
                j++;
            }

            toks[n].tipo = TK_IDENT;
            toks[n].lexema = copia_lexema(src, i, j);
            toks[n].posicao = pos;

            n++;
            i = j;
            continue;
        }

        if (isdigit((unsigned char)c)) {
            int j = i;

            while (isdigit((unsigned char)src[j])) {
                j++;
            }

            if (isalpha((unsigned char)src[j])) {
                while (isalnum((unsigned char)src[j])) {
                    j++;
                }

                toks[n].tipo = TK_ERRO;
                toks[n].lexema = copia_lexema(src, i, j);
                toks[n].posicao = pos;

                n++;
                *out_n = n;
                return toks;
            }

            toks[n].tipo = TK_NUMERO;
            toks[n].lexema = copia_lexema(src, i, j);
            toks[n].posicao = pos;

            n++;
            i = j;
            continue;
        }

        TipoToken tipo;

        switch (c) {
            case '+': tipo = TK_SOMA; break;
            case '-': tipo = TK_SUB; break;
            case '*': tipo = TK_MULT; break;
            case '/': tipo = TK_DIV; break;
            case '=': tipo = TK_IGUAL; break;
            case ';': tipo = TK_PONTO_VIRGULA; break;
            case '(': tipo = TK_PAREN_ESQ; break;
            case ')': tipo = TK_PAREN_DIR; break;

            default:
                toks[n].tipo = TK_ERRO;
                toks[n].lexema = copia_lexema(src, i, i + 1);
                toks[n].posicao = pos;

                n++;
                *out_n = n;
                return toks;
        }

        toks[n].tipo = tipo;
        toks[n].lexema = copia_lexema(src, i, i + 1);
        toks[n].posicao = pos;

        n++;
        i++;
    }

    if (n >= cap) {
        cap++;
        Token *novo = realloc(toks, cap * sizeof(Token));

        if (!novo) {
            fprintf(stderr, "Erro de memoria.\n");
            exit(1);
        }

        toks = novo;
    }

    toks[n].tipo = TK_EOF;
    toks[n].lexema = copia_lexema(src, i, i);
    toks[n].posicao = i;

    n++;

    *out_n = n;
    return toks;
}

// Arvore de sintaxe abstrata

typedef enum {
    ND_CONST,
    ND_VAR,
    ND_OPBIN
} TipoExp;

typedef struct Exp {
    TipoExp tipo;

    long valor;
    char op;
    char *nome;

    struct Exp *esq;
    struct Exp *dir;
} Exp;

static Exp *novo_const(long valor) {
    Exp *e = calloc(1, sizeof(Exp));

    if (!e) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    e->tipo = ND_CONST;
    e->valor = valor;

    return e;
}

static Exp *novo_var(const char *nome) {
    Exp *e = calloc(1, sizeof(Exp));

    if (!e) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    e->tipo = ND_VAR;
    e->nome = copia_lexema(nome, 0, (int)strlen(nome));

    return e;
}

static Exp *novo_opbin(char op, Exp *esq, Exp *dir) {
    Exp *e = calloc(1, sizeof(Exp));

    if (!e) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    e->tipo = ND_OPBIN;
    e->op = op;
    e->esq = esq;
    e->dir = dir;

    return e;
}

static void libera_exp(Exp *e) {
    if (!e) return;

    libera_exp(e->esq);
    libera_exp(e->dir);

    free(e->nome);
    free(e);
}

// Estrutura do programa e declaracoes

typedef struct Declaracao {
    char *nome;
    Exp *expressao;
    struct Declaracao *prox;
} Declaracao;

typedef struct {
    Declaracao *inicio;
    Declaracao *fim;
    Exp *resultado;
} Programa;


// Analise sintatica(Descendente Recursiva)

static Token *toks_global;
static int idx_global;
static int erro_sintatico;
static char erro_msg[256];

static Token *avanca(void) {
    Token *t = &toks_global[idx_global];

    if (t->tipo != TK_EOF) {
        idx_global++;
    }

    return t;
}

static Token *token_atual(void) {
    return &toks_global[idx_global];
}

static void reporta_erro_sintatico(Token *tok, const char *esperado) {
    if (erro_sintatico) return;

    erro_sintatico = 1;

    snprintf(
        erro_msg,
        sizeof(erro_msg),
        "Erro sintatico na posicao %d: esperado %s, encontrado '%s'",
        tok->posicao,
        esperado,
        tok->lexema
    );
}

static int aceita(TipoToken esperado) {
    if (token_atual()->tipo == esperado) {
        avanca();
        return 1;
    }

    return 0;
}

static void verifica_prox_token(TipoToken esperado, const char *desc) {
    if (erro_sintatico) return;

    Token *tok = avanca();

    if (tok->tipo != esperado) {
        reporta_erro_sintatico(tok, desc);
    }
}

/*
   Gramatica das expressoes:

   exp   ::= termo (('+' | '-') termo)*
   termo ::= primario (('*' | '/') primario)*
   primario ::= numero | identificador | '(' exp ')'
*/

static Exp *analisa_exp(void);
static Exp *analisa_termo(void);
static Exp *analisa_primario(void);

static Exp *analisa_primario(void) {
    if (erro_sintatico) return NULL;

    Token *tok = avanca();

    if (tok->tipo == TK_NUMERO) {
        return novo_const(atol(tok->lexema));
    }

    if (tok->tipo == TK_IDENT) {
        return novo_var(tok->lexema);
    }

    if (tok->tipo == TK_PAREN_ESQ) {
        Exp *e = analisa_exp();

        verifica_prox_token(TK_PAREN_DIR, "')'");

        if (erro_sintatico) {
            libera_exp(e);
            return NULL;
        }

        return e;
    }

    reporta_erro_sintatico(
        tok,
        "um numero, identificador ou '('"
    );

    return NULL;
}

static Exp *analisa_termo(void) {
    Exp *esq = analisa_primario();

    if (erro_sintatico || !esq) {
        return NULL;
    }

    while (
        token_atual()->tipo == TK_MULT ||
        token_atual()->tipo == TK_DIV
    ) {
        Token *tok = avanca();

        char op = tok->tipo == TK_MULT ? '*' : '/';

        Exp *dir = analisa_primario();

        if (erro_sintatico || !dir) {
            libera_exp(esq);
            return NULL;
        }

        esq = novo_opbin(op, esq, dir);
    }

    return esq;
}

static Exp *analisa_exp(void) {
    Exp *esq = analisa_termo();

    if (erro_sintatico || !esq) {
        return NULL;
    }

    while (
        token_atual()->tipo == TK_SOMA ||
        token_atual()->tipo == TK_SUB
    ) {
        Token *tok = avanca();

        char op = tok->tipo == TK_SOMA ? '+' : '-';

        Exp *dir = analisa_termo();

        if (erro_sintatico || !dir) {
            libera_exp(esq);
            return NULL;
        }

        esq = novo_opbin(op, esq, dir);
    }

    return esq;
}


// Analise do programa
/*
   programa ::= declaracao* '=' exp
   declaracao ::= identificador '=' exp ';'
*/

static Programa *analisa_programa(void) {
    Programa *p = calloc(1, sizeof(Programa));

    if (!p) {
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    while (token_atual()->tipo == TK_IDENT) {
        Token *ident = avanca();

        verifica_prox_token(TK_IGUAL, "'='");

        if (erro_sintatico) break;

        Exp *e = analisa_exp();

        if (erro_sintatico || !e) break;

        verifica_prox_token(TK_PONTO_VIRGULA, "';'");

        if (erro_sintatico) {
            libera_exp(e);
            break;
        }

        Declaracao *d = calloc(1, sizeof(Declaracao));

        if (!d) {
            fprintf(stderr, "Erro de memoria.\n");
            exit(1);
        }

        d->nome = copia_lexema(
            ident->lexema,
            0,
            (int)strlen(ident->lexema)
        );

        d->expressao = e;

        if (p->fim) {
            p->fim->prox = d;
        } else {
            p->inicio = d;
        }

        p->fim = d;
    }

    if (!erro_sintatico) {
        verifica_prox_token(
            TK_IGUAL,
            "'=' antes da expressao final"
        );
    }

    if (!erro_sintatico) {
        p->resultado = analisa_exp();
    }

    if (!erro_sintatico) {
        verifica_prox_token(TK_EOF, "o final da entrada");
    }

    if (erro_sintatico || !p->resultado) {
        Declaracao *d = p->inicio;

        while (d) {
            Declaracao *prox = d->prox;
            free(d->nome);
            libera_exp(d->expressao);
            free(d);
            d = prox;
        }

        libera_exp(p->resultado);
        free(p);

        return NULL;
    }

    return p;
}


// Impressao da arvore

static void imprime_arvore(Exp *e) {
    if (!e) return;

    if (e->tipo == ND_CONST) {
        printf("%ld", e->valor);
    }
    else if (e->tipo == ND_VAR) {
        printf("%s", e->nome);
    }
    else {
        printf("(");
        imprime_arvore(e->esq);
        printf(" %c ", e->op);
        imprime_arvore(e->dir);
        printf(")");
    }
}

static void imprime_programa(Programa *p) {
    for (Declaracao *d = p->inicio; d; d = d->prox) {
        printf("%s = ", d->nome);
        imprime_arvore(d->expressao);
        printf(";\n");
    }

    printf("= ");
    imprime_arvore(p->resultado);
    printf("\n");
}

// Analise semantica: tabela de simbolos

typedef struct {
    char **nomes;
    int quantidade;
    int capacidade;
} TabelaSimbolos;

static int busca_simbolo(TabelaSimbolos *t, const char *nome) {
    for (int i = 0; i < t->quantidade; i++) {
        if (strcmp(t->nomes[i], nome) == 0) {
            return 1;
        }
    }

    return 0;
}

static void adiciona_simbolo(TabelaSimbolos *t, const char *nome) {
    if (t->quantidade >= t->capacidade) {
        int nova_capacidade = t->capacidade == 0
            ? 8
            : t->capacidade * 2;

        char **novo = realloc(
            t->nomes,
            nova_capacidade * sizeof(char *)
        );

        if (!novo) {
            fprintf(stderr, "Erro de memoria.\n");
            exit(1);
        }

        t->nomes = novo;
        t->capacidade = nova_capacidade;
    }

    t->nomes[t->quantidade] = copia_lexema(
        nome,
        0,
        (int)strlen(nome)
    );

    t->quantidade++;
}

static void libera_tabela(TabelaSimbolos *t) {
    for (int i = 0; i < t->quantidade; i++) {
        free(t->nomes[i]);
    }

    free(t->nomes);
}

static int verifica_variaveis(Exp *e, TabelaSimbolos *t) {
    if (!e) return 1;

    if (e->tipo == ND_CONST) {
        return 1;
    }

    if (e->tipo == ND_VAR) {
        if (!busca_simbolo(t, e->nome)) {
            fprintf(
                stderr,
                "Erro semantico: variavel '%s' utilizada antes de ser declarada.\n",
                e->nome
            );

            return 0;
        }

        return 1;
    }

    return verifica_variaveis(e->esq, t) &&
           verifica_variaveis(e->dir, t);
}

static int analisa_semantica(Programa *p, TabelaSimbolos *t) {
    for (Declaracao *d = p->inicio; d; d = d->prox) {

        if (busca_simbolo(t, d->nome)) {
            fprintf(
                stderr,
                "Erro semantico: variavel '%s' declarada mais de uma vez.\n",
                d->nome
            );

            return 0;
        }

        if (!verifica_variaveis(d->expressao, t)) {
            return 0;
        }

        adiciona_simbolo(t, d->nome);
    }

    return verifica_variaveis(p->resultado, t);
}


// geracao do codigo assembly em x86-64 LINUX

static void gera_codigo(FILE *f, Exp *e) {
    if (e->tipo == ND_CONST) {
        fprintf(f, "    movq $%ld, %%rax\n", e->valor);
        return;
    }

    if (e->tipo == ND_VAR) {
        fprintf(f, "    movq %s(%%rip), %%rax\n", e->nome);
        return;
    }

    gera_codigo(f, e->esq);

    fputs("    pushq %rax\n", f);

    gera_codigo(f, e->dir);

    fputs("    popq %rbx\n", f);

    switch (e->op) {
        case '+':
            fputs("    addq %rbx, %rax\n", f);
            break;

        case '-':
            fputs(
                "    subq %rax, %rbx\n"
                "    movq %rbx, %rax\n",
                f
            );
            break;

        case '*':
            fputs("    imulq %rbx, %rax\n", f);
            break;

        case '/':
            fputs(
                "    movq %rax, %rcx\n"
                "    movq %rbx, %rax\n"
                "    cqto\n"
                "    idivq %rcx\n",
                f
            );
            break;
    }
}

static int gera_assembly(
    const char *caminho,
    Programa *p,
    TabelaSimbolos *t
) {
    FILE *f = fopen(caminho, "w");

    if (!f) {
        fprintf(stderr, "Erro ao criar arquivo Assembly.\n");
        return 0;
    }

    fputs(".section .bss\n", f);

    for (int i = 0; i < t->quantidade; i++) {
        fprintf(f, "    .lcomm %s, 8\n", t->nomes[i]);
    }

    fputs(
        "\n.section .text\n"
        ".globl _start\n"
        "_start:\n",
        f
    );

    for (Declaracao *d = p->inicio; d; d = d->prox) {
        gera_codigo(f, d->expressao);

        fprintf(
            f,
            "    movq %%rax, %s(%%rip)\n",
            d->nome
        );
    }

    gera_codigo(f, p->resultado);

    fputs(
        "    call imprime_num\n"
        "    call sair\n\n"
        ".include \"runtime.s\"\n",
        f
    );

    return fclose(f) == 0;
}


// Leitura do arquivo de entrada

static char *le_arquivo(const char *caminho) {
    FILE *f = fopen(caminho, "r");

    if (!f) {
        fprintf(
            stderr,
            "Erro: nao foi possivel abrir o arquivo '%s'\n",
            caminho
        );

        exit(1);
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        fprintf(stderr, "Erro ao ler arquivo.\n");
        exit(1);
    }

    long tamanho = ftell(f);

    if (tamanho < 0 || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        fprintf(stderr, "Erro ao ler arquivo.\n");
        exit(1);
    }

    char *buffer = malloc((size_t)tamanho + 1);

    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Erro de memoria.\n");
        exit(1);
    }

    size_t lidos = fread(buffer, 1, (size_t)tamanho, f);

    fclose(f);

    if (lidos != (size_t)tamanho) {
        free(buffer);
        fprintf(stderr, "Erro ao ler arquivo.\n");
        exit(1);
    }

    buffer[lidos] = '\0';

    return buffer;
}


// Liberacao do programa

static void libera_programa(Programa *p) {
    if (!p) return;

    Declaracao *d = p->inicio;

    while (d) {
        Declaracao *prox = d->prox;

        free(d->nome);
        libera_exp(d->expressao);
        free(d);

        d = prox;
    }

    libera_exp(p->resultado);
    free(p);
}

static void libera_tokens(Token *toks, int n) {
    for (int i = 0; i < n; i++) {
        free(toks[i].lexema);
    }

    free(toks);
}


// main

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.ev>\n", argv[0]);
        return 1;
    }

    char *fonte = le_arquivo(argv[1]);

    int n;
    Token *toks = analisa_lexico(fonte, &n);

    free(fonte);

    printf("== Tokens ==\n");

    for (int i = 0; i < n; i++) {
        printf(
            "<%s, \"%s\", %d>\n",
            nome_tipo(toks[i].tipo),
            toks[i].lexema,
            toks[i].posicao
        );
    }

    for (int i = 0; i < n; i++) {
        if (toks[i].tipo == TK_ERRO) {
            fprintf(
                stderr,
                "\nErro lexico na posicao %d: caractere invalido '%s'\n",
                toks[i].posicao,
                toks[i].lexema
            );

            libera_tokens(toks, n);
            return 1;
        }
    }

    toks_global = toks;
    idx_global = 0;
    erro_sintatico = 0;

    Programa *programa = analisa_programa();

    printf("\n== Arvore sintatica ==\n");

    if (erro_sintatico || !programa) {
        printf("%s\n", erro_msg);

        libera_tokens(toks, n);
        return 1;
    }

    imprime_programa(programa);

    printf("\n== Analise semantica ==\n");

    TabelaSimbolos tabela = {0};

    if (!analisa_semantica(programa, &tabela)) {
        libera_tabela(&tabela);
        libera_programa(programa);
        libera_tokens(toks, n);
        return 1;
    }

    printf("Analise semantica concluida com sucesso.\n");

    size_t tamanho = strlen(argv[1]);
    size_t base = tamanho;

    if (tamanho >= 3 &&
        strcmp(argv[1] + tamanho - 3, ".ev") == 0) {
        base = tamanho - 3;
    }

    char *saida = malloc(base + 3);

    if (!saida) {
        fprintf(stderr, "Erro de memoria.\n");

        libera_tabela(&tabela);
        libera_programa(programa);
        libera_tokens(toks, n);

        return 1;
    }

    memcpy(saida, argv[1], base);
    memcpy(saida + base, ".s", 3);

    printf("\n== Geracao de codigo ==\n");

    int sucesso = gera_assembly(saida, programa, &tabela);

    if (sucesso) {
        printf("Assembly x86-64/Linux gerado: %s\n", saida);
    } else {
        fprintf(stderr, "Erro ao gerar Assembly.\n");
    }

    free(saida);

    libera_tabela(&tabela);
    libera_programa(programa);
    libera_tokens(toks, n);

    return sucesso ? 0 : 1;
}