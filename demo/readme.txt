Linux: 
	Simply type "make" to compile the demos.
	Set the environment variable "LD_LIBRARY_PATH" to the correct path that contains the *.so file (e.g., export LD_LIBRARY_PATH=../centos6_x64_gcc482).
	Put the license key file together with the *.so file.
	Run the demos.

Windows:
	Create a console application project with Visual Studio and add one of demp.cpp, demo_l.cpp and benchmark.cpp into the project. 
	Put cktso.h and cktso.lib (for demo.cpp or benchmark.cpp) or cktso_l.lib (for demo_l.cpp) in the project folder. The *.lib file is required during complilation.
	Compile the project.
	Put cktso.dll (for demo.cpp or benchmark.cpp) or cktso_l.dll (for demo_l.cpp) together with the generated executable file. The *.dll file is required for running.
	Put the license key file together with the *.dll file. 
	Run the demo. The console window will directly exit when ended if it is launched by a double-click. To avoid this, add "getchar();" before exiting the main function, or launch the executable file from a command prompt.

The benchmark.cpp can be used to test the performance of CKTSO on matrix market files (an example of add20.mtx is provided), which can be downloaded from the SuiteSparse Matrix Collection (https://sparse.tamu.edu/).