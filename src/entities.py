#!/usr/bin/env python
from __future__ import print_function
import json

header_a = """/*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://html.spec.whatwg.org/entities.json
 * by means of entities.py in the libgd src dir
*/
"""

header_b = """
#include <entities.h>

const struct entities_s entities[NR_OF_ENTITIES] = {"""

header_c = """/*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://html.spec.whatwg.org/entities.json
 * by means of entities.py in the libgd src dir
*/
#ifndef GD_ENTITIES_H

//html entity strings are entity prefix + string + suffix limited
//hex and dec should be limited to current unicode spec + entity prefix + suffix
"""

header_d = """
#define GD_ENTITIES_H 1
#define ENTITY_NAME_LENGTH_MAX 22
#define ENTITY_HEX_LENGTH_MAX 10
#define ENTITY_DEC_LENGTH_MAX 10
#define NR_OF_ENTITIES 1448
"""

header_e = """
struct entities_s{
                  	char *name;
                        int unicode_a;
                        int unicode_b;
                };
extern const struct entities_s entities[NR_OF_ENTITIES];

#endif
"""

### Open files
#try:
file_json = open("entities.json", mode="r")
file_ent_h = open("entities.h", mode="w")
file_ent_c = open("entities.c", mode="w")
#finally:
#    file_json.close()


#with open("./entities.json", mode="r", encoding="utf-8") as file:
#    x = json.load(file)

x = json.load(file_json)
curline = 0
lastline = len(x) - 1

### Open entities.h
#with open('./entities.h', 'w') as file:

### Open entities.c


### Print file headers
file_ent_c.write(header_a + header_b + "\n")
file_ent_h.write(header_c)
file_ent_h.write("#define GD_ENTITIES_H 1\n")
#file_ent_h.write("#define ENTITY_NAME_LENGTH_MAX 22\n")
#file_ent_h.write("#define ENTITY_HEX_LENGTH_MAX 10\n")
#file_ent_h.write("#define ENTITY_DEC_LENGTH_MAX 10\n")
file_ent_h.write("#define NR_OF_ENTITIES " + str(len(x)) + "\n");


len_name_max = 0
len_hex_max = 0
len_dec_max = 0

### Print json entities to struct
for e in x:
    if len(e) > len_name_max:
        len_name_max = len(e)
    string = "\t\t\t" + "{\"" + e + "\", "
    file_ent_c.write(string)
    if len(x[e]["codepoints"]) > 1:
        string = str(x[e]["codepoints"][0]) + ", " + str(x[e]["codepoints"][1]) + "}"
    else:
        string = str(x[e]["codepoints"][0]) + ", 0}"

    if curline == lastline:
        file_ent_c.write(string + "\n")
    else:
        file_ent_c.write(string + "," + "\n")
    curline+=1
### Print file ends
file_ent_c.write("\t\t\t};" + "\n")
file_ent_h.write("#define ENTITY_NAME_LENGTH_MAX " + str(len_name_max) + "\n");
file_ent_h.write("#define ENTITY_HEX_LENGTH_MAX 10\n")
file_ent_h.write("#define ENTITY_DEC_LENGTH_MAX 10\n")
file_ent_h.write(header_e)

#finally:
file_json.close()
file_ent_h.close()
file_ent_c.close()
