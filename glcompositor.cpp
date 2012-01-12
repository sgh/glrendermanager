
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <png.h>
#include "glcompositor.h"
// #include <X11/X.h>
// #include <X11/Xlib.h>



#include <list>

std::list<SceneElement*> _elementList;

Display                 *dpy;
Window                  root;
GLint                   att[] = {
	GLX_RGBA,
	GLX_DOUBLEBUFFER,  True, /* Request a double-buffered color buffer with */
	GLX_RED_SIZE,      8,     /* the manimum number of bits per component    */
	GLX_GREEN_SIZE,    8, 
	GLX_BLUE_SIZE,     8,
	GLX_DEPTH_SIZE,    24,
	GLX_STENCIL_SIZE,  8,
	None };

XVisualInfo             *vi;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

		
static GLfloat rotX = -90;
static GLfloat rotY = 0;
static GLfloat rotZ;
static GLfloat cameraDistance = -5;
GLfloat light_position[] = { 1.0,  1.0,  1.0,  1.0 };

void MyMultMatrix(GLfloat a[16], const GLfloat b[16]) {
	GLfloat _a[16];
	memcpy(_a, a, sizeof(GLfloat[16]));

	/* First columns */
	a[0] = _a[0]*b[0] + _a[4]*b[1] + _a[8]*b[2]  + _a[12]*b[3];
	a[1] = _a[1]*b[0] + _a[5]*b[1] + _a[9]*b[2]  + _a[13]*b[3];
	a[2] = _a[2]*b[0] + _a[6]*b[1] + _a[10]*b[2] + _a[14]*b[3];
	a[3] = _a[3]*b[0] + _a[7]*b[1] + _a[11]*b[2] + _a[15]*b[3];

	/* Second columnm */
	a[4] = _a[0]*b[4] + _a[4]*b[5] + _a[8]*b[6]  + _a[12]*b[7];
	a[5] = _a[1]*b[4] + _a[5]*b[5] + _a[9]*b[6]  + _a[13]*b[7];
	a[6] = _a[2]*b[4] + _a[6]*b[5] + _a[10]*b[6] + _a[14]*b[7];
	a[7] = _a[3]*b[4] + _a[7]*b[5] + _a[11]*b[6] + _a[15]*b[7];

	/* Third column */
	a[8]  = _a[0]*b[8] + _a[4]*b[9] + _a[8]*b[10]  + _a[12]*b[11];
	a[9]  = _a[1]*b[8] + _a[5]*b[9] + _a[9]*b[10]  + _a[13]*b[11];
	a[10] = _a[2]*b[8] + _a[6]*b[9] + _a[10]*b[10] + _a[14]*b[11];
	a[11] = _a[3]*b[8] + _a[7]*b[9] + _a[11]*b[10] + _a[15]*b[11];

	/* Fourth column */
	a[12] = _a[0]*b[12] + _a[4]*b[13] + _a[8]*b[14]  + _a[12]*b[15];
	a[13] = _a[1]*b[12] + _a[5]*b[13] + _a[9]*b[14]  + _a[13]*b[15];
	a[14] = _a[2]*b[12] + _a[6]*b[13] + _a[10]*b[14] + _a[14]*b[15];
	a[15] = _a[3]*b[12] + _a[7]*b[13] + _a[11]*b[14] + _a[15]*b[15];
}

void MyIdentityMatrix(GLfloat a[16]) {
	memset(a, 0, sizeof(GLfloat[16]));
	a[0] = a[5] = a[10] = a[15] = 1;
}

void MyTranslateMatrix(GLfloat a[16], float x, float y, float z) {
	a[iTransX] += x;
	a[iTransY] += y;
	a[iTransZ] += z;
}

void MyRotateMatrix(GLfloat a[16], float x, float y, float z) {
	GLfloat tmp[16];
	
	/* Rotate X */
	if (x != 0) {
		MyIdentityMatrix(tmp);
		tmp[5] = tmp[10] = cos(x);
		tmp[6] = sin(x);
		tmp[9] = -tmp[6];
		MyMultMatrix(a, tmp);
	}
	
	/* Rotate Y */
	if (y != 0) {
		MyIdentityMatrix(tmp);
		tmp[0] = tmp[10] = cos(y);
		tmp[2] = -sin(y);
		tmp[8] = -tmp[2];
		MyMultMatrix(a, tmp);
	}
	
	/* Rotate Z */
	if (z != 0) {
		MyIdentityMatrix(tmp);
		tmp[0] = tmp[5] = cos(z);
		tmp[1] = sin(z);
		tmp[4] = -tmp[1];
		MyMultMatrix(a, tmp);
	}
}

