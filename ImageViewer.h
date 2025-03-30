#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"
#include "PolygonWidget.h"
#include "LineVectorWidget.h"
#include "CircleWidget.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;
	//PolygonWidget* pW;
	vector <PolygonWidget*> pW;
	int objIndex = 0;
	bool new_object = false;

	QSettings settings;
	QMessageBox msgBox;
	QColor globalColor = Qt::black;
	

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionExit_triggered();

	//void on_pushButtonHello_clicked();
	void on_circleButton_clicked();
	void on_pushButtonKreslit_clicked();
	void on_pushButtonVymazat_clicked();

	void on_paintButton_clicked();
	void on_lineButton_clicked();
	void on_polygonButton_clicked();
	void on_polyLineButton_clicked();
	void on_cubePushButton_clicked();

	void on_pushButtonSetColor_clicked();
	void on_checkBox_stateChanged(int state);

	void on_fillMode_currentIndexChanged(int index);
	void on_reflectPushButton_clicked();
	void changeShear(double shear);

	void polygonAngleValueChanged(int value);
	void polygonYCordsValueChanged(int y);
	void polygonXCordsValueChanged(int x);
	void polygonScaleValueChanged(double scale);
	void drawAllPoly();

	void vectorWidthChanged(int value);
	void lineTypeVecIndexChanged(int index);

	void onSpinBoxValueChanged();

	void loadObjects();

	void on_tableWidget_cellClicked(int row, int column);

	void on_deleteObjButton_clicked();

	void on_saveToFile_clicked();
	void on_loadFromFile_clicked();

	void on_screanWidth_valueChanged(int value);
	void on_screanHeight_valueChanged(int value);

	void on_rotateAngel_clicked();

	void on_saveCubeToFile_clicked();

};


