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
