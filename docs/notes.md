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

