@echo off
REM ============================================================================
REM BUILD SCRIPT WINDOWS — compila todos os módulos com MinGW/gcc
REM Uso: build_win.bat
REM ============================================================================

set CC=gcc
set CFLAGS=-Wall -Wextra -Werror -std=c99 -I. -O2
set OBJDIR=build\win

echo === Criando diretorios de build ===
if not exist %OBJDIR%\src\lexer   mkdir %OBJDIR%\src\lexer
if not exist %OBJDIR%\src\parser  mkdir %OBJDIR%\src\parser
if not exist %OBJDIR%\src\semantic mkdir %OBJDIR%\src\semantic

set ERRORS=0

echo === Compilando Lexer ===
%CC% %CFLAGS% -c src/lexer/lexer.c        -o %OBJDIR%/src/lexer/lexer.o        || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/lexer_io.c     -o %OBJDIR%/src/lexer/lexer_io.o     || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/lexer_fsm.c    -o %OBJDIR%/src/lexer/lexer_fsm.o    || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/lexer_token.c  -o %OBJDIR%/src/lexer/lexer_token.o  || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/lexer_init.c   -o %OBJDIR%/src/lexer/lexer_init.o   || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/tokens.c       -o %OBJDIR%/src/lexer/tokens.o       || set ERRORS=1
%CC% %CFLAGS% -c src/lexer/keywords.c     -o %OBJDIR%/src/lexer/keywords.o     || set ERRORS=1

echo === Compilando Parser ===
%CC% %CFLAGS% -c src/parser/parser.c       -o %OBJDIR%/src/parser/parser.o       || set ERRORS=1
%CC% %CFLAGS% -c src/parser/ast.c          -o %OBJDIR%/src/parser/ast.o          || set ERRORS=1
%CC% %CFLAGS% -c src/parser/scope.c        -o %OBJDIR%/src/parser/scope.o        || set ERRORS=1
%CC% %CFLAGS% -c src/parser/parser_expr.c  -o %OBJDIR%/src/parser/parser_expr.o  || set ERRORS=1
%CC% %CFLAGS% -c src/parser/parser_stmt.c  -o %OBJDIR%/src/parser/parser_stmt.o  || set ERRORS=1
%CC% %CFLAGS% -c src/parser/parser_decl.c  -o %OBJDIR%/src/parser/parser_decl.o  || set ERRORS=1
%CC% %CFLAGS% -c src/parser/parser_error.c -o %OBJDIR%/src/parser/parser_error.o || set ERRORS=1

echo === Compilando Analisador Semantico ===
%CC% %CFLAGS% -c src/semantic/semantic.c      -o %OBJDIR%/src/semantic/semantic.o      || set ERRORS=1
%CC% %CFLAGS% -c src/semantic/semantic_expr.c -o %OBJDIR%/src/semantic/semantic_expr.o || set ERRORS=1
%CC% %CFLAGS% -c src/semantic/semantic_stmt.c -o %OBJDIR%/src/semantic/semantic_stmt.o || set ERRORS=1
%CC% %CFLAGS% -c src/semantic/semantic_decl.c -o %OBJDIR%/src/semantic/semantic_decl.o || set ERRORS=1

echo === Compilando main ===
%CC% %CFLAGS% -c main.c -o %OBJDIR%/main.o || set ERRORS=1

if "%ERRORS%"=="1" (
    echo.
    echo FALHOU: erros de compilacao encontrados.
    exit /b 1
)

echo === Linkando executavel ===
%CC% %CFLAGS% -o compilador.exe ^
    %OBJDIR%/main.o ^
    %OBJDIR%/src/lexer/lexer.o ^
    %OBJDIR%/src/lexer/lexer_io.o ^
    %OBJDIR%/src/lexer/lexer_fsm.o ^
    %OBJDIR%/src/lexer/lexer_token.o ^
    %OBJDIR%/src/lexer/lexer_init.o ^
    %OBJDIR%/src/lexer/tokens.o ^
    %OBJDIR%/src/lexer/keywords.o ^
    %OBJDIR%/src/parser/parser.o ^
    %OBJDIR%/src/parser/ast.o ^
    %OBJDIR%/src/parser/scope.o ^
    %OBJDIR%/src/parser/parser_expr.o ^
    %OBJDIR%/src/parser/parser_stmt.o ^
    %OBJDIR%/src/parser/parser_decl.o ^
    %OBJDIR%/src/parser/parser_error.o ^
    %OBJDIR%/src/semantic/semantic.o ^
    %OBJDIR%/src/semantic/semantic_expr.o ^
    %OBJDIR%/src/semantic/semantic_stmt.o ^
    %OBJDIR%/src/semantic/semantic_decl.o

if %ERRORLEVEL% neq 0 (
    echo FALHOU: erro de linkagem.
    exit /b 1
)

echo.
echo OK: compilador.exe criado com sucesso!
