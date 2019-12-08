#include <kernel/driver/mouse.h>
#include <kernel/cpu/ports.h>
#include <stdint.h>
#include <stddef.h>

//Mouse.inc by SANiK
//License: Use as you wish, except to cause damage
unsigned char mouse_cycle=0;     //unsigned char
char mouse_byte[3];    //signed char
int mouse_x=640;         //signed char
int mouse_y=512;         //signed char

//Mouse functions
void mouse_handler()
{
  switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inb(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inb(0x60);
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2]=inb(0x60);
      mouse_x+=mouse_byte[1];
      mouse_y-=mouse_byte[2];
      mouse_cycle=0;

      break;
  }
}

inline void mouse_wait(uint8_t a_type) //unsigned char
{
  uint32_t _time_out=100000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((inb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

inline void mouse_write(uint8_t a_write) //unsigned char
{
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  outb(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  outb(0x60, a_write);
}

uint8_t mouse_read()
{
  //Get's response from mouse
  mouse_wait(0);
  return inb(0x60);
}

void mouse_init()
{
  unsigned char _status;  //unsigned char

  //Enable the auxiliary mouse device
  mouse_wait(1);
  outb(0x64, 0xA8);
  mouse_read();
 
  //Enable the interrupts
  mouse_wait(1);
  outb(0x64, 0x20);
  mouse_wait(0);
  _status=(inb(0x60) | 2);
  _status=_status|(0x20 >> 5);
  mouse_wait(1);
  outb(0x64, 0x60);
  mouse_wait(1);
  outb(0x60, _status);
 
  //Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  //Acknowledge
 
  //Enable the mouse
  mouse_write(0xF4);
  mouse_read();  //Acknowledge
}

void mouse_getposition(int* x, int* y) {
	*x = mouse_x;
	*y = mouse_y;
}
