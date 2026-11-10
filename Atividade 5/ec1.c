#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Analise lexica  (mesmo analisador da Atividade 04)

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

static char *copia_lexema(const char *src, int inicio, int fim) {
    int n = fim - inicio;
    char *s = malloc(n + 1);
    memcpy(s, src + inicio, n);
    s[n] = '\0';
    return s;
}

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
    long valor;   // usado quando tipo == ND_CONST
    char op;      // usado quando tipo == ND_OPBIN ('+', '-', '*', '/')
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

// Analise sintatica (descendente recursiva, igual Atividade 04)

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
    if (erro_sintatico) return;
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

// Geracao de codigo
//
// Esquema da Atividade 05 (secao 4.1 do guia): cada operacao binaria
// 1. gera o codigo do operando esquerdo (resultado fica em X0)
// 2. empilha X0                              (str x0, [sp, #-16]!)
// 3. gera o codigo do operando direito (resultado fica em X0)
// 4. desempilha o valor do esquerdo para X1   (ldr x1, [sp], #16)
// 5. aplica a operacao, deixando o resultado em X0
//
// Para constantes, usa "mov" se o valor cabe em 16 bits (0-65535,
// conforme o guia), ou "ldr x0, =N" para constantes maiores.

static void gera_mov_constante(long valor, FILE *out) {
    if (valor >= 0 && valor <= 0xFFFF) {
        fprintf(out, "    mov x0, #%ld\n", valor);
    } else {
        fprintf(out, "    ldr x0, =%ld\n", valor);
    }
}

static void gera_codigo(Exp *e, FILE *out) {
    if (e->tipo == ND_CONST) {
        gera_mov_constante(e->valor, out);
        return;
    }

    gera_codigo(e->esq, out);
    fprintf(out, "    str x0, [sp, #-16]!\n");
    gera_codigo(e->dir, out);
    fprintf(out, "    ldr x1, [sp], #16\n");

    switch (e->op) {
        case '+': fprintf(out, "    add x0, x0, x1\n"); break;
        case '-': fprintf(out, "    sub x0, x1, x0\n"); break; // esq - dir
        case '*': fprintf(out, "    mul x0, x0, x1\n"); break;
        case '/': fprintf(out, "    sdiv x0, x1, x0\n"); break; // esq / dir
    }
}

// Modelo do arquivo assembly de saida (igual Atividade 02)

static void gera_assembly(Exp *arvore, FILE *out) {
    fprintf(out,
        "//\n"
        "// saida gerada pelo compilador EC1\n"
        "// versao arm64 + macOS\n"
        "//\n"
        ".global _main\n"
        "\n"
        ".text\n"
        ".align 2\n"
        "\n"
        "_main:\n"
        "\n");

    gera_codigo(arvore, out);

    fprintf(out,
        "\n"
        "    bl imprime_num\n"
        "    b sair\n"
        "\n"
        ".include \"runtime.s\"\n");
}

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

static char *gerar_caminho_saida(const char *caminho_entrada) {
    size_t len = strlen(caminho_entrada);
    const char *sufixo = ".ec1";
    size_t sufixo_len = strlen(sufixo);

    int tem_sufixo = (len >= sufixo_len) &&
                      (strcmp(caminho_entrada + len - sufixo_len, sufixo) == 0);

    size_t base_len = tem_sufixo ? (len - sufixo_len) : len;

    char *saida = malloc(base_len + 3); // ".s" + '\0'
    memcpy(saida, caminho_entrada, base_len);
    saida[base_len] = '.';
    saida[base_len + 1] = 's';
    saida[base_len + 2] = '\0';

    return saida;
}

// main

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.ec1>\n", argv[0]);
        return 1;
    }

    const char *caminho_entrada = argv[1];
    char *fonte = le_arquivo(caminho_entrada);

    int n;
    Token *toks = analisa_lexico(fonte, &n);

    if (toks[n - 1].tipo == TK_ERRO) {
        fprintf(stderr, "Erro lexico na posicao %d: caractere invalido '%s'\n",
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

    if (erro_sintatico) {
        fprintf(stderr, "%s\n", erro_msg);
        return 1;
    }

    char *caminho_saida = gerar_caminho_saida(caminho_entrada);
    FILE *out = fopen(caminho_saida, "w");
    if (!out) {
        fprintf(stderr, "Erro: nao foi possivel escrever '%s'\n", caminho_saida);
        return 1;
    }

    gera_assembly(arvore, out);
    fclose(out);

    printf("arquivo de entrada: %s\n", caminho_entrada);
    printf("arquivo gerado: %s\n", caminho_saida);

    return 0;
}
