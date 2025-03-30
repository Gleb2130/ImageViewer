#pragma once
#include <QtWidgets>
class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool ActivatedFlag = false;
	QPoint beginPoint = QPoint(0, 0);

	int mode = 0;
	int mode2 = 0;

public:
	ViewerWidget() {};
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
	void setBegin(QPoint begin) { beginPoint = begin; }
	QPoint getBegin() { return beginPoint; }
	void setActivatedFlag(bool state) { ActivatedFlag = state; }
	bool getActivatedFlag() { return ActivatedFlag; }

	//Get/Set functions
	void setPainter() { painter = new QPainter(img); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }

	void clear(int delenie);

	//void drawHelloWorld();
	void drawGraf(int graf, int gType, int N,QColor color);
	void drawAxis(int delenie);
	void drawCircle(const QPoint begin, const QPoint end, const QColor color);


	void drawLine(QPoint begin, QPoint end, QColor color,int algType);
	void setPixel(int x, int y, const QColor& color);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA);
	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a);
	QColor getPixel(int x, int y);

	void setMode(int m) { mode = m; }
	int getMode() { return mode; }

	void setMode2(int m) { mode2 = m; }
	int getMode2() { return mode2; }

	QSize getImgSize() { return img->size(); }


public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};