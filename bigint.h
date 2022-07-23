/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   bigint.h                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/07/22 21:48:47 by tbruinem      #+#    #+#                 */
/*   Updated: 2022/07/23 02:25:09 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

typedef struct BigInt{
	char*	buffer;
	size_t	len;
	size_t	capacity;
} BigInt;

void	bigint_grow(BigInt* bigint);
void	bigint_init(BigInt* bigint, size_t initial_capacity);
void	bigint_add(BigInt* bigint, int8_t value, size_t offset);
char*	bigint_finalize(BigInt* bigint);
void	bigint_add_bigint(BigInt* bigint, BigInt* other);
void	bigint_mult(BigInt* bigint, int8_t scalar);
