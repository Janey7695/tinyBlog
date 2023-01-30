all:
	cd libhv;mkdir build;cd build;cmake .. -DBUILD_EXAMPLES=OFF;cmake --build .;cd ../../
	mkdir build;cd build;cmake ..;cmake --build .

clean:
	rm -fr build;