#include <cmath>
#include "tgaimage.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	bool steep = false;//x,y置换标志
	if(abs(x0 - x1) < abs(y0 - y1)) {
		swap(x0, y0);
		swap(x1, y1);
		steep = true;
	}
	if(x0 > x1) {//保证正反都能划线
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = abs(dy)*2;
	int error2 = 0;//累积偏移量
	int y = y0;
	for(int x = x0; x <= x1; x++) {
		if(steep) {
			image.set(y, x, TGAColor(255, 1));//image.set(y, x, color);
		}
		else {
			image.set(x, y, TGAColor(255, 1));//image.set(y, x, color);
		}
		error2 += derror2;
		if(error2 > dx) {
			y += (y1 > y0 ? 1 : -1);//线的朝向
			error2 -= dx*2;
		}
	}
}

int main(int argc, char** argv)
{
	TGAImage image(100, 100, TGAImage::RGB);
	for(int i = 0; i < 1000000; i++) {
		line(13, 20, 80, 40, image, white);
		line(20, 13, 40, 80, image, red);
		line(80, 40, 13, 20, image, red);//需要改上面的set这里颜色才能生效
	}
	image.flip_vertically();
    image.write_tga_file("output.tga");
	return 0;
}
