#pragma once
#include <QtWidgets>
#include <vector>
#include "ViewerWidget.h"
#include "PolygonWidget.h"
using namespace std;

class LineVectorWidget : public QWidget, public PolygonWidget
{
private:
	vector<QPointF> tangents;
	int vecWidth = 150;
	int typeL = 0;

	QPointF rotateVector(const QPointF& vector, double angleDegrees) {
		double angleRadians = qDegreesToRadians(angleDegrees);
		double cosA = cos(angleRadians);
		double sinA = sin(angleRadians);
		return QPointF(
			vector.x() * cosA - vector.y() * sinA,
			vector.x() * sinA + vector.y() * cosA
		);
	}
public:
	LineVectorWidget() {};
	LineVectorWidget(ViewerWidget* vW) :PolygonWidget(vW) {}
	LineVectorWidget(ViewerWidget* vW, vector<QPoint> points) : PolygonWidget(vW, points) {}
	LineVectorWidget(ViewerWidget* vW, vector<QPoint> points, vector<QPoint> temp_points) : PolygonWidget(vW, points, temp_points) {}

	int getVecWidth() { return vecWidth; }
	void setVecWidth(int width) { vecWidth = width; }

	void drawGraf(int type) override;

	int getTypeL() { return typeL; }
	void setTypeL(int type) { typeL = type; }

	QString toString() const override {
		QString result = "LineVectorWidget\n";
		for (const auto& point : points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}

		result += "\n";
		for (const auto& point : temp_points) {
			result += QString::number(point.x()) + " " + QString::number(point.y()) + " ";
		}
		result += "\n";
		result += QString::number(editable) + " " + QString::number(drawing) + " " + QString::number(center.x()) + " " + QString::number(center.y()) + " "
			+ QString::number(1) + " " + QString::number(rotationAngle) + " " + QString::number(1) + " " + QString::number(fill) + " "
			+ QString::number(interType) + " " + QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " "
			+ QString::number(color.alpha()) + " " + QString::number(shearX) + " " + QString::number(vecWidth) + " " + QString::number(typeL);
		return result;
	}


	void generateTangents();


    void rotateTangentAtIndex(int index, double angleDegrees) {
        if (index < 0 || index >= tangents.size()) {
            qWarning("Invalid index in rotateTangentAtIndex");
            return;
        }
        tangents[index] = rotateVector(tangents[index], angleDegrees);
    }

    const vector<QPointF>& getTangents() const {
        return tangents;
    }

	int getTangentCount() const {
		return tangents.size();
	}
};

