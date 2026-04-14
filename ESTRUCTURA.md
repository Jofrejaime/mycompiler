# 📁 Estrutura do Projeto - Analisador Léxico

## 🏗️ Organização de Diretórios

```
mycompiler/
├── 📄 Arquivos Raiz (Configuração)
│   ├── README.md                    # Guia principal do projeto
│   ├── Makefile                     # Compilação do projeto
│   ├── .gitignore                   # Ignorar arquivos no Git
│   ├── mp.md                        # Manual do Programador ⭐
│   └── mu.md                        # Manual do Usuário ⭐
│
├── 📁 src/                          # CÓDIGO-FONTE
│   ├── lexer/                       # Analisador Léxico (Fase 1)
│   │   ├── lexer.h                 # Interface do lexer
│   │   ├── lexer.c                 # Implementação principal
│   │   ├── lexer_io.c              # Input/Output
│   │   ├── lexer_patterns.c        # Processamento de padrões
│   │   ├── lexer_token.c           # Gerenciamento de tokens
│   │   ├── tokens.h                # Definições de tokens
│   │   ├── tokens.c                # Implementação de tokens
│   │   ├── keywords.h              # Palavras-chave
│   │   └── keywords.c              # Tabela de palavras-chave
│   │
│   └── parser/                      # Analisador Sintático (Fase 2) - FUTURO
│       └── (será desenvolvido)
│
├── 📁 build/                        # ARTEFATOS DE BUILD (GERADO) ✗
│   ├── analisador_lexico.exe       # Executável compilado
│   ├── main.o                       # Arquivos objeto
│   └── tabela_simbolos.txt          # Saída de execução
│
├── 📁 automato/                     # MÁQUINAS DE ESTADO
│   └── CompiladorAutomatos.jff     # Autômato JFLAP ⭐
│
├── 📁 docs/                         # DOCUMENTAÇÃO
│   ├── mp.md                        # Manual do Programador (cópia ref.)
│   ├── mu.md                        # Manual do Usuário (cópia ref.)
│   ├── RELATORIO_VALIDACAO_FINAL.md # Relatório de validação
│   ├── ARQUITETURA_COMPILADOR.md   # Arquitetura geral (referência)
│   ├── AS_7_FASES_COMPILACAO.md    # As 7 fases (referência)
│   ├── GUIA_COMPLETO_COMPILADOR.md # Guia completo (referência)
│   ├── PLANO_COMPILADOR_COMPLETO.md # Plano geral (referência)
│   └── RESUMO_VISUAL.md            # Resumo visual (referência)
│
├── 📁 tests/                        # TESTES E VALIDAÇÃO
│   ├── teste_entrada.c             # Arquivo C para teste
│   ├── enunciado_fase1.txt         # Enunciado original
│   └── validacao_*.sh              # Scripts de validação
│
├── 📁 scripts/                      # SCRIPTS AUXILIARES
│   └── build.sh                     # Script de build alternativo
│
└── 📁 artifacts/                    # IMAGENS E MÍDIA (não versionado)
    └── WhatsApp Image *.jpeg       # Screenshots/imagens
```

---

## 🚀 Como Usar

### Compilar o projeto
```bash
make              # Usa o Makefile (recomendado)
# ou
bash scripts/build.sh
```

### Executar o analisador léxico
```bash
./analisador_lexico teste_entrada.c
# Gera: build/tabela_simbolos.txt
```

### Limpar artefatos
```bash
make clean        # Remove build/
```

---

## 📚 Arquivos Importantes

| Arquivo | Descrição | Deve estar no Git |
|---------|-----------|-------------------|
| `mp.md` | Manual do Programador (completo) | ✅ SIM |
| `mu.md` | Manual do Usuário (completo) | ✅ SIM |
| `automato/CompiladorAutomatos.jff` | Autômato JFLAP | ✅ SIM |
| `src/**` | Código-fonte C | ✅ SIM |
| `Makefile` | Build configuration | ✅ SIM |
| `README.md` | Guia principal | ✅ SIM |
| **build/** | Executáveis (gerados) | ❌ NÃO |
| **artifacts/** | Imagens (pessoais) | ❌ NÃO |
| **docs/** | Documentação redundante | ⚠️ OPCIONAL |

---

## 🔧 Configuração do Git

O arquivo `.gitignore` está configurado para **ignorar**:
- ✗ Arquivos compilados (`.exe`, `.o`, `.a`, `.so`)
- ✗ Diretório `build/` (artefatos gerados)
- ✗ Imagens (`*.jpeg`, `*.jpg`, `*.png`)
- ✗ Arquivos temporários e logs
- ✗ IDE settings (`.vscode/`, `.idea/`)

**Resultado**: Repositório limpo e apenas código-fonte essencial!

---

## 📋 Checklist de Entrega

- ✅ Código-fonte organizado (`src/`)
- ✅ Manuais disponíveis (`mp.md`, `mu.md`)
- ✅ Autômato JFLAP (`automato/`)
- ✅ `.gitignore` configurado
- ✅ `Makefile` funcional
- ✅ Testes em `tests/`
- ✅ Documentação em `docs/`

---

## 📞 Informações

**Professor**: André Filemon  
**Disciplina**: Compiladores - M3  
**Data**: 14 de Abril de 2026  
**Status**: ✅ Pronto para GitHub!

---

*Generated on April 14, 2026 - Organized Project Structure*