static GLuint FontBase = 0;

void DrawGeometry(SceneElement* a, Geometry* g) {
		if (!g->points)
			return;

		if (g->colors) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, g->colors);
			
		} else
			glColor4fv(a->_color);

		if (g->normals) {
			glEnableClientState(GL_NORMAL_ARRAY);	
			glNormalPointer(GL_FLOAT, 0,  g->normals);
		}


		glEnableClientState(GL_VERTEX_ARRAY);
		if (!g->vbo)
			glVertexPointer(3, GL_FLOAT, 0, g->points);
		else {
			glBindBuffer(GL_ARRAY_BUFFER, g->vbo);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		if (g->draw_solid) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(g->primitive_type, 0, g->npoints);
		}

		if (g->draw_lines) {
			glLineWidth(2);
			glColor4f(0.0 ,0.0 ,0.0, 0.5);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(g->primitive_type	, 0, g->npoints);
		}
		
		if (g->draw_points) {
			glPointSize(1);
			glColor3f(0.0 ,0.0 ,0.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDrawArrays(g->primitive_type, 0, g->npoints);
		}
	
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
}

void Draw2DText(SceneElement* a, Text* text) {
	FT_UInt glyph_index;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode ( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gwa.width, 0, gwa.height, -	1, 1);
	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glColor4fv(a->_color);
	
	int x = text->x;
	if (x < 0)
		x += gwa.width;
	
	int y = text->y;
	if (y < 0)
		y += gwa.height;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelZoom(1, -1);
	glPixelTransferf( GL_RED_BIAS,   a->_color[0] ); 
	glPixelTransferf( GL_GREEN_BIAS, a->_color[1] );
	glPixelTransferf( GL_BLUE_BIAS,  a->_color[2] ); 
	int idx = 0;
	int err;
	while (text->str[idx] != 0) {
		glyph_index = FT_Get_Char_Index(text->face, text->str[idx]);
		if (!glyph_index) {
			fprintf(stderr, "character %d not found in font.\n", text->str[idx]);
			break;
		}

		err = FT_Load_Glyph(text->face, glyph_index, FT_LOAD_RENDER);
		if (err) {
			fprintf(stderr, "error loading glyph.\n");
			break;
		}

		glRasterPos2i(x-4, y+text->face->glyph->bitmap_top-4);
		glRasterPos2i(x, y+text->face->glyph->bitmap_top);
		glDrawPixels(text->face->glyph->bitmap.width, text->face->glyph->bitmap.rows,GL_ALPHA, GL_UNSIGNED_BYTE, text->face->glyph->bitmap.buffer);
		x += text->face->glyph->advance.x >> 6;
		y += text->face->glyph->advance.y >> 6;
		idx++;
	}
	glPixelZoom(1,1);
	glPixelTransferf( GL_RED_BIAS,   0 ); 
	glPixelTransferf( GL_GREEN_BIAS, 0 );
	glPixelTransferf( GL_BLUE_BIAS,  0 );

	
	glMatrixMode ( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode ( GL_MODELVIEW );
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void DrawScreenBitmap(ScreenBitmap* bitmap) {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode ( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gwa.width, 0, gwa.height, -	1, 1);
	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	
	glPixelTransferf( GL_ALPHA_BIAS, bitmap->alpha); 
	int x = bitmap->x;
	if (x < 0)
		x += gwa.width;
	
	int y = bitmap->y;
	if (y < 0)
		y += gwa.height;
	glRasterPos2i(x, y);


	glDrawPixels(bitmap->width, bitmap->height, GL_BGRA, GL_UNSIGNED_BYTE	, bitmap->data);
	glPixelTransferf( GL_ALPHA_BIAS, 0);

	glMatrixMode ( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode ( GL_MODELVIEW );
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void DrawScene() {
	unsigned int i;
// 	printf("Draw\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glTranslatef(0.0, 0.0, cameraDistance);
  	glRotatef(rotX, 1.0, 0.0, 0.0);
	glRotatef(rotY, 0.0, 1.0, 0.0);
	glRotatef(rotZ, 0.0, 0.0, 1.0);
	
	std::list<SceneElement*>::iterator it = _elementList.begin();
	while (it != _elementList.end()) {
		SceneElement* a = *it;
		a->bufferSetup();
		
		glMaterialf(GL_FRONT_AND_BACK,  GL_SHININESS, a->_shininess );
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  a->_specular );

		glPushMatrix();
		glMultMatrixf(a->_matrix);
		switch( a->_type) {
			case GLRENDER_GEOMETRY: DrawGeometry(a, &a->_d.geometry);      break;
			case GLRENDER_2D_TEXT:  Draw2DText(a, &a->_d.text);            break;
			case GLRENDER_2D_IMAGE: DrawScreenBitmap(&a->_d.screenBitmap); break;
			default:
				printf("No draw\n");
		}
		glPopMatrix();

// 		if (a->use_bufferid) {
// 			glBindBuffer(GL_ARRAY_BUFFER, 0);
// 			assert( glGetError() == GL_NO_ERROR);
// 		}
		it++;
	}

	/* Draw coordinate system */
	// X Axis
	glDisable( GL_LIGHTING );
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f( 0, 0, 0);
	glVertex3f( 20, 0, 0);
	
	// Y Axis
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0,  20, 0);
	
	// Z
	glColor3f(0, 0, 1);
	glVertex3f(0, 0,  0);
	glVertex3f(0, 0,  20);
	glEnd();
	glEnable( GL_LIGHTING );
// 	glBegin(GL_POINTS);
// 	glColor3f(1.0, 1.0, 0.0);
// 	glPointSize(10);
// 	glVertex3f(0,0,0);
// 	glEnd();

}


void initGL()
{
    	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_COLOR_MATERIAL );
	
	glEnable( GL_NORMALIZE );
	glShadeModel(GL_SMOOTH);
	glEnable(GL_ALPHA_TEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);                   // background color

	// Lightning
	glEnable(GL_LIGHTING);
	
	GLfloat light_ambient[]  = { 0.0,  0.0,  0.0,  1.0 };
	GLfloat light_diffuse[]  = { 1.0,  1.0,  1.0,  1.0 };
	GLfloat light_specular[] = { 1.0,  1.0,  1.0,  1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT0);
}


GLfloat circle_points[13*3];

void readSTL(const char* filename, struct STLData* data) {
	unsigned int i;
	FILE* f = fopen(filename, "r");
	fseek(f, 80, SEEK_SET);
	
	fread(&data->numvertices, 4, 1, f);
	data->numvertices *= 3;
// 	printf("Num vertices : %d\n", stl_numvertices);
	
	data->points  = (GLfloat*)malloc( 3*sizeof(GLfloat) * data->numvertices);
	data->normals = (GLfloat*)malloc( 3*sizeof(GLfloat) * data->numvertices);
		
	GLfloat current_normal[3];
	for (i=0; i<data->numvertices; i++) {
		if ((i+3) % 3 == 0) {
			fread(&current_normal[iX], 4, 1, f);
			fread(&current_normal[iY], 4, 1, f);
			fread(&current_normal[iZ], 4, 1, f);
// 			printf(" X:%.1f  Y:%.1f Z:%.1f\n", stl_normals[i + iX], stl_normals[i + iY], stl_normals[i + iZ]); 
		}
		
		data->normals[i*3 + iX] = current_normal[iX];
		data->normals[i*3 + iY] = current_normal[iY];
		data->normals[i*3 + iZ] = current_normal[iZ];
		
		fread(&data->points[i*3 + iX], 4, 1, f);
		fread(&data->points[i*3 + iY], 4, 1, f);
		fread(&data->points[i*3 + iZ], 4, 1, f);
		
		if (i>0 && ((i+1) % 3) == 0)
			fseek(f, 2, SEEK_CUR); // Skip  Attribute byte count
	}
	fclose(f);
}


void initGLX() {
	dpy = XOpenDisplay(NULL);

	if(dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(0); }
		
	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy,  DefaultScreen(dpy), att);

	if(vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0); } 
	else {
		printf("\n\tvisual %p selected: depth %d\n", (void *)vi->visualid, vi->depth); }/* %p creates hexadecimal output like in glxinfo */

	swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask| StructureNotifyMask;
	swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);

	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen) , 0, 0, 800, 480, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);

	glc = glXCreateContext(dpy, vi, NULL, True);
	XMapWindow(dpy, win);
	glXMakeCurrent(dpy, win, glc);
		
	XStoreName(dpy, win, "VERY SIMPLE APPLICATION");

}

