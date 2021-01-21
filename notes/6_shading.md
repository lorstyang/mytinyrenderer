# shading

整体代码改动很大，但是中心思想很好理解，也就是要把坐标变换（VertexShader）和像素着色（FragmentShader）这两部分分离出来

当然实际渲染管线中还有很多其他的内容，但是本质上与此也有共通之处。

OpenGL-2.0-Programmable-Shader-Pipeline

![OpenGL-2.0-Programmable-Shader-Pipeline.png](https://i.loli.net/2021/01/21/ryRsFCYOezVfTW3.png)

光栅化的情况下，主要有三种着色方式，flatshading，Gouraud shading，Phongshading

flatshading逐三角形面着色，计算每个三角形的光照，每个三角形是一样的颜色

Gouraud shading逐顶点着色，计算每个顶点的光照，三角形内每个像素的颜色用重心坐标插值得到

Phongshading逐像素着色，计算每个像素的光照

## flat shading

* ``` c++
Vec2f bboxmin(numeric_limits<float>::max(), numeric_limits<float>::max());
Vec2f bboxmax(-numeric_limits<float>::max(), -numeric_limits<float>::max());
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
        //齐次坐标
        bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
        bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
    }
}
  ```
  
  检测时候在视野范围内的部分去掉了，不过也无所谓，本来模型的顶点是肯定在范围内的，不过在实际中是否在视野内就要涉及到模板测试了

* 代码里其实每个像素都计算了一次fragmentshader，同一个三角形内的像素因为法线一样，计算得到的颜色其实是一样的，从这个角度应该可以优化一下，每个三角形只计算一次FS，后面的直接用就可以了。

* 我吐了，编译的时候一直提示找不到triangle函数，检查了几遍觉得没问题啊，然后把函数名+参数重写了一遍编译过了，明明和之前的一模一样。难道是复制过来的原因，这也太扯了吧。

## gouraud shading

flatshading能搞出来，其他两种也蛮简单了，改一下VS和FS就行

``` c++
struct vertex {
	Vec4f v;//坐标
	Vec3f vn;//法线
	Vec2f uv;//纹理
};

struct GouraudShader: public IShader {
	float intensity[3] = {0};
	virtual Vec4f VertexShader(int iface, int nthvert) {
		vertex vert;
		vert.v = embed<4>(model->vert(iface, nthvert));
		vert.vn = model->normal(iface, nthvert);
		intensity[nthvert] = vert.vn * light_dir;
		vert.v = Projection * ModelView * vert.v;
		vert.v = ViewPort * vert.v;
		return vert.v;
	}

	virtual bool FragmentShader(Vec3f bar, TGAColor& color) {
		float PixIntens = bar[0]*intensity[0] + bar[1]*intensity[1] + bar[2]*intensity[2];
		color = TGAColor(255, 255, 255) * PixIntens;
		return false;
	}
};
```

* gouraud距离点光源很近时会出现问题
* 才发现我重载的向量乘法运算符只支持左操作数向量，右操作数常数，实在是过于弱智，懒得改了，淦

## phong shading

* phong需要做更多的计算，这里算光照很简单，就一个强度与法线乘就完事了，看起来没什么，但实际上算光照会用很多其他复杂的光照模型，blinn-phong这种，就会导致大量的计算了。

``` c++
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
```

## 其他

所以通过shader可以实现很多酷炫的效果

比如教程里的

``` c++
virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity*bar;
        if (intensity>.85) intensity = 1;
        else if (intensity>.60) intensity = .80;
        else if (intensity>.45) intensity = .60;
        else if (intensity>.30) intensity = .45;
        else if (intensity>.15) intensity = .30;
        else intensity = 0;
        color = TGAColor(255, 155, 0)*intensity;
        return false;
}
```

看着就有点卡通的味道

![fs1.png](https://i.loli.net/2021/01/21/ogy6FUTlRIsaEcX.png)