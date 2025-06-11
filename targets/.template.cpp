#include "dde.h"

int main() {
    dde::start();

    dde::var("x");
    double x = 1.0;
    dde::endvar();

    // the 2nd argument - an output flag
    dde::var("y", true);  
    double y = 0.0;
    dde::endvar();

    // TODO: add your function below
    // y = my_function(x);

    dde::stop();

    // writes the graph into /tmp/prog.gr
    dde::dump_graph();

    return 0;
}

