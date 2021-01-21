#include <vector>
#include <cmath>
#include <iostream> 
#include <ctime>
#include <cstdlib>
#include <limits>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

using namespace std;

Model *model = NULL;
const int width  = 600; 
const int height = 600; 
 
Vec3f barycentric(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = p2[i]-p0[i];
		s[i][1] = p1[i]-p0[i];
		s[i][2] = p0[i]-P[i];
	}
	Vec3f u = cross(s[0], s[1]);
    if (abs(u[2])<1) return Vec3f(-1, 1, 1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
} 

void triangle(Vec3f *pts, Vec2f *texture, float *zbuffer, TGAImage &image) { 
    Vec2f bboxmin(numeric_limits<float>::max(), numeric_limits<float>::max());
    Vec2f bboxmax(-numeric_limits<float>::max(), -numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i = 0; i < 3; i++) { 
        for (int j = 0; j < 2; j++) { 
            bboxmin[j] = max(0.f, min(bboxmin[j], pts[i][j])); 
            bboxmax[j] = min(clamp[j], max(bboxmax[j], pts[i][j])); 
        } 
    } 
    Vec3f P; 
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) { 
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P); 
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;//记得初始化!!!
			Vec2f Ptex(0, 0);
			//barycentric[0]*pts[0].z + ...
			for(int i = 0; i < 3; i++) {
				P.z += bc_screen[i]*pts[i][2];
				Ptex[0] += bc_screen[i]*texture[i][0];
				Ptex[1] += bc_screen[i]*texture[i][1];
			}
			if(P.z > zbuffer[int(P.x + width*P.y)]) {
				TGAColor color = model->diffuse(Ptex);
				//这里只把纹理上的颜色填上了，并没有光照
				image.set(P.x, P.y, color); 
				zbuffer[int(P.x + width*P.y)] = P.z;
			}
        } 
    }
} 

Vec3f world2screen(Vec3f v) {
	//四舍五入取整
	return Vec3f(int((v.x+1.)*width/2. + .5), int((v.y+1.)*height/2. + .5), v.z);
}


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	} else {
		model = new Model("obj/african_head.obj");
	}
	
    TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir(0, 0, -1);
	float *zbuffer = new float[width*height];
	for (int i = width*height; i--; zbuffer[i] = -numeric_limits<float>::max());
	
	for (int i = 0; i < model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		Vec3f screen_coords[3]; 
		Vec3f world_coords[3];
		Vec2f texture[3];
 		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->vert(face[2*j]); 
			screen_coords[j] = world2screen(world_coords[j]);
			texture[j] = model->uv(face[2*j + 1]);
		}
		triangle(screen_coords, texture, zbuffer, image);
	}
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0; 
}