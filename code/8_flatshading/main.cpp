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
Vec3f eye(3, 3, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct FlatShader: public IShader {
	mat<3, 3, float> varying_triangle; //保留转换后三角形三条边的向量,用于计算三角形的法向量

	virtual Vec4f VertexShader(int iface, int nthvert) {
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));//转成齐次坐标
		gl_Vertex = Projection * ModelView * gl_Vertex;
		varying_triangle.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3])); //proj->projection 投影，取前几位
		gl_Vertex = ViewPort * gl_Vertex;
		return gl_Vertex;
	}

	virtual bool FragmentShader(Vec3f bar, TGAColor& color) {
		Vec3f n = cross(varying_triangle.col(1) - varying_triangle.col(0), varying_triangle.col(2) -
		varying_triangle.col(0)).normalize();
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

	FlatShader shader;

	for (int i = 0; i < model->nfaces(); i++) { 
		vector<int> face = model->face(i); 
		Vec4f screen_coords[3]; 
 		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.VertexShader(i, j);
		}
		triangle(screen_coords, shader, image, zbuffer);
	}

	zbuffer.flip_vertically();
	zbuffer.write_tga_file("zbuffer.tga");
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0; 
}