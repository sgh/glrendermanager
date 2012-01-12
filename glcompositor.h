#ifndef GLCOMPOSITOR_H
#define GLCOMPOSITOR_H


#include <GL/gl.h>
#include <list>
#include <ft2build.h>
#include FT_FREETYPE_H

#define iX 0
#define iY 1
#define iZ 2

#define iTransX     12
#define iTransY     13
#define iTransZ     14
#define iTransScale 15

#define DEG2RAD(a) ((a)*M_PI/180)

#define GLRENDER_NONE      0
#define GLRENDER_GEOMETRY  1
#define GLRENDER_2D_TEXT   2
#define GLRENDER_2D_IMAGE  3

#warning http://www.opengl.org/wiki/VBO_-_just_examples

struct Geometry {
	unsigned int npoints;
	GLfloat* points;
	GLuint   vbo;
	GLfloat* colors;
	GLfloat* normals;
	unsigned int primitive_type;
	bool draw_points;
	bool draw_lines;
	bool draw_solid;
};

struct Text {
	int x;
	int y;
	const char* str;
	FT_Face face;
	FT_Library ftlib;
};

struct ScreenBitmap {
	int x;
	int y;
	int width;
	int height;
	const char* data;
	float alpha;
};

struct STLData {
	GLfloat* points;
	GLfloat* normals;;
	unsigned int numvertices;
};
void readSTL(const char* filename, struct STLData* data);

void MyIdentityMatrix(GLfloat a[16]);
void MyRotateMatrix(GLfloat a[16], float x, float y, float z);

union SceneElementData {
	struct Geometry     geometry;
	struct Text         text;
	struct ScreenBitmap screenBitmap;
};

class SceneElement {
public:
	int _type;
	float _shininess;
	
	GLfloat _color[4];
	GLfloat _specular[4];
	GLfloat _matrix[16];

	union SceneElementData _d;

	SceneElement(int type = GLRENDER_NONE);
	~SceneElement();
	void setPoints(float* points, int n, int primitive_type);
	void setColors(float* colors);
	void setColor(float r, float g, float b, float alpha = 1);
	void setNormals(float* normals);
	void setPolygonMode(bool points, bool lines, bool solid);
	void setSpecular(float r, float g, float b, float alpha = 1);
	void setShininess(float shininess);
	void setXY(int x, int y);
	void setAlpha(float alpha);
	void setText(const char* str);
	void setImage(const char* filename);
	
	void bufferSetup();
};

void glcompositor_init(void);

extern std::list<SceneElement*> _elementList;

#endif
