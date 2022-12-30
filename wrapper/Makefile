all:
	gcc cktso_wrapper.c -O3 -fPIC -shared -s -fvisibility=hidden -ffunction-sections -fdata-sections -Wl,--gc-sections -I../include -L../centos6_x64_gcc482 -lcktso -lcktso_l -o libcktso_wrapper.so