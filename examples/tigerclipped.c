#include "gd.h"
#include <stdio.h>

void save_png(gdSurfacePtr surface, const char *filename)
{
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Can't save png image %s\n", filename);
        return;
    }
    gdSurfacePng(surface, fp);
    fclose(fp);
}
#include "tiger.inc"

int main()
{
    const int width = 1000;
    const int height = 1000;
    gdSurface *surface;
    unsigned int i;

    surface = gdSurfaceCreate(width, width, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }

    gdContextPtr cr = gdContextCreate(surface);
    gdContextSetLineWidth(cr, 10.0);
    gdContextArc(cr, width/2, height/2, width/3, 0, M_PI*2);
    gdContextClip(cr);

    gdContextSetSourceRgba(cr, 0.1, 0.2, 0.3, 1.0);
    gdContextTranslate(cr, width/3, height/3);

    gdContextScale(cr, 1.5, 1.5);
    //gdContextMoveTo(cr, width/2, height/2);
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

    save_png(surface, "tigerclipped.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
}