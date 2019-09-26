#ifndef GETOPT_H
#define GETOPT_H 1

#ifdef __cplusplus
extern "C" {
#endif

extern int opterr;
extern int optind;
extern int optopt;
extern int optreset;
extern char *optarg;

int getopt(int nargc, char * const nargv[], const char *ostr);

#ifdef __cplusplus
}
#endif

#endif
