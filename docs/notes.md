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
- Added if else - doesn't contain optimisations for true or false within condition - TODO fix reg allocation
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
- Plan is to create a getFunctionCall() virtual method in BaseExpression, and handle implementation in the FunctionCall class, which will then be passed up.
- Classes that require knowledge of whether there is a function call are: `FunctionDefinition` - store ra or not, and `BinaryOperation` - to save left operand to callee-saved reg.
- Ideally, I want to check FunctionCall with a simple if, but then it is hard to differentiate between a function call with no args (or less than 8), and a function call with 9 args. Could use -1 for no function call, 0 for <8 args, n for > 8 args - but now gets ugly. Used a FunctionCallInfo struct, that has a boolean + no of extra args.
- In terms of multiple function calls, don't think I need to care about them, as they will always happen one after the other, maybe have a max comparison for the extra args needed.
- Problem is... FunctionCall is an expression, and can be found basically anywhere, could even be a part of a declaration eg. `int a = g()`, so that means declaration objects also need to have this virtual method. Tried having it in all 3 base classes and still only just overriding in FunctionCall, but doesn't seem to work. I think I'm just bad with poplymorphism + how dynamic dispatch works. Will keep the functions and struct the same, as ideally it should work, and I manage to get the FuncCallInfo all the way up to FunctionDefinition to handle calls in a better way. Also means that the extra arg counting + stack sie increase doesn't work either.
- Managed to get BinaryOperations functioning properly when one is a function call, by saving to reg `s1` if there is a function call, and by default we also store and load s1 on the stack in FunctionDefinition too - pls fix at some point.
