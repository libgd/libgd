#include <string.h>

#include "gd.h"
#include "gdtest.h"

static void test_lifecycle_and_profiles(void)
{
	gdImageMetadata *metadata;
	const unsigned char input[] = { 'a', 'b', 'c' };
	const unsigned char replacement[] = { 'x', 'y' };
	const unsigned char *profile;
	const char *key;
	size_t size;

	metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);

	gdTestAssert(gdImageMetadataGetProfileCount(metadata) == 0);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "exif", input, sizeof(input)) == GD_META_OK);
	gdTestAssert(gdImageMetadataGetProfileCount(metadata) == 1);

	profile = gdImageMetadataGetProfile(metadata, "exif", &size);
	gdTestAssert(profile != NULL);
	gdTestAssert(size == sizeof(input));
	gdTestAssert(memcmp(profile, input, sizeof(input)) == 0);

	gdTestAssert(gdImageMetadataGetProfileAt(metadata, 0, &key, &profile, &size) == GD_META_OK);
	gdTestAssert(strcmp(key, "exif") == 0);
	gdTestAssert(size == sizeof(input));
	gdTestAssert(memcmp(profile, input, sizeof(input)) == 0);

	gdTestAssert(gdImageMetadataSetProfile(metadata, "exif", replacement, sizeof(replacement)) == GD_META_OK);
	gdTestAssert(gdImageMetadataGetProfileCount(metadata) == 1);
	profile = gdImageMetadataGetProfile(metadata, "exif", &size);
	gdTestAssert(profile != NULL);
	gdTestAssert(size == sizeof(replacement));
	gdTestAssert(memcmp(profile, replacement, sizeof(replacement)) == 0);

	gdTestAssert(gdImageMetadataRemoveProfile(metadata, "exif") == GD_META_OK);
	gdTestAssert(gdImageMetadataGetProfileCount(metadata) == 0);
	gdTestAssert(gdImageMetadataGetProfile(metadata, "exif", &size) == NULL);
	gdTestAssert(size == 0);

	gdImageMetadataFree(metadata);
}

static void test_limits(void)
{
	gdImageMetadata *metadata;
	const unsigned char one[] = { 1 };
	const unsigned char two[] = { 1, 2 };
	size_t max_profile_size = 0;
	size_t max_total_size = 0;

	metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);

	gdImageMetadataGetLimits(metadata, &max_profile_size, &max_total_size);
	gdTestAssert(max_profile_size == GD_METADATA_DEFAULT_MAX_PROFILE_SIZE);
	gdTestAssert(max_total_size == GD_METADATA_DEFAULT_MAX_TOTAL_SIZE);

	gdTestAssert(gdImageMetadataSetLimits(metadata, 1, 2) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "icc", one, sizeof(one)) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "xmp", one, sizeof(one)) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "iptc", one, sizeof(one)) == GD_META_ERR_LIMIT);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "icc", two, sizeof(two)) == GD_META_ERR_LIMIT);

	gdTestAssert(gdImageMetadataSetLimits(metadata, 0, 0) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "icc", two, sizeof(two)) == GD_META_OK);

	gdImageMetadataFree(metadata);
}

static void test_invalid_inputs(void)
{
	gdImageMetadata *metadata;
	const unsigned char data[] = { 1 };

	metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);

	gdTestAssert(gdImageMetadataSetProfile(NULL, "exif", data, sizeof(data)) == GD_META_ERR_INVALID);
	gdTestAssert(gdImageMetadataSetProfile(metadata, NULL, data, sizeof(data)) == GD_META_ERR_INVALID);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "", data, sizeof(data)) == GD_META_ERR_INVALID);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "exif", NULL, sizeof(data)) == GD_META_ERR_INVALID);
	gdTestAssert(gdImageMetadataRemoveProfile(NULL, "exif") == GD_META_ERR_INVALID);
	gdTestAssert(gdImageMetadataGetProfileCount(NULL) == 0);
	gdTestAssert(gdImageMetadataGetProfileAt(NULL, 0, NULL, NULL, NULL) == GD_META_ERR_INVALID);

	gdImageMetadataFree(metadata);
}

int main(void)
{
	test_lifecycle_and_profiles();
	test_limits();
	test_invalid_inputs();

	return gdNumFailures();
}
