# Tests

## Configuration of the test framework

To run these tests you'll need to have Bazel and Google tests installed


### Installing gtests (googletest)

Install it with sudo and get the directory where googletest is located
```
sudo apt install googletest
whereis googletest
```
Build it as a shared library
```
cd /usr/src/googletest/googletest
sudo cmake -DBUILD_SHARED_LIBS=ON .
sudo make
```
Then you can copy the .so file to one of libraries that your linker searches on
```
sudo cp libgtest*.so /usr/lib
```
You might need to refresh the linker with `sudo ldconfig`

## How to run the tests

After installing bazel and google tests, to run the tests in the tests folder just run
``` 
make tests
```
