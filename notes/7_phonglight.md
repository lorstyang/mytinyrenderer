# phong reflection model

* 环境光 
  $$
  c_{ambient}
  $$
  

* 漫反射
  $$
  c_{diffuse} = (c_{light} \cdot m_{diffuse})\max(0, \vec n \cdot \vec l)
  $$
  $$ \vec n $$是法线  $$ \vec l $$是光源方向（指向光源）  $$ m_{diffuse} $$是漫反射颜色

* 高光（镜面）反射
  $$
  c_{specular} = (c_{light} \cdot m_{specular})\max(0, \vec v \cdot \vec r)^{m_{gloss}}
  $$
  $$ \vec v $$是视角方向  $$ \vec r $$是反射方向  

  $$ m_{gloss} $$是光泽度，用于控制高光区域大小，$$ m_{gloss} $$越大，亮点越小
  $$
  \vec r = 2(\vec n - \vec l)\vec n - \vec l
  $$
  

三者加起来就是完整的phong光照模型，blinn-phong模型就是在原有基础上改进了计算方式，本来需要算视角方向和反射方向的夹角cos，他用了半程向量和法向量之间的夹角cos代替之。

* ```c++
  for (int i = 0; i < 3; i++) color[i] = min<float>(5 + c[i]*(diff + .6*spec), 255);
  ```

  他这个算color的方式有点迷惑，按照bgr的顺序一个一个分量去算确实没见过

# 各种mapping

* 法线贴图
* 阴影贴图
* 甚至高光都是贴图

总之，还是很方便的，跟纹理一样，拿着插值出来的uv坐标去读完事了

这东西应该是美工做的吧，辛苦美工了XD