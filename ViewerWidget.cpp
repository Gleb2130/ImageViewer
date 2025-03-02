#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;

}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}

//Draw functions
void ViewerWidget::freeDraw(QPoint end, QPen pen)
{
	painter->setPen(pen);
	painter->drawLine(beginPoint, end);

	update();
}

void ViewerWidget::clear(int delenie = -1)
{
	img->fill(Qt::white);
	update();

	if (delenie != -1) {
		drawAxis(delenie);
	}
}

void ViewerWidget::drawGraf(int graf, int gType, int N, QColor color)
{
	painter->save();

	float xMin = -M_PI;
	float xMax = M_PI;
	float xStep = (xMax - xMin) / N;
	QVector<QPointF> points;

	for (int i = 0; i <= N; i++) {
		float x = xMin + i * xStep;
		float y = (graf == 0) ? sin(x) : cos(x);
		float xCanvas = 250 + x * 240 / M_PI;  
		float yCanvas = 250 - y * 200;        
		points.append(QPointF(xCanvas, yCanvas));
	}
	


	if (gType == 0) { // Bodový
		painter->setPen(QPen(color, 5));
		for (const QPointF& point : points) {
			painter->drawPoint(point);
		}
	}
	else if (gType == 1) { // Čiarový
		painter->setPen(QPen(color, 2));
		for (int i = 1; i < points.size(); i++) {
			painter->drawLine(points[i - 1], points[i]);
		}
	}
	else if (gType == 2) { // Stĺpcový
		painter->setPen(QPen(color, 1));
		for (const QPointF& point : points) {
			painter->drawLine(point.x(), 250, point.x(), point.y());
		}
	}

	painter->restore();
	update();
}

void ViewerWidget::drawAxis(int delenie)
{
	painter->save();	

	painter->setPen(QPen(Qt::black));
	painter->drawLine(10, 250, 490, 250);
	painter->drawLine(250, 10, 250, 490);

	float step = 480.0 / delenie;

	for (int i = 0; i <= delenie; i++) {
		painter->drawLine(i * step + 10, 245, i * step + 10, 255);
		painter->drawLine(245, i * step + 10, 255, i * step + 10);
	}

	painter->restore();
	update();
}


//void ViewerWidget::drawHelloWorld()
//{
//	//Save default QPainter setting
//	painter->save();
//
//	//H
//	painter->setPen(QPen(Qt::blue));
//	painter->drawLine(20, 20, 20, 120);
//	painter->drawLine(20, 70, 50, 70);
//	painter->drawLine(50, 20, 50, 120);
//	//e
//	painter->drawLine(80, 95, 110, 95);
//	QRect rectangle(60, 70, 50, 50);
//	painter->drawArc(rectangle, 0, 300 * 16);
//	//l
//	painter->drawLine(120, 20, 120, 120);
//	//l
//	painter->drawLine(130, 20, 130, 120);
//	//o
//	painter->drawEllipse(QPointF(170, 95), 25, 25);
//
//	//W
//	const QPointF points[5] = {
//		QPointF(20,140),
//		QPointF(30,240),
//		QPointF(40,190),
//		QPointF(50,240),
//		QPointF(60,140)
//	};
//	painter->drawPolyline(points, 5);
//	//o
//	painter->setBrush(QBrush(Qt::blue));
//	painter->drawEllipse(QPointF(85, 215), 25, 25);
//	//r
//	painter->setFont(QFont("Arial", 50));
//	painter->drawText(QPoint(120, 235), "r");
//	//l
//	painter->fillRect(150, 140, 10, 100, QBrush(QColor(0, 255, 100)));
//	//d
//	painter->setFont(QFont("Arial", 50));
//	painter->drawText(QPoint(170, 235), "d");
//
//	//Restore default QPainter setting
//	painter->restore();
//	update();
//}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

void ViewerWidget::drawLine(QPoint begin, QPoint end,QColor color,int algType)
{
	painter->save();
	if (algType == 0) {
		if (begin.x() == end.x()) {
			if (begin.y() > end.y()) {
				qSwap(begin, end);
			}
			for (int y = begin.y(); y <= end.y(); y++) {
				setPixel(begin.x(), y, color);
			}
		}
		else {
			double m = (double)(end.y() - begin.y()) / (double)(end.x() - begin.x());

			if (m > 1 || m < -1) {
				if (begin.y() > end.y()) {
					qSwap(begin, end);
				}

				for (double y = begin.y(); y <= end.y(); y++) {
					double x = begin.x() + (y - begin.y()) / m;
					setPixel(std::round(x), y, color);
				}
			}
			else if (m < 1 && m > -1) {
				if (begin.x() > end.x()) {
					qSwap(begin, end);
				}

				for (double x = begin.x(); x <= end.x(); x++) {
					double y = begin.y() + m * (x - begin.x());
					setPixel(x, std::round(y), color);
				}
			}
			else if (m == 1 || m == -1) {
				if (begin.x() > end.x()) {
					qSwap(begin, end);
				}
				for (int x = begin.x(); x <= end.x(); x++) {
					setPixel(x, begin.y() + (x - begin.x()), color);
				}
			}
		}
	}
	else if (algType == 1) {
		int x1 = begin.x(), y1 = begin.y();
		int x2 = end.x(), y2 = end.y();

		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);

		int sx = (x1 < x2) ? 1 : -1;
		int sy = (y1 < y2) ? 1 : -1;

		int err = dx - dy;

		while (true) {
			setPixel(x1, y1, color);

			if (x1 == x2 && y1 == y2) break;

			int e2 = 2 * err;

			if (e2 > -dy) {
				err -= dy;
				x1 += sx;
			}
			if (e2 < dx) {
				err += dx;
				y1 += sy;
			}
		}
	}
	painter->restore();
	update();
}


void ViewerWidget::setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a)
{
	r = r > 255 ? 255 : (r < 0 ? 0 : r);
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = b;
	data[startbyte + 1] = g;
	data[startbyte + 2] = r;
	data[startbyte + 3] = a;
}
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{
	valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = static_cast<uchar>(255 * valB);
	data[startbyte + 1] = static_cast<uchar>(255 * valG);
	data[startbyte + 2] = static_cast<uchar>(255 * valR);
	data[startbyte + 3] = static_cast<uchar>(255 * valA);
}
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (color.isValid() && x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
		size_t startbyte = y * img->bytesPerLine() + x * 4;

		data[startbyte] = color.blue();
		data[startbyte + 1] = color.green();
		data[startbyte + 2] = color.red();
		data[startbyte + 3] = color.alpha();
	}
}

void ViewerWidget::drawCircle(const QPoint begin, const QPoint end, const QColor color) {
	painter->save();
	int x1 = begin.x(), y1 = begin.y();
	int x2 = end.x(), y2 = end.y();
	int r = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	int x = r, y = 0;
	int P = 1 - r;
	while (x > y) {
		if (P <= 0) {
			P = P + 2 * y + 3;
		}
		else {
			P = P + 2 * (y - x) + 5;
			x--;
		}
		y++;
		setPixel(x1 + x, y1 + y, color);
		setPixel(x1 - x, y1 + y, color);
		setPixel(x1 + x, y1 - y, color);
		setPixel(x1 - x, y1 - y, color);
		setPixel(x1 + y, y1 + x, color);
		setPixel(x1 - y, y1 + x, color);
		setPixel(x1 + y, y1 - x, color);
		setPixel(x1 - y, y1 - x, color);
	}
	painter->restore();
	update();
}