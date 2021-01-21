# texture

纹理就是一张二维图片，可以把它理解为贴在几何体表面上的一层贴图，用来达到想要显示的材质效果

代表的实际上是物体不同地方的反射系数

那么几何体上的顶点怎么对应到纹理相应的位置上呢

* 通过纹理坐标，一般用u，v表示纹理坐标

* obj文件里（随便用个编辑器打开）的vt就是纹理坐标

* ```
  f 24/1/24 25/2/25 26/3/26
  ```

  有三组数字，每组代表一个顶点，每一组里面用斜杠分开的三个数字表示顶点带着的信息，意思是 顶点索引/顶点法向量索引/顶点纹理坐标索引

  之前读face的时候，实际上部分数据被丢弃了

  通过这个就可以找到对应的纹理坐标和法向量

* 至于对应几何体的纹理怎么做，那就是美工的事了XD

<img src="https://i.loli.net/2021/01/19/PQlzc6RpIOC8AuN.png" width="50%" height="50%">