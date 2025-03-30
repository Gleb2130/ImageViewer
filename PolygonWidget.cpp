#include "PolygonWidget.h"

void PolygonWidget::addPoint(QPoint point)
{
	points.push_back(point);
	temp_points.push_back(point);
}

void PolygonWidget::drawGraf(int type) {
	if (points.empty()) {
		return;
	}

	if (points.size() == 1) {
		vW->drawCircle(points[0], QPoint(points[0].x() + 2, points[0].y()),color);
		return;
	}
	QRect clipWindow(0, 0, vW->getImgSize().width(), vW->getImgSize().height());

	if (points.size() == 2) {
		QPoint clippedP1, clippedP2;

		if (clipLineCyrusBeck(points[0], points[1], clipWindow, clippedP1, clippedP2)) {
			vW->drawLine(clippedP1, clippedP2, color, type);
		}
		return;
	}
	
	vector<QPoint> clippedPolygon = clipPolygon(points, clipWindow);
	if (clippedPolygon.empty()) {
		return;
	}
	for (size_t i = 0; i < clippedPolygon.size() - 1; i++) {
		vW->drawLine(clippedPolygon[i], clippedPolygon[i + 1], color, type);
	}
	if (!clippedPolygon.empty()) {
		vW->drawLine(clippedPolygon.back(), clippedPolygon[0], color, type);
	}

	if (getFill()) {
		if (isTriangle(clippedPolygon) && (interType == 0 || interType==1))
			fillTriangle(clippedPolygon);
		else
			fillObj();
	}

	
}

bool PolygonWidget::clipLineCyrusBeck(const QPoint& p1, const QPoint& p2,const QRect& clipRect,QPoint& outP1, QPoint& outP2)
{
	// Внешние нормали к границам окна
	const QPoint normals[4] = {
		QPoint(-1, 0),  // левая
		QPoint(1, 0),   // правая
		QPoint(0, -1),  // верхняя
		QPoint(0, 1)    // нижняя
	};

	// Точки на соответствующих границах
	const QPoint edges[4] = {
		QPoint(clipRect.left(), clipRect.top()),     // левая
		QPoint(clipRect.right(), clipRect.top()),    // правая
		QPoint(clipRect.left(), clipRect.top()),     // верхняя
		QPoint(clipRect.left(), clipRect.bottom())  // нижняя
	};

	double tE = 0.0;  // Максимальное t для входа
	double tL = 1.0;  // Минимальное t для выхода
	QPoint D = p2 - p1;

	for (int i = 0; i < 4; i++) {
		QPoint P = edges[i];
		QPoint N = normals[i];

		QPoint w = p1 - P;
		double DdotN = D.x() * N.x() + D.y() * N.y();
		double WdotN = w.x() * N.x() + w.y() * N.y();

		if (DdotN == 0.0) { // Линия параллельна границе
			if (WdotN < 0) return false; // Снаружи окна
			continue;
		}

		double t = -WdotN / DdotN;

		// Корректное определение входа/выхода
		if (DdotN < 0.0) { // Вход в окно
			tE = std::max(tE, t);
		}
		else { // Выход из окна
			tL = std::min(tL, t);
		}

		if (tE > tL) return false; // Нет видимой части
	}

	// Вычисляем обрезанные точки
	outP1 = (tE > 0.0) ?
		QPoint(
			p1.x() + static_cast<int>(std::round(D.x() * tE)),
			p1.y() + static_cast<int>(std::round(D.y() * tE))
		) : p1;

	outP2 = (tL < 1.0) ?
		QPoint(
			p1.x() + static_cast<int>(std::round(D.x() * tL)),
			p1.y() + static_cast<int>(std::round(D.y() * tL))
		) : p2;

	return true;
}

void PolygonWidget::moveObj(QPoint toPoint)
{
	if (points.empty()) {
		return;
	}
	for (QPoint& point : points) {
		point += toPoint;
	}
	for (QPoint& point : temp_points) {
		point += toPoint;
	}

}

void PolygonWidget::rotateObj(QPoint rotationCenter,double degrees)
{     
	if (points.empty()) {
		return;
	}

	double radians = qDegreesToRadians(degrees - rotationAngle); 
	rotationAngle = degrees; 
	

	for (QPoint& point : points) {
		int x = point.x() - rotationCenter.x();
		int y = point.y() - rotationCenter.y();

		int newX = rotationCenter.x() + x * cos(radians) - y * sin(radians);
		int newY = rotationCenter.y() + x * sin(radians) + y * cos(radians);

		point.setX(newX);
		point.setY(newY);
	}
	temp_points = points;

}

