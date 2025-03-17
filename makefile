all:
	mkdir build;cd build;cmake .. -DBUILD_EXAMPLES=OFF;cmake --build .;
clean:
	rm -fr build;