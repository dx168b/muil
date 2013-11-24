#include "muil_widgets.hpp"

namespace muil {

void paint_colored_bitmap(int x1, int y1, int x2, int y2, Color color, const uint8_t *bmp_data, int bmp_width, int bmp_height)
{
	int width1 = bmp_width-1;
	int height1 = bmp_height-1;
	int new_width = x2-x1+1;
	int new_height = y2-y1+1;

	int color_summ = (color.r + color.g + color.b) / 3;

	bool same_width = (new_width == bmp_width);
	bool same_height = (new_height == bmp_height);

	int v00, v01, v10, v11;
	int oxPrev = -1, oyPrev = -1;
	int xcnt_incr = same_width ? bmp_width : bmp_width-1;
	int ycnt_incr = same_height ? bmp_height : bmp_height-1;
	int xcnt = same_width ? 0 : xcnt_incr / 2;
	int ox = 0;
	for (int x = 0; x < new_width; x++)
	{
		int oy = 0;
		int ycnt = same_height ? 0 : ycnt_incr/2;
		for (int y = 0; y < new_height; y++)
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

			int r = v * color.r / color_summ;
			int g = v * color.g / color_summ;
			int b = v * color.b / color_summ;
			if (r < 0) r = 0;
			if (r > 255) r = 255;
			if (g < 0) g = 0;
			if (g > 255) g = 255;
			if (b < 0) b = 0;
			if (b > 255) b = 255;
			display_set_point(x+x1, y+y1, Color(r, g, b));

			ycnt += ycnt_incr;
			while (ycnt >= new_height) { oy++; ycnt -= new_height; }
		}

		xcnt += xcnt_incr;
		while (xcnt >= new_width) { ox++; xcnt -= new_width; }
	}
}

void paint_bitmapped_widget(
	const Rect &rect,
	Color color,
	const MonochromeBitmap &bmp_lt,
	const MonochromeBitmap &bmp_t,
	const MonochromeBitmap &bmp_rt,
	const MonochromeBitmap &bmp_l,
	const MonochromeBitmap &bmp_c,
	const MonochromeBitmap &bmp_r,
	const MonochromeBitmap &bmp_lb,
	const MonochromeBitmap &bmp_b,
	const MonochromeBitmap &bmp_rb)
{
	int x1 = rect.x1;
	int x2 = rect.x1+bmp_l.width-1;
	int x5 = rect.x2-(bmp_r.width-1);
	int x6 = rect.x2;
	int x3 = x2+1;
	int x4 = x5-1;

	int y1 = rect.y1;
	int y2 = rect.y1+bmp_t.height-1;
	int y5 = rect.y2-(bmp_b.height-1);
	int y6 = rect.y2;
	int y3 = y2+1;
	int y4 = y5-1;

	paint_colored_bitmap(x1, y1, x2, y2, color, bmp_lt.data, bmp_lt.width, bmp_lt.height);
	paint_colored_bitmap(x3, y1, x4, y2, color, bmp_t.data, bmp_t.width, bmp_t.height);
	paint_colored_bitmap(x5, y1, x6, y2, color, bmp_rt.data, bmp_rt.width, bmp_rt.height);

	paint_colored_bitmap(x1, y3, x2, y4, color, bmp_l.data, bmp_l.width, bmp_l.height);
	paint_colored_bitmap(x3, y3, x4, y4, color, bmp_c.data, bmp_c.width, bmp_c.height);
	paint_colored_bitmap(x5, y3, x6, y4, color, bmp_r.data, bmp_r.width, bmp_r.height);

	paint_colored_bitmap(x1, y5, x2, y6, color, bmp_lb.data, bmp_lb.width, bmp_lb.height);
	paint_colored_bitmap(x3, y5, x4, y6, color, bmp_b.data, bmp_b.width, bmp_b.height);
	paint_colored_bitmap(x5, y5, x6, y6, color, bmp_rb.data, bmp_rb.width, bmp_rb.height);
}


} // namespace muil
