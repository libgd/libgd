#include <fstream>
#include <memory>
#include <vector>

#include <exiv2/exiv2.hpp>

extern "C" {
#include "gd.h"
char *gdTestTempFile(const char *);
int gdNumFailures(void);
int _gdTestAssert(const char *, unsigned int, int);
}

#define gdTestAssert(cond) _gdTestAssert(__FILE__, __LINE__, (cond))

#if EXIV2_TEST_VERSION(0,28,0)
#define GD_EXIV2_IMAGE_PTR std::unique_ptr<Exiv2::Image>
#else
#define GD_EXIV2_IMAGE_PTR Exiv2::Image::AutoPtr
#endif

static bool write_file(const char *path, const void *data, int size)
{
	std::ofstream out(path, std::ios::binary);
	if (!out) {
		return false;
	}
	out.write(static_cast<const char *>(data), size);
	return out.good();
}

static bool read_file(const char *path, std::vector<unsigned char>& data)
{
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return false;
	}
	data.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	return in.good() || in.eof();
}

static bool exiv2_write_exif(const char *path)
{
	GD_EXIV2_IMAGE_PTR image = Exiv2::ImageFactory::open(path);
	if (image.get() == 0) {
		return false;
	}

	Exiv2::ExifData exif;
	exif["Exif.Image.Make"] = "libgd";
	exif["Exif.Image.Model"] = "metadata-exiv2";
	image->setExifData(exif);
	image->writeMetadata();
	return true;
}

static bool exiv2_read_expected_exif(const char *path)
{
	GD_EXIV2_IMAGE_PTR image = Exiv2::ImageFactory::open(path);
	if (image.get() == 0) {
		return false;
	}

	image->readMetadata();
	const Exiv2::ExifData& exif = image->exifData();
	Exiv2::ExifData::const_iterator make = exif.findKey(Exiv2::ExifKey("Exif.Image.Make"));
	Exiv2::ExifData::const_iterator model = exif.findKey(Exiv2::ExifKey("Exif.Image.Model"));

	return make != exif.end() && model != exif.end() &&
		make->toString() == "libgd" &&
		model->toString() == "metadata-exiv2";
}

int main(void)
{
	gdImagePtr im;
	gdImagePtr decoded;
	gdImageMetadata *metadata;
	void *jpeg;
	void *roundtrip;
	int size = 0;
	int roundtrip_size = 0;
	char *source_path;
	char *roundtrip_path;
	std::vector<unsigned char> source_data;
	size_t exif_size = 0;
	const unsigned char *exif;

	im = gdImageCreateTrueColor(12, 12);
	gdTestAssert(im != NULL);
	gdImageFilledRectangle(im, 0, 0, 11, 11, gdTrueColor(240, 240, 240));

	jpeg = gdImageJpegPtr(im, &size, 90);
	gdTestAssert(jpeg != NULL);
	gdTestAssert(size > 0);

	source_path = gdTestTempFile("metadata_exiv2_source.jpg");
	roundtrip_path = gdTestTempFile("metadata_exiv2_roundtrip.jpg");
	gdTestAssert(source_path != NULL);
	gdTestAssert(roundtrip_path != NULL);
	gdTestAssert(write_file(source_path, jpeg, size));
	gdFree(jpeg);
	jpeg = NULL;

	gdTestAssert(exiv2_write_exif(source_path));
	gdTestAssert(read_file(source_path, source_data));
	gdTestAssert(!source_data.empty());

	metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);
	decoded = gdImageCreateFromJpegPtrWithMetadata((int)source_data.size(), &source_data[0], metadata);
	gdTestAssert(decoded != NULL);

	exif = gdImageMetadataGetProfile(metadata, "exif", &exif_size);
	gdTestAssert(exif != NULL);
	gdTestAssert(exif_size > 6);
	gdTestAssert(exif[0] == 'E' && exif[1] == 'x' && exif[2] == 'i' && exif[3] == 'f' && exif[4] == 0 && exif[5] == 0);

	roundtrip = gdImageJpegPtrWithMetadata(decoded, &roundtrip_size, 90, metadata);
	gdTestAssert(roundtrip != NULL);
	gdTestAssert(roundtrip_size > 0);
	gdTestAssert(write_file(roundtrip_path, roundtrip, roundtrip_size));
	gdTestAssert(exiv2_read_expected_exif(roundtrip_path));

	gdFree(roundtrip);
	gdImageDestroy(decoded);
	gdImageDestroy(im);
	gdImageMetadataFree(metadata);
	free(source_path);
	free(roundtrip_path);

	return gdNumFailures();
}
