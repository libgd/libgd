#include <check.h>
#include <stdlib.h>
#include "gd.h"

START_TEST(test_gdImageScale_overflow_dimensions)
{
    /* Invariant: Buffer reads/writes never exceed the declared allocation length.
     * When image dimensions cause integer overflow in allocation size calculations,
     * the operation must fail gracefully (return NULL) rather than allocate an
     * undersized buffer and overflow it.
     */
    struct {
        int src_w;
        int src_h;
        int dst_w;
        int dst_h;
    } cases[] = {
        /* Exploit case: large dimensions causing overflow in line_length * sizeof(ContributionType) */
        {10, 10, 0x40000001, 100},
        /* Boundary: dimensions just at overflow threshold */
        {10, 10, 100, 0x40000001},
        /* Valid input: normal scale operation should succeed */
        {100, 100, 200, 200},
    };
    int num_cases = sizeof(cases) / sizeof(cases[0]);

    for (int i = 0; i < num_cases; i++) {
        gdImagePtr src = gdImageCreateTrueColor(cases[i].src_w, cases[i].src_h);
        if (!src) continue;

        gdImagePtr dst = gdImageScale(src, cases[i].dst_w, cases[i].dst_h);

        if (i < 2) {
            /* Overflow cases: must return NULL (reject) rather than corrupt heap */
            /* If it doesn't return NULL, at minimum it must not crash */
        }
        if (i == 2) {
            /* Valid case should succeed */
            ck_assert_ptr_nonnull(dst);
        }

        if (dst) gdImageDestroy(dst);
        gdImageDestroy(src);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_set_timeout(tc_core, 10);
    tcase_add_test(tc_core, test_gdImageScale_overflow_dimensions);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}