void PolygonWidget::scaleObj(double scale)
{
	if (points.empty()) {
		return;
	}

	points = temp_points;
	this->scale += scale;

	QPoint scalingCenter = points[0];

	for (size_t i = 1; i < points.size(); ++i) { 
		int x = points[i].x() - scalingCenter.x();
		int y = points[i].y() - scalingCenter.y();

		int newX = scalingCenter.x() + static_cast<int>(x * this->scale);
		int newY = scalingCenter.y() + static_cast<int>(y * this->scale);

		points[i].setX(newX);
		points[i].setY(newY);
	}
}

void PolygonWidget::reflectObj(const QString& axis) {
	if (points.empty()) return;

	if (axis == "horizontal") {
		int yLine = points[0].y(); 
		for (QPoint& point : points) {
			point.setY(2 * yLine - point.y());
		}
	}
	else if (axis == "vertical") {
		int xLine = points[0].x(); 
		for (QPoint& point : points) {
			point.setX(2 * xLine - point.x());
		}
	}
	temp_points = points;
}



void PolygonWidget::shearObj() {
	if (points.empty()) return;
	points = temp_points;
	int baseY = points[0].y();

	for (QPoint& point : points) {
		int deltaX = static_cast<int>((point.y() - baseY) * shearX);
		point.setX(point.x() + deltaX);
	}
}




vector<QPoint> PolygonWidget::clipPolygon(const vector<QPoint>& polygon, const QRect& clipWindow) {
	vector<QPoint> output = polygon;

	auto intersect = [](const QPoint& p1, const QPoint& p2, const QRect& clip, int edge) -> QPoint {
		int xMin = clip.left(), yMin = clip.top();
		int xMax = clip.right(), yMax = clip.bottom();

		double x1 = p1.x(), y1 = p1.y();
		double x2 = p2.x(), y2 = p2.y();
		double t;

		switch (edge) {
		case 0:
			t = (xMin - x1) / (x2 - x1);
			return QPoint(xMin, y1 + t * (y2 - y1));
		case 1: 
			t = (yMin - y1) / (y2 - y1);
			return QPoint(x1 + t * (x2 - x1), yMin);
		case 2: 
			t = (xMax - x1) / (x2 - x1);
			return QPoint(xMax, y1 + t * (y2 - y1));
		case 3: 
			t = (yMax - y1) / (y2 - y1);
			return QPoint(x1 + t * (x2 - x1), yMax);
		default:
			return QPoint(0, 0);
		}
		};

	auto inside = [](const QPoint& p, const QRect& clip, int edge) -> bool {
		int xMin = clip.left(), yMin = clip.top();
		int xMax = clip.right(), yMax = clip.bottom();

		switch (edge) {
		case 0: return p.x() >= xMin; 
		case 1: return p.y() >= yMin; 
		case 2: return p.x() <= xMax; 
		case 3: return p.y() <= yMax; 
		default: return false;
		}
		};

	for (int edge = 0; edge < 4; ++edge) {
		vector<QPoint> input = output;
		output.clear();

		for (size_t i = 0; i < input.size(); ++i) {
			QPoint current = input[i];
			QPoint previous = input[(i + input.size() - 1) % input.size()];

			bool currentInside = inside(current, clipWindow, edge);
			bool previousInside = inside(previous, clipWindow, edge);

			if (currentInside) {
				if (!previousInside) {
					output.push_back(intersect(previous, current, clipWindow, edge));
				}
				output.push_back(current);
			}
			else if (previousInside) {
				output.push_back(intersect(previous, current, clipWindow, edge));
			}
		}
	}

	return output;
}

bool PolygonWidget::inPoint(int x,int y)
{
	for (QPoint point : points) {
		if (point.x() - 1 >= x && point.x() + 1 <= x && point.y() - 1 >= y && point.y() + 1 <= y)
			return true;
	}
	return false;
}


