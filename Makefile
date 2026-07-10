# ============================================================================
# MAKEFILE - COMPILADOR C SIMPLIFICADO
#
# Disciplina: Compiladores - M3
# Projeto: Mini-Compilador C→C
#   Fase 1: Análise Léxica    (✅ Completo)
#   Fase 2: Análise Sintática (✅ Completo)
#   Fase 3: Análise Semântica (✅ Completo)
#
# REQUISITO: este Makefile usa comandos POSIX (mkdir -p, rm -rf). Compilar num
# shell POSIX — WSL, Git Bash ou MSYS2. No Windows nativo use `mingw32-make`
# a partir de um shell com sh disponível (não do cmd.exe puro).
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

# ----------------------------------------------------------------------------
# Build isolation (Windows/MinGW vs WSL/Linux)
#
# Evita misturar objetos do MinGW (COFF/PE) com o linker do Linux (ELF) quando
# se roda `wsl make` depois de compilar no Windows (ou vice-versa).
# ----------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
  OBJDIR ?= build/win
else
  ifdef WSL_DISTRO_NAME
    OBJDIR ?= build/wsl
  else
    OBJDIR ?= build/linux
  endif
endif

# Arquivo executável
TARGET = compilador
TARGET_PATH = $(TARGET)

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
                 src/parser/scope.c \
                 src/parser/parser_expr.c \
                 src/parser/parser_stmt.c \
				 src/parser/parser_decl.c \
				 src/parser/parser_error.c \

# Arquivos-fonte do Analisador Semântico (Fase 3)
SEMANTIC_SOURCES = src/semantic/semantic.c \
                   src/semantic/semantic_expr.c \
                   src/semantic/semantic_stmt.c \
                   src/semantic/semantic_decl.c \


# Utilitários
# UTILS_SOURCES = utils/printer.c

# Todos os arquivos-fonte
SOURCES = main.c \
          $(LEXER_SOURCES) \
          $(PARSER_SOURCES) \
          $(SEMANTIC_SOURCES)

# Headers
LEXER_HEADERS = src/lexer/lexer.h src/lexer/tokens.h src/lexer/keywords.h
PARSER_HEADERS = src/parser/parser.h src/parser/ast.h src/parser/scope.h
SEMANTIC_HEADERS = src/semantic/semantic.h
UTILS_HEADERS = utils/printer.h
HEADERS = $(LEXER_HEADERS) $(PARSER_HEADERS) $(SEMANTIC_HEADERS) $(UTILS_HEADERS)

# Arquivos-objeto
OBJECTS = $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

# Arquivo de teste
TESTE_ENTRADA = teste_entrada.c
TESTE_SAIDA = tabela_simbolos.txt

# ============================================================================
# REGRAS PRINCIPAIS
# ============================================================================

all: $(TARGET_PATH)

$(TARGET_PATH): $(OBJECTS)
	@echo "Linkando $(TARGET)..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET_PATH) $(OBJECTS)
	@echo "OK: Executavel criado: $(TARGET_PATH)"

$(OBJDIR)/%.o: %.c $(HEADERS)
	@echo "Compilando $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# ============================================================================
# REGRAS ESPECIAIS
# ============================================================================

clean:
	@echo "Limpando arquivos compilados..."
	rm -rf ./build
	@echo "OK: Limpeza concluida"

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET_PATH)
	@echo "OK: Build de debug concluido"

run: $(TARGET_PATH)
	@echo "Executando analisador lexico com teste..."
	@echo "-------------------------------------------"
	./$(TARGET_PATH) $(TESTE_ENTRADA)
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
	@echo "  Fase 1: Análise Léxica (Lexer)      ✅ Completo"
	@echo "  Fase 2: Análise Sintática (Parser)  ✅ Completo"
	@echo "  Fase 3: Análise Semântica            ✅ Completo"
	@echo ""
	@echo "Arquivos gerados:"
	@echo "  $(TARGET_PATH)     - Executável do compilador"
	@echo "  $(TESTE_SAIDA) - Saída da análise"

parser: $(addprefix $(OBJDIR)/,$(PARSER_SOURCES:.c=.o))
	@echo "OK: Parser compilado com sucesso"

.PHONY: all clean debug run help parser
