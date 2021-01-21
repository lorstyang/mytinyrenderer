#include <cmath>
#include <limits>
#include <cstdlib>
#include "our_gl.h"

using namespace std;

Matrix ModelView;
Matrix ViewPort;
Matrix Projection;

const float depth = 255.0;

IShader::~IShader() {}

void viewport(int x, int y, int w, int h) {
	ViewPort = Matrix::identity();
	ViewPort[0][3] = x + w / 2.f;
	ViewPort[1][3] = y + h / 2.f;
	ViewPort[2][3] = depth / 2.f;
	ViewPort[0][0] = w / 2.f;
	ViewPort[1][1] = h / 2.f;
	ViewPort[2][2] = depth / 2.f;
}

void projection(float coeff) {
	Projection = Matrix::identity();
	Projection[3][2] = coeff;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    Matrix Minv = Matrix::identity();
    Matrix Tr = Matrix::identity();
    for (int i = 0; i < 3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    ModelView = Minv * Tr;
}

Vec3f barycentric(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = p2[i] - p0[i];
		s[i][1] = p1[i] - p0[i];
		s[i][2] = p0[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (abs(u[2]) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(Vec4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer) {
    Vec2f bboxmin(numeric_limits<float>::max(), numeric_limits<float>::max());
    Vec2f bboxmax(-numeric_limits<float>::max(), -numeric_limits<float>::max());
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            //齐次坐标
            bboxmin[j] = min(bboxmin[j], pts[i][j] / pts[i][3]);
            bboxmax[j] = max(bboxmax[j], pts[i][j] / pts[i][3]);
        }
    }
    Vec2i P;
    TGAColor color;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f bc = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]),
			proj<2>(pts[2] / pts[2][3]), P);
            //不在三角形内的像素也计算深度值虽然浪费时间，但是可以得到灰度图
            float z = 0.0, w = 0.0;
            for (int i = 0; i < 3; i++) {
                z += bc[i] * pts[i][2];
                w += bc[i] * pts[i][3];
            }
            int frag_depth = max(0, min(255, int(z / w + 0.5)));
            if (bc.x < 0 || bc.y < 0 || bc.z < 0 || zbuffer.get(P.x, P.y)[0] > frag_depth) continue;
            bool discard = shader.FragmentShader(bc, color);
            if (!discard) {
                image.set(P.x, P.y, color);
                zbuffer.set(P.x, P.y, TGAColor(frag_depth));
            }
        }
    }
}

