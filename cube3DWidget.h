#pragma once

#include <QtWidgets>
#include <vector>
#include "ViewerWidget.h"
#include "PolygonWidget.h"
using namespace std;

class cube3DWidget : public QWidget, public PolygonWidget
{
	QPoint origin; 
	int sideLength;
	vector<QPoint> vertices;

	void calculateCube();
public:
	cube3DWidget() {};
	cube3DWidget(ViewerWidget* vW) :PolygonWidget(vW) {}
	cube3DWidget(ViewerWidget* vW, vector<QPoint> points) : PolygonWidget(vW, points) {}
	cube3DWidget(ViewerWidget* vW, vector<QPoint> points, vector<QPoint> temp_points) : PolygonWidget(vW, points, temp_points) {}


	void saveCubeToVTK(const QString& filename);

	void drawGraf(int type) override;
};

