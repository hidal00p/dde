#ifndef DDE_H
#define DDE_H

void __dde_start(){};
void __dde_stop(){};

void __dde_mark_start(const char *mark){};
void __dde_mark_stop(){};

#define DDE_START __dde_start();
#define DDE_STOP __dde_stop();
#define DDE_MARK(mark, decl_expr)                                              \
  __dde_mark_start(mark);                                                      \
  decl_expr;                                                                   \
  __dde_mark_stop();

#endif
