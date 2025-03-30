#pragma once
#include <QtWidgets>
#include <vector>
#include "ViewerWidget.h"
using namespace std;
class PolygonWidget : public QWidget
{
protected:
	ViewerWidget* vW;
	vector<QPoint> points;
	vector<QPoint> temp_points;
	bool editable = false;
	bool drawing = false;
	QPoint center;
	bool move = false;
	double rotationAngle = 0.0;
	double scale = 1;
	bool fill = false;
	int interType = 0;
	QColor color;
	double shearX =0.;

public:
	PolygonWidget() {};
	PolygonWidget(ViewerWidget* vW) :vW(vW) {}
	PolygonWidget(ViewerWidget* vW, vector<QPoint> points) : points(points), vW(vW) {}
	PolygonWidget(ViewerWidget* vW, vector<QPoint> points, vector<QPoint> temp_points) : points(points), temp_points(temp_points), vW(vW) {}


	void addPoint(QPoint point);
	virtual void drawGraf(int type);

	void moveObj(QPoint toPoint);
	void rotateObj(QPoint rotationCenter, double degrees);
	void scaleObj(double scale);
	void reflectObj(const QString& axis);
	void shearObj();
	vector<QPoint> clipPolygon(const vector<QPoint>& polygon, const QRect& clipWindow);
	bool clipLineCyrusBeck(const QPoint& p1, const QPoint& p2, const QRect& clipRect, QPoint& outP1, QPoint& outP2);

	void setEdit(bool edit) {
		editable = edit;
	}
	bool getEdit() { return editable; }

	void setDraw(bool draw) { drawing = draw; };
	bool getDraw() { return drawing; }

	void setCenter(QPoint point) { center = point; };
	QPoint getCenter() { return center; };

	void setMove(bool move) { this->move = move; };
	bool getMove() { return move; };
	bool inPoint(int x, int y);

	virtual void fillObj();
	double getAngle() { return rotationAngle; };
	void setAngle(double angle) { rotationAngle = angle; };

	QPoint getFirst() {
		if (!points.empty())
		{
			return points[0];
		}
		else
		{
			return QPoint(0, 0);
		}
	};

	double getScale() { return scale; };
	void setScale(double scale) { this->scale = scale; };
	void clear() { points.clear(); temp_points.clear(); rotationAngle = 0.0; scale = 1; };

	QPoint getMinPoint();
	QPoint getMaxPoint();
	void setFill(bool flag) { fill = flag; };
	bool getFill() { return fill; };

	bool isTriangle(const vector<QPoint>& polygon) const {
		return polygon.size() == 3;
	}
	void fillTriangle(vector<QPoint> clippedPolygon);
	QColor nearestNeighbor(const QPoint& p, vector<QPoint> clippedPolygon);
	QColor barycentricInterpolation(const QPoint& p, vector<QPoint> clippedPolygon);

	void setFillMode(int mode) { interType = mode; };
	int getFillMode() { return interType; };

	void setColor(QColor color) {
		this->color = color;
	};

	void setShear(double val) { shearX = val; };
	double getShear() { return shearX; };

	int getPointSize() { return points.size(); };

	virtual QString toString() const {
		QString result = "PolygonWidget\n";
		for (const auto& point : points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}
		result += "\n";
		for (const auto& point : temp_points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}
		result += "\n";

		result += QString::number(editable) + " " + QString::number(drawing) + " " + QString::number(center.x()) + " " + QString::number(center.y()) + " "
			+ QString::number(move) + " " + QString::number(rotationAngle) + " " + QString::number(scale) + " " + QString::number(fill) + " "
			+ QString::number(interType) + " " + QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " "
			+ QString::number(color.alpha()) + " " + QString::number(shearX) ;
		return result;
	}



};


