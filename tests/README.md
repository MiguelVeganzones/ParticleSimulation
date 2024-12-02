# Tests

## Configuration of the test framework

To run these tests you'll need to have Bazel and Google tests installed

### Installing Bazel

To install bazel in an ubuntu Linux OS you'll need to follow the next steps extracted from the [Bazel installation guide](https://bazel.build/install?hl=es-419)
```
sudo apt install apt-transport-https curl gnupg -y
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
```
Then install it with sudo
```
sudo apt update && sudo apt install bazel
```
### Installing gtests (googletest)

Install it with sudo 
```
sudo apt install googletest
```

## How to run the tests

After installing bazel and google tests, to run the tests in the tests folder just run
``` 
bazel test --cxxopt=-std=c++14 --test_output=all //tests:test
```
To run it with a verbose error just run the `--sandbox_debug` flag in the previous command