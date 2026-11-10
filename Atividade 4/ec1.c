#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Analise lexica

typedef enum {
    TK_NUMERO,
    TK_SOMA,
    TK_SUB,
    TK_MULT,
    TK_DIV,
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
        case TK_NUMERO:    return "Numero";
        case TK_SOMA:      return "Soma";
        case TK_SUB:       return "Sub";
        case TK_MULT:      return "Mult";
        case TK_DIV:       return "Div";
        case TK_PAREN_ESQ: return "ParenEsq";
        case TK_PAREN_DIR: return "ParenDir";
        case TK_EOF:       return "EOF";
        default:           return "ErroLexico";
    }
}

static char *copia_lexema(const char *src, int inicio, int fim) {
    int n = fim - inicio;
    char *s = malloc(n + 1);
    memcpy(s, src + inicio, n);
    s[n] = '\0';
    return s;
}

// Produz a sequencia de tokens do programa de entrada. Se um caractere
// invalido for encontrado, a analise para e o ultimo token da lista e
// um token do tipo TK_ERRO com o caractere invalido como lexema.
static Token *analisa_lexico(const char *src, int *out_n) {
    int cap = 16, n = 0;
    Token *toks = malloc(cap * sizeof(Token));
    int i = 0;

    while (src[i] != '\0') {
        char c = src[i];

        if (isspace((unsigned char) c)) {
            i++;
            continue;
        }

        if (n >= cap) {
            cap *= 2;
            toks = realloc(toks, cap * sizeof(Token));
        }

        int pos = i;

        if (isdigit((unsigned char) c)) {
            int j = i;
            while (isdigit((unsigned char) src[j])) j++;
            toks[n].tipo = TK_NUMERO;
            toks[n].lexema = copia_lexema(src, i, j);
            toks[n].posicao = pos;
            n++;
            i = j;
            continue;
        }

        TipoToken tipo;
        switch (c) {
            case '+': tipo = TK_SOMA;      break;
            case '-': tipo = TK_SUB;       break;
            case '*': tipo = TK_MULT;      break;
            case '/': tipo = TK_DIV;       break;
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
        cap += 1;
        toks = realloc(toks, cap * sizeof(Token));
    }
    toks[n].tipo = TK_EOF;
    toks[n].lexema = copia_lexema(src, i, i);
    toks[n].posicao = i;
    n++;

    *out_n = n;
    return toks;
}

// Arvore de sintaxe abstrata

typedef enum { ND_CONST, ND_OPBIN } TipoExp;

typedef struct Exp {
    TipoExp tipo;
    long valor;        // usado quando tipo == ND_CONST
    char op;            // usado quando tipo == ND_OPBIN ('+', '-', '*', '/')
    struct Exp *esq;
    struct Exp *dir;
} Exp;

static Exp *novo_const(long valor) {
    Exp *e = malloc(sizeof(Exp));
    e->tipo = ND_CONST;
    e->valor = valor;
    return e;
}

static Exp *novo_opbin(char op, Exp *esq, Exp *dir) {
    Exp *e = malloc(sizeof(Exp));
    e->tipo = ND_OPBIN;
    e->op = op;
    e->esq = esq;
    e->dir = dir;
    return e;
}

// Analise sintatica (descendente recursiva)

static Token *toks_global;
static int idx_global;
static int erro_sintatico;
static char erro_msg[256];

static Token *avanca(void) {
    Token *t = &toks_global[idx_global];
    if (t->tipo != TK_EOF) idx_global++;
    return t;
}

static void reporta_erro_sintatico(Token *tok, const char *esperado) {
    if (erro_sintatico) return; // mantem a primeira mensagem de erro
    erro_sintatico = 1;
    snprintf(erro_msg, sizeof(erro_msg),
             "Erro sintatico na posicao %d: esperado %s, encontrado '%s'",
             tok->posicao, esperado, tok->lexema);
}

static Exp *analisa_exp(void);

static char analisa_operador(void) {
    if (erro_sintatico) return 0;
    Token *tok = avanca();
    switch (tok->tipo) {
        case TK_SOMA: return '+';
        case TK_SUB:  return '-';
        case TK_MULT: return '*';
        case TK_DIV:  return '/';
        default:
            reporta_erro_sintatico(tok, "um operador ('+', '-', '*' ou '/')");
            return 0;
    }
}

static void verifica_prox_token(TipoToken esperado, const char *desc) {
    if (erro_sintatico) return;
    Token *tok = avanca();
    if (tok->tipo != esperado) {
        reporta_erro_sintatico(tok, desc);
    }
}

static Exp *analisa_exp(void) {
    if (erro_sintatico) return NULL;

    Token *tok = avanca();

    if (tok->tipo == TK_NUMERO) {
        return novo_const(atol(tok->lexema));
    }

    if (tok->tipo == TK_PAREN_ESQ) {
        Exp *esq = analisa_exp();
        char op = analisa_operador();
        Exp *dir = analisa_exp();
        verifica_prox_token(TK_PAREN_DIR, "')'");
        if (erro_sintatico) return NULL;
        return novo_opbin(op, esq, dir);
    }

    reporta_erro_sintatico(tok, "um numero ou '('");
    return NULL;
}

// Interpretador (tree-walking)

static long interpreta(Exp *e) {
    if (e->tipo == ND_CONST) return e->valor;

    long v_esq = interpreta(e->esq);
    long v_dir = interpreta(e->dir);

    switch (e->op) {
        case '+': return v_esq + v_dir;
        case '-': return v_esq - v_dir;
        case '*': return v_esq * v_dir;
        case '/':
            if (v_dir == 0) {
                fprintf(stderr, "Erro: divisao por zero\n");
                exit(1);
            }
            return v_esq / v_dir;
    }
    return 0; // inalcancavel
}

// Impressao da arvore

static void imprime_arvore(Exp *e) {
    if (e->tipo == ND_CONST) {
        printf("%ld", e->valor);
    } else {
        printf("(");
        imprime_arvore(e->esq);
        printf(" %c ", e->op);
        imprime_arvore(e->dir);
        printf(")");
    }
}

// Leitura do arquivo de entrada

static char *le_arquivo(const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'\n", caminho);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);

    char *buffer = malloc(tamanho + 1);
    size_t lidos = fread(buffer, 1, tamanho, f);
    fclose(f);
    buffer[lidos] = '\0';

    return buffer;
}

// main

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.ec1>\n", argv[0]);
        return 1;
    }

    char *fonte = le_arquivo(argv[1]);

    int n;
    Token *toks = analisa_lexico(fonte, &n);

    printf("== Tokens ==\n");
    for (int i = 0; i < n; i++) {
        printf("<%s, \"%s\", %d>\n", nome_tipo(toks[i].tipo), toks[i].lexema, toks[i].posicao);
    }

    if (toks[n - 1].tipo == TK_ERRO) {
        printf("\nErro lexico na posicao %d: caractere invalido '%s'\n",
               toks[n - 1].posicao, toks[n - 1].lexema);
        return 1;
    }

    toks_global = toks;
    idx_global = 0;
    erro_sintatico = 0;

    Exp *arvore = analisa_exp();
    if (!erro_sintatico) {
        verifica_prox_token(TK_EOF, "o final da entrada");
    }

    printf("\n== Arvore sintatica ==\n");
    if (erro_sintatico) {
        printf("%s\n", erro_msg);
        return 1;
    }
    imprime_arvore(arvore);
    printf("\n");

    printf("\n== Resultado ==\n");
    printf("%ld\n", interpreta(arvore));

    return 0;
}
