cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Release -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make -C Release VERBOSE=1 -j $(nproc)