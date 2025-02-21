#pragma once
#include <QtWidgets>
class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;

	bool freeDrawActivated = false;
	QPoint freeDrawBegin = QPoint(0, 0);

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	//Draw functions
	void freeDraw(QPoint end, QPen pen);
	void setFreeDrawBegin(QPoint begin) { freeDrawBegin = begin; }
	QPoint getFreeDrawBegin() { return freeDrawBegin; }
	void setFreeDrawActivated(bool state) { freeDrawActivated = state; }
	bool getFreeDrawActivated() { return freeDrawActivated; }

	//Get/Set functions
	void setPainter() { painter = new QPainter(img); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear();

	void drawHelloWorld();
	void drawGraf(int graf, int gType, int delenie, int N);


public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};