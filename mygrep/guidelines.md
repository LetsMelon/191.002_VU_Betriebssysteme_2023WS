[ ] All source files of your program(s) must compile with ... without warnings and info messages and your program(s) must link without warnings.

[ ] There must be a Makefile implementing the targets: all to build the program(s) (i.e. generate executables) from the sources (this must be the first target in the Makefile); clean to delete all files that can be built from your sources with the Makefile.

[ ] All targets of your Makefile must be idempotent. I.e. execution of make clean; make clean must yield the same result as make clean, and must not fail with an error.

[ ] The program shall operate according to the specification/assignment without major issues (e.g., segmentation fault, memory corruption).

[ ] Arguments have to be parsed according to UNIX conventions ...

[ ] Correct (=normal) termination, including a cleanup of resources.

[ ] Upon success the program has to terminate with exit code 0, in case of errors with an exit code greater than 0. We recommend to use the macros `EXIT SUCCESS` and `EXIT FAILURE` (defined in `stdlib.h`) to enable portability of the program.

[ ] If a function indicates an error with its return value, it should be checked in general. If the sub- sequent code depends on the successful execution of a function (e.g. resource allocation), then the return value must be checked.

[ ] Functions that do not take any parameters have to be declared with void in the signature, e.g., `int get_random_int(void)`;.

[ ]  Procedures (i.e., functions that do not return a value) have to be declared as void.

[ ]  Error messages shall be written to stderr and should contain the program name argv[0].

[ ]  It is forbidden to use the functions: `gets`, `scanf`, `fscanf`, `atoi` and `atol` to avoid crashes due to invalid inputs. ...

[ ]  Documenation is mandatory. Format the documentation in Doxygen style (see Wiki and Doxygen’s intro).

[ ]  Write meaningful comments. For example, meaningful comments describe the algorithm, or why a particular solution has been chosen, if there seems to be an easier solution at a first glance. Avoid comments that just repeat the code itself

[ ]  The documentation of a module must include: name of the module, name and student id of the author (@author tag), purpose of the module (@brief, @details tags) and creation date of the module (@date tag). Also the Makefile has to include a header, with author and program name at least.

[ ]  Each function shall be documented either before the declaration or the implementation. It should include purpose (@brief, @details tags), description of parameters and return value (@param, @return tags) and description of global variables the function uses (@details tag). You should also document static functions (see EXTRACT STATIC in the file Doxyfile). Document visible/exported functions in the header file and local (static) functions in the C file. Document variables, constants and types (especially structs) too.

[ ]  Documentation, names of variables and constants shall be in English.

[ ]  Internal functions shall be marked with the static qualifier and are not allowed to be exported (e.g., in a header file). Only functions that are used by other modules shall be declared in the header file.

[ ]  All exercises shall be solved with functions of the C standard library. If a required function is not available in the standard library, you can use other (external) functions too. Avoid reinventing the wheel (e.g., re-implementation of `strcmp`).

[ ]  Name of constants shall be written in upper case, names of variables in lower case (maybe with fist letter capital).

[ ]  Use meaningful variable and constant names (e.g., also semaphores and shared memories).

[ ]  Avoid using global variables as far as possible.

[ ]  All boundaries shall be defined as constants (macros). Avoid arbitrary boundaries. If boundaries are necessary, treat its crossing.

[ ]  Avoid side effects with `&&` and `||`, e.g., write `if(b != 0) c = a/b`; instead of `if(b != 0 && c = a/b)`.

[ ]  Each switch block must contain a default case. If the case is not reachable, write assert(0) to
this case (defensive programming).

[ ]  Logical values shall be treated with logical operators, numerical values with arithmetic operators (e.g., test 2 strings for equality by `strcmp(...) == 0` instead of `!strcmp(...)`).

[ ]  Indent your source code consistently (there are tools for that purpose, e.g., indent).

[ ]  Avoid tricky arithmetic statements. Programs are written once, but read more times. Your program is not better if it is shorter!

[ ]  For all I/O operations ...

[ ]  If asked in the assignment, you must implement signal handling (`SIGINT`, `SIGTERM`). You must only use async-signal-safe functions in your signal handlers.

[ ]  Close files, free dynamically allocated memory, and remove resources after usage.

[ ]  Don’t waste resources due to inconvenient programming. Header files shall not include implementation parts (exception: macros).

[ ]  To comply with the given testcases, the program output must exactly match the given specification. Therefore you are only allowed to to print any debug information if the compile flag `-DDEBUG` is set.
