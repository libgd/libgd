/*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://html.spec.whatwg.org/entities.json
 * by means of entities.py in the libgd src dir
*/
#ifndef GD_ENTITIES_H

//html entity strings are entity prefix + string + suffix limited
//hex and dec should be limited to current unicode spec + entity prefix + suffix
#define GD_ENTITIES_H 1
#define NR_OF_ENTITIES 2231
#define ENTITY_NAME_LENGTH_MAX 33
#define ENTITY_HEX_LENGTH_MAX 10
#define ENTITY_DEC_LENGTH_MAX 10

struct entities_s{
                  	char *name;
                        int unicode_a;
                        int unicode_b;
                };
extern const struct entities_s entities[NR_OF_ENTITIES];

#endif
