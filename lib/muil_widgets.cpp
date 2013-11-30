#include <stddef.h>
#include "muil_widgets.hpp"

namespace muil {

void paint_colored_bitmap(int x1, int y1, int x2, int y2, Color color, const uint8_t *bmp_data, int bmp_width, int bmp_height)
{
	if (bmp_data == NULL) return;

	int width1 = bmp_width-1;
	int height1 = bmp_height-1;
	int new_width = x2-x1+1;
	int new_height = y2-y1+1;

	if ((bmp_width) == 1 && (bmp_height == 1))
	{
		display_fill_rect(x1, y1, x2, y2, color);
		return;
	}

	int color_summ = (color.r + color.g + color.b) / 3;

	bool same_width = (new_width == bmp_width);
	bool same_height = (new_height == bmp_height);

	int v00 = 0, v01 = 0, v10 = 0, v11 = 0;
	int oxPrev = -1, oyPrev = -1;
	int xcnt_incr = same_width ? bmp_width : bmp_width-1;
	int ycnt_incr = same_height ? bmp_height : bmp_height-1;
	int ycnt = same_height ? 0 : ycnt_incr/2;
	int oy = 0;
	for (int y = 0; y < new_height; y++)
	{
		int xcnt = same_width ? 0 : xcnt_incr / 2;
		int ox = 0;
		int sx = -1;
		int prev_v = -1;
		for (int x = 0; x < new_width; x++)
		{
			if ((oxPrev != ox) || (oyPrev != oy))
			{
				v00 = bmp_data[ox+oy*bmp_width];
				if (oy < height1)
					v01 = bmp_data[ox+(oy+1)*bmp_width];
				else
					v01 = 0;
				if (ox < width1)
					v10 = bmp_data[ox+1+oy*bmp_width];
				else
					v10 = 0;
				if (oy < height1 && ox < width1)
					v11 = bmp_data[ox+1+(oy+1)*bmp_width];
				else
					v11 = 0;

				oxPrev = ox;
				oyPrev = oy;
			}

			int cf1 = (new_height-ycnt);
			int v1 = v00*cf1/new_height + v01*ycnt/new_height;
			int v2 = v10*cf1/new_height + v11*ycnt/new_height;

			int cf2 = (new_width-xcnt);
			int v = v1*cf2/new_width + v2*xcnt/new_width;

			if (v > 255) v = 255;
			if (v < 0) v = 0;

			if (v != prev_v)
			{
				if ((prev_v != 255) && (prev_v != -1))
					display_fill_rect(sx, y+y1, x+x1-1, y+y1, color.adjusted(prev_v, color_summ));
				sx = x+x1;
				prev_v = v;
			}

			xcnt += xcnt_incr;
			while (xcnt >= new_width) { ox++; xcnt -= new_width; }
		}

		if ((prev_v != 255) && (prev_v != -1))
			display_fill_rect(sx, y+y1, x2, y+y1, color.adjusted(prev_v, color_summ));

		ycnt += ycnt_incr;
		while (ycnt >= new_height) { oy++; ycnt -= new_height; }
	}
}

void paint_bitmapped_widget(
	const Rect    &rect,
	Color         color,
	const uint8_t *bmp_lt,
	const uint8_t *bmp_t,
	const uint8_t *bmp_rt,
	const uint8_t *bmp_l,
	const uint8_t *bmp_c,
	const uint8_t *bmp_r,
	const uint8_t *bmp_lb,
	const uint8_t *bmp_b,
	const uint8_t *bmp_rb,
	int           width_l,
	int           width_c,
	int           width_r,
	int           height_t,
	int           height_c,
	int           height_b)
{
	int x1 = rect.x1;
	int x2 = rect.x1+width_l-1;
	int x5 = rect.x2-(width_r-1);
	int x6 = rect.x2;
	int x3 = x2+1;
	int x4 = x5-1;
	int y1 = rect.y1;
	int y2 = rect.y1+height_t-1;
	int y5 = rect.y2-(height_b-1);
	int y6 = rect.y2;
	int y3 = y2+1;
	int y4 = y5-1;

	paint_colored_bitmap(x1, y1, x2, y2, color, bmp_lt, width_l, height_t);
	paint_colored_bitmap(x3, y1, x4, y2, color, bmp_t,  width_c, height_t);
	paint_colored_bitmap(x5, y1, x6, y2, color, bmp_rt, width_r, height_t);
	paint_colored_bitmap(x1, y3, x2, y4, color, bmp_l,  width_l, height_c);
	paint_colored_bitmap(x3, y3, x4, y4, color, bmp_c,  width_c, height_c);
	paint_colored_bitmap(x5, y3, x6, y4, color, bmp_r,  width_r, height_c);
	paint_colored_bitmap(x1, y5, x2, y6, color, bmp_lb, width_l, height_b);
	paint_colored_bitmap(x3, y5, x4, y6, color, bmp_b,  width_c, height_b);
	paint_colored_bitmap(x5, y5, x6, y6, color, bmp_rb, width_r, height_b);
}


} // namespace muil
