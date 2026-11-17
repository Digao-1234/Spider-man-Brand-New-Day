# Homem-Aranha: Um Novo Dia

Jogo estilo *Space Invaders* em C com [raylib](https://www.raylib.com/). Três fases: ondas de ninjas, Escorpião, Hulk e Jean Grey.

## Controles

| Tecla | Ação |
|---|---|
| A / D ou setas | mover |
| ESPAÇO | atirar teia (segure para atirar continuamente) |
| P | pausar / continuar |
| ENTER | jogar de novo (na tela de vitória/derrota) |
| ESC | sair (o recorde da partida em curso é salvo) |

## Dependências

Para compilar e executar o projeto, é necessário ter:

- **GCC**: compilador C.
- **Raylib**: biblioteca gráfica utilizada pelo jogo.
- **Visual Studio Code** (opcional): editor recomendado.
- **Extensão C/C++** (opcional): extensão da Microsoft para suporte à linguagem C e IntelliSense.

## Configuração do ambiente

### Windows (MSYS2)

1. Instale o [MSYS2](https://www.msys2.org/).
2. Abra o terminal **MSYS2 UCRT64** (menu Iniciar). Não use o terminal "MSYS2 MSYS" comum, pois ele gera executáveis que não rodam bem no Windows.
3. Instale o compilador e a Raylib (só uma vez):
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib mingw-w64-ucrt-x86_64-pkgconf
   ```
4. (Opcional) Para usar o `make`: `pacman -S make`.

Como a Raylib vem do próprio MSYS2, não é preciso configurar caminhos de inclusão e bibliotecas à mão.

### Linux

Instale o `gcc`, o `pkg-config` e a Raylib pelo gerenciador de pacotes da sua distribuição (ou compile a Raylib a partir do código-fonte).

### macOS

Instale o compilador (Xcode Command Line Tools) e a Raylib (por exemplo, `brew install raylib`).

> Os caminhos de instalação podem variar de acordo com o computador. As configurações locais do VS Code não são compartilhadas pelo repositório.

## Como compilar e rodar

Abra o terminal **na pasta raiz do projeto** (a que contém `src/`, `include/`, `assets/` e `saves/`). No MSYS2, o disco `C:` aparece como `/c/`, por exemplo: `cd /c/Users/SeuNome/Documents/Spider-man-Brand-New-Day`.

Os caminhos de `assets/` e `saves/` são relativos a essa pasta: rodar de outro lugar faz o jogo cair nos retângulos e não salvar o recorde.

### Com make (mais fácil)

```
mingw32-make          # compila
mingw32-make run      # compila e executa (Linux/macOS e MSYS2)
mingw32-make clean    # apaga o executável
```

O `make` é só um atalho: o jogo pode ser compilado sem ele, com os comandos abaixo.

### Sem make

**Windows (MSYS2 UCRT64):**
```
gcc -std=c11 -Iinclude src/main.c src/jogador.c src/inimigo.c src/projeteis.c src/fases.c src/arquivos.c src/sprites.c -o homem_aranha.exe $(pkg-config --cflags --libs raylib)
./homem_aranha.exe
```
Se o `pkg-config` der problema, troque o final do comando por `-lraylib -lopengl32 -lgdi32 -lwinmm`.

**Windows (w64devkit / instalador da Raylib):**
```
gcc -std=c11 -Iinclude src/main.c src/jogador.c src/inimigo.c src/projeteis.c src/fases.c src/arquivos.c src/sprites.c -o homem_aranha.exe -lraylib -lopengl32 -lgdi32 -lwinmm
.\homem_aranha.exe
```
Se der `raylib.h: No such file`, acrescente o caminho da sua instalação: `-IC:/raylib/raylib/src -LC:/raylib/raylib/src`.

**Linux:**
```
gcc -std=c11 -Iinclude src/main.c src/jogador.c src/inimigo.c src/projeteis.c src/fases.c src/arquivos.c src/sprites.c -o homem_aranha $(pkg-config --cflags --libs raylib) -lm
./homem_aranha
```

**macOS:**
```
gcc -std=c11 -Iinclude src/main.c src/jogador.c src/inimigo.c src/projeteis.c src/fases.c src/arquivos.c src/sprites.c -o homem_aranha -lraylib -framework IOKit -framework Cocoa -framework OpenGL
./homem_aranha
```

> **Dica (Windows/MSYS2):** execute o jogo pelo terminal MSYS2. Dar duplo clique no `.exe` pode falhar, porque a Raylib do MSYS2 costuma ser uma DLL que só é encontrada dentro desse terminal.

> Se criar um arquivo `.c` novo, lembre de incluí-lo no comando de compilação (ou na linha `SRC` do `Makefile`).

## Estrutura

```
include/          cabeçalhos (.h)
src/              código (.c); main.c é o loop do jogo
assets/sprites/   imagens PNG
saves/            recorde.txt (criado automaticamente)
Makefile          atalho opcional de compilação
```
