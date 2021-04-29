#!/usr/bin/env python3
import json
import re
import urllib.request
import os
import os.path
from pathlib import Path

SRCDIR = Path(__file__).resolve().parent
os.chdir(SRCDIR)

# Declare headers
HEADER_C_FILE = """ /*
 * Generated file - do not edit directly.
 * This file was generated from:
 *     https://html.spec.whatwg.org/entities.json
 * by means of entities.py in the libgd src dir
 */

#include "entities.h"

const struct entities_s entities[NR_OF_ENTITIES] = {
"""
HEADER_H_FILE = """ /*
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

struct entities_s {
                  	char *name;
                        uint32_t unicode_a;
                        uint32_t unicode_b;
                };
extern const struct entities_s entities[NR_OF_ENTITIES];

"""

# Open files
if not os.path.exists("entities.json"):
    print("entities.json not found, attempting to download...")
    with open("entities.json", "wb") as file_json:
        file_json.write(urllib.request.urlopen("https://html.spec.whatwg.org/entities.json").read())
        file_json.close()
        print("    successful")

with open("entities.json", "rb") as file_json:
    entities = json.load(file_json)

file_ent_h = open("entities.h", mode="w")
file_ent_c = open("entities.c", mode="w")

# Initialize some objects
curline = 1
totallines = 0
len_name_max = 0
name_matcher = re.compile("&\S+;")

# Print file headers
file_ent_c.write(HEADER_C_FILE)
file_ent_h.write(HEADER_H_FILE)

# Count how many matching lines we have
for key in entities:
    if name_matcher.match(key):
        totallines+=1
# Print json entities to struct
for key in entities:
    if name_matcher.match(key):
        if len(key) > len_name_max:
            len_name_max = len(key)
        string = "\t" + "{\"" + key.replace("&", "") + "\", "
        file_ent_c.write(string.replace(";", ""))
        codepoints = entities[key]["codepoints"]
        if len(codepoints) > 1:
            string = str(codepoints[0]) + ", " + str(codepoints[1]) + "}"
        else:
            string = str(codepoints[0]) + ", 0}"

        if curline == totallines:
            file_ent_c.write(string + "\n")
        else:
            file_ent_c.write(string + "," + "\n")
        curline+=1
# Print file ends
file_ent_c.write("\t};")
file_ent_h.write(f"#define NR_OF_ENTITIES {totallines}\n");
file_ent_h.write(f"#define ENTITY_NAME_LENGTH_MAX {len_name_max}\n");
file_ent_h.write(f"#define ENTITY_HEX_LENGTH_MAX 10\n")
file_ent_h.write(f"#define ENTITY_DEC_LENGTH_MAX 10\n#endif")
# Close files
file_json.close()
file_ent_h.close()
file_ent_c.close()
print("entities.h & entities.c updated from entities.json")
