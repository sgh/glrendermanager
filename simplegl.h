#ifndef SIMPLEGL_H
#define SIMPLEGL_H

#include "ui_mainwidget.h"


class MyWidget : public  QWidget {

	Q_OBJECT
	
	Ui_MyWidget ui;
	SceneElement* _face;
	void blockUiSignals(bool b);

public:
	MyWidget();
	void setFace(SceneElement* a);

public slots:
	void update();
	void activate(int index);
	
	
};

#include "moc_simplegl.h"

#endif