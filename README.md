# VarInt
The idea is quite simple, can we create a integer that consists of a variable number of bits?  
The value of the integer is manipulated by doing simple bitwise arithmetic.

This implementation has lots of room for improvement
- Currently only the first of the 4 basic arithmetic operations are supported. (addition, subtraction, multiplication, division)
- Arithmetic is performed on 8 bits at a time, instead of 32 or even 64 bits
- No extra capacity is stored when growing, and growths happen 1 byte at a time.
- Nothing is optimized
