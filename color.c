#include "color.h"

Rgb666 ColorCompress(Bgr888 bgr)
{
	Rgb666 result;
	const uint16_t red = (uint16_t)((bgr.red & 0xFC) << 8);
	const uint16_t green = (uint16_t)(bgr.green & 0xFC);
	const uint16_t blue = (uint16_t)(bgr.blue << 8);
	result.first = red | green;
	result.second = blue;
	return result;
}
