#ifndef DDE_H
#define DDE_H

void __dde_start(){};
void __dde_stop(){};

#define DDE_START __dde_start();
#define DDE_STOP __dde_stop();

#endif
