# ============================================================================
# MAKEFILE - COMPILADOR C SIMPLIFICADO
#
# Disciplina: Compiladores - M3
# Projeto: Mini-Compilador C→C
#   Fase 1: Análise Léxica (✅ Completo)
#   Fase 2: Análise Sintática (🚧 Em Desenvolvimento)
#
# Compilação:
#   make              # Compilar normalmente
#   make clean        # Remover arquivos compilados
#   make run          # Compilar + executar teste
#   make debug        # Compilar com símbolos de debug
#   make parser       # Compilar apenas o parser
#
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -I.
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2

# Arquivo executável
TARGET = compilador

# Arquivos-fonte do Lexer (Fase 1)
LEXER_SOURCES = src/lexer/lexer.c \
                src/lexer/lexer_io.c \
                src/lexer/lexer_fsm.c \
                src/lexer/lexer_token.c \
                src/lexer/lexer_init.c \
                src/lexer/tokens.c \
                src/lexer/keywords.c

# Arquivos-fonte do Parser (Fase 2)
PARSER_SOURCES = src/parser/parser.c \
                 src/parser/ast.c \
                 src/parser/scope.c

# Utilitários
UTILS_SOURCES = utils/printer.c

# Todos os arquivos-fonte
SOURCES = main.c \
          $(LEXER_SOURCES) \
          $(PARSER_SOURCES) \
          $(UTILS_SOURCES)

# Headers
LEXER_HEADERS = src/lexer/lexer.h src/lexer/tokens.h src/lexer/keywords.h
PARSER_HEADERS = src/parser/parser.h src/parser/ast.h src/parser/scope.h
UTILS_HEADERS = utils/printer.h
HEADERS = $(LEXER_HEADERS) $(PARSER_HEADERS) $(UTILS_HEADERS)

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
	@echo "  all     - Compilar projeto completo (padrão)"
	@echo "  parser  - Compilar apenas o parser"
	@echo "  clean   - Remover arquivos compilados"
	@echo "  debug   - Compilar com debug symbols"
	@echo "  run     - Compilar e executar teste"
	@echo "  help    - Mostrar esta mensagem"
	@echo ""
	@echo "Fases do compilador:"
	@echo "  Fase 1: Análise Léxica (Lexer)    ✅ Completo"
	@echo "  Fase 2: Análise Sintática (Parser) 🚧 Em Desenvolvimento"
	@echo ""
	@echo "Arquivos gerados:"
	@echo "  $(TARGET)     - Executável do compilador"
	@echo "  $(TESTE_SAIDA) - Saída da análise"

parser: $(PARSER_SOURCES:.c=.o)
	@echo "OK: Parser compilado com sucesso"

.PHONY: all clean debug run help parser
