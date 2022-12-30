The purpose of this code is to provide a wrapper of CKTSO for languages that do not have pointers to pointers. A set of functions with prefix CKTSO2_ is provided to replace the original functions. Pointers to pointers are not involved in the interface of the new functions. The source code of the wrapper is provided so that you can modify the code as per your requirement.
Usage:
1. Compile cktso_wrapper.c to a dynamic linked library (libcktso_wrapper.so on Linux or cktso_wrapper.dll on Windows). Prebuilt libraries are provided for Win7/10/11 and Linux mainstream distributions.
2. Call the CKTSO2_* functions using a language. You may need to first declare the functions using the target language based on the C declaration. Both the wrapper library and the original libraries are needed.
Notes:
1. You do not need to handle the members of struct __cktso_wrapper or __cktso_l_wrapper. __cktso_wrapper * or __cktso_l_wrapper * can be treated as void * (an opaque) when calling the CKTSO2_* functions.
2. Different from the original libraries, both 32-bit and 64-bit integer wrapper functions are compiled in a single library.