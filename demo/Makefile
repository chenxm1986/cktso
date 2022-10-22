all:
	g++ -O3 -std=c++11 demo.cpp -o demo -I ../include -L ../centos6_x64_gcc482 -lcktso
	g++ -O3 -std=c++11 demo_l.cpp -o demo_l -I ../include -L ../centos6_x64_gcc482 -lcktso_l
	g++ -O3 -std=c++11 benchmark.cpp -o benchmark -I ../include -L ../centos6_x64_gcc482 -lcktso
	g++ -O3 -std=c++11 demo_complex.cpp -o demo_complex -I ../include -I ../ext -L ../centos6_x64_gcc482 -lcktso