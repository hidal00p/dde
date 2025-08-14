<h1 align="center">dde - dynamic DAG extractor</h1>

<div align="left">

![test](https://github.com/hidal00p/dde/actions/workflows/test.yml/badge.svg)

</div>

---

`dde` is a tool based on Intel Pin, which attempts to extract a DAG representation
of a user-marked portion of an executable.

Since the primary target application is the derivative evaluation based on the obtained DAG,
the instructions tracked are only those from an engineered subset of the entire x86 ISA deemed
relevant for the DAG construction.


<details>
  <summary>Runtime and overhead estimations</summary>

```text
Test                Max [ms]    Min [ms]    Mean [ms]    Total [ms]  Overhead [dde / raw]
----------------  ----------  ----------  -----------  ------------  ----------------------
mul                 0.865513    2.5e-05   0.000115224      1.15224   -
mul dde             2.68702     0.015267  0.0186773      186.773     162.0963
add                 0.252552    2.5e-05   6.90574e-05      0.690574  -
add dde             1.71118     0.016764  0.018505       185.05      267.9651
sub                 0.257937    2.6e-05   5.37878e-05      0.537878  -
sub dde             1.44957     0.016768  0.0188062      188.062     349.6377
div                 0.26628     2.6e-05   5.55229e-05      0.555229  -
div dde             1.44847     0.015829  0.0185059      185.059     333.3028
sin                 1.37289     0.000118  0.000329713      3.29713   -
sin dde             2.5715      0.012942  0.0149685      149.685     45.3985
compound            2.2045      0.00046   0.000821527      8.21527   -
compound dde        5.22581     0.055423  0.0675805      675.805     82.2621
compound_sac        1.3037      0.000487  0.000697094      6.97094   -
compound_sac dde    4.37545     0.066463  0.0860439      860.439     123.4322
```
</details>


## Supported list of instructions

`mov`
`movq`
`movzx`
`movapd`
`movsd`
`movsdxmm`

`addsd`
`mulsd`
`subsd`
`divsd`

`call` to `sin` `cos` `exp`


<details>
  <summary>Unsupported instructions</summary>

- Instructions related to floating-point bit tricks, e.g. `xord` to swap the sign bit
- VEX instructions, e.g., `vmovsd` `vmulsd` etc
- SIMD fused instructions, e.g., `vfmadd213sd` `vfnmadd213sd` etc (to extract DAG of SDL routines)
</details>


## Mechanics kurzgesagt

This is a binary instrumentation tool, which tracks memory, arithmetic and certain
call instructions to construct a computational graph of the executed program (or more
specifically of the subprogram of interest).

Internally the tool builds a hash map of nodes, which are associated with
memory addresses of variables of interest. Move and arithmetic operations
applied to those address are also applied to the corresponding nodes to have
coherence between the memory state and the node state. All operations that
are applied to other variables are ignored to reduce the size of the DAG, and
tracking overhead.


## Project structure

The project is divided into 3 primary components (associated with the following folders):

```
-- src
  Core sources for the tool's implementation.

-- examples
  Demo programs which show possible ways to use the tool.

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
- Properly setup Intel Pin Environment

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
pin -t src/obj-intel64/dde.so -- examples/newton.exe
```

- A success is indicated by a successful build of the program and a set of solutions printed out to the screen!
</details>