static int glx_initiated = 0;

void* glx_event_thread(void *arg) {
		char left_pressed = 0;
	char right_pressed = 0;
	int last_x;
	int last_y;
	
	initGLX();
	initGL();

	glx_initiated = 1;
	
	while(1) {
		while (1) {
			struct timespec starttime;
			struct timespec endtime;
			if (XPending(dpy)) {
				XNextEvent(dpy, &xev);
				break;
			}

			clock_gettime(CLOCK_MONOTONIC_RAW, &starttime);
			DrawScene();
			clock_gettime(CLOCK_MONOTONIC_RAW, &endtime);
			glXSwapBuffers(dpy, win);

			unsigned int drawtime = (endtime.tv_sec*1000+endtime.tv_nsec/1000000) - (starttime.tv_sec*1000+starttime.tv_nsec/1000000);
			printf("Drawtime: %d ms\n", drawtime);

			if (drawtime < 30000)
				usleep(30000 - drawtime);
			else
				usleep(1000);
		}
// 		printf("event\n");
			
		if(xev.type == Expose) {
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60.0f, (float)(gwa.width)/gwa.height, 1.0f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip
		}
		else if(xev.type == KeyPress) {
// 			printf("%d \n", xev.xkey.keycode);

			if (xev.xkey.keycode == 24) {
				glXMakeCurrent(dpy, None, NULL);
				glXDestroyContext(dpy, glc);
				XDestroyWindow(dpy, win);
				XCloseDisplay(dpy);
				exit(0);
			}
		} else if(xev.type == ButtonPress) {
			last_x = xev.xbutton.x;
			last_y = xev.xbutton.y;
			switch (xev.xbutton.button) {
				case 1:
					left_pressed = 1;
					break;
				case 3:
					right_pressed = 1;
					break;
			}
		} else if(xev.type == ButtonRelease) {
			switch (xev.xbutton.button) {
				case 1:
					left_pressed = 0;
					break;
				case 3:
					right_pressed = 0;
					break;
			}
		} else if(xev.type == MotionNotify) {
			if (left_pressed) {
				rotZ -= (float)(last_x - xev.xmotion.x)/2;
				rotX -= (float)(last_y - xev.xmotion.y)/2;
			}
			if (right_pressed) {
				cameraDistance += (float)(last_y - xev.xmotion.y);
			}
			if (left_pressed || right_pressed) {
				last_x = xev.xmotion.x;
				last_y = xev.xmotion.y;
			}
		}
	}
}

