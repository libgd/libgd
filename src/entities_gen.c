#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	FILE *infile;
	FILE *outfile;
	char linestring[1024];
	char buff[128];
	int num1;
	int cur_char = 0;
	int delimiter_count = 0;
	int longest_num = 0;
	int longest_name = 0;
	int entities = 0;

        if( ( infile = fopen("./entities.html", "r" ) ) == NULL ) {
                fprintf( stderr, "Error opening %s\n", "./entities.html" );
                return 1;
        }
	if( ( outfile = fopen("./entities.h", "w" ) ) == NULL ) {
		fprintf( stderr, "Error opening %s\n", "./entities.h" );
		return 1;
	}

	//print header to outfile
	fprintf(outfile, "%s%s\n", "/", "*");
	fprintf(outfile, "%s\n", " * Generated file - do not edit directly.");
	fprintf(outfile, "%s\n", " * This file was generated from:");
	fprintf(outfile, "%s\n", " *     https://dev.w3.org/html5/html-author/charref");
	fprintf(outfile, "%s\n", " * by means of entities_gen.c in the libgd src dir");
	fprintf(outfile, "%s%s\n", "*", "/");
	fprintf(outfile, "%s\n", "#ifdef __cplusplus");
	fprintf(outfile, "%s\n", "extern \"C\" {");
	fprintf(outfile, "%s\n\n", "#endif");
	fprintf(outfile, "\t%s\n", "static struct entities_s {");
	fprintf(outfile, "\t\t%s\n", "char    *name;");
	fprintf(outfile, "\t\t%s\n", "int     value;");
	fprintf(outfile, "\t%s\n", "} entities[] = {");

        memset((void*) linestring, '\0', 1024);

	while( fgets(linestring, 1024, infile) != NULL){
		memset((void*)buff, '\0', 128);
		delimiter_count = 0;
		cur_char = 0;
		if(linestring[cur_char++] != '<')
			continue;
		if(linestring[cur_char++] != 't')
			continue;
		if(linestring[cur_char++] != 'r')
			continue;
		delimiter_count++;

		while(linestring[cur_char] != '\n' && linestring[cur_char] != '\0'){
			if(linestring[cur_char++] != '<')
				continue;
			delimiter_count++;
			//output format {"AElig", 198},
			//name field <code>&amp;AElig;
			if(delimiter_count == 4){
				while(linestring[cur_char] != '\n' && linestring[cur_char] != '\0' && linestring[cur_char] != ';'){
					cur_char++;
				}
				if(linestring[cur_char] == '\n' || linestring[cur_char] == '\0')
					break;
				cur_char++;

				num1=0;
				while(linestring[cur_char] != '\n' && linestring[cur_char] != '\0' && linestring[cur_char] != ';'){
					buff[num1++] = linestring[cur_char++];
				}
				if(linestring[cur_char] == '\n' || linestring[cur_char] == '\0')
					break;
				if(num1>longest_name)
					longest_name = num1;
				buff[num1] = '\0';
				//printf("\t\t\t{\"%s\", ", buff);
				fprintf(outfile, "\t\t\t{\"%s\", ", buff);
			}
			//number field <code>&amp;#198;
			if(delimiter_count == 10){
				while(linestring[cur_char] != '\n' && linestring[cur_char] != '\0' && linestring[cur_char] != ';'){
					cur_char++;
				}
				if(linestring[cur_char] == '\n' || linestring[cur_char] == '\0')
					break;
				cur_char++;
				num1=0;
				while(linestring[cur_char] != '\n' && linestring[cur_char] != '\0' && linestring[cur_char] != ';'){
					if(linestring[cur_char] == '#'){
						cur_char++;
						continue;
					}
					buff[num1++] = linestring[cur_char++];
				}
				if(linestring[cur_char] == '\n' || linestring[cur_char] == '\0')
					break;
				if(num1>longest_num)
					longest_num = num1;
				buff[num1] = '\0';
				//printf("%s},\n", buff);
				fprintf(outfile, "%s},\n", buff);
				entities++;
			}
		}


	}
	printf("longest name: %d longest num: %d\n", longest_name, longest_num);
	printf("entities: %d\n", entities);

	(void)fseek(outfile, -2, SEEK_CUR);
	fprintf(outfile, "\n");
	fprintf(outfile, "\t\t\t%s\n\n", "};");

	//print last lines to outfile
	fprintf(outfile, "%s%s\n", "/", "/html entity strings are entity prefix + string + suffix limited");
	fprintf(outfile, "%s%s\n", "/", "/hex and dec should be limited to current unicode spec + entity prefix + suffix");
	fprintf(outfile, "%s %d\n", "#define ENTITY_NAME_LENGTH_MAX", longest_name+2);
	fprintf(outfile, "%s %d\n", "#define ENTITY_HEX_LENGTH_MAX", longest_num+4);
	fprintf(outfile, "%s %d\n", "#define ENTITY_DEC_LENGTH_MAX", longest_num+4);
	fprintf(outfile, "%s %d\n\n", "#define NR_OF_ENTITIES", entities);
	fprintf(outfile, "%s\n", "#ifdef __cplusplus");
	fprintf(outfile, "%s\n", "}");
	fprintf(outfile, "%s\n", "#endif");

	fclose(infile);
	fclose(outfile);
	return 0;
}
