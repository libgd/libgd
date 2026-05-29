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
	exif["Exif.Image.Model"] = "metadata-png-exiv2";
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
		model->toString() == "metadata-png-exiv2";
}

int main(void)
{
	gdImagePtr im;
	gdImagePtr decoded;
	gdImageMetadata *metadata;
	gdIOCtx *in_ctx;
	FILE *out_file;
	gdIOCtx *out_ctx;
	void *png;
	int size = 0;
	char *source_path;
	char *roundtrip_path;
	std::vector<unsigned char> source_data;
	size_t exif_size = 0;
	const unsigned char *exif;

	im = gdImageCreateTrueColor(12, 12);
	gdTestAssert(im != NULL);
	gdImageFilledRectangle(im, 0, 0, 11, 11, gdTrueColorAlpha(100, 120, 140, 0));
	gdImageSaveAlpha(im, 1);

	png = gdImagePngPtr(im, &size);
	gdTestAssert(png != NULL);
	gdTestAssert(size > 0);

	source_path = gdTestTempFile("metadata_png_exiv2_source.png");
	roundtrip_path = gdTestTempFile("metadata_png_exiv2_roundtrip.png");
	gdTestAssert(source_path != NULL);
	gdTestAssert(roundtrip_path != NULL);
	gdTestAssert(write_file(source_path, png, size));
	gdFree(png);
	png = NULL;

	gdTestAssert(exiv2_write_exif(source_path));
	gdTestAssert(read_file(source_path, source_data));
	gdTestAssert(!source_data.empty());

	metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);

	in_ctx = gdNewDynamicCtxEx((int)source_data.size(), &source_data[0], 0);
	gdTestAssert(in_ctx != NULL);
	decoded = gdImageCreateFromPngCtxWithMetadata(in_ctx, metadata);
	in_ctx->gd_free(in_ctx);
	gdTestAssert(decoded != NULL);

	exif = gdImageMetadataGetProfile(metadata, "exif", &exif_size);
	gdTestAssert(exif != NULL);
	gdTestAssert(exif_size > 8);

	out_file = fopen(roundtrip_path, "wb");
	gdTestAssert(out_file != NULL);
	out_ctx = gdNewFileCtx(out_file);
	gdTestAssert(out_ctx != NULL);
	gdImagePngCtxWithMetadata(decoded, out_ctx, metadata);
	out_ctx->gd_free(out_ctx);
	fclose(out_file);

	gdTestAssert(exiv2_read_expected_exif(roundtrip_path));

	gdImageDestroy(decoded);
	gdImageDestroy(im);
	gdImageMetadataFree(metadata);
	free(source_path);
	free(roundtrip_path);

	return gdNumFailures();
}
