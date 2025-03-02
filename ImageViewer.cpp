#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	ui->spinBox_2->setRange(20,50);
	ui->spinBox_2->setValue(20);
	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);


	connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageViewer::onSpinBoxValueChanged);

	onSpinBoxValueChanged();
	on_paintButton_clicked();
}

void ImageViewer::onSpinBoxValueChanged()
{
	vW->clear(ui->spinBox->value());
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}
		if (event->type() == QEvent::MouseButtonPress) {
			ViewerWidgetMouseButtonPress(w, event);
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			ViewerWidgetMouseButtonRelease(w, event);
		}
		else if (event->type() == QEvent::MouseMove) {
			ViewerWidgetMouseMove(w, event);
		}
		else if (event->type() == QEvent::Leave) {
			ViewerWidgetLeave(w, event);
		}
		else if (event->type() == QEvent::Enter) {
			ViewerWidgetEnter(w, event);
		}
		else if (event->type() == QEvent::Wheel) {
			ViewerWidgetWheel(w, event);
		}

	return QObject::eventFilter(obj, event);
}

void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton) {
		switch (w->getMode()) {
		case 0:
			w->setBegin(e->pos());
			w->setActivatedFlag(true);
			break;
		case 1:
			w->setBegin(e->pos());
			break;
		case 2: {
			if (w->getActivatedFlag()) {
				w->drawCircle(w->getBegin(), e->pos(), globalColor);
				w->setActivatedFlag(false);
			}
			else {
				w->setBegin(e->pos());
				w->setActivatedFlag(true);
			}
			break;
			}
		}
	}
}

void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton) {
		switch (w->getMode()) {
		case 0:
			if (w->getActivatedFlag()) {
				w->freeDraw(e->pos(), QPen(globalColor));
				w->setActivatedFlag(false);
			}
			break;
		case 1:
			w->drawLine(w->getBegin(), e->pos(), globalColor, ui->comboBoxLineAlg->currentIndex());
			break;
		}
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->buttons() == Qt::LeftButton && w->getActivatedFlag()) {
		switch (w->getMode()){
			case 0: {
			w->freeDraw(e->pos(), QPen(globalColor));
			w->setBegin(e->pos());
			break;
			}
		}
	}
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	vW->clear(ui->spinBox->value());
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

//void ImageViewer::on_pushButtonHello_clicked()
//{
//	vW->drawHelloWorld();
//}


void ImageViewer::on_pushButtonVymazat_clicked()
{
	vW->clear(ui->spinBox->value());
}

void ImageViewer::on_pushButtonKreslit_clicked()
{
	int graf = ui->comboBox->currentIndex();
	int tGraf = 0;

	if (ui->radioButton->isChecked())
		tGraf = 0;
	else if (ui->radioButton_2->isChecked())
		tGraf = 1;
	else if (ui->radioButton_3->isChecked())
		tGraf = 2;
	int N = ui->spinBox_2->value();

	vW->drawGraf(graf, tGraf ,N,globalColor);
}

void ImageViewer::on_paintButton_clicked()
{
	vW->setMode(0);
	ui->paintButton->setEnabled(false);
	ui->circleButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
}

void ImageViewer::on_lineButton_clicked()
{
	vW->setMode(1);
	ui->paintButton->setEnabled(true);
	ui->circleButton->setEnabled(true);
	ui->lineButton->setEnabled(false);
}

void ImageViewer::on_circleButton_clicked()
{
	vW->setMode(2);
	ui->paintButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
	ui->circleButton->setEnabled(false);
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;
	}
}