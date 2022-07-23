#include "bigint.h"
#include <assert.h>

size_t	translate_idx(BigInt* string, size_t index) {
	assert(string->capacity);
	assert(index <= (string->capacity - 1));
	return (string->capacity - 1) - index;
}

void	bigint_grow(BigInt* bigint) {
	// memmove(bigint->buffer+1, bigint->buffer, bigint->len);
	if (bigint->len < bigint->capacity) {
		return;
	}

	void* old_buffer = bigint->buffer;
	bigint->capacity *= 2;
	bigint->buffer = malloc((sizeof(int8_t) * bigint->capacity) + 1);
	memcpy(bigint->buffer + bigint->len, old_buffer, bigint->len);
	memset(bigint->buffer, '0', bigint->len);
	free(old_buffer);
}

void	increase_string(BigInt* string) {
	char* old_buffer = string->buffer;
	assert(string->capacity == string->len);
	string->capacity *= 2;
	string->buffer = malloc((sizeof(char) * string->capacity) + 1);

	memcpy(string->buffer + string->len, old_buffer, string->len);
	memset(string->buffer, '0', string->len);
	free(old_buffer);
}

void	bigint_init(BigInt* bigint, size_t initial_capacity) {
	if (!initial_capacity) {
		initial_capacity++;
	}
	bigint->capacity = initial_capacity;
	bigint->buffer = malloc((sizeof(int8_t) * bigint->capacity) + 1);
	memset(bigint->buffer, '0', bigint->capacity);
	bigint->len = 0;
}

int	bigint_mult_single(BigInt* bigint, int8_t scalar, int8_t overflow, size_t offset) {
	if (offset >= bigint->len) {
		bigint->len++;
		bigint_grow(bigint);
	}
	size_t index = translate_idx(bigint, offset);
	int8_t old_value = bigint->buffer[index] - '0';
	int32_t result = (scalar * old_value) + overflow;
	bigint->buffer[index] = (result % 10) + '0';
	return result / 10;
}

void	bigint_mult(BigInt* bigint, int8_t scalar) {
	int8_t overflow = 0;
	size_t original_len = bigint->len;
	for (size_t i = 0; i < bigint->len || overflow; i++) {
		overflow = bigint_mult_single(bigint, scalar, overflow, i);
	}
}

static char* get_memory_location(BigInt* string, size_t index) {
	const size_t i = translate_idx(string, index);
	char* const buffer = string->buffer;
	return buffer + i;
}

void	set_value(BigInt* string, size_t index, char value) {
	char* buffer = get_memory_location(string, index);
	*buffer = value;
}

char	get_value(BigInt* string, size_t index) {
	char* buffer = get_memory_location(string, index);
	return *buffer;
}

void	bigint_add(BigInt* string, int8_t increase, size_t offset) {
	//! Increase the length
	if (offset >= string->len) {
		string->len++;
		bigint_grow(string);
	}

	int8_t current_digit_value = get_value(string, offset) - '0';
	int8_t increased_value = current_digit_value + increase;
	if (increased_value >= 10) {
		bigint_add(string, increased_value / 10, offset + 1);
	}
	set_value(string, offset, (increased_value % 10) + '0');
}

void	bigint_add_bigint(BigInt* bigint, BigInt* other) {
	for (size_t i = 0; i < other->len; i++) {
		size_t other_idx = translate_idx(other, i);
		int8_t value = other->buffer[other_idx] - '0';
		bigint_add(bigint, value, i);
	}
}

char*	bigint_finalize(BigInt* bigint) {
	size_t start_idx = bigint->capacity - bigint->len;
	memmove(bigint->buffer, bigint->buffer + start_idx, bigint->len);
	bigint->buffer[bigint->len] = '\0';
	return bigint->buffer;
}
