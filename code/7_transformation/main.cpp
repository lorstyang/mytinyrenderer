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

Vec3f light_dir(0, 0, -1);
Vec3f eye(3, 3, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

Matrix ModelView;
Matrix ViewPort;
Matrix Projection;

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
	Vec3f z = (eye-center).normalize();//摄像机坐标系的z轴方向与观察方向相反
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    Matrix Minv = Matrix::identity();//rotation
    Matrix Tr   = Matrix::identity();//translation
    for (int i = 0; i < 3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    ModelView = Minv*Tr;
}

void viewport(int x, int y, int w, int h) {
	ViewPort = Matrix::identity();
	ViewPort[0][3] = x + w/2.f;
	ViewPort[1][3] = y + h/2.f;
	ViewPort[2][3] = 1.f;
	ViewPort[0][0] = w/2.f;
	ViewPort[1][1] = h/2.f;
	ViewPort[2][2] = 1.f;
}

void projection(float coeff) {
	Projection = Matrix::identity();
	Projection[3][2] = coeff;
}

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

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) { 
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
			//barycentric[0]*pts[0].z + ...
			for(int i = 0; i < 3; i++) {
				P.z += bc_screen[i]*pts[i][2];
			}
			if(P.z > zbuffer[int(P.x + width*P.y)]) {
				image.set(P.x, P.y, color); 
				zbuffer[int(P.x + width*P.y)] = P.z;
			}
        } 
    }
} 

Vec3f world2screen(Vec3f v) {
	Vec4f gl_vertex = embed<4>(v); // embed Vec3f to homogenius coordinates
	gl_vertex = ViewPort * Projection * ModelView * gl_vertex; // MVP + ViewPort
	Vec3f v3 = proj<3>(gl_vertex/gl_vertex[3]); // transfromed vec3f vertex
	return Vec3f(int(v3.x + .5), int(v3.y + .5), v3.z);
}


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	} else {
		model = new Model("obj/african_head.obj");
	}
	
    TGAImage image(width, height, TGAImage::RGB);
	float *zbuffer = new float[width*height];
	for (int i = width*height; i--; zbuffer[i] = -numeric_limits<float>::max());
	
	lookat(eye, center, up);
	viewport(width/8, height/8, width*3/4, height*3/4);
	projection(-1.f/3);
	
	for (int i = 0; i < model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		Vec3f screen_coords[3]; 
		Vec3f world_coords[3];
 		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->vert(face[j]); 
			screen_coords[j] = world2screen(world_coords[j]);
		}
		Vec3f n = cross((world_coords[2]-world_coords[0]), (world_coords[1]-world_coords[0]));
		n.normalize();//归一化
		float intensity = n*light_dir;
		
		if(intensity > 0) {
			int r = intensity*255;
			int g = intensity*255;
			int b = intensity*255;
			triangle(screen_coords, zbuffer, image, TGAColor(r, g, b, 255));
		}
	}
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0; 
}