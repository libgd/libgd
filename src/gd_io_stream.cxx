/* *****************************************************************************
** Initial file written and documented by:
** Kevin Shepherd <kshepherd@php.net> December 2007
** of Scarlet Line http://www.scarletline.com/
*******************************************************************************/
/** \file gd_io_stream.cxx
	\brief Implementation of the methods of the gdIOCtx std stream specialization.

	Implements the derived specializations of gdIOCtx.
	These methods are not called by users of libgd, they
	are internal implementation.
	Note that half of the below methods are trivial stubs,
	as an input stream has no need of output methods, and vice-versa.
*/
#ifdef __cplusplus

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_io_stream.h"

/**	Read into buffer from stream
	Return the number of bytes successfully read.
	If an error occurs, or the end-of-file is reached, the return value
	is a short byte	count (or zero).
*/
int	istreamIOCtx::Getbuf (struct gdIOCtx * ctx, void * buf, int size)
	{
	stream_type * _str = ( (istreamIOCtx * ) ctx )->_M_stream;
	_str->read((char * )buf, size);
	return _str->gcount();
	}
/**	Write from buffer to stream
	Return the number of bytes successfully written.
	If an error occurs, or the end-of-file is reached, the return value
	is a short byte	count (or zero).
*/
int	istreamIOCtx::Putbuf (struct gdIOCtx * , const void * , int )
	{
	return 0;
	}

/**	Reads the next character from stream and returns it as an
	unsigned char cast to an int, or EOF on end of file or error.
*/
int	istreamIOCtx::Getchar (struct gdIOCtx * ctx)
	{
	stream_type * _str = ( (istreamIOCtx * ) ctx )->_M_stream;
	return _str->get();
	}
/**	Write the character to stream
	Character is cast to unsigned char before writing
*/
void	istreamIOCtx::Putchar (struct gdIOCtx * , int )
	{
	}

/** Seek to position offset from the beginning of the stream
	must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
*/
int	istreamIOCtx::Seek (struct gdIOCtx * ctx, const int pos)
	{
	stream_type * _str = ( (istreamIOCtx * ) ctx )->_M_stream;
	_str->seekg(pos);
	return !_str->fail();
	}
/** Obtains the current value of the stream position.
	Returns -1 on error.
*/
long	istreamIOCtx::Tell (struct gdIOCtx * ctx)
	{
	stream_type * _str = ( (istreamIOCtx * ) ctx )->_M_stream;
	return _str->tellg();
	}
/** Deallocate the context
*/
void	istreamIOCtx::FreeCtx (struct gdIOCtx * ctx)
	{
	delete (istreamIOCtx * )ctx;
	}

/**	Read into buffer from stream
	Return the number of bytes successfully read.
	If an error occurs, or the end-of-file is reached, the return value
	is a short byte	count (or zero).
*/
int	ostreamIOCtx::Getbuf (struct gdIOCtx * , void * , int )
	{
	return 0;
	}
/**	Write from buffer to stream
	Return the number of bytes successfully written.
	If an error occurs, or the end-of-file is reached, the return value
	is a short byte	count (or zero).
*/
int	ostreamIOCtx::Putbuf (struct gdIOCtx * ctx, const void * buf, int size)
	{
	stream_type * _str = ( (ostreamIOCtx * ) ctx )->_M_stream;
	_str->write((const char * )buf, size);
	return _str->bad()?0:size;
	}

/**	Reads the next character from stream and returns it as an
	unsigned char cast to an int, or EOF on end of file or error.
*/
int	ostreamIOCtx::Getchar (struct gdIOCtx * )
	{
	return EOF;
	}
/**	Write the character to stream
	Character is cast to unsigned char before writing
*/
void	ostreamIOCtx::Putchar (struct gdIOCtx * ctx, int c)
	{
	stream_type * _str = ( (ostreamIOCtx * ) ctx )->_M_stream;
	_str->put((char)c);
	}

/** Seek to position offset from the beginning of the stream
	must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
*/
int	ostreamIOCtx::Seek (struct gdIOCtx * ctx, const int pos)
	{
	stream_type * _str = ( (ostreamIOCtx * ) ctx )->_M_stream;
	_str->seekp(pos);
	return !_str->fail();
	}
/** Obtains the current value of the stream position.
	Returns -1 on error.
*/
long	ostreamIOCtx::Tell (struct gdIOCtx * ctx)
	{
	stream_type * _str = ( (ostreamIOCtx * ) ctx )->_M_stream;
	return _str->tellp();
	}
/** Deallocate the context
*/
void	ostreamIOCtx::FreeCtx (struct gdIOCtx * ctx)
	{
	delete (ostreamIOCtx * )ctx;
	}

#endif /* __cplusplus */
