BCD - Binary Coded Decimal

I cannot remember what I needed that for.

Why would you want to use BCD ?
Normally, you won't. But when implementing accounting-like programs you have to be sure that rounding, truncation and so on works as expected. And accounting laws may impose special rounding rules and so on which is not supported by floating point numbers.


Anyway, the bcd class implements a BCD representation where each byte stores 0..99. The default precision is 22 digits (sign, 18 digits, and 4 decimal places) (12 bytes). Roughly equivalent to COBOL's PIC S9(18)V9(4) COMP-3

I have left in the Watcom C/C++ inline assembler tweaks which really makes this implementation fly. Feel free to optimize for you compiler/platform.
