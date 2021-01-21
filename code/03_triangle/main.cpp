#include <vector> 
#include <iostream> 
#include <ctime>
#include <cstdlib>
#include "geometry.h"
#include "tgaimage.h" 

using namespace std;

const int width  = 200; 
const int height = 200; 
 
/* Vec3f barycentric(Vec2i p0, Vec2i p1, Vec2i p2, Vec2i P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {//学到的很帅的写法
		s[i][0] = p2[i]-p0[i];
		s[i][1] = p1[i]-p0[i];
		s[i][2] = p0[i]-P[i];
	}
	Vec3f u = cross(s[0], s[1]);
    //Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]), 
					//Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]));
    if (abs(u[2])<1) return Vec3f(-1, 1, 1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
}  */

bool insideTriangle(Vec2i *pts, Vec2i P) {
	Vec2i p01 = pts[0] - pts[1];
	Vec2i p12 = pts[1] - pts[2];
	Vec2i p20 = pts[2] - pts[0];
	Vec3f p0p1(p01.x, p01.y, 0);//假设图片平面为xy平面，z坐标都为0
	Vec3f p1p2(p12.x, p12.y, 0);
	Vec3f p2p0(p20.x, p20.y, 0);
	Vec3f p0P(P.x - pts[0][0], P.y - pts[0][1], 0);
	Vec3f p1P(P.x - pts[1][0], P.y - pts[1][1], 0);
	Vec3f p2P(P.x - pts[2][0], P.y - pts[2][1], 0);
	float flag1 = cross(p0p1, p0P).z;
	float flag2 = cross(p1p2, p1P).z;
	float flag3 = cross(p2p0, p2P).z;
	if((flag1 < 0 && flag2 < 0 && flag3 < 0) || (flag1 > 0 && flag2 > 0 && flag3 > 0)) {
		return true;
	}
	return false;
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) { 
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    for (int i=0; i<3; i++) { 
        for (int j=0; j<2; j++) { 
            bboxmin[j] = max(0, min(bboxmin[j], pts[i][j])); 
            bboxmax[j] = min(clamp[j], max(bboxmax[j], pts[i][j])); 
        } 
    } 
    Vec2i P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            /* Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
            image.set(P.x, P.y, color); */ 
			if(insideTriangle(pts, P)) {
				image.set(P.x, P.y, color);
			}
        } 
    }
} 
 
int main(int argc, char** argv) { 
    TGAImage frame(200, 200, TGAImage::RGB); 
    Vec2i pts[3] = {Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160)}; 
    triangle(pts, frame, TGAColor(255, 0, 0, 255)); 
    frame.flip_vertically();
    frame.write_tga_file("framebuffer.tga");
    return 0; 
}