SceneElement::SceneElement(int type) {
	_type = type;
	_shininess = 0;
	memset(_color, 0, sizeof(_color));
	memset(_specular, 0, sizeof(_specular));
	MyIdentityMatrix(_matrix);
	memset(&_d, 0, sizeof(_d));
	
	if (type == GLRENDER_2D_TEXT) {
		FT_Error err;

		err = FT_Init_FreeType(&_d.text.ftlib);
		if (err) {
			fprintf(stderr, "error initialising libfreetype.\n");
			return;
		}

		int point_size = 50;
		int res_dpi = 96;
		const char* font_filename = "DejaVuSans.ttf";
		err = FT_New_Face(_d.text.ftlib, font_filename, 0, &_d.text.face);
		if (err == FT_Err_Unknown_File_Format) {
			fprintf(stderr, "%s: font file '%s' can't be read (might be"
				" unsupported).\n", font_filename);
			return;
		}
		else if (err) {
			fprintf(stderr, "error reading font file '%s'.\n", font_filename);
			return;
		}

		err = FT_Set_Char_Size(_d.text.face, 0, point_size * 64, res_dpi, res_dpi);
		if (err) {
			fprintf(stderr, "error setting font size.\n");
			return;
		}
		
	}
	
	if (type == GLRENDER_2D_IMAGE) {
		_d.screenBitmap.alpha = 1;
	}
}

SceneElement::~SceneElement() {
		if (_type == GLRENDER_2D_TEXT) {
			int err = FT_Done_FreeType(_d.text.ftlib);
			if (err) {
				fprintf(stderr, "error cleaning up after libfreetype.\n");
				return;
			}
		}
}

void SceneElement::setPoints(float* points, int n, int primitive_type) {
	_d.geometry.points = points;
	_d.geometry.npoints = n;
	_d.geometry.primitive_type = primitive_type;
}

