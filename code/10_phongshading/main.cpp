#include <vector>
#include <iostream> 
#include "tgaimage.h"
#include "model.h"
#include "our_gl.h"
#include "geometry.h"

#define CLAMP(t) ((t > 1.f) ? 1.f : ((t < 0.f) ? 0.f : t))

using namespace std;

Model *model = NULL;
const int width  = 600; 
const int height = 600; 

Vec3f light_dir(-1, 1, 1);
Vec3f eye(0, 0, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct vertex {
	Vec4f v;//坐标
	Vec3f vn;//法线
	Vec2f uv;//纹理
};

struct GouraudShader: public IShader {
	mat<3, 3, float> varying_vn; //三个顶点法向量
	virtual Vec4f VertexShader(int iface, int nthvert) {
		vertex vert;
		vert.v = embed<4>(model->vert(iface, nthvert));
		vert.vn = model->normal(iface, nthvert);
		vert.v = Projection * ModelView * vert.v;
		varying_vn.set_col(nthvert, vert.vn);
		vert.v = ViewPort * vert.v;
		return vert.v;
	}

	virtual bool FragmentShader(Vec3f bar, TGAColor& color) {
		Vec3f n = varying_vn.col(0)*bar[0] + varying_vn.col(1)*bar[1] + varying_vn.col(2)*bar[2];
		float intensity = CLAMP(n * light_dir);
		color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	} else {
		model = new Model("obj/african_head.obj");
	}
	
    TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
	
	lookat(eye, center, up);
	viewport(width/8, height/8, width*3/4, height*3/4);
	projection(-1.f / (eye - center).norm());//norm是求模，不是归一化
	light_dir.normalize();

	GouraudShader shader;

	for (int i = 0; i < model->nfaces(); i++) { 
		vector<int> face = model->face(i); 
		Vec4f screen_coords[3]; 
 		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.VertexShader(i, j);
		}
		triangle(screen_coords, shader, image, zbuffer);
	}

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0; 
}