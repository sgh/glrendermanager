
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

#include "glcompositor.h"

#include <QtGui>


#include "simplegl.h"

float box_colors[] = { 1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		1, 1, 0,
		};

float box_points[] = { -0.75, -0.75, 0,
		0.75, -0.75, 0,
		0.75,  0.75, 0,
		-0.75, 0.75, 0,
		};


MyWidget::MyWidget() {
	ui.setupUi(this);
	STLData stldata;
	SceneElement* se;

	
	// Terrain
	readSTL("teststl.stl", &stldata);
	se = new SceneElement(GLRENDER_GEOMETRY);
	se->setPoints(stldata.points, stldata.numvertices, GL_TRIANGLES);
	se->setNormals(stldata.normals);
	se->setColor(0, 1, 0);
	se->setPolygonMode(false, false, true);
	se->_matrix[iTransScale] = 1;
	MyRotateMatrix(se->_matrix,  DEG2RAD(90), 0, 0);
	_elementList.push_back(se);
	
	se = new SceneElement(GLRENDER_GEOMETRY);
	se->setPoints(box_points, 4, GL_QUADS);
	se->setColors(box_colors);
	se->setPolygonMode(false, false, true);
	se->_matrix[iTransZ] =  1;
	_elementList.push_back(se);

	se = new SceneElement(GLRENDER_GEOMETRY);
	se->setPoints(box_points, 4, GL_QUADS);
	se->setColors(box_colors);
	se->setPolygonMode(false, false, true);
	se->_matrix[iTransX] =  0.0;
	se->_matrix[iTransY] =  0.0;
	se->_matrix[iTransZ] = -1.5;
	_elementList.push_back(se);

	se = new SceneElement(GLRENDER_GEOMETRY);
	se->setPoints(box_points, 4, GL_QUADS);
	se->setColors(box_colors);
	se->setPolygonMode(false, false, true);
	se->_matrix[iTransX] =  0.0;
	se->_matrix[iTransY] = -0.75;
	se->_matrix[iTransZ] = -0.75;
	_elementList.push_back(se);

	for (int i=0; i<4; i++) {
		se = new SceneElement(GLRENDER_GEOMETRY);

		se->setPolygonMode(false, false, true);
		se->_shininess = 25;
		se->_matrix[iTransScale] = 1000;

		switch (i) {
			case 3: // Frame
				readSTL("no_name-blade_frame-b.stl", &stldata);
				se->setColor(0.5, 0.5, 0.5);
			break;
			case 1: // Tracks
				readSTL("no_name-track-b.stl", &stldata);
				se->setColor(0.5, 0.5, 0.5);
			break;
			case 2: // body
				readSTL("no_name-machine-b.stl", &stldata);
				se->setColor(1, 0.8, 0);
			break;
			case 0: // Blade
				readSTL("no_name-blade-b.stl", &stldata);
				se->setColor(0.3, 0.3, 0.3);
				
				se->_matrix[iTransX] += 2.6*1000;
				se->_matrix[iTransZ] += -0.60*1000;
				se->setSpecular(1, 1, 1);
				se->setShininess(25);
			break;
		}
		se->setPoints(stldata.points, stldata.numvertices, GL_TRIANGLES);
		se->setNormals(stldata.normals);

		SceneElement* se2;
		for (int j=0; j<360; j+=10) {
			se2 = new SceneElement();
			*se2 = *se;
			se2->_matrix[iTransY] += 30 * sin(DEG2RAD(j)) * 1000;
			se2->_matrix[iTransX] += 30 * cos(DEG2RAD(j)) * 1000;
			_elementList.push_back(se2);
		}
	}
	
	
	se = new SceneElement(GLRENDER_2D_TEXT);
	se->setColor(1, 1, 1);
	se->setText("Test Hest");
	se->setXY(100, 200);
	_elementList.push_back(se);

	se = new SceneElement(GLRENDER_2D_IMAGE);
	se->setImage("test.png");
	
	for (int i=0; i<10; i++) {
		for (int j=0; j<4; j++) {
			SceneElement* tmp = new SceneElement();
			*tmp = *se;
			tmp->setXY(i*64,j*100);
			tmp->setAlpha(-0.5);
			_elementList.push_back(tmp);
		}
	}
	
	
	connect( ui.combo, SIGNAL(activated(int)), this, SLOT(activate(int)));
	connect( ui.red, SIGNAL(valueChanged(int)), this, SLOT(update()));
	connect( ui.green, SIGNAL(valueChanged(int)), this, SLOT(update()));
	connect( ui.blue, SIGNAL(valueChanged(int)), this, SLOT(update()));
	connect( ui.alpha, SIGNAL(valueChanged(int)), this, SLOT(update()));
	
	connect( ui.wireframe, SIGNAL(stateChanged(int)), this, SLOT(update()));
	connect( ui.points, SIGNAL(stateChanged(int)), this, SLOT(update()));
	connect( ui.fill, SIGNAL(stateChanged(int)), this, SLOT(update()));
	connect( ui.shininess, SIGNAL(valueChanged(int)), this, SLOT(update()));
	connect( ui.specular, SIGNAL(valueChanged(int)), this, SLOT(update()));
}

void MyWidget::activate(int index) {
// 	setFace(&actors[ui.combo->currentText().toInt()]);
}

void MyWidget::blockUiSignals(bool b) {
	ui.red->blockSignals(b);
	ui.green->blockSignals(b);
	ui.blue->blockSignals(b);
	ui.alpha->blockSignals(b);	
	ui.wireframe->blockSignals(b);
	ui.points->blockSignals(b);
	ui.fill->blockSignals(b);
	ui.shininess->blockSignals(b);
	ui.specular->blockSignals(b);

}

void MyWidget::setFace(SceneElement* a) {
	_face = a;
	
	blockUiSignals(true);
	
// 	ui.red->setValue(_face->color[0] * 1000);
// 	ui.green->setValue(_face->color[1] * 1000);
// 	ui.blue->setValue(_face->color[2] * 1000);
// 	ui.alpha->setValue(_face->color[3] * 1000);
// 	
// 	ui.fill->setCheckState(_face->_geometry.draw_solid ? Qt::Checked : Qt::Unchecked );
// 	ui.wireframe->setCheckState(_face->_geometry.draw_lines ? Qt::Checked : Qt::Unchecked );
// 	ui.points->setCheckState(_face->_geometry.draw_points ? Qt::Checked : Qt::Unchecked );
// 	
// 	ui.shininess->setValue(_face->shininess*10);
// 	ui.specular->setValue(_face->specular[0] * 1000);
	
	blockUiSignals(false);
}

void MyWidget::update() {
// 	_face->color[0] = (float)ui.red->value()/1000;
// 	_face->color[1] = (float)ui.green->value()/1000;
// 	_face->color[2] = (float)ui.blue->value()/1000;
// 	_face->color[3] = (float)ui.alpha->value()/1000;
// 	
// 	_face->_geometry.draw_solid  = ui.fill->checkState();
// 	_face->_geometry.draw_lines  = ui.wireframe->checkState();
// 	_face->_geometry.draw_points = ui.points->checkState();
// 	
// 	_face->shininess = (float)ui.shininess->value()/10;
// 	_face->specular[0] = _face->specular[1] = _face->specular[2] = (float)ui.specular->value()/1000;
	
}



int main(int argc, char *argv[]) {
	QApplication qapp(argc, argv);
	
	glcompositor_init();

	MyWidget w;
// 	w.setFace(&actors[4]);
	w.show();
	
	qapp.exec();
}
