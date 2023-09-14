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

To use the *command line arguments* define `VRGOPTS` before including.

## Variadic functions
The `vrg` macro is a versatile tool that facilitates the creation of variadic functions in C, allowing for different functions to be called depending on the number of arguments passed to the macro. It automatically adds a suffix to the function name based on the number of arguments provided, streamlining the process of defining and using variadic functions.

### Description

The `vrg` macro takes the name of another macro (like `vfun`) and a variable list of arguments (`__VA_ARGS__`) to transform the macro call into a function call with a suffix that corresponds to the number of arguments passed. 

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
   #define vfun_01(a) vfun_02(a,0)   // make the second parameter optional and set 0 as default.

   void vfun_02(int a, int b) {  
    // Implementation of vfun(a,b)
   }

```

### Conclusion

The `vrg` macro brings significant benefits to the table when it comes to defining and using variadic functions in C. Here are two notable advantages:

1. **Handling Zero Arguments**: Traditional variadic functions in C cannot have zero arguments, as at least one argument is needed to determine where the list of variable arguments begins. However, with the `vrg` macro, you can neatly handle cases where no arguments are passed. This enhancement expands the versatility of variadic functions, permitting a broader range of use cases and functionalities.

2. **Type-Checking by the Compiler**: One of the inherent limitations of traditional variadic functions is the lack of type checking for variable arguments, which can lead to runtime errors and undefined behaviors. However, by using the `vrg` macro, each argument passed will be properly type-checked by the compiler, as it directs to a specific function designed to handle that exact number of arguments with specified types. This ensures that you catch type mismatches and related errors at compile time, promoting safer and more robust code.

In summary, the `vrg` macro enhances the safety and flexibility of variadic functions in C, making it easier to define functions that can handle a variable number of arguments with type safety and zero-argument compatibility. It promotes cleaner, more organized, and error-resistant code, which can be especially beneficial in complex, large-scale C programming projects.

