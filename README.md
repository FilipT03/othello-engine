# Othello (Reversi) engine
This project is an Othello (Reversi) engine developed in Python and C++.

This application is Linux only.
External modules used:
    pybind11, invoke, tabulate, tkinter

If the c++ module is not built, run:
    invoke all

    
Most of the program uses 64 bit numbers instead of the matrix to store and calculate boards.
Because the 64 bit number has 64 bits, we can use each of these bits as spaces for the board. However,
we need two numbers for the full state of the board, one for occupation (called 'occupied') and one
for colors (called 'colors').

64 bit number 'occupied' stores which space have pieces on them and which are empty. Ones present
occupied spaces and zeros present empty.

64 bit number colors stores the colors of the pieces on the board for the occupied spaces. One presents
white pieces and zeros present black. For the empty spaces, the value may be random.

To access and modify previously mentioned numbers, we will use bitwise operations and a bitmask.
The bitmask is defined in the "parse" module using hexadecimal numbers. It's stored as a matrix with
element[i][j] having the value of 2^(8*i+j).

To get if space is full or empty we use:
    full = bool(occupied & parse.MASK[i][j])

To get the color of a piece (on an occupied space) we use:
    color = bool(colors & parse.MASK[i][j])

To set the space occupation we use:
    occupied |= parse.MASK[i][j]

To switch the color of a piece (on an occupied space) we use:
    colors ^= parse.MASK[i][j]