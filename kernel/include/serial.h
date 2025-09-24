#ifndef SERIAL_H
#define SERIAL_H


int init_serial();

void write_serial(char a);
char read_serial();

void outb(uint16_t port, uint8_t val);

#endif