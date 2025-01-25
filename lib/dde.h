#ifndef DDE_H
#define DDE_H

void __dde_start(){};
void __dde_stop(){};

void __dde_mark_start(const char *mark, bool output = false){};
void __dde_mark_stop(){};

void __dde_dump_graph(){};

#define DDE_START __dde_start();
#define DDE_STOP __dde_stop();

#define DDE_VAR(mark, decl_expr)                                               \
  __dde_mark_start(mark);                                                      \
  decl_expr;                                                                   \
  __dde_mark_stop();

#define DDE_OUTPUT(mark, decl_expr)                                            \
  __dde_mark_start(mark, true);                                                \
  decl_expr;                                                                   \
  __dde_mark_stop();

#define DDE_DUMP_GRAPH __dde_dump_graph();

#endif
