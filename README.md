# Projeto de Teoria dos Grafos - Parte 2

O objetivo é identificar as panelinhas e as pessoas influentes na rede de
amizades de um condomínio. O grafo é não orientado e tem 25 moradores e
29 amizades.

## Integrantes

| Nome | RA |
|---|---:|
| Fabyani Tiva Yan | 10431835 |
| Rafael Araujo Cabral Moreira | 10441919 |
| Bruna Amorim Maia | 10431883 |

## Compilação e execução 

```powershell

cc -ansi -pedantic -Wall -Wextra projeto_grafos_parte2.c -o projeto_grafos_parte2
./projeto_grafos_parte2

```


A rede é definida no código. O programa exibe o cadastro e as amizades dos
moradores, os resultados das consultas da Parte 1 e os resultados da Parte 2.
Na rede usada, são encontradas sete panelinhas. Ana, Carla e Helena são as
pessoas influentes.

## Estrutura do código

Todas as funções estão em `projeto_grafos_parte2.c`.

| Funções | Papel |
|---|---|
| `main` | Cria a rede com os dados fixos e chama as consultas. |
| `criarGrafo`, `acrescentarAresta`, `imprimirGrafo`, `destruirGrafo` | Criam, exibem e liberam o grafo usando listas de adjacência. |
| `pessoasSemContato`, `pessoasComUmContato`, `pessoasComMaisContatos`, `gruposIsolados` | Executam as consultas da Parte 1. |
| `encontrarPanelinhas`, `buscarPanelinhas`, `exibirPanelinhas` | Encontram e listam os grupos completos maximais com pelo menos três moradores. |
| `unicoEmComum`, `exibirPessoasInfluentes` | Comparam pares de panelinhas e exibem quem é o único membro em comum. |

## Particularidades da implementação

**Representação do grafo.** A estrutura de listas de adjacência segue a
implementação fornecida na disciplina. Cada amizade aparece na lista dos dois
moradores. A busca de panelinhas usa uma matriz auxiliar montada a partir
dessas listas.

**Panelinhas.** A busca acrescenta moradores em ordem crescente de identificador
para visitar cada grupo uma vez. Antes de registrar um grupo, verifica se
algum morador de fora é amigo de todos os membros. O grupo só é registrado se
tiver pelo menos três moradores e não puder ser ampliado.

**Pessoas influentes.** O programa percorre os pares de panelinhas e procura
interseções com exatamente um morador. Cada pessoa é exibida uma vez, junto
com o primeiro par de panelinhas que confirma o resultado.
