Linux: 
Simply type "make" to compile the demos. To run the demos, set the environment variable "LD_LIBRARY_PATH" to the correct path that contains the *.so file (e.g., export LD_LIBRARY_PATH=../centos6_x64_gcc482), and put the license key file together with the *.so file.

Windows:
Create a console application project with Visual Studio and add demp.cpp/demo_l.cpp/benchmark.cpp into the project. Put cktso.h and cktso.lib/cktso_l.lib/cktso.lib in the project folder. The *.lib file is required in complilation. To run the demos, put cktso.dll/cktso_l.dll/cktso.dll together with the executable file and also put the license key file together with the *.dll file. The programs will directly exit when ended if they are launched by a double-click. To avoid this, add "getchar();" before exiting the main function, or launch the executable files from a command prompt.

The benchmark.cpp can be used to test the performance of CKTSO on matrix market files (an example of add20.mtx is provided), which can be downloaded from the SuiteSparse Matrix Collection (https://sparse.tamu.edu/).