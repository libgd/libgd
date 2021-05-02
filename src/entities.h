/*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://html.spec.whatwg.org/entities.json
 * by means of entities.py in the libgd src dir
 */

#ifndef GD_ENTITIES_H
#define GD_ENTITIES_H 1

#include <stdlib.h>
#include <stdint.h>

// html entity strings are entity prefix + string + suffix limited
// hex and dec should be limited to current unicode spec + entity prefix + suffix

#define ENTITY_HEX_LENGTH_MAX 10
#define ENTITY_DEC_LENGTH_MAX 10
#define NR_OF_ENTITIES 2125
#define ENTITY_NAME_LENGTH_MAX 33

struct entities_s {
	const char *name;
	uint32_t codepoint1;
	uint32_t codepoint2;
};
extern const struct entities_s gd_entities[NR_OF_ENTITIES];

#endif
