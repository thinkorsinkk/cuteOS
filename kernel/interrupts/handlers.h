#ifndef HANDLERS_H
#define HANDLERS_H

typedef void (*handler_t[2])(void);
extern handler_t handlers[2];

void df(void);
void pf(void);

#endif