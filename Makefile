# ============================================================================
# MAKEFILE - COMPILADOR ANALISADOR LÉXICO
#
# Disciplina: Compiladores - M3
# Projeto: Mini-Compilador C→C (Fase 1: Análise Léxica)
#
# Compilação:
#   make              # Compilar normalmente
#   make clean        # Remover arquivos compilados
#   make run          # Compilar + executar teste
#   make debug        # Compilar com símbolos de debug
#
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2

# Arquivo executável
TARGET = analisador_lexico

# Arquivos-fonte
SOURCES = main.c \
          src/lexer/lexer.c \
          src/lexer/lexer_io.c \
          src/lexer/lexer_fsm.c \
          src/lexer/lexer_token.c \
          src/lexer/lexer_init.c \
          src/lexer/tokens.c \
          src/lexer/keywords.c \
          utils/printer.c
HEADERS = src/lexer/lexer.h src/lexer/tokens.h src/lexer/keywords.h utils/printer.h

# Arquivos-objeto
OBJECTS = $(SOURCES:.c=.o)

# Arquivo de teste
TESTE_ENTRADA = teste_entrada.c
TESTE_SAIDA = tabela_simbolos.txt

# ============================================================================
# REGRAS PRINCIPAIS
# ============================================================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linkando $(TARGET)..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET) $(OBJECTS)
	@echo "OK: Executavel criado: $(TARGET)"

%.o: %.c $(HEADERS)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# ============================================================================
# REGRAS ESPECIAIS
# ============================================================================

clean:
	@echo "Limpando arquivos compilados..."
	rm -f $(OBJECTS) $(TARGET) $(TESTE_SAIDA)
	@echo "OK: Limpeza concluida"

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)
	@echo "OK: Build de debug concluido"

run: $(TARGET)
	@echo "Executando analisador lexico com teste..."
	@echo "-------------------------------------------"
	./$(TARGET) $(TESTE_ENTRADA)
	@echo "-------------------------------------------"
	@echo "\nResultado salvo em: $(TESTE_SAIDA)"
	@echo "Conteudo da tabela:"
	@cat $(TESTE_SAIDA)

help:
	@echo "Uso: make [alvo]"
	@echo ""
	@echo "Alvos disponíveis:"
	@echo "  all     - Compilar projeto (padrão)"
	@echo "  clean   - Remover arquivos compilados"
	@echo "  debug   - Compilar com debug symbols"
	@echo "  run     - Compilar e executar teste"
	@echo "  help    - Mostrar esta mensagem"
	@echo ""
	@echo "Arquivos gerados:"
	@echo "  $(TARGET)     - Executável do analisador léxico"
	@echo "  $(TESTE_SAIDA) - Saída da análise"

.PHONY: all clean debug run help
