#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "bigint.h"

typedef uint8_t byte;

typedef struct VariableInt {
	byte*	data;	//! The data containing
	size_t	bytes;	//! How many bytes our integer consists of
} VariableInt;

static size_t combined_offset(size_t byte, size_t bit) {
	return byte * 8 + bit;
}

static void set_bit(byte *x, int bitNum) {
	*x |= (1 << bitNum);
}

static void clear_bit(byte* x, int bitNum) {
	*x &= ~(1 << bitNum);
}

static bool bit_is_set(byte x, int bitNum) {
	return (x >> bitNum) & 1;
}

static void varint_set_bit(VariableInt* value, size_t byte, size_t bit) {
	set_bit(value->data + byte, bit);
}

static void varint_clear_bit(VariableInt* value, size_t byte, size_t bit) {
	clear_bit(value->data + byte, bit);
}

static bool varint_bit_is_set(VariableInt* value, size_t byte, size_t bit) {
	return bit_is_set(value->data[byte], bit);
}

static BigInt create_bit_value(int power) {
	BigInt bigint;
	bigint_init(&bigint, 4);
	bigint_add(&bigint, 1, 0);

	for (size_t i = 0; i < power; i++) {
		bigint_mult(&bigint, 2);
	}
	return bigint;
}

char*	varint_to_string(VariableInt* value) {
	BigInt bigint;
	bigint_init(&bigint, 4);

	//! Loop over all bytes in the variable integer
	for (size_t i = 0; i < value->bytes; i++) {
		//! Every byte consists of 8 bits
		for (size_t bit = 0; bit < 8; bit++) {
			//! Check if the bit is set
			bool bit_set = varint_bit_is_set(value, i, (7 - bit));
			if (bit_set) {
				//! Create the value represented by this bit, and add it to our integer
				const int remaining_bits = (value->bytes - i)*8;
				BigInt bit_value = create_bit_value(remaining_bits - bit - 1);
				bigint_add_bigint(&bigint, &bit_value);
			}
		}
	}
	return bigint_finalize(&bigint);
}

void	print_varint_bits(VariableInt* value) {
	for (size_t i = 0; i < value->bytes; i++) {
		for (size_t bit = 0; bit < 8; bit++) {
			bool bit_set = varint_bit_is_set(value, i, (7 - bit));
			char visual = bit_set + '0';
			write(STDOUT_FILENO, &visual, 1);
		}
		if (i + 1 < value->bytes) {
			write(STDOUT_FILENO, " ", 1);
		}
	}
	write(STDOUT_FILENO, "\n", 1);
}

void	print_varint(VariableInt* value) {
	char* str = varint_to_string(value);
	printf("%s\n", str);
	free(str);
}

void	varint_grow(VariableInt* value) {
	void* old_data = value->data;
	value->bytes += 1;
	value->data = malloc(sizeof(byte) * value->bytes);
	if (old_data) {
		memcpy(value->data+1, old_data, value->bytes-1);
	}
	value->data[0] = 0;
	free(old_data);
}

void	varint_init(VariableInt* value) {
	value->bytes = 4;
	value->data = malloc(sizeof(byte) * value->bytes);
	memset(value->data, 0, value->bytes);
}

static size_t translate_idx(VariableInt* value, size_t byte_offset) {
	return value->bytes - 1 - byte_offset;
}

static size_t get_byte_offset(size_t offset) {
	size_t byte_offset = offset / 8;
	return byte_offset;
}

static size_t get_bit_offset(size_t offset) {
	size_t bit_offset = offset % 8;
	return bit_offset;
}

//! Add the value 'addition' to the variable int 'value', starting at 'bit_offset' position
void	varint_add(VariableInt* value, byte addition, size_t bit_offset) {
	size_t byte_offset = get_byte_offset(bit_offset);
	size_t bit = get_bit_offset(bit_offset);
	if (byte_offset >= value->bytes) {
		varint_grow(value);
	}

	//! Loop over all bits in the 'addition' byte
	for (size_t i = 0; i < 8; i++) {
		bool bit_a_set = bit_is_set(addition, i);
		bool bit_b_set = varint_bit_is_set(value, translate_idx(value, byte_offset), bit);
		//! If both bits are set, apply the overflow to the next bit
		if (bit_a_set && bit_b_set) {
			varint_add(value, 1, combined_offset(byte_offset, bit+1));
			varint_clear_bit(value, translate_idx(value, byte_offset), bit);
		}
		else if (bit_a_set && !bit_b_set) {
			varint_set_bit(value, translate_idx(value, byte_offset), bit);
		}
		//! Move to the next byte if we've reached the end of the current byte
		bit++;
		if (bit >= 8) {
			bit = 0;
			byte_offset++;
			if (byte_offset >= value->bytes) {
				varint_grow(value);
			}
		}
	}
}

int main() {
	VariableInt value;

	varint_init(&value);
	print_varint(&value);
	// value.data[3] = 124;
	print_varint_bits(&value);
	// varint_add(&value, 125, combined_offset(0, 0));
	print_varint(&value);
	print_varint_bits(&value);
	varint_add(&value, 25, combined_offset(2, 4));
	print_varint(&value);
	print_varint_bits(&value);
	varint_add(&value, 25, combined_offset(2, 4));
	print_varint(&value);
	print_varint_bits(&value);
	return 0;
}
