#pragma once

namespace dde {
static void start(){};
static void stop(){};
static void var(double* var_addr, const char *mark, int ordinal = -1){};
static void output(double* var_addr, const char *mark){};
static void dump_graph(){};
} // namespace dde
