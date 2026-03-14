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
- a simple interactive CLI for setting variables and evaluating expressions

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
- run expressions interactively through a CLI

## Project Structure

```text
arena.c      - arena allocator and block management
backprop.c   - reverse-mode backpropagation over created nodes
cli.c        - interactive command-line interface for setting variables and evaluating expressions
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

## CLI Usage

The repository includes an interactive CLI in `cli.c`.

When the program starts, it creates one parser instance and then repeatedly prompts with:

```text
>>
```

### Supported CLI commands

#### 1. Set a variable

```text
set x 2
set y 3.5
```

This assigns a value to a variable inside the parser state.

#### 2. Evaluate an expression

Any line that is not `set ...` or `exit` is treated as an expression.

Examples:

```text
x*x+x
sin(x)+sqrt(x)
x*y+x
```

The CLI parses the expression, runs backpropagation, prints the derivatives for the currently known variables, and then prints the final expression value.

#### 3. Exit

```text
exit
```

### Example CLI session

```text
>> set x 3
>> x*x+x
d(x) = 7.000000
12.000000
>> set y 2
>> x*y+x
d(x) = 3.000000
d(y) = 3.000000
9.000000
>> exit
```

## Example Usage in Code

### Simple numeric expression

```c
int main() {
    parser_t parser = create_parser();
    set_expr("3*(3+3)", &parser);
    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_parser(&parser);
}
```

### Variable expression

```c
int main() {
    parser_t parser = create_parser();
    set_expr("x*x+x", &parser);
    set_var(&parser, "x", 3.0);

    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_parser(&parser);
}
```

### Function expression

```c
int main() {
    parser_t parser = create_parser();
    set_expr("sin(x)+sqrt(x)", &parser);
    set_var(&parser, "x", 4.0);

    node_t* root = parse(&parser);

    print_node(root);
    putchar('\n');

    release_parser(&parser);
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
parser_t parser = create_parser();
set_expr("x*y+x", &parser);
set_var(&parser, "x", 2.0);
set_var(&parser, "y", 3.0);

node_t* root = parse(&parser);
```

After parsing and backpropagation, variable entries store both value and gradient.

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

### Build the CLI

```bash
gcc arena.c backprop.c lexer.c node.c parser.c queue.c stack.c string_t.c token.c vector.c cli.c -lm -o cli
./cli
```

If you are using a Makefile, point the build target at `cli.c` instead of another test file.

## Current Limitations

Some likely limitations or areas to improve:

- function support is currently limited to `sin`, `cos`, `tan`, and `sqrt`
- unary minus handling is not currently handled
- parser error handling can be improved for malformed expressions
- function dispatch currently compares names at graph-construction time rather than using a more direct function registry
- the CLI currently uses a small fixed input buffer and a very simple command format

## Future Improvements

Possible next steps:

- add `exp`, `log`, and more trig/inverse trig functions
- support multi-argument functions
- improve parser diagnostics
- support unary negation explicitly
- add a cleaner public API
- add unit tests
- add pretty-printing for graphs
- support reevaluating a graph after changing variable values
- extend the CLI with better help text and command parsing

## What I've Learned

This repo taught me several good low-level C topics in one place:

- memory management
- custom data structures
- lexical analysis
- parsing
- postfix conversion
- graph construction
- automatic differentiation
- numerical testing
- simple REPL/CLI design
