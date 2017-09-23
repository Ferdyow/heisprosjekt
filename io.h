// Wrapper for libComedi I/O.
// These functions provide and interface to libComedi limited to use in
// the real time lab.
//
// 2006, Martin Korsgaard
#ifndef __INCLUDE_IO_H__
#define __INCLUDE_IO_H__

//Initialize libComedi in "Sanntidssalen"
int io_init();

//@param channel: Channel bit to set.
void io_set_bit(int channel);

//@param channel: Channel bit to clear.
void io_clear_bit(int channel);

//writes value to channel
void io_write_analog(int channel, int value);

//read bit from digital channel
int io_read_bit(int channel);

//read bit from analog channel
int io_read_analog(int channel);

#endif // #ifndef __INCLUDE_IO_H__

