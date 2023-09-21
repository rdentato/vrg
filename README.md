# vrg
Dealing with arguments

This single header library provides two different functionalities:

   - Define functions with a variable number of argument in a simpler way than `stdarg.h`)
   - Deal with command line options and switches in a simpler way than `getopt()`)

Both *simpler* are, of course, IMHO!

See my article on [DEV.to](https://dev.to/rdentato/default-parameters-in-c-4kl1) for variadic functions
See the vrgtest.c for a rather complete example on command line options.
See the makefile for how to include it in your project 
See the source code for further documentation.

Feel free to contact me if you have any question/feedback/suggestion. 
Join the [my Discord Server](https://discord.gg/gENEmAmxE2) to join the Community.

## Usage

Just include the `vrg.h` header in your code.

To use the *command line arguments* define `VRGMAIN` before including.

## Variadic functions
The `vrg` macro is a versatile tool that facilitates the creation of variadic functions in C, allowing for different functions to be called depending on the number of arguments passed to the macro. It automatically adds a suffix to the function name based on the number of arguments provided, streamlining the process of defining and using variadic functions.

### Description

The `vrg` macro takes the base name of function (like `vfun`) and a variable list of arguments (`__VA_ARGS__`) to transform the macro call into a function call with a suffix that corresponds to the number of arguments passed. 

### Examples

You define a variadic function by definining it as a macro:

```c
   #define vfun(...) vrg(vfun, __VA_ARGS__)
```
that will be expanded differently depending on the number of arguments:

1. **No Arguments**:
   When no arguments are passed, the suffix `01` is appended to the function name.
   
   ```c
   vfun();  // This will call vfun01()
   ```

2. **Single Argument**:
   When one argument is passed, the suffix `_1` is appended to the function name.
   
   ```c
   vfun(a);  // This will call vfun_1(a)
   ```

3. **Two Arguments**:
   When two arguments are passed, the suffix `_2` is appended to the function name.
   
   ```c
   vfun(a, b);  // This will call vfun_2(a, b)
   ```

And so on for higher numbers of arguments up to 8.

You would then define your functions with the respective suffixes to handle the different cases:

```c
void vfun01() {
   // Implementation for no argument
}

void vfun_1(int a) {
   // Implementation for one argument
}

void vfun_2(int a, int b) {
   // Implementation for two arguments
}

// ... and so on for more arguments
```

This setup allows for a clear, organized structure when working with variadic functions, enhancing readability and maintainability of your C code.

### Optional arguments with default values

You can also use it for defining default parameters. For example:

```c

   #define vfun(...) vrg(vfun, __VA_ARGS__)
   #define vfun01()  vfun_2(1,0)   // make both arguments optional and set by default a = 1, b = 0
   #define vfun_1(a) vfun_2(a,0)   // make second argument optional and set it to 0 by default.

   void vfun_2(int a, int b) {  
    // Implementation of vfun(a,b)
   }

   // vfun()     will become vfun_2(1,0)
   // vfun(2)    will become vfun_2(2,0)
   // vfun(2,3)  will become vfun_2(2,3)

```

### Summmry

The `vrg` macro brings significant benefits to the table when it comes to defining and using variadic functions in C. Here are two notable advantages:

1. **Handling Zero Arguments**: Traditional variadic functions in C cannot have zero arguments, as at least one argument is needed to determine where the list of variable arguments begins. However, with the `vrg` macro, you can neatly handle cases where no arguments are passed. This enhancement expands the versatility of variadic functions, permitting a broader range of use cases and functionalities.

2. **Type-Checking by the Compiler**: One of the inherent limitations of traditional variadic functions is the lack of type checking for variable arguments, which can lead to runtime errors and undefined behaviors. However, by using the `vrg` macro, each argument passed will be properly type-checked by the compiler, as it directs to a specific function designed to handle that exact number of arguments with specified types. This ensures that you catch type mismatches and related errors at compile time, promoting safer and more robust code.

In summary, the `vrg` macro enhances the safety and flexibility of variadic functions in C, making it easier to define functions that can handle a variable number of arguments with type safety and zero-argument compatibility. It promotes cleaner, more organized, and error-resistant code, which can be especially beneficial in complex, large-scale C programming projects.

## Command line options

The VRG library provides a convenient way to parse command-line arguments for C programs. It offers a high-level abstraction for argument parsing through a simple and intuitive API. This manual describes how to use the `vrgcli()`, `vrgarg()`, and `vrgusage()` functions.


### Getting Started

Include the VRG header in your program and define `VRGMAIN` in one (and only one) source file that includes `vrg.h`:

```c
#define VRGMAIN
#include "vrg.h"
```

### vrgcli()

Encloses the actions to perform when flags and arguments are encountered. Takes as an argument an optional informative message on the program (e.g., version, copyright) that will be printed by `vrgusage()`.

```c
vrgcli("vrg test program (c) 2022 by me") {
  // ... vrgarg() blocks to handle flags and arguments
}

```

By default, `vrgcli()` assumes that the idiomatic `argc` and `argv` variables are used to access the command line arguments. Should this not be the case, you can specify them:

```c
vrgcli("my nice program v0.0.1",counter_of_args, array_of_args) {
   // ...
}
```
If you don't want to print any informative message, you can pass NULL as first argument (or just nothing if the default `argc` and `argv` are used):

```c
vrgcli() {
   // ...
}
```

### Arguments and Description Formatting

In the VRG library, each flag or argument string is divided into two parts separated by a tab character `\t`. The first part is the flag or argument specification, and the second part is a textual description of the flag or argument.

For example, in the string:

```c
"-m mandatory\tDescription of the flag"
```

- The part before the tab (`-m mandatory`) is the actual flag or argument specification that the user has to input in the command line.
  
  - Flags that start with a dash `-` are options (e.g., `-m`).
  
  - If a flag or argument is mandatory, it is simply written as is (e.g., `mandatory`).
  
  - If a flag or argument is optional, it is enclosed in square brackets (e.g., `[optional]`).

- The part after the tab (`Description of the flag`) is the text that will be displayed when the `vrgusage()` function is called to print the usage instructions.

Therefore, in this example, if a user inputs an incorrect flag and `vrgusage()` is called, the output will include:

```text
-m mandatory   Description of the flag
```

This structure allows you to both describe how the flag or argument should be used and provide useful information that will be displayed in the help message.

### vrgarg()

Encloses the code that must be executed when a specific flag or argument is found. The `vrgarg` variable inside the block holds a pointer to the argument itself (or the flag option).

```c
vrgarg("-o [optional]\tYou may or may not specify an option for '-o'") {
  // ... Your code here
}
```

### vrgusage()

Prints the list of flags and arguments in the order they are specified within `vrgcli` and then exits with an error. You can optionally specify an error message to print before the usage using the same syntax as `printf()`.

```c
vrgusage("ERROR: unknown flag '-%c'\n", vrgarg[1]);
```

## Variables

- **vrgarg**: A `char` pointer that points to the argument value of the current flag or argument.
- **vrgargn**: An integer variable that stores the index of the next command-line argument to be processed.
  

## Examples

### Flags with No Options

```c
vrgarg("-h\tPrint help") {
  printf("You specified the '-h' flag with option '%s'\n", vrgarg);
  vrgusage();
}
```

### Flags with Optional Options

```c
vrgarg("-o [optional]\tYou may or may not specify an option for '-o'") {
  printf("You specified the '-o' flag with option '%s'\n", vrgarg);
}
```

### Flags with Mandatory Options

```c
vrgarg("-m mandatory\tYou must specify an argument for '-m'") {
  printf("You specified the '-m' flag with option '%s'\n", vrgarg);
}
```

### Positional Arguments

```c
vrgarg("inputfile\tThe name of the file to process") {
  printf("File to be processed: `%s`\n", vrgarg);
}
```
### Optional Positional Arguments

```c
vrgarg("[outputfile]\tThe name of the file to create") {
  printf("File to be created: `%s`\n", vrgarg);
}
```

### Unknown Flags and Extra Arguments

You can use a `vrgarg()` block with no argument to capture any other
argument that has not been handled:

```c
vrgarg() {
  if (vrgarg[0] == '-') {
    vrgusage("ERROR: unknown flag '-%c'\n", vrgarg[1]);
  }
  fprintf(stderr, "Additional argument: '%s'\n", vrgarg);
}
```

## Flexible Argument Specification

One of the noteworthy features of VRG is its flexibility in handling arguments attached to flags. VRG allows you to specify arguments either directly after the flag or as a separate argument. For instance, both `-x32` and `-x 32` are considered equivalent when parsed by VRG. This gives users the flexibility to input arguments in a manner that's most intuitive for them and also simplifies the parsing logic you have to write. This feature is particularly useful for applications that require quick command-line inputs, as it allows for both compact and spaced syntax, catering to a broader range of user preferences.

## Validation Functions

In addition to simple command-line parsing, the VRG library also provides the capability to validate arguments through custom validation functions. This feature allows you to enforce specific constraints on the values that an argument can assume, making your CLI more robust and user-friendly.

Typical uses cases are to check that an argument is an integer within a specified range, or that the specified file exists and is readable.

### Defining a Validation Function

A validation function is a simple C function that takes a string argument (and optionally, other parameters) and returns 1 if the string is a valid argument, or 0 otherwise. The signature of such a function should be as follows:

```c
int my_validation_function(char *arg) {
  // Validation logic here
}
```

### Example: Checking for an Integer

Here's an example of a validation function that checks if a given string is an integer:

```c
// Check if the string is an integer
int isint(char *arg) {
  if (arg == NULL || *arg == '\0') return 0;
  for(; *arg; arg++) if (*arg < '0' || '9' < *arg) return 0;
  return 1;
}
```

To use a validation function, you simply pass it as an additional argument when defining a command-line argument using `vrgarg()`.

Here's how you would define a command-line argument that uses the `isint` validation function:

```c
vrgarg("-n slots\tThe number of allowed slots", isint) {
  // Your code here
}
```

With this setup, an error will be thrown if the argument specified for `-n` is not an integer, making your CLI more resilient against incorrect inputs.


### Example: Checking for a positive Integer

VRG's validation functions are designed to be versatile, supporting not only the argument value but also additional parameters. This feature allows you to make your validation functions more dynamic and adaptable to various use-cases.

Let's consider an example where you want to validate that an integer argument is greater than a specific number. You can define the following validation function:

```c
// Check if the string is an integer greater than n
int isgreaterthan(char *arg, int n) {
  if (arg == NULL || *arg == '\0') return 0;
  if (atoi(arg) <= n) return 0;
  return 1;
}
```

In this function, `n` is an additional parameter that specifies the lower limit for valid integers.

To use a validation function that takes additional parameters, you would pass those parameters along with the function in your `vrgarg()` definition like so:

```c
vrgarg("-n slots\tThe number of allowed slots", isgreaterthan, 0) {
  // Your code here
}
```

In this example, the `isgreaterthan` validation function is passed an extra parameter `0`, meaning the function will only validate integers that are greater than zero.

### Example: File Existence

Another common use-case for validation in command-line interfaces is to check whether a specified file exists and is readable. This could be particularly useful for programs that read data from files. For this, we can define a validation function called `isfile`.

Here is how you can define this function:

```c
// Check if the specified file exists and is readable.
int isfile(char *arg) {
  if (arg == NULL || *arg == '\0') return 0;
  FILE *f = fopen(arg, "rb");
  if (f == NULL) return 0;
  fclose(f);
  return 1;
}
```

In this function, we try to open the file in read-binary mode. If the `fopen` call returns `NULL`, it means the file could not be opened, either because it does not exist or because it is not readable. Otherwise, we close the file and return 1, indicating that the file exists and is readable.

You can then use this `isfile` validation function with `vrgarg` as follows:

```c
vrgarg("-f file\tSpecify the input file", isfile) {
  // Your code here
  // For example, you can now safely read the file knowing it exists and is readable.
}
```

With this setup, if the user specifies a file that does not exist or is not readable, `vrgarg` will throw an error, thereby preventing any subsequent issues related to file handling in your application.

## Limitations: Short Options Only

While VRG offers a fantastic range of features wrapped in a simplified interface, it's essential to note that, as of its current version, the library only supports short options (e.g., `-h` for help, `-f` for file). Long options like `--help` or `--file` are not available. This limitation might not be a deal-breaker for small to medium-sized projects, where short options could suffice. However, for larger, more complex applications where long, descriptive option names could enhance readability and usability, this could be a limitation worth considering. Nonetheless, the simplicity and ease-of-use that VRG offers may outweigh this constraint for many developers.

## Summary

With the VRG library, parsing command-line arguments in C programs becomes more straightforward and maintainable. This manual aimed to explain the key functionalities provided by this library, and we hope that it helps you in writing better C programs.
