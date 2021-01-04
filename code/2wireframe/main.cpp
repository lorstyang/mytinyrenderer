#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

/* void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
	//这里没有使用优化的划线法，原因是编译器做的优化已经足够好了
} */

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

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i=0; i<model->nfaces(); i++) {
        vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) { //每个(三角)面有三条线
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);//0--1,1--2,2--0
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image, white);
        }
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}