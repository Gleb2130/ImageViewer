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
		update();
	}

	return true;
}

//Draw functions
void ViewerWidget::freeDraw(QPoint end, QPen pen)
{
	painter->setPen(pen);
	painter->drawLine(freeDrawBegin, end);

	update();
}

void ViewerWidget::clear()
{
	img->fill(Qt::white);
	update();
}
void ViewerWidget::drawGraf(int graf, int gType, int delenie, int N)
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
		painter->setPen(QPen(Qt::red, 5));
		for (const QPointF& point : points) {
			painter->drawPoint(point);
		}
	}
	else if (gType == 1) { // Čiarový
		painter->setPen(QPen(Qt::red, 2));
		for (int i = 1; i < points.size(); i++) {
			painter->drawLine(points[i - 1], points[i]);
		}
	}
	else if (gType == 2) { // Stĺpcový
		painter->setPen(QPen(Qt::red, 1));
		for (const QPointF& point : points) {
			painter->drawLine(point.x(), 250, point.x(), point.y());
		}
	}

	painter->restore();
	update();
}

void ViewerWidget::drawHelloWorld()
{
	//Save default QPainter setting
	painter->save();

	//H
	painter->setPen(QPen(Qt::blue));
	painter->drawLine(20, 20, 20, 120);
	painter->drawLine(20, 70, 50, 70);
	painter->drawLine(50, 20, 50, 120);
	//e
	painter->drawLine(80, 95, 110, 95);
	QRect rectangle(60, 70, 50, 50);
	painter->drawArc(rectangle, 0, 300 * 16);
	//l
	painter->drawLine(120, 20, 120, 120);
	//l
	painter->drawLine(130, 20, 130, 120);
	//o
	painter->drawEllipse(QPointF(170, 95), 25, 25);

	//W
	const QPointF points[5] = {
		QPointF(20,140),
		QPointF(30,240),
		QPointF(40,190),
		QPointF(50,240),
		QPointF(60,140)
	};
	painter->drawPolyline(points, 5);
	//o
	painter->setBrush(QBrush(Qt::blue));
	painter->drawEllipse(QPointF(85, 215), 25, 25);
	//r
	painter->setFont(QFont("Arial", 50));
	painter->drawText(QPoint(120, 235), "r");
	//l
	painter->fillRect(150, 140, 10, 100, QBrush(QColor(0, 255, 100)));
	//d
	painter->setFont(QFont("Arial", 50));
	painter->drawText(QPoint(170, 235), "d");

	//Restore default QPainter setting
	painter->restore();
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}