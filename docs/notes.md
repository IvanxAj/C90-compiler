## Journal

**02/09/2023** Declarations stuff + stack frame

- Declaration class handles variable bindings in context, InitDeclarator used for initialisation
- FunctionDefinition should handle initial stack frame set up - finding the required size (for local vars) by asking compound_statements, which should then iterate through its declarations list and find the total required size
- Parameters should be dealt with by a FuncDeclarator object, which should have a list of parameters, and similar apporoach can be done as with declarations to find total size required in the frame for parameters
- Need to figure out base classes for some of these objects
