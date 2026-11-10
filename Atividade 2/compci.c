#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ERRO_IO      -1
#define ERRO_SINTAXE -2

int read_and_validate(const char *caminho, char **num_out);
char *gerar_caminho_saida(const char *caminho_entrada);
int gerar_assembly(const char *numero, const char *caminho_saida);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Erro! Use: %s <arquivo.ci>\n", argv[0]);
        return 1;
    }

    const char *caminho_entrada = argv[1];

    // Verificar o arquivo de entrada lido
    printf("arquivo de entrada: %s\n", caminho_entrada);

    // próximo passo: ler o conteúdo do arquivo
    char *numero = NULL;
    int status = read_and_validate(caminho_entrada, &numero);

    if (status == ERRO_IO) {
        fprintf(stderr, "Erro: não foi possível ler o arquivo %s.\n", caminho_entrada);
        return 1;
    } else if (status == ERRO_SINTAXE) {
        fprintf(stderr, "Erro de sintaxe: o arquivo %s deve conter apenas constantes inteiras válidas.\n", caminho_entrada);
        return 1;
    }

    char *caminho_saida = gerar_caminho_saida(caminho_entrada);
    if (!caminho_saida) {
        fprintf(stderr, "erro: falha ao alocar memória\n");
        free(numero);
        return 1;
    }

    if (gerar_assembly(numero, caminho_saida) != 0) {
        fprintf(stderr, "erro: não foi possível escrever '%s'\n", caminho_saida);
        free(numero);
        free(caminho_saida);
        return 1;
    }

    printf("arquivo gerado: %s\n", caminho_saida);

    free(numero);
    free(caminho_saida);

    return 0;
}

int read_and_validate(const char *caminho, char **num_out) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        return ERRO_IO;
    }

    // Descobrir o tamanho do arquivo
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    if (tamanho < 0) {
        fclose(f);
        return ERRO_IO;
    }
    rewind(f);

    // Alocar buffer (+1 para o terminador '\0')
    char *buffer = malloc(tamanho + 1);
    if (!buffer) {
        fclose(f);
        return ERRO_IO;
    }

    // Ler o conteúdo inteiro de uma vez
    size_t lidos = fread(buffer, 1, tamanho, f);
    fclose(f);
    if (lidos != (size_t) tamanho) {
        free(buffer);
        return ERRO_IO;
    }
    buffer[tamanho] = '\0';

    // Remover espaços/quebras de linha nas pontas
    char *inicio = buffer;
    while (*inicio != '\0' && isspace((unsigned char) *inicio)) {
        inicio++;
    }
    char *fim = buffer + strlen(buffer);
    while (fim > inicio && isspace((unsigned char) *(fim - 1))) {
        fim--;
    }
    *fim = '\0';

    // Validar: pelo menos 1 caractere, todos dígitos
    size_t tamanho_num = fim - inicio;
    if (tamanho_num == 0) {
        free(buffer);
        return ERRO_SINTAXE;
    }
    for (size_t i = 0; i < tamanho_num; i++) {
        if (!isdigit((unsigned char) inicio[i])) {
            free(buffer);
            return ERRO_SINTAXE;
        }
    }

    // Copiar só a parte válida para uma string nova (buffer vai ser liberado)
    char *resultado = malloc(tamanho_num + 1);
    if (!resultado) {
        free(buffer);
        return ERRO_IO;
    }
    memcpy(resultado, inicio, tamanho_num);
    resultado[tamanho_num] = '\0';
    free(buffer);

    *num_out = resultado;
    return 0;
}

char *gerar_caminho_saida(const char *caminho_entrada) {
    size_t len = strlen(caminho_entrada);
    const char *sufixo = ".ci";
    size_t sufixo_len = strlen(sufixo);

    int tem_sufixo = (len >= sufixo_len) &&
                      (strcmp(caminho_entrada + len - sufixo_len, sufixo) == 0);

    size_t base_len = tem_sufixo ? (len - sufixo_len) : len;

    char *saida = malloc(base_len + 3); // ".s" + '\0'
    if (!saida) return NULL;

    memcpy(saida, caminho_entrada, base_len);
    saida[base_len] = '.';
    saida[base_len + 1] = 's';
    saida[base_len + 2] = '\0';

    return saida;
}

int gerar_assembly(const char *numero, const char *caminho_saida) {
    FILE *f = fopen(caminho_saida, "w");
    if (!f) {
        return -1;
    }

    fprintf(f,
        "//\n"
        "// saida gerada pelo compilador CI\n"
        "// versao arm64 + macOS\n"
        "//\n"
        ".global _main\n"
        ".text\n"
        ".align 2\n"
        "_main:\n"
        "    mov x0, #%s\n"
        "    bl imprime_num\n"
        "    b sair\n"
        ".include \"runtime.s\"\n",
        numero
    );

    fclose(f);
    return 0;
}