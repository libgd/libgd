#ifdef __cplusplus
extern "C" {
#endif

#ifndef GDFX_H
#define GDFX_H 1

BGD_DECLARE(gdImagePtr) gdImageSquareToCircle(gdImagePtr im, int radius);

BGD_DECLARE(char *) gdImageStringFTCircle(
    gdImagePtr im,
    int cx,
    int cy,
    double radius,
    double textRadius,
    double fillPortion,
    char *font,
    double points,
    char *top,
    char *bottom,
    int fgcolor);

BGD_DECLARE(void) gdImageSharpen (gdImagePtr im, int pct);

#endif /* GDFX_H */


#ifdef __cplusplus
}
#endif
