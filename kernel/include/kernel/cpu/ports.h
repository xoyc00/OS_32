#ifndef _PORTS_H
#define _PORTS_H

unsigned char inb (unsigned short port);
void outb (unsigned short port, unsigned char data);
unsigned short inw (unsigned short port);
void outw (unsigned short port, unsigned short data);
unsigned char* insw (unsigned short port);
void outsw (unsigned short port, unsigned char* data);

#endif
