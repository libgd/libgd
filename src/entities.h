/*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://dev.w3.org/html5/html-author/charref
 * by means of entities_gen.c in the libgd src dir
*/

#ifndef GD_ENTITIES_H

//html entity strings are entity prefix + string + suffix limited
//hex and dec should be limited to current unicode spec + entity prefix + suffix
#define GD_ENTITIES_H 1
#define ENTITY_NAME_LENGTH_MAX 22
#define ENTITY_HEX_LENGTH_MAX 10
#define ENTITY_DEC_LENGTH_MAX 10
#define NR_OF_ENTITIES 1448

struct entities_s{
			char *name;
			int value;
		};
extern const struct entities_s entities[NR_OF_ENTITIES];

#endif
