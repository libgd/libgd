/* *****************************************************************************
** Initial file written and documented by:
** Kevin Shepherd <kshepherd@php.net> December 2007
** of Scarlet Line http://www.scarletline.com/
*******************************************************************************/
/** \file gdpp.cxx
	\brief Implements the non-trivial methods of GD::Image.

	Implementation of the more complex methods defined
	in gdpp.h.
	Notably includes the methods which determine the image file
	format of a file before reading it into memory.
*/
#ifdef __cplusplus
#include "gdpp.h"

namespace GD
	{
	/**
		Load an image from a file, after attempting to
		determine it's image file format.
		Invoke CreateFrom with an already opened
		pointer to a file containing the desired image.
		CreateFrom does not close the file.
		\param[in] in An opened FILE * pointer.
		\return true for success, or false if unable to load the image (most often because the
		file is corrupt or does not contain a recognized image format).
		You can call Width() and Height() member functions of the image to determine its size.
	*/
	bool Image::CreateFrom(FILE * in)
		{
		bool rtn;
		int c = fgetc(in);
		ungetc(c, in);
		switch (c)
			{
		/* PNG
		The first eight bytes of a PNG file always contain the following (decimal) values:
		   0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A
		   == .PNG\r\n.\n
		*/
		case 0x89: // PNG
			rtn = CreateFromPng(in);
			break;

		/* GIF
			0x47 0x49 0x46
		*/
		case 0x47: // GIF
			rtn = CreateFromGif(in);
			break;
		/* JPEG
		A JFIF-standard file will start with the four bytes (hex) FF D8 FF E0,
		    followed by two variable bytes (often hex 00 10), followed by 'JFIF'.
		*/
		case 0xFF: // JPEG
			rtn = CreateFromJpeg(in);
			break;
		/* WBMP
		WBMP Type 0: B/W, Uncompressed bitmap is the only gd supported type
		*/
		case 0x00: // WBMP
			rtn = CreateFromWBMP(in);
			break;
		/* GD2
			0x67 0x64 0x32 0x00
			== GD2\0
		Starts with gd2
		*/
		case 0x67: // GD2
			rtn = CreateFromGd2(in);
			break;
		/* GD
			0xFF 0xFE
			or
			0xFF 0xFF
			Conflicts with Jpeg
		*/
		/* XBM
			#define test_width 16
			#define test_height 7
		*/
		case 0x23: // XBM
			rtn = CreateFromXbm(in);
			break;
		default:
			rtn = false;
			break;
			}
		return rtn;
		}

	/**
		Load an image from a standard input stream, after attempting to
		determine it's image file format.
		Invoke CreateFrom with an already opened stream
		containing the desired image.
		CreateFrom does not close the stream.
		\param[in] in An opened standard library input stream.
		\return true for success, or false if unable to load the image (most often because the
		file is corrupt or does not contain a recognized image format).
		You can call Width() and Height() member functions of the image to determine its size.
		Example usage, convert anything to gif:
		#include <fstream>
		#include <gdpp.h>

		std::ifstream in("image.xxx", std::ios_base::in | std::ios_base::binary );
		GD::Image im;
		im.CreateFrom(in);
		if (im.good())
			{
			std::ofstream out("image.gif", std::ios_base::out | std::ios_base::binary );
			im.Gif(out);
			}
	*/
	bool Image::CreateFrom(std::istream & in)
		{
		bool rtn;
		switch (in.peek())
			{
		/* PNG
		The first eight bytes of a PNG file always contain the following (decimal) values:
		   0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A
		   == .PNG\r\n.\n
		*/
		case 0x89: // PNG
			rtn = CreateFromPng(in);
			break;

		/* GIF
			0x47 0x49 0x46
		*/
		case 0x47: // GIF
			rtn = CreateFromGif(in);
			break;

		/* JPEG
		A JFIF-standard file will start with the four bytes (hex) FF D8 FF E0,
		    followed by two variable bytes (often hex 00 10), followed by 'JFIF'.
		*/
		case 0xFF: // JPEG
			rtn = CreateFromJpeg(in);
			break;

		/* WBMP
		WBMP Type 0: B/W, Uncompressed bitmap is the only gd supported type
		*/
		case 0x00: // WBMP
			rtn = CreateFromWBMP(in);
			break;
		/* GD2
			0x67 0x64 0x32 0x00
			== GD2\0
		Starts with gd2
		*/
		case 0x67: // GD2
			rtn = CreateFromGd2(in);
			break;
		/* GD
			0xFF 0xFE
			or
			0xFF 0xFF
			Conflicts with Jpeg
		*/
		default:
			rtn = false;
			break;
			}
		return rtn;
		}

	/**
		Load an image from an in-RAM memory block, after attempting to
		determine it's image format.
		CreateFrom does not de-allocate the memory.
		\param[in] size The byte count of the memory block.
		\param[in] data A pointer to the memory block.
		\return true for success, or false if unable to load the image (most often because the
		formatting is corrupt or does not contain a recognized image format).
		You can call Width() and Height() member functions of the image to determine its size.
	*/
	bool Image::CreateFrom(int size, void * data)
		{
		bool rtn;
		switch (((unsigned char * )data)[0])
			{

		/* PNG
		The first eight bytes of a PNG file always contain the following (decimal) values:
		   0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A
		   == .PNG\r\n.\n
		*/
		case 0x89: // PNG
			rtn = CreateFromPng(size, data);
			break;

		/* GIF
			0x47 0x49 0x46
		*/
		case 0x47: // GIF
			rtn = CreateFromGif(size, data);
			break;

		/* JPEG
		A JFIF-standard file will start with the four bytes (hex) FF D8 FF E0,
		    followed by two variable bytes (often hex 00 10), followed by 'JFIF'.
		*/
		case 0xFF: // JPEG
			rtn = CreateFromJpeg(size, data);
			break;

		/* WBMP
		WBMP Type 0: B/W, Uncompressed bitmap is the only gd supported type
		*/
		case 0x00: // WBMP
			rtn = CreateFromWBMP(size, data);
			break;
		/* GD2
			0x67 0x64 0x32 0x00
			== GD2\0
		Starts with gd2
		*/
		case 0x67: // GD2
			rtn = CreateFromGd2(size, data);
			break;
		/* GD
			0xFF 0xFE
			or
			0xFF 0xFF
			Conflicts with Jpeg
		*/
		default:
			rtn = false;
			break;
			}
		return rtn;
		}
	} // namespace GD
/**
	Load an image from a standard input stream, regardless of it's image file format.
	You can call Width() and Height() member functions of the image to determine its size.
	Example usage, convert anything to gif:
	#include <fstream>
	#include <gdpp.h>

	std::ifstream in("image.xxx", std::ios_base::in | std::ios_base::binary );
	GD::Image im;
	in >> im;
	if (im.good())
		{
		std::ofstream out("image.gif", std::ios_base::out | std::ios_base::binary );
		im.Gif(out);
		}
*/
std::istream & operator>> (std::istream & in, GD::Image & img)
	{
	img.CreateFrom(in);
	return in;
	}

#endif /* __cplusplus */
