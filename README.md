# C Automatic Differentiation Expression Engine

A small C project for parsing mathematical expressions, building a computation graph, evaluating the expression, and computing gradients with reverse-mode automatic differentiation.

This project includes:

- a custom arena allocator
- custom container types:
  - vector
  - stack
  - queue
  - string
- a lexer and parser for math expressions
- postfix conversion via the shunting-yard algorithm
- computation graph construction
- reverse-mode backpropagation for gradients

## Features

### Supported expression elements

- numeric literals
- variables
- binary operators:
  - `+`
  - `-`
  - `*`
  - `/`
  - `^`
- parentheses
- unary functions:
  - `sin(...)`
  - `cos(...)`
  - `tan(...)`
  - `sqrt(...)`

### Supported capabilities

- tokenize infix math expressions
- convert infix to postfix notation
- build a node graph from postfix tokens
- evaluate node values
- compute gradients with reverse-mode autodiff
- create variables and assign values before parsing/evaluation
- use a custom arena allocator for most project memory

## Project Structure

```text
arena.c      - arena allocator and block management
backprop.c   - reverse-mode backpropagation over created nodes
lexer.c      - tokenizes the input expression
node.c       - node construction and backward functions for operations
parser.c     - infix -> postfix -> graph pipeline, plus function handling
queue.c      - generic queue implementation
stack.c      - generic stack implementation
string_t.c   - custom string type and helpers
token.c      - token constructors and token printing
vector.c     - generic vector implementation
```

## How It Works

### 1. Lexing

The lexer scans an input expression and produces tokens for:

- numbers
- variables
- functions
- operators
- parentheses

Examples:

- `3*(3+3)`
- `x*x+x`
- `sin(x)+sqrt(x)`

### 2. Parsing

The parser:

1. collects infix tokens
2. converts them to postfix using the shunting-yard algorithm
3. builds a computation graph from the postfix output

### 3. Graph Construction

Each number or variable becomes a leaf node.

Each operator or function creates a node whose children are the operands already on the node stack.

Examples:

- `x + x` creates one add node with both children pointing to the same variable node
- `x * x + x` builds a multiply node and then an add node
- `sin(x)` builds a unary function node with `x` as its left child

### 4. Backpropagation

After graph creation, the project performs reverse-mode automatic differentiation by:

1. resetting all node gradients to `0`
2. setting the root gradient to `1`
3. iterating through nodes in reverse creation order
4. calling each node's stored backward function

This allows the project to compute gradients for all variables used in the expression.

## Supported Derivatives

### Binary operations

- `a + b`
- `a - b`
- `a * b`
- `a / b`
- `a ^ b`

### Unary functions

- `sin(x)`
- `cos(x)`
- `tan(x)`
- `sqrt(x)`

## Example Usage

### Simple numeric expression

```c
int main() {
    arena_t arena = create_arena(PAGE_SIZE);

    parser_t parser = create_parser("3*(3+3)", &arena);
    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_arena(&arena);
}
```

### Variable expression

```c
int main() {
    arena_t arena = create_arena(PAGE_SIZE);

    parser_t parser = create_parser("x*x+x", &arena);
    set_var(&parser, "x", 3.0);

    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_arena(&arena);
}
```

### Function expression

```c
int main() {
    arena_t arena = create_arena(PAGE_SIZE);

    parser_t parser = create_parser("sin(x)+sqrt(x)", &arena);
    set_var(&parser, "x", 4.0);

    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_arena(&arena);
}
```

## Variable API

The parser exposes helpers for variable creation and assignment:

```c
void create_var(parser_t* parser, char* name);
void set_var(parser_t* parser, char* name, double val);
```

Typical pattern:

```c
parser_t parser = create_parser("x*y+x", &arena);
set_var(&parser, "x", 2.0);
set_var(&parser, "y", 3.0);

node_t* root = parse(&parser);
```

After parsing and backpropagation, variable entries store both value and gradient.

## Example Test Cases

### Numeric tests

```text
3+4 = 7
3*3+3 = 12
3*(3+3) = 18
8-3-2 = 3
20/5/2 = 2
12/(3+1)*2 = 6
2^3 = 8
```

### Variable tests

```text
x+x          with x=5   -> value 10, dx = 2
x*x          with x=5   -> value 25, dx = 10
x*x+x        with x=3   -> value 12, dx = 7
x+y          with x=2,y=3 -> value 5, dx = 1, dy = 1
x*y+x        with x=2,y=3 -> value 8, dx = 4, dy = 2
x/y          with x=8,y=2 -> value 4, dx = 0.5, dy = -2
x^y          with x=2,y=3 -> value 8, dx = 12, dy ≈ 5.545177444
```

### Function tests

```text
sin(x)       with x=0     -> value 0, dx = 1
cos(x)       with x=0     -> value 1, dx = 0
tan(x)       with x=0     -> value 0, dx = 1
sqrt(x)      with x=4     -> value 2, dx = 0.25
sin(x*x)     with x=2     -> value ≈ -0.7568024953, dx ≈ -2.6145744835
sqrt(x*x+1)  with x=3     -> value ≈ 3.1622776602, dx ≈ 0.9486832981
```

## Design Notes

### Arena allocation

Most structures in the project are designed to work with a custom arena allocator. This makes memory management simpler for graph-heavy workloads and reduces many small heap allocations.

### Shared variable nodes

Repeated variables in an expression are reused as the same node when names match. This is important for correct gradient accumulation.

Example:

```text
x + x
```

Both children of the add node should point to the same variable node so the gradient of `x` becomes `2`.

### Reverse creation order for backprop

The project stores created nodes in a vector and performs backpropagation by iterating over that vector in reverse order. This avoids issues that appear when recursively traversing a shared computation graph.

## Build Notes

This project is plain C and appears intended for a Unix-like environment because the arena allocator uses `mmap`/`munmap`.

You will likely need to link against the math library.

Example:

```bash
gcc *.c -lm -o autodiff
```

If you split headers and source files into folders, adjust the command accordingly.

## Current Limitations

Some likely limitations or areas to improve:

- function support is currently limited to `sin`, `cos`, `tan`, and `sqrt`
- unary minus handling may need special-case parsing if not already implemented
- parser error handling can be improved for malformed expressions
- some container edge cases and bounds checks could be tightened further
- function dispatch currently compares names at graph-construction time rather than using a more direct function registry
- the custom containers and allocator are useful learning projects, but they should be tested heavily before production use

## Future Improvements

Possible next steps:

- add `exp`, `log`, and more trig/inverse trig functions
- support multi-argument functions
- improve parser diagnostics
- support unary negation explicitly
- add a cleaner public API
- add unit tests
- separate headers and sources into `include/` and `src/`
- add pretty-printing for graphs
- support reevaluating a graph after changing variable values

## Why this project is interesting

This repo combines several good low-level C topics in one place:

- memory management
- custom data structures
- lexical analysis
- parsing
- postfix conversion
- graph construction
- automatic differentiation
- numerical testing

It is a strong educational project for learning both systems programming and applied math tooling in C.

## License

Add a license here if you plan to publish the repository publicly.

For example:

- MIT
- Apache-2.0
- GPL-3.0

## Author

Add your name and any links you want here.

Example:

```text
Bennett Brown
```
