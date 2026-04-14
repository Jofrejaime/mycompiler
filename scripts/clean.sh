#!/bin/bash
# Script de limpeza do projeto - Remove artefatos de build

echo "🧹 Limpando artefatos de build..."

# Remover arquivos compilados
rm -f build/*.o build/*.exe build/*.out
rm -f main.o

# Limpar diretório build
if [ -d "build" ]; then
    rm -rf build/*
    echo "  ✓ Diretório build/ limpo"
fi

# Opcional: limpar saída de testes
if [ -f "build/tabela_simbolos.txt" ]; then
    rm -f build/tabela_simbolos.txt
    echo "  ✓ Arquivos de saída removidos"
fi

echo ""
echo "✅ Limpeza concluída!"
echo "📁 Estrutura pronta para GitHub"
echo ""
echo "Para fazer commit:"
echo "  git add ."
echo "  git commit -m 'Organização do projeto'"
echo "  git push origin main"
