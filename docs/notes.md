## Journal

**02/09/2023** Declarations stuff + stack frame

- Declaration class handles variable bindings in context, InitDeclarator used for initialisation
- FunctionDefinition should handle initial stack frame set up - finding the required size (for local vars) by asking compound_statements, which should then iterate through its declarations list and find the total required size
- Parameters should be dealt with by a FuncDeclarator object, which should have a list of parameters, and similar apporoach can be done as with declarations to find total size required in the frame for parameters
- Need to figure out base classes for some of these objects

**06/09/2023** Context

- Refactored context, should now work properly. Didn't need to store a vector of stack frames (which would be for the whole program), just needed to store scopes for each function. Should support nested scopes, will flesh out global variables later - should be pretty easy with scopes[0].

**07/09/2023** Variable declaration + initialisation

- Hierachy for variable declaration is as follows: `Declaration` (has type) -> `InitDeclarator` (initialisation) -> `Declarator` (variable name). In declaration, we addVar to variable bindings map - has type info available, however this also requires the variable name, which has to be fetched from the Declarator object.
- A possible implementation of having a tempdec object in context, that gets filled + carries information from various points of code gen - eg Declaration manages the object (reset + add to bindings at the end), Declarator would fill in the var name field, InitDeclarator would set initialisation. However this was dropped, in favour of using nested getIdentifier calls, as it is easier + quicker to implement, and reduces the chances of state leaks, where we loose track of what is modifying what.
- Right now we have identical BaseExpression and BaseDeclarator base classes, but I thought we'd start needing to separate later anyway, as we start implementing more stuff, and would rather have it split early.
- Nested scopes work as expected, with a new scope created and popped at the end in `CompoundStatement` codegen
- For now just support initialisted declaration only `int a = 5;` as we dont have the assignment operator yet - next thing to do

**08/09/2023** Operators

- Finished assignment, and started other arithmetic operators. Need to consider how to efficiently use registers - especially when expressions start being chained eg `int a = 1 + 2 + 3 + 4 + 5` results in `Addition` recursively from left to right. Can't really implement any kind of liveness analysis. Have hardcoded whatever register optimisations I could think of for add - fine for now. Might have to start storing stuff on the stack?

**09/09/2023** Parameters

- Having parameters now means I have to actually have a separate FunctionDeclarator that handles these params instead of just relying on default Declarator
- Added required grammar to paser w/ param_list + param_declarations
- Params are stored in regs a0-8, and extra params are stored at the bottom of bottom of previous stack frame (use positive offsets from s0)

**10/09/2023** Code review

-  Had to use dynamic_cast for getSize() of FundDeclarator to iterate through the vector of base Node objects, considered adjusting ListPtr to explcitly define more Lists, other than just vectors of Nodes eg BaseDeclaration, or use templates, so don't have to keep writing utility functions. Dynamic cast in this case isn't that bad as we can guarantee that the param_list contains BaseDeclaration objects - which have the getSize method.
- getSize() was meant to be only for BaseDeclaration, but now realised CompoundStatement requires it too. Really tried to avoid having to add stuff to base Node class, but might be inevitable to avoid pain. Visitor design pattern time? idrk how that works
- enum Types is unscoped, should switch it to a scoped enum - so use enum class to stop enum names leaking into global scope

**11/09/2023** Arguments
- Ended up creating a BaseStatement class anticipating more specialised methods, just has another getSize() for statements rn. Completed implementation to work out required stack size based on Declarations as well.
- Added handling arguments to `FunctionCall`. For less than 8 arguments, store in registers, and remaining go at the bottom of the stack.
- Too much effort to add type checking at this level, so just assuming extra's are ints when finding offset - TODO
- Problem is that when calculating required stack size at start of function, don't take into account any extra size needed for the extra arguments stored in the stack, so local variables can start to get overwritten due to insufficient stack size
- Handling this will mean adding a check to see if there is a function call, and increasing the size appropriately - implmenting this is long ngl - TODO
- Changed tests a bit, now `test.sh` take in an argument for the test folder - `/compiler_tests` or `/custom_tests` and the output is cleaned up a lot, with proper folder structure to make it easier to track down tests. `test_single.sh` takes in a relative path from `/custom_tests` for a single test to execute.
- Need to consider callee-saved registers. Ex `int i = f() + g();` s1 register is used to store the return value from function f temporarily while g is being called.

