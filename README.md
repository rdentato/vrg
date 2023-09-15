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

### Conclusion

The `vrg` macro brings significant benefits to the table when it comes to defining and using variadic functions in C. Here are two notable advantages:

1. **Handling Zero Arguments**: Traditional variadic functions in C cannot have zero arguments, as at least one argument is needed to determine where the list of variable arguments begins. However, with the `vrg` macro, you can neatly handle cases where no arguments are passed. This enhancement expands the versatility of variadic functions, permitting a broader range of use cases and functionalities.

2. **Type-Checking by the Compiler**: One of the inherent limitations of traditional variadic functions is the lack of type checking for variable arguments, which can lead to runtime errors and undefined behaviors. However, by using the `vrg` macro, each argument passed will be properly type-checked by the compiler, as it directs to a specific function designed to handle that exact number of arguments with specified types. This ensures that you catch type mismatches and related errors at compile time, promoting safer and more robust code.

In summary, the `vrg` macro enhances the safety and flexibility of variadic functions in C, making it easier to define functions that can handle a variable number of arguments with type safety and zero-argument compatibility. It promotes cleaner, more organized, and error-resistant code, which can be especially beneficial in complex, large-scale C programming projects.

## Command line options

The `vrg` library also offers a way to facilitate the management of command-line switches in programs written in the C language. This section serves as a programmer's manual to guide developers on how to effectively utilize the `vrgswitch`, `vrgcase`, `vrgdefault`, `vrgoption`, `vrgusage`, and `vrgversion` elements of the `vrg` library to define and manage command-line switches within C programs.

To use these functions, ensure that the vrg.h file is included in your project, and define `VRGMAIN` in one and only one of the source files that include `vrg.h`.

```c
#define VRGMAIN
#include "vrg.h"
```

### `vrgversion`

`vrgversion` is a string variable where you can assign an informative message about the program, including version, copyright information, and others. This message will be printed when `vrgusage()` is called.

```c
vrgversion = "vrg test program (c) 2022 by me";
```

### `vrgswitch`

`vrgswitch` is a macro that initiates a block where you define various case blocks to handle the different command-line switches. It takes two arguments: `argc` and `argv`, which represent the count of command-line arguments and an array of pointers to those arguments, respectively (the same you got in your `main()` function).

```c
vrgswitch(argc, argv) {
    // ...
}
```

### `vrgcase`

`vrgcase` allows you to define a specific switch and associate an action to be performed when the switch is specified in the command line. A switch is defined as a string divided into two parts separated by a tab (`\t`), with the first part being the switch and the second part a description. The subsequent block contains the code to be executed if that switch is specified on the command line.

```c
vrgcase("h\tPrint help") {
    printf("You specified the '-h' switch with option '%s'\n", vrgoption);
}
```

### `vrgoption`

`vrgoption` is a variable that contains the option specified with a switch. It can be used within the `vrgcase` block to perform actions based on the specified option.

```c
printf("You specified the '-o' switch with option '%s'\n", vrgoption);
```
Options can be attached to the switch or be specified as the next argument. For example, the following two are fully equivalent:

```bash
  myprogram -o data.out

  myprogram -odata.out
```


### `vrgdefault`

`vrgdefault` provides a way to deal with unknown switches. Within this block, you can define what actions to take if an unknown switch is encountered.

```c
vrgdefault {
    fprintf(stderr, "WARNING: unknown switch -%c\n", argv[vrgargn][1]);
}
```

### `vrgusage`

`vrgusage` is a function that, when called, prints the list of defined switches (along with the message defined in `vrgversion`) and exits the program with an error. Note that the switches will appear in reverse order.

```c
vrgusage();
```

### Other Variables

- `vrgargn`: An integer variable that can be used to determine if there are any additional arguments to be processed in the command line. If it's equal to `argc`, no other arguments are present on the command line.

### Creating a Command Line Switches

When defining a switch with `vrgcase`, you need to specify whether the switch takes an option and whether it is mandatory or optional:

1. No option: The switch does not take any option.
2. Optional option: Enclose the option in brackets `[]`.
3. Mandatory option: Specify the option without brackets.

Examples:

```c
vrgcase("o [optional]\tYou may or may not specify an option for '-o'") {
    // ...
}

vrgcase("m mandatory\tYou must specify an argument for '-m'") {
    // ...
}
```

### Handling Additional Command Line Arguments

You can also process additional command-line arguments that are not covered by the switches defined in `vrgswitch`. You can use the `vrgargn` variable to determine if there are additional arguments and process them accordingly:

```c
if (vrgargn < argc) {
    printf("You have additional arguments:\n");
    for (int k = vrgargn; k < argc; k++) 
        printf(" %-2d '%s'\n", k, argv[k]);
}
```

### Switch stop

You can mark the end of the list of switches using `--`. In the example below, `-x` is not considered a switch:

```bash
  myprogram -m 123 -- -x
```

