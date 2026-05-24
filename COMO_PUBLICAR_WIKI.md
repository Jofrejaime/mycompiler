# 📖 Como Publicar a Wiki no GitHub

Este guia mostra **3 opções** para publicar sua wiki no GitHub.

---

## 🎯 Opção 1: Wiki no Repositório Principal (Mais Simples) ⭐

Esta é a opção **mais simples** e já está pronta! A wiki está na pasta `wiki/` do seu repositório.

### Passo 1: Fazer Commit e Push

```bash
# Adicionar todos os arquivos da wiki
git add wiki/

# Fazer commit
git commit -m "Add complete wiki documentation"

# Fazer push para o GitHub
git push origin main
```

### Passo 2: Acessar no GitHub

Depois do push, sua wiki estará disponível em:

```
https://github.com/username/Compiladores/tree/main/wiki
```

E a página principal será:

```
https://github.com/username/Compiladores/blob/main/wiki/Home.md
```

### ✅ Vantagens
- ✅ Simples e rápido
- ✅ Versionado junto com o código
- ✅ Fácil de manter
- ✅ Links funcionam automaticamente no GitHub

### ❌ Desvantagens
- ❌ Não aparece na aba "Wiki" do GitHub
- ❌ URL não é `github.com/username/repo/wiki`

---

## 🎯 Opção 2: GitHub Wiki Oficial (Recomendado para Wikis)

O GitHub tem um sistema de wiki integrado que cria uma aba "Wiki" no seu repositório.

### Passo 1: Habilitar a Wiki no GitHub

1. Vá para seu repositório no GitHub
2. Clique em **Settings** (Configurações)
3. Role até a seção **Features**
4. Marque a opção **Wikis**

### Passo 2: Clonar o Repositório da Wiki

O GitHub cria um repositório Git separado para a wiki:

```bash
# Clonar o repositório da wiki (substitua 'username' e 'Compiladores')
git clone https://github.com/username/Compiladores.wiki.git

# Entrar na pasta
cd Compiladores.wiki
```

### Passo 3: Copiar os Arquivos da Wiki

```bash
# Voltar para a pasta do projeto
cd ..

# Copiar todos os arquivos .md da pasta wiki/ para o repositório da wiki
cp wiki/*.md Compiladores.wiki/

# Entrar no repositório da wiki
cd Compiladores.wiki
```

### Passo 4: Renomear Home.md (Importante!)

O GitHub Wiki usa `Home.md` como página principal:

```bash
# O arquivo já se chama Home.md, então não precisa renomear!
# Mas se precisasse, seria:
# mv Home.md Home.md
```

### Passo 5: Fazer Commit e Push

```bash
# Adicionar todos os arquivos
git add .

# Fazer commit
git commit -m "Add complete wiki documentation"

# Fazer push
git push origin master
```

### Passo 6: Acessar a Wiki

Sua wiki estará disponível em:

```
https://github.com/username/Compiladores/wiki
```

### ✅ Vantagens
- ✅ Aparece na aba "Wiki" do GitHub
- ✅ URL limpa: `github.com/username/repo/wiki`
- ✅ Interface de edição web do GitHub
- ✅ Histórico de versões separado

### ❌ Desvantagens
- ❌ Repositório Git separado (mais complexo)
- ❌ Precisa sincronizar manualmente

---

## 🎯 Opção 3: GitHub Pages (Para Sites Estáticos)

Transformar a wiki em um site estático com GitHub Pages.

### Passo 1: Criar Branch `gh-pages`

```bash
# Criar e mudar para branch gh-pages
git checkout -b gh-pages

# Copiar arquivos da wiki para a raiz
cp wiki/* .

# Adicionar e fazer commit
git add .
git commit -m "Setup GitHub Pages"

# Fazer push
git push origin gh-pages
```

### Passo 2: Habilitar GitHub Pages

1. Vá para **Settings** no GitHub
2. Role até **Pages**
3. Em **Source**, selecione `gh-pages` branch
4. Clique em **Save**

### Passo 3: Acessar o Site

Seu site estará disponível em:

```
https://username.github.io/Compiladores/
```

### ✅ Vantagens
- ✅ Site estático completo
- ✅ Pode usar temas Jekyll
- ✅ URL personalizada possível

### ❌ Desvantagens
- ❌ Mais complexo de configurar
- ❌ Requer conhecimento de Jekyll/HTML

---

## 🎯 Recomendação

### Para o Seu Caso (Projeto Acadêmico):

**Use a Opção 1** (Wiki no Repositório Principal)

**Por quê?**
- ✅ Mais simples e rápido
- ✅ Já está pronto para usar
- ✅ Fácil de manter
- ✅ Links funcionam perfeitamente no GitHub
- ✅ Versionado junto com o código

### Comandos Rápidos (Opção 1):

```bash
# 1. Adicionar a wiki
git add wiki/

# 2. Adicionar o README atualizado
git add README.md

# 3. Fazer commit
git commit -m "Add complete wiki documentation with 12 pages"

# 4. Fazer push
git push origin main
```

### Depois do Push:

Adicione um link no README.md do GitHub apontando para:

```markdown
📚 [Documentação Completa na Wiki](wiki/Home.md)
```

---

## 📝 Estrutura da Wiki Criada

```
wiki/
├── Home.md                      ← Página principal (12 seções)
├── Token-Table.md               ← 82+ tokens organizados
├── Syntactic-Grammar.md         ← Gramática LL(1) completa
├── Manual-Usuario.md            ← Manual do usuário (4-8 páginas)
├── Manual-Programador.md        ← Manual do programador (8-16 páginas)
├── Lexer.md                     ← Fase 1 completa (✅)
├── Parser.md                    ← Fase 2 em desenvolvimento (🚧)
├── Parser-Requirements.md       ← 20 requisitos detalhados
├── Parser-Design.md             ← Arquitetura técnica
├── Parser-Tasks.md              ← 24 tarefas organizadas
├── FIRST-Sets.md                ← Conjuntos FIRST para LL(1)
└── Grammar-Transformations.md   ← Eliminação de recursividade
```

**Total:** 12 páginas de documentação completa!

---

## 🔗 Links Úteis

- **GitHub Docs - About Wikis:** https://docs.github.com/en/communities/documenting-your-project-with-wikis/about-wikis
- **GitHub Pages:** https://pages.github.com/
- **Markdown Guide:** https://www.markdownguide.org/

---

## ❓ Dúvidas Comuns

### 1. Os links entre páginas vão funcionar?

**Sim!** No GitHub, links relativos como `[Parser](Parser.md)` funcionam perfeitamente.

### 2. Preciso converter para HTML?

**Não!** O GitHub renderiza Markdown automaticamente.

### 3. Posso editar a wiki depois?

**Sim!** Basta editar os arquivos `.md` e fazer commit/push novamente.

### 4. Como adiciono imagens?

Crie uma pasta `wiki/images/` e use:

```markdown
![Descrição](images/diagrama.png)
```

---

## ✅ Checklist de Publicação

- [ ] Fazer commit da pasta `wiki/`
- [ ] Fazer commit do `README.md` atualizado
- [ ] Fazer push para o GitHub
- [ ] Verificar se os links funcionam no GitHub
- [ ] Adicionar link para a wiki no README principal
- [ ] (Opcional) Habilitar GitHub Wiki oficial
- [ ] (Opcional) Configurar GitHub Pages

---

**Boa sorte com a publicação! 🚀**
