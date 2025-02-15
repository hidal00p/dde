# dde - dynamic DAG extractor

<div align="left">

![test](https://github.com/hidal00p/dde/actions/workflows/test.yml/badge.svg)

</div>

![DAG by DALLE](.github/dalle-dag.png)

`dde` is a tool based on Intel Pin, which attempts to extract a DAG representation
of an executabe.

Since the primary target application is the derivative evaluation based on the obtianed DAG,
the instructions tracked are only those from an engineered subset of the entire x86 ISA deemed
relevant for the DAG construction.

<details>
  <summary>Runtime and overhead estimations</summary>

```text
Test               Max [ms]    Min [ms]    Mean [ms]    Total [ms]  Overhead [dde / raw]
---------------  ----------  ----------  -----------  ------------  ----------------------
mul                0.000397    1.9e-05   2.17061e-05      0.217039  -
mul dde            0.582912    0.009716  0.0104572      104.562     511
add                0.003472    1.9e-05   2.30744e-05      0.230721  -
add dde            0.278239    0.009635  0.0102653      102.643     507
sub                0.000388    2e-05     2.23188e-05      0.223166  -
sub dde            0.183274    0.009777  0.0105846      105.836     488
div                6.2e-05     2e-05     2.26752e-05      0.226729  -
div dde            0.182987    0.009662  0.0102604      102.593     483
sin                0.001739    3.5e-05   4.0124e-05       0.4012    -
sin dde            0.185833    0.007373  0.00781787      78.1709    210
compound           0.001273    7.9e-05   8.56864e-05      0.856778  -
compound dde       0.287404    0.038859  0.0407766      407.726     491
compound_sa        0.003531    8.4e-05   8.81544e-05      0.881456  -
compound_sa dde    0.224431    0.03944   0.042315       423.108     469
```
</details>


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
