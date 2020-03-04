#ifndef _PORTS_H
#define _PORTS_H

//Read a byte from the specified port
unsigned char inb (unsigned short port);

//Write a byte from the specified port
void outb (unsigned short port, unsigned char data);

//Read a word from the specified port
unsigned short inw (unsigned short port);

//Write a word from the specified port
void outw (unsigned short port, unsigned short data);

//Read a long from the specified port
unsigned int inl (unsigned int port);

//Write a long from the specified port
void outl (unsigned int port, unsigned int data);

#endif