**12/09/2023** Arithmetic + logic operations
- Changed lexer to make tokens more clear + simpler. Extended parser to support the operations + statements we are trying to cover next.
- Seems like there will be lots of repeated code in setting up the registers with correct info before actually doing the op - which is pretty much one line most of the time. Might implement some kind of base class for these binary operations to reduce this repetition.
- Implemented base class BinaryOperations, which has prepLeft and prepRight methods, which should take care of loading the operands into the correct registers. Also made in anticipation that function calls in operands will also change implementation, so having these inherited methods should make it a lot quicker when refactoring in the future.
- Removed the "optimisation" in the arithmetic ops that was there earlier - which was freeing the destReg between the setting up the left and right operands. Was very naive and didn't actually work as expected, will try find another way but its rough.
- This made it a lot faster in implementing the rest of the operators - arithmetic and logic. All but LogicalAnd and LogicalOr left, which will take some time - special cases which have some jumps etc.

**13/09/2023** Control flow - while and if/else
- Added if else - doesn't contain optimisations for true or false within condition ~TODO fix reg allocation~
- Added while impentation, need to find a way to handle empty compound statements properly - ideally output a `nop`
- Haven't really thought about how nested stuff works.

**14/09/2023** Jump statements
- Was going through some of the compiler_tests and the if_true case wasn't passing. Looking at the assembly, the way we handled `return` was just wrong this entire time, didn't really get the chance to check it, since we didn't support selection until now. Basically if we have multiple returns, we want to jump to the end (function epilogue) whenever the return actually goes through, but all this time we just were moving stuff into a0 for fun thinking that was the return.
- To fix this, added a ret_label in context, which is created in FunctionDefinition, and `return` jumps to this label. This label is output before the function epilogue - so before the `mv a0, a5` and rest of function clean up - and marks the section of the code I want executed on function return. The `Return` statement is now under a JumpStatement class, which handles all the jump statements, and compiles the expression if it has one into register fixed register `a5` (in preparation of the mv later).
- Added support for `continue` and `break` which required tracking the current loops' labels. To make it easier, refactored while statement codegen, such that there is a clear start and end label. Context now also has functions to track these labels - support nesting.
- When doing recursive internal - the recursive call `return n+f(n-1);` really fks us up. Current arithmetic operators don't care if either of operands are function calls, and so set up the registers in either order, currently it's left then right operand. This means the following occurs:
    ```assembly
    lw t0, -20(s0)
    lw t2, -20(s0)
    li a3,1
    sub a0, t2, a3
    call f
    mv t1,a0
    lw t0, -29(s0)
    add a5, t0, t1
    ```
    where the `n` is loaded into t0 before the function call, and `add` expects it to still be there after :(. To just pass the test case, easy enough I can just change the operands around to prepRight first, but the problem would still be there. Need to add some kind of way to check if one of the operands are function calls, and handle appropriately.

**15/09/2023** Function call handling
- Added a `getFunctionCall()` method in `BaseExpression`. It's fully implemented in the `FunctionCall` class.
- Key classes needing this info are `FunctionDefinition` (to decide whether to save ra or not + increase stack size if more than 8 args) and `BinaryOperation` (to preserve the left operand).
- To distinguish between various function call scenarios, used a `FunctionCallInfo` struct that contains a boolean and extra argument count.
- Don't need to worry about multiple function calls; they occur sequentially. Use a max comparison for extra args.
- Issue: Function calls can appear almost anywhere, even in declarations like `int a = g()`. Tried implementing this virtual method in multiple base classes, but realised this would have to be overriden everywhere - just to have getFunctionCall available at the top level FunctionDefinition
- Decided this would add too much code bloat with the current implementation of our AST hierarchy, for a relatively insignificant case when having more than 8 arguments.
- Changed `getFunctionCall()` to only be a boleean used to check if the right operand is FunctionCall or not, and use callee-saved regs as appropriate allowing BinaryOperations and recursion to work properly.

**16/09/23** Fix stuff
- Implemented `LogicalOr` and `LogicalAnd` - took into account that these should short circuit
- Examining `/programs/sqrt` test, noticed that program didn't see the declarations within while statements - forgot to implement getSize for these
- Correctly implemented now for all the different statements - if/else, for, while
- Refactored `JumpStatement` to use a `JumpType` enum to be more safe
- Added a simple implementation for switch case, but doesn't behave properly with break yet. Case needs to be changed, such that it has a `case_start` label, which had been prepared by the previous case, which is what it branches to. Allows multiple statements after a case to work properly - which should then fix break.

**19/09/23** More operations
- Added Unary operations and the other assignment operations (+= etc).
- Due to the implementation of += etc, both `BinaryOperation` and `Assignment` had pointer to the left object, and so when deleting AST had to make sure only one of these objects were actually deleting the left object. Smart pointers would make this a lot easier, with reference counting, but had to use a kinda hacky fix to specifiy when the `BinaryOperation` actually had ownership, and let `Assignment` delete it.

**21/09/23** Start types
- Fix switch case implementation, used earlier suggested method of having previous switch case define the next case label, which is what the condition branches to. Required refactoring context `LoopLabels` slightly, to support defining a `start_label`, with no `end_label`, and still having the other methods work fine.
- Started types implementation, with the aim of supporting float and doubles. This required the use of a different set of 32 fp registers, which have been added to context, along with the mnemonics - extended the original data structure to reduce having to rewrite the same code.
- Main focus was on adding relevant type checking to generate and use correct assembly operator + registers in:
  - Function parameters (store into stack from argument regs fa0...7)
  - Identifier (load with correct op based on type from bindings)
  - Arithmetic (allocate regs + use correct op based on type of left and right operands)

**22/09/23** Complete types
- Function arguments of different types are stored in independent argument regs (a0...a7 and fa0...fa7), so changed implementation of arguments and parameters to use the correct regs
- Fixed destReg to pass the correct reg based on type
- Complete type support for float literals (double literals not supported)

**24/09/23** Array
- Array declarations need to know the actual size - have to add a `getValue()` to expressions, to allow compiler to allocate the correct space on stack
- Added a more appropriate `getArraySize()` to `BaseDeclarations`, as `Declaration` needs to both differentiate between normal variables and arrays, as well as appropriately changing Context. This getArraySize() is then overriden in InitDeclarator, which can return the size of the array by calling getValue() on its expression object.
- Array index:
  - When used as an r-value, needs to load correct memory address based on index (handle in `ArrayIndex`)
  - When used as an l-value, need to store to correct memory address based on index (handle in `Assignment`). However, at `Assignment` level, there is no easy way to differentiate between the l-value being a variable and an array (can't use getValue on ArrayIndex as the index could also be a variable, and would prefer not to have getValue assume multiple responsibilities). This lead to adding `isArray()` to `BaseExpression`, which should make other checks in code more readable as well
  - Another issue with array index as an l-value, is that the index is required at the `Assignment` level, however the AST structure is Assignment expr1-> ArrayIndex->Index, and we don't know if the index is a variable or value, so can't just use `getValue()`.
  - I considered having a method unique to `ArrayIndex`, which is used to correctly get the index into a specific destReg, and can then be reused inside the compile method of ArrayIndex, as well as Assignment higher up. An alternative would be to add a getNode or similar method, which returns a specific Node to a higher level. Taking into account that handling arrays declared at a global scope would require more specific implementation, decided to go with returning the Index pointer. Only needed in this specific case, so is not a virtual function, and instead `Assignment` casts the l-value expression to an `ArrayIndex` object when it is determined as an array access, and therefore is an ArrayIndex object

**26/09/23**
- Completed most of array implementation, but still need to handle initialiser lists.
- Main idea is to call compile on each of the initialiser objects in the list, and then store in the correct location in memory. However the array identifier, which is needed to find correct memory location + offset, is only known at the InitDeclarator level. This leaves two options:
  - Fetch the initialiser list object directly and handle all implementation at InitDeclarator level
  - InitDeclarator stores the memory address of array object in a certain register, which is then passed as the argument to compile on the initialiser list object, which can then offset from this reg (which has address of array element 0)
- Proceeded with option 2, which seemed to be cleaner (have not considered global scope at this point)

Pointers
- Dereferenced pointer (when used as an r-value), just loads the variable pointed to by the pointer - handle type later
- Dereferenced pointer (when used as an l-value), has to be handled in assignment. Now need to differentiate between pointers and other objects too.
- Initially considered adding pointer as a Specifier type, but that might require different int_ptr and float_ptr, and would also break existing switch statements
- Consider code snippet:
  ```C
  int f()
  {
      float a = 5.0;
      float* b = &a;
      return *b;
  }
  ```
  The variable b needs to store both that it is a float type, and is a pointer. Most reasonable method was to add isPointer as a field to the Variable struct, and default initialise to false, when constructed with just two args - minimising the impact on the rest of the codebase.