void SceneElement::setColors(float* colors) {
	_d.geometry.colors = colors;
}

void SceneElement::setNormals(float* normals) {
	_d.geometry.normals = normals;
}

void SceneElement::setPolygonMode(bool points, bool lines, bool solid) {
	_d.geometry.draw_points = points;
	_d.geometry.draw_lines  = lines;
	_d.geometry.draw_solid  = solid;
}

void SceneElement::setColor(float r, float g, float b, float alpha) {
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
	_color[3] = alpha;
}

void SceneElement::setSpecular(float r, float g, float b, float alpha) {
	_specular[0] = r;
	_specular[1] = g;
	_specular[2] = b;
	_specular[3] = alpha;
}

void SceneElement::setShininess(float shininess) {
	_shininess = shininess;
}

void SceneElement::setAlpha(float alpha) {
	if (_type == GLRENDER_2D_IMAGE) {
		_d.screenBitmap.alpha = alpha;
	}
}

void SceneElement::setXY(int x, int y) {
	if (_type == GLRENDER_2D_IMAGE) {
		_d.screenBitmap.x = x;
		_d.screenBitmap.y = y;
	}
	
	if (_type == GLRENDER_2D_TEXT) {
		_d.text.x = x;
		_d.text.y = y;
	}
}

void SceneElement::setText(const char* str) {
	_d.text.str = str;
}

void SceneElement::setImage(const char* filename) {
	if (_type == GLRENDER_2D_IMAGE) {
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		FILE *fp;

		if ((fp = fopen("test.png", "rb")) == NULL)
			return;
		/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
		* you can supply NULL for the last three parameters.  We also supply the
		* the compiler header file version, so that we know if the application
		* was compiled with a compatible version of the library.  REQUIRED
		*/
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL/*(void *)user_error_ptr*/, NULL/*user_error_fn*/, NULL/*user_warning_fn*/);

		if (png_ptr == NULL) {
			fclose(fp);
			return;
		}

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			return;
		}

		/* Set error handling if you are using the setjmp/longjmp method (this is
		* the normal method of doing things with libpng).  REQUIRED unless you
		* set up your own error handlers in the png_create_read_struct() earlier.
		*/
		if (setjmp(png_ptr->jmpbuf))
		{
			/* Free all of the memory associated with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			fclose(fp);
			/* If we get here, we had a problem reading the file */
			return;
		}

		/* Set up the input control if you are using standard C streams */
		png_init_io(png_ptr, fp);

		/* The call to png_read_info() gives us all of the information from the
		* PNG file before the first IDAT (image data chunk).  REQUIRED
		*/
		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);

		if (color_type == PNG_COLOR_TYPE_GRAY &&
			bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png_ptr);

		png_read_update_info(png_ptr, info_ptr);

		/* allocate the memory to hold the image using the fields of info_ptr. */

		/* the easiest way to read the image */
		png_bytep row_pointers[height];

		int row;
		_d.screenBitmap.data = (char*)malloc(height * png_get_rowbytes(png_ptr, info_ptr));
		for (row = 0; row < height; row++)
			row_pointers[row] = (png_bytep)&_d.screenBitmap.data[(height - row - 1) * png_get_rowbytes(png_ptr, info_ptr)];

		/* Now it's time to read the image.  One of these methods is REQUIRED */
		png_read_image(png_ptr, row_pointers);

		/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(png_ptr, info_ptr);

		_d.screenBitmap.width  = width;
		_d.screenBitmap.height = height;

		/* clean up after the read, and free any memory allocated - REQUIRED */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		/* close the file */
		fclose(fp);
	}
}

void SceneElement::bufferSetup() {
	if (_type != GLRENDER_GEOMETRY)
		return;
	
	GLuint id = _d.geometry.vbo;

	// Points
	if (!_d.geometry.vbo && _d.geometry.points) {
		printf("Allocating vertex buffer\n");
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, 12*_d.geometry.npoints * sizeof(*_d.geometry.points), _d.geometry.points, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_d.geometry.vbo = id;
	}
}


void glcompositor_init(void) {
	pthread_attr_t attr;
	pthread_t event_thread;
	
	pthread_attr_init(&attr);
	pthread_create(&event_thread, &attr, glx_event_thread, NULL);
	
	while (!glx_initiated) {
		usleep(1000);
	}
}