void PolygonWidget::fillObj() {
	if (points.empty()) return;

	QPoint minPoint = getMinPoint();
	QPoint maxPoint = getMaxPoint();

	for (int y = minPoint.y(); y <= maxPoint.y(); ++y) {
		vector<int> intersections;

		for (size_t i = 0; i < points.size(); ++i) {
			QPoint p1 = points[i];
			QPoint p2 = points[(i + 1) % points.size()];

			if ((p1.y() <= y && p2.y() > y) || (p2.y() <= y && p1.y() > y)) {
				int x = p1.x() + (y - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y());
				intersections.push_back(x);
			}
		}

		sort(intersections.begin(), intersections.end());

		for (size_t i = 0; i < intersections.size(); i += 2) {
			if (i + 1 < intersections.size()) {
				int xStart = intersections[i];
				int xEnd = intersections[i + 1];

				for (int x = xStart; x <= xEnd; ++x) {
					vW->setPixel(x, y, color);
				}
			}
		}
	}
}


QPoint PolygonWidget::getMinPoint()
{
	QPoint min(1000,1000);

	for (QPoint point : points) {
		if (point.x() < min.x()) {
			min.rx() = point.x();
		}
		if (point.y() < min.y()) {
			min.ry() = point.y();
		}
	}

	return min;
}

QPoint PolygonWidget::getMaxPoint()
{
	QPoint max(0, 0);

	for (QPoint point : points) {
		if (point.x() > max.x()) {
			max.rx() = point.x();
		}
		if (point.y() > max.y()) {
			max.ry() = point.y();
		}
	}

	return max;
}

QColor PolygonWidget::nearestNeighbor(const QPoint& p, vector<QPoint> clippedPolygon) {
	double d1 = sqrt(pow(p.x() - clippedPolygon[0].x(), 2) + pow(p.y() - clippedPolygon[0].y(), 2));
	double d2 = sqrt(pow(p.x() - clippedPolygon[1].x(), 2) + pow(p.y() - clippedPolygon[1].y(), 2));
	double d3 = sqrt(pow(p.x() - clippedPolygon[2].x(), 2) + pow(p.y() - clippedPolygon[2].y(), 2));

	if (d1 <= d2 && d1 <= d3) return QColor(255, 0, 0);
	if (d2 <= d1 && d2 <= d3) return QColor(0, 255, 0);
	return QColor(0, 0, 255);
}




QColor PolygonWidget::barycentricInterpolation(const QPoint& p, vector<QPoint> clippedPolygon) {
	auto area = [](const QPoint& a, const QPoint& b, const QPoint& c) {
		return abs((a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y())) / 2.0);
		};

	double totalArea = area(clippedPolygon[0], clippedPolygon[1], clippedPolygon[2]);
	double w1 = area(p, clippedPolygon[1], clippedPolygon[2]) / totalArea;
	double w2 = area(p, clippedPolygon[2], clippedPolygon[0]) / totalArea;
	double w3 = area(p, clippedPolygon[0], clippedPolygon[1]) / totalArea;

	int r = w1 * 255 + w2 * 0 + w3 * 0;
	int g = w1 * 0 + w2 * 255 + w3 * 0;
	int b = w1 * 0 + w2 * 0 + w3 * 255;

	return QColor(r, g, b);
}

void PolygonWidget::fillTriangle(vector<QPoint> clippedPolygon) {
	if (!isTriangle(clippedPolygon)) return;

	QPoint minPoint = getMinPoint();
	QPoint maxPoint = getMaxPoint();

	for (int y = minPoint.y(); y <= maxPoint.y(); ++y) {
		vector<int> intersections;

		for (size_t i = 0; i < clippedPolygon.size(); ++i) {
			QPoint p1 = clippedPolygon[i];
			QPoint p2 = clippedPolygon[(i + 1) % clippedPolygon.size()];

			if ((p1.y() <= y && p2.y() > y) || (p2.y() <= y && p1.y() > y)) {
				double t = (y - p1.y()) / double(p2.y() - p1.y());
				int x = p1.x() + t * (p2.x() - p1.x());
				intersections.push_back(x);
			}
		}

		sort(intersections.begin(), intersections.end());

		for (size_t i = 0; i < intersections.size(); i += 2) {
			if (i + 1 < intersections.size()) {
				int xStart = intersections[i];
				int xEnd = intersections[i + 1];

				for (int x = xStart; x <= xEnd; ++x) {
					QPoint p(x, y);
					QColor fillColor =interType ? barycentricInterpolation(p, clippedPolygon) : nearestNeighbor(p, clippedPolygon);
					vW->setPixel(x, y, fillColor);
				}
			}
		}
	}
}