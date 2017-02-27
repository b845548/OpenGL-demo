

extern void triangle_edge(GLfloat *im, int x, int y, int w, int h, int width);
extern void triangleNormal(GLfloat * out, GLfloat * p0, GLfloat * p1, GLfloat * p2);
extern void dataNormals(GLfloat * data, int w, int h);
extern GLfloat * heightMap2Data(GLfloat * hm, int w, int h);
extern GLuint * heightMapIndexedData(int w, int h);
