# Summary

Overview of what we implemented and why we made our design choices

### Table of contents
**[Context](#context)**<br>
**[Lexing and Parsing](#lexing--parsing)**<br>
**[AST implementations](#ast-node)**<br>
&nbsp;&nbsp;&nbsp;&nbsp;**[Control flow](#control-flow)**<br>
&nbsp;&nbsp;&nbsp;&nbsp;**[Floats](#floats)**<br>
&nbsp;&nbsp;&nbsp;&nbsp;**[Arrays](#arrays)**<br>
&nbsp;&nbsp;&nbsp;&nbsp;**[Pointers](#pointers)**<br>
**[Future scope]()**<br>

## [Context](../../include/context.hpp)
- Context is a struct that facilitates tracking the state for code gen. This includes managing registers, variable bindings, stack offsets and provides other utility methods
- Most functionality is encapsulated in different structs:
  - `Variable` - stores relevant variable data, eg type and location in memory
  - `Scope` - each scope contains bindings, which maps the variable identifier to their Variable - symbol table
  - `LoopLabel` - manage loops, tracking start and end labels, crucial for jumps

The types implementation consists of an enum class `Specifier` which defines the supported types, and a map `typeSizes`, which is used to define the size for each type.
- This is effective as a basic type system, but is currently too verbose to support more complex derived types, including supporting `structs`, `unions` or `typedefs`.

### Stack frame implementation

  The stack grows downwards, from higher addressed memory to lower.

| _Higher address_ `(s0)`      |
| -------------                |
| Return address (`ra`)        |
| Saved registers (`s0`, `s1`) |
| Local variables              |
| Function params              |
| **_Lower address_ `(sp)`**   |

Local variables and function parameters are stored in 16-byte aligned segments, so for function:
```C
int f(int a) {
    int b = 1;
    return a + b;
}
```

variable `b` would be stored at offset `-20(s0)`, and param `a` would be stored at `-36(s0)`, where `s0` is the frame pointer. This alignment is handled by allocating the appropriate size of memory for the two, and tracking the offsets as variables are added.

-------

## [Lexing](../../src/lexer.flex) / [Parsing](../../src/parser.y)

- The parser follows the original published [ANSI-C grammar](https://www.lysator.liu.se/c/ANSI-C-grammar-y.html)
- This produces an Abstract Syntrax Tree, which can then be traversed for code gen. Memory management is manual, so custom destructors are also defined for all AST nodes to prevent memory leaks.
- Smart pointers?
  - difficult to implement as `shared_ptr` / `unique_ptr` cannot be members of the Bison union, which requires POD data types - would have to switch to using C++ Bison api, or have some more indirection
  - smart pointers could be considered slightly overkill in this case, where object ownership should be clear due to the tree structure of AST

## [AST node](../../include/ast/ast_node.hpp)
- Polymorphism with `Node` as an interface, which defines the `compile` (codegen) function as a pure virtual method, as all objects derive from this base `Node` class
- Virtual destructor is also defined to ensure proper destruction of AST after code gen

## [Function definitions and Function calls](https://github.com/IvanxAj/C90-compiler/blob/c8e6c031a2bb/src/ast/ast_functions.cpp#L12)
- `FunctionDefinition` is the top level class for defining functions
- Stores function return type in `Context` (for return statement) and accordingly sets the destReg for statements to be compiled into
- Manages the stack frame, by calculating and allocating the correct size required for its params and declarations to be stored, as well as handling the stack frame tear down.
- `addVar()`, `addParam()` changes internal offsets within `Context`, which is stack frame independent, therefore this must be reset after every function definition.

## [Multiple functions / statements](../../include/ast.hpp)
- Defined `List` (vector of node pointers), to store lists of node pointers - to handle multiple statements / declarations / functions
- Ideally would have lists of `BaseExpression`, `BaseStatement`, `BaseDeclaration` as relevant object-specific virtual functions are defined are this level eg `getIdentifier()` - but would have to define multiple very similar ExpressionList, StatementList etc, so decided to use lists of node pointers + dynamic cast appropriately as grammar guarantees we know what object is stored at any point.
- In retrospect, we could have used templates, where we just pass in required list type

## Variables: declarations + initialisation
- [Declaration](../../src/ast/ast_declaration.cpp) -> [InitDeclarator](../../src/ast/ast_init_declarator.cpp) -> Declarator / PointerDeclarator / ArrayDeclarator
- `Declaration` adds variable to variable bindings (mapping of var_name to Variable (containing offset and type fields)) in context (as it has type info)
- `InitDeclarator` handles initialisation for variables (including arrays, pointers)

## [Assignment](../../src/ast/ast_assignment.cpp)
Evaluates the r-value into an allocated register, then depending on the l-value, stores on the stack appropriately.
  - for variables, fetches offset directly from context variable bindings
  - for arrays, load memory address of the offset required based on index
  - for dereferenced pointers, load into a register, the memory address that is being pointed to, and use that as the address to store at

## [Parameters and Arguments](../../src/ast/ast_functions.cpp)
- In RISC-V, Parameters can be found in registers `a0-7` (and `fa0-7` depending on type). These are then stored ot the bottom of the stack frame (growing downwards), in the next 16 byte aligned segment after local variables.
- In our impelementation, parameters are added to the variable bindings using `addParam()`, which then returns the offset at which the param should be stored on the stack frame.
- An `int_param_count` and `float_param_count` are maintained at the `FuncDeclarator` level, which contains the `param_list`. This is used to ensure that the parameter is stored from the correct argument registers, as integer and float / double params are stored in separate sets of argument registers.
- In RISC-V, arguments are stored into registers` a0-7` as part of a function call. Our implementation for args works similar as for params, however the type information is not directly available, so lookup the type in bindings, and store in the appropriate register.

## [Arithmetic](../../src/ast/ast_arithmetic.cpp) and [Logical](../../src/ast/ast_logical.cpp) operations
- All derive from `BinaryOperation`, which contains methods to handle loading left and right operands of the operation into two registers, reducing repeated code when preparing for the operation.
- Account for function call operands, which could leave allocated temp registers in unknown state when called, by checking if the right operand is a function call, then using a callee saved register `s1` for the left reg if that is the case
- Specific classes then output the corresponding instruction for the operation


## Control flow

Unique labels generated in context, are used

Context contains relevant methods to track loop labels, which is needed for jump statements within iteration and selection statements

### [Selection](../../src/ast/ast_selection.cpp)
- **if/else statement** - evaluates condition and jumps to either the else label, or label at the end of the if statement
- **switch** - evaluates condition expression into a register, which is passed to case statements to compare against. If case expression doesn't match, then branch to start label of next case - this means the start label for a particular case statement is defined by the previous case
  - we store the start label of next case as `Case` structures hold one statement (as defined by ANSI-C grammar) therefore subsequent statements are considered as part of the `Switch` structure (instead of the `Case` structure), this means we cannot use end case labels as `compile` would generate an end label immediately after the first statement - which results in incorrectly executing the subsequent statements, instead of jumping to the next case comparison

### Iteration
- [**while loop**](../../src/ast/ast_while.cpp) - consist of a start label, to support continue statements, and an end_label to support break statements
    ```
    .start_while:
        condition->compile(cond_reg)
        beq cond_reg, zero, .end_while
        statements->compile
        j start_while
    .end_while:
    ```
- [**for loop**](../../src/ast/ast_for.cpp) - continue statements jump to the update_loop label
    ```
        initialise->compile
    .start_loop:
        condition->compile(cond_reg)
        beq cond_reg, zero, .end_loop
        statements->compile
    .update_loop:
        increment->compile
        j start_loop
    .end_loop:
    ```
### [Jump statements](../../src/ast/ast_jump_statement.cpp)

- `return`, `continue`, `break` - utilise the corresponding labels tracked by context


## Floats
- Requires use of additional set of [32 fp registers](../../include/context.hpp)
- Type checking and implementation done in relevant classes:
  - Function parameters and arguments - use argument regs `fa0...7`
  - Function definition - handle function return type
  - Identifier (e.g. r-value in an assignment) - load with correct operation based on type from bindings
  - Arithmetic - allocate regs + use correct operation based on type of left and right operands
- [Float literals](../../src/ast/ast_primitives.cpp) - have to be converted to bit representation
  - Use type punning via a union to convert float to IEEE754 representation
  - Floats binary representation loaded into integer register, temporarily stored on stack, then loaded into a fp register

## [Arrays](../../src/ast/ast_array.cpp)
- Arrays declarations require compiler to know the array size for memory allocation, so uses `getValue()` method in `BaseExpressions` to evaluate the size (e.g. `a[5]` where 5 is the size) and a specialized `getArraySize()` in `BaseDeclarations`.
- Array indexing:
  - As an r-value, load the memory address based on the index via `ArrayIndex`.
  - As an l-value in [Assignment](../../src/ast/ast_assignment.cpp), store data to the appropriate memory address calculated based on index, using `isArray()` in `BaseExpression` to distinguish.
- Array initialization calls the compile method on each item in the initialiser list. The address of the first array element is stored in a specific register by `InitDeclarator`, serving as a reference for offsetting during initialisation.

## Pointers

- [Variable struct](../../include/context.hpp) contains `is_pointer` field which is default-initialised to false, manually updated for pointers
- Pointer declarations involve the [Declaration](../../src/ast/ast_declaration.cpp) object checking `isPointer()` and storing in binding with `is_pointer` field set accordingly. `InitDeclarator` uses this for correct register allocation and storage instructions.
- Dereferenced pointers:
  - As an r-value, it loads the variable that is pointed to.
  - As an l-value, implemented in [Assignment](../../src/ast/ast_assignment.cpp), to store the rhs into the memory address it points to
- Pointer arithmetic:
  - Managed in `BinaryOperations`. When a pointer is involved, the other operand (an integer) is multiplied by the size of the type to which the pointer points.
  - A similar strategy is adopted for the dereferenced pointer notation like `a[1]`, interpreting the index as an offset, where this means pointer to `a + (1 * var_size)`

-----

## Future scope

### Globals

Declarations could be handled as following:

1) If a `Declaration` is being compiled when context is at `Scope[0]`, this means it is in the global scope - so can be given specific implementation, which would be to output the correct codegen eg `int x = 5;` should generate:
   ```
    .globl x
    .data
    .size x, 4
    x:
        .word 42
   ```
2) `Declaration` can add to the same bindings as previously, but the offset field can be specially marked as -1, to allow read / write codegen to use the correct operations for dealing with global variables stored in `.data` section of memory
    ```
    lui         a5,%hi(a)
    lw / sw     a4,%lo(a)(a5)
    ```

### Types implementation

- To support a more complex type system, a more hierarchical type system would be required, where basic types could be objects, and complex types would be composed of these objects.
- Meta-information like size, and other attributes, could also be embedded within the type itself for more dynamic type handling.
- For a simpler implementation, `typedef` / aliasing could however be handled at the lexer level, where a type_map is shared between the lexer and parser. The parser, upon detecting a typedef, updates this shared map, and the lexer, when encountering an identifier, checks against this map to determine if the identifier should be tokenised as a type or a variable name.
- This allows the lexer to dynamically adapt to user-defined type names, with minimal changes to the types implementation.

### Pointer implementation
  - Currently handle as a boolean flag `is_pointer`, but this doesn't extend well for multiple levels of indirection.
- A more versatile approach might be to represent pointers as a type wrapping another type.

