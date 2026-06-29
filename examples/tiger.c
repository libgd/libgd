#include "vector2d_example.h"
#include <stdio.h>

#include "tiger.inc"

int main()
{
    const int width = 1000;
    const int height = 1000;
    gdImagePtr image;
    unsigned int i;

    image = vector2d_create_image(width, width, gdTrueColorAlpha(255, 255, 255, 127));
    if (!image)
    {
        fprintf(stderr, "Can't create image\n");
        return 1;
    }

    gdContextPtr cr = gdContextCreateForImage(image);

    gdContextSetSourceRgba(cr, 0.1, 0.2, 0.3, 1.0);
    gdContextTranslate(cr, width/3, height/3);

    gdContextScale(cr, 1.5, 1.5);
    for (i = 0; i < sizeof(tiger_commands) / sizeof(tiger_commands[0]); i++)
    {
        const struct command *cmd = &tiger_commands[i];
        switch (cmd->type)
        {
        case 'm':
            gdContextMoveTo(cr, cmd->x0, cmd->y0);
            break;
        case 'l':
            gdContextLineTo(cr, cmd->x0, cmd->y0);
            break;
        case 'c':
            gdContextCurveTo(cr,
                           cmd->x0, cmd->y0,
                           cmd->x1, cmd->y1,
                           cmd->x2, cmd->y2);
            break;
        case 'f':
            gdContextSetSourceRgba(cr,
                                  cmd->x0, cmd->y0, cmd->x1, cmd->y1);
            gdContextFill(cr);
            break;
        }
    }

    gdContextSetSourceRgba(cr, 1, 0.2, 0.3, 1.0);
    gdContextMoveTo(cr, width/2, height/2);
    gdContextSetLineWidth(cr, 4);
    gdContextArc(cr, width/2, height/2, 30, M_PI/2, M_PI/2);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "tiger.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
}
