#include "gd.h"
#include "gdtest.h"

#include <stddef.h>
#include <stdio.h>

#include <jpeglib.h>

typedef enum {
	JPEG_EXPECT_VALID,
	JPEG_EXPECT_12BIT,
	JPEG_EXPECT_ROBUST,
	JPEG_EXPECT_INVALID
} jpeg_expectation;

typedef struct {
	const char *directory;
	const char *subdirectory;
	const char *filename;
	jpeg_expectation expectation;
} jpeg_case;

static const jpeg_case cases[] = {
	{"valid", NULL, "16bit-qtables.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Canon_40D.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Fujifilm_FinePix_E500.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Konica_Minolta_DiMAGE_Z3.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Nikon_D70.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Olympus_C8080WZ.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Panasonic_DMC-FZ30.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Pentax_K10D.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Reconyx_HC500_Hyperfire.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Ricoh_Caplio_RR330.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Samsung_Digimax_i50_MP3.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "Sony_HDR-HC3.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "blank_800x280.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "cmyk_logo.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "cmyk.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "extraneous-data.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "grayscale_16x24_sampling2x2.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "grayscale_24x16_sampling2x2.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "grayscale_large.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "grayscale_long.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "grayscale_square.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "mjpeg.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "non-interleaved-mcu.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "partial_progressive.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "progressive-missing-ac.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "progressive-missing-dc.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "progressive3.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "progressive_rst_420.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "restarts.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rgb.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_16block.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_1block.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_1row.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_2row.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_4row.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "rst_8block.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "testimgari.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "testimgint.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "testorig.jpg", JPEG_EXPECT_VALID},
	{"valid", NULL, "testorig12.jpg", JPEG_EXPECT_12BIT},
	{"valid", NULL, "ycck.jpg", JPEG_EXPECT_VALID},
	{"invalid", NULL, "138d3b9e0d9fbf641b8135981e597c3a.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "194531363df5b73f59c4c0517422f917.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "1cbb1bb37d62c44f67374cd451643dc4.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "2183d39878e734cf79b62428b02fafb5.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "21a84b8472f6d18f5bb5c0026e97cfaa.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "21ad703b38e2c350215bb92a849486f3.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "255015e07b6f9137b53b0f97d67a8aef.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "28968137f4fc75fbf56f16d7a7a8551a.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "28c74d9284d9836017fd519f6932efd8.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "2c9e7a1805f8b47630bbb83d21bf8222.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "316be81dfdeeb942e904feb3a77f4f83.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "32d08f4a5eb10332506ebedbb9bc7257.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "3976a754ef0aca80e84e2c403d714579.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "39f43f280b31152f1d27df3f9d189317.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "3ba6af611cc5467cfdbd5566561b8478.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "3cc4a7fc6481ea3681138da4643f3d16.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "3ea649db8e81a46ca4f92fb3238f78ff.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "3ef05501315073d9d4e1c6b654d99ac0.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "4085c929e00c446d3fee18b5b20a27f9.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "40bb78b1ac031125a6d8466b374962a8.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "46e5ac4a62d7a445a7c1fb704fafe05c.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "46f5d9c1b0fe352353688f736e5617b6.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "4838ece0d3900220d33528ee027289bc.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5315c35bbcc28d8eee419028ac9f38e0.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5482a54657765056f1a94116a8dbffe7.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "551c2656a4f6f9f5ea7e9945b9081202.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5633ed9d0eb700d0093bf85d86a95ebf.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "56d4a1bb53241f7c5ed6ab531320a542.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "59d3b529c78ac722127c41ba75b3355b.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5a43fa2cf9c1e47f0331ef71b928ee55.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5baad44ca4702949724234e35c5bb341.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5bc61724b33e34a6188a817f9f2f8138.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5c67195f6993c9f8d0d32d4ffe0d8e62.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "5dc71b1d868ef137394d3cc23abea65a.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "627c0779eb46b98f751187c5c9f43aa3.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "6903d4538fd33c8fd0ded32cb30d618e.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "6de166ee2a3a60df9017650e2a808408.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "72d091e08c93c9e590360130fa35221b.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "754664a12e36abff7950e796c906ae39.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "75e4bd7544a85af6438497980b62fba5.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "786b67badc535fc95a4a76c29a0e0146.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "7997b6b229f25315d33f5c7085e37500.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "79f5fc6bca756e1f067c6fc83e18b32e.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "7acc832f70b2ca62e58a953f3b90fd82.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "7dbf474f80e466e9e25ee46b84166420.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "7e7cdf7f4ee50b308531313bbf43e0c3.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "8417a305e3b43d5b1bda4ff06a660c54.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "8546907dbe574744d7fea6ca9de1de6b.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "865db3dd2d380626f16b6f9dc6d62dba.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "897b8b6d8feb466aa6cad5f512c3fce2.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "8a9cc8eeed66aeb423a91c44111d9450.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "8e330afbd99ba01b66570ed62fcdc6ab.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "8e5e74dbf9b68a322fbb9512db837329.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "90e46387f562ca8fa106b51dfcda1dc6.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "96b3e939852157613fa2e48d58fe35fe.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "9efd60f04cd971daa83d3131e6d6f389.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "a17806f32b45d63eea5230e7893e1f15.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "a54f8c866cbef6e6cda858c85d72dfc8.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "a7326ba8f3f4559991126474dd30083d.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "acb1fac4e618f636d415f62496e8b70e.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "acce3629083f0e348e94fb58f952d3de.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "adcb34b94f4c839bdd29037419a0ee53.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "b0b8914cc5f7a6eff409f16d8cc236c5.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "b4103df93880fc5677c2a081e4bfc712.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "b5369bcbddca7135a5708c5237ad64e4.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "b55977028a3a574336966b6536640fc9.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "ba60305ac83fe3d8ef01da1d9a0ecc79.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "bd8cf05698aee36b82b4caf58edea442.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c1ca5583e4bfadc73e7fe9418b6e6bf4.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c3018ebe53d0046eecb58858ca869a99.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c4ced510f44a9bfe85c696c05a7f791d.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c52ffdd6a0346c4d09271f8ccbdfd5a3.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c8bc97335529d069a753c67475b8c82c.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "c8c1a5675f82021d92b928a10c597bad.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "cc23dd79637b606cf5ba234a037e17ba.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "cc4ee796d16c9fe68978166c7cd1ae1b.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "ce380515a534e8226209daae00e7b4e8.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "corrupted.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "d085a42245996e5750a30ccb48791bcf.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "d15b71b8cebe35a57cc6e996cc09218b.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "d22db5be7594c17a18a047ca9264ea0a.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "d3b044a94486cae0224c002800ddd642.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "dc-predictor-overflow.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "de4ae285a275bcfe2ac87c0126742552.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "de5884cec093257d239f3b8be3e2f2e5.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "derive-huffman-codes-overflow.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "e18bb52107598f65b81b02be2c6c5124.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "e6d9eca2c7405e13cfb850b7d0ef7476.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "eddea4ef9629be031f750a8ff0b7497c.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "eecb78b937a7c5f04aae2f5b0f5b5acc.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "ef1f8a057bb6056674fad92f6b8c0acd.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "ef724193653930f52acffa90e6426fd2.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "empty.jpg", JPEG_EXPECT_INVALID},
	{"invalid", NULL, "f006e96f3b27fdfaa075322d759ea2e8.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f012a4321f00f12af6b1eee7580ffb9c.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f1fad47f213bb64c99f714652f30e49e.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f6419b06a39ff09604343848658b1a41.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f6b4389c3cf0f5997b2e5a4b905aea8d.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f6d3f522dcb693d9e731d5a0fb4e1393.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "f8e19feecd246156b5d7e79efc455e99.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "fd44dc63fa7bdd12ee34fc602231ef02.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "fddcfc778ada60229380c2493fc4c243.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image00971.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image01088.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image01137.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image01551.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image01713.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image01980.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "image02206.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "invalid-dimensions.jpg", JPEG_EXPECT_INVALID},
	{"invalid", NULL, "invalid-prediction-shift.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "max_size.jpg", JPEG_EXPECT_ROBUST},
	{"invalid", NULL, "missing-sof.jpg", JPEG_EXPECT_INVALID},
	{"invalid", NULL, "missing-sos.jpg", JPEG_EXPECT_INVALID},
	{"invalid", NULL, "null_height.jpg", JPEG_EXPECT_INVALID},
	{"invalid", NULL, "subtract-with-overflow.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "extraneous-data", "extraneous-bytes-after-sos.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "marker-quirks", "multiple-0xff-before-eoi.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "metadata-quirks", "icc_chunk_count_mismatch.jpeg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "metadata-quirks", "icc_chunk_double_seq_no.jpeg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "metadata-quirks", "icc_chunk_order.jpeg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "metadata-quirks", "icc_chunk_seq_no_0.jpeg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "metadata-quirks", "icc_missing_chunk.jpeg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "progressive-quirks", "mozjpeg-rs-ac-refine-q95.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "after_app0.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "after_soi.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "after_sos.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "mid_header.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "missing-frame-image-1410.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "missing_eoi.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "progressive_25pct.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "progressive_50pct.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "progressive_75pct.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "scan_10pct.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "scan_50pct.jpg", JPEG_EXPECT_ROBUST},
	{"non-conformant", "truncated", "scan_90pct.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_001_decode_failure.jpeg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_043_sony_ericsson.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_052_rst_marker.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_095_exif_zero.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_110_invalid_dht.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_125_oob_access.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_130_progressive_jpeg.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_148_assertion_failed.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_151_unset_quant_table.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_169_corrupt_no_error.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_173_corrupt_output.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_194_release_panic.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_249_incorrect_decode.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_251_fill_buffer.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder", "jd_262_truncated_eof.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "03032011362.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "1133349251_i_5957_full.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "1212.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "1335488m.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "154.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "21082008025.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "21082008029.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSC07578.JPG", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSC07826.JPG", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSC09303.JPG", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3918.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3924.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3930.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3934.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3936.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3937.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3942.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "DSCN3946.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "Greenwich 80.JPG", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "IMG0176.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "amazed2k1424ktv.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "e881bf9e898c2383818f18d9972fb18b_full.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "fantasy-20180613-the-shrine-zhenshchina-koloss-kamni.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "l1048750854.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "myasnik-1120120038_i_6092.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "painting-20180529-peter-miork-mionsted-peder-mork-monsted-danish-realist-pai-4.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "jpeg-decoder-257", "textures-20181017-osen-listia-fon-background-autumn-leaves-osennie-maple.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_259_int_overflow_dc_first.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_347_signed_overflow_dc_1.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_347_signed_overflow_dc_2.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_509_missing_huffman_table.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_669_uaf_ycc_rgb_a.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "libjpeg-turbo", "ljt_758_segv_adjust_quant_poc.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_004_garbled.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_005_black_stripe.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_007_shifted_rows.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_008_shifted_rows.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_040_decode_diff_2.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_064_red_channel_shift.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_067_no_more_bytes.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_104_incorrect_decode.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_162_mcu_assert.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_167_marker_soi.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_188_upsample_assert.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_207_mcu_range_cmyk.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_243_progressive_lowres.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_246_unknown_marker.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_261_xno_p10_camera.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_266_exhausted_data.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_269_exhausted_data.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_270_no_more_bytes_1.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_270_no_more_bytes_2.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_275_cmyk_3comp.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_276_vsample_not_pow2.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_277_broken_decode.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_278_marker_soi.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_288_two_components.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_291_cjpegli_incorrect.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_292_exhausted_data_panorama.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_323_regression_arm.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_340_decode_regression.jpg", JPEG_EXPECT_ROBUST},
	{"crash-repro", "zune-jpeg", "zj_341_noninterleaved_original.jpg", JPEG_EXPECT_ROBUST},
};

static void validate_image(gdImagePtr image, const jpeg_case *test,
					   const char *decoder) {
	if (image == NULL) {
		return;
	}
	gdTestAssertMsg(gdImageSX(image) > 0 && gdImageSY(image) > 0,
					"%s returned invalid dimensions for %s\n", decoder,
					test->filename);
	gdImageDestroy(image);
}

static void assert_case(const jpeg_case *test) {
	gdImagePtr image;
	FILE *fp;
	int require_decode = test->expectation == JPEG_EXPECT_VALID;
	int require_reject = test->expectation == JPEG_EXPECT_INVALID;

#if BITS_IN_JSAMPLE == 12
	if (test->expectation == JPEG_EXPECT_12BIT) {
		require_decode = 1;
	}
#endif

	if (test->subdirectory == NULL) {
		fp = gdTestFileOpenX("conformance", "jpeg", "conformance",
						 test->directory, test->filename, NULL);
	} else {
		fp = gdTestFileOpenX("conformance", "jpeg", "conformance",
						 test->directory, test->subdirectory, test->filename,
						 NULL);
	}
	gdTestAssertMsg(fp != NULL, "cannot open JPEG corpus file: %s\n",
					test->filename);
	if (fp == NULL) {
		return;
	}
	image = gdImageCreateFromJpeg(fp);
	fclose(fp);

	if (require_decode) {
		gdTestAssertMsg(image != NULL, "valid JPEG failed decoding: %s\n",
						test->filename);
	} else if (require_reject) {
		gdTestAssertMsg(image == NULL, "invalid JPEG decoded: %s\n",
						test->filename);
	}

	validate_image(image, test, "JPEG decoder");
}

int main(void) {
	size_t i;
	const size_t count = sizeof(cases) / sizeof(cases[0]);

	gdTestAssertMsg(count == 254, "JPEG corpus has %u cases, expected 254\n",
					(unsigned int)count);
	gdSetErrorMethod(gdSilence);
	for (i = 0; i < count; i++) {
		assert_case(&cases[i]);
	}
	gdClearErrorMethod();
	return gdNumFailures();
}
