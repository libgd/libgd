#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * mkstemp --
 *  Generate a unique temporary filename from template, create and open the
 *    file, and return an open file descriptor for the file.
 *  A return value of -1 indicates an error while errno is set.
 */
int
mkstemp(char *template)
{
	if (template == NULL)
		return -1;
	if (_mktemp_s(template, strlen(template) + 1) == EINVAL)
		return -1;
	return _open(template, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY,
			_S_IREAD | _S_IWREITE);
}
