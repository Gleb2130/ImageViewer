#pragma once
#include <QtWidgets>
#include <vector>
#include "ViewerWidget.h"
#include "PolygonWidget.h"
using namespace std;

class CircleWidget : public QWidget, public PolygonWidget
{
private:
	int radius = 150;

public:
	CircleWidget() {};
	CircleWidget(ViewerWidget* vW) :PolygonWidget(vW) {}
	CircleWidget(ViewerWidget* vW, vector<QPoint> points) : PolygonWidget(vW, points) {}
	CircleWidget(ViewerWidget* vW, vector<QPoint> points,vector<QPoint> temp_points) : PolygonWidget(vW, points, temp_points) {}

	int getRadius() { return radius; }
	void setRadius(int r) { radius = r; }
	void drawGraf(int type) override;
	void fillObj() override;

	QString toString() const override{
		QString result = "CircleWidget\n";
		for (const auto& point : points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}
		result += "\n";
		for (const auto& point : temp_points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}
		result += "\n";
		result += QString::number(editable) + " " + QString::number(drawing) + " " + QString::number(center.x()) + " " + QString::number(center.y()) + " "
			+ QString::number(1) + " " + QString::number(rotationAngle) + " " + QString::number(scale) + " " + QString::number(fill) + " "
			+ QString::number(interType) + " " + QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " "
			+ QString::number(color.alpha()) + " " + QString::number(shearX) + " " + QString::number(radius);
		return result;
	}

};

