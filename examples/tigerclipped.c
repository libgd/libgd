#include "vector2d_example.h"
#include <stdio.h>

#include "tiger.inc"

int main(void)
{
	const int width = 1000;
	const int height = 1000;
	gdImagePtr image = vector2d_create_image(width, height,
		gdTrueColorAlpha(255, 255, 255, 127));
	gdContextPtr context;
	unsigned int i;

	if (!image) {
		fprintf(stderr, "Can't create image\n");
		return 1;
	}
	context = gdContextCreateForImage(image);
	if (!context) {
		gdImageDestroy(image);
		return 1;
	}

	gdContextArc(context, width / 2.0, height / 2.0, width / 3.0,
		0, M_PI * 2);
	gdContextClip(context);
	gdContextTranslate(context, width / 3.0, height / 3.0);
	gdContextScale(context, 1.5, 1.5);

	for (i = 0; i < sizeof(tiger_commands) / sizeof(tiger_commands[0]); i++) {
		const struct command *cmd = &tiger_commands[i];
		switch (cmd->type) {
		case 'm':
			gdContextMoveTo(context, cmd->x0, cmd->y0);
			break;
		case 'l':
			gdContextLineTo(context, cmd->x0, cmd->y0);
			break;
		case 'c':
			gdContextCurveTo(context, cmd->x0, cmd->y0, cmd->x1, cmd->y1,
				cmd->x2, cmd->y2);
			break;
		case 'f':
			gdContextSetSourceRgba(context, cmd->x0, cmd->y0, cmd->x1, cmd->y1);
			gdContextFill(context);
			break;
		}
	}

	gdContextFlushImage(context);
	vector2d_save_png(image, "tigerclipped.png");
	gdContextDestroy(context);
	gdImageDestroy(image);
	return 0;
}
