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

#define NR_OF_ENTITIES 2125
#define ENTITY_NAME_LENGTH_MAX 33
#define ENTITY_HEX_LENGTH_MAX 10
#define ENTITY_DEC_LENGTH_MAX 10

// html entity strings are entity prefix + string + suffix limited
// hex and dec should be limited to current unicode spec + entity prefix + suffix

struct entities_s {
                  	char *name;
                        uint32_t unicode_a;
                        uint32_t unicode_b;
                };
extern const struct entities_s entities[NR_OF_ENTITIES];

#endif
