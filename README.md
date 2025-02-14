# dde - dynamic DAG extractor

![DAG by DALLE](.github/dalle-dag.png)

`dde` is a tool based on Intel Pin, which attempts to extract a DAG representation
of an executabe.

Since the primary target application is the derivative evaluation based on the obtianed DAG,
the instructions tracked are only those from an engineered subset of the entire x86 ISA deemed
relevant for the DAG construction.


## Project structure

The project is devided into 3 primary components (associated with the following folders):

```
-- src
  Core sources for the tool's implementation.

-- targets
  A demo target program which provides a use case for the tool.

-- tests
  A set of tests (both unit and integration) to provide some reassurance about tool's correctness.
```


## Prerequisites and setup

**System disclaimer:**

- The tool requires either native or emulated x86 processor.
- Development and tests were carried out on *Ubuntu 22.04* with *g++ 11.0*.
- Pin-3.31 was used for development and verification. There is a potential for incompatibility between newer and older versions.

<details>
  <summary>Required tools</summary>

- GNU Make
- GNU C++ compiler
- Properly setup Intel Pin Environement

Both GNU make and the compilation stack can be installed on the Linux-based system using the following commands:

```bash
sudo apt-get update
sudo apt-get install build-essential
```

To verify that the tools are now available to you execute this command:

```bash
make --version
g++ --version
```

</details>

<details>
  <summary>Pin setup</summary>

This setup is only valid for Linux.

- Grab Intel Pin from [here](https://software.intel.com/sites/landingpage/pintool/downloads/pin-external-3.31-98869-gfa6f126a8-gcc-linux.tar.gz).

```bash
# Load the file into a current directory
wget https://software.intel.com/sites/landingpage/pintool/downloads/pin-external-3.31-98869-gfa6f126a8-gcc-linux.tar.gz
```

- Extract the downloaded file into the directory of your choosing.

```bash
tar -xf <pin-tar-file-name>
```

- Define an infrastructure critical environment variable.

```bash
export PIN_ROOT=$(pwd)/<pin-dir> 
```

- Append this variable to the path.

```bash
export PATH=$(PATH):$(PIN_ROOT)
```

- Tip - add both of the above commands to your `.bashrc`, save it, and source it for the changes to take action.
- Run the following set of commands to build the entire project and launch a test run.

```bash
# First build
make

# Run the test case
pin -t src/obj-intel64/dde.so -- targets/main.exe
```

- If you get a successful build and a set of solutions printed out to your screen congratulations you are good to go!
</details>
