#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(1000, 1000));

	
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	ui->spinBox_2->setRange(20,50);
	ui->spinBox_2->setValue(20);
	
	ui->polygonScale->setRange(-10, 10);
	ui->polygonScale->setDecimals(2);
	ui->polygonScale->setSingleStep(0.25);
	ui->polygonScale->setValue(1);

	ui->polygonXCords->setRange(0, 1000);
	
	ui->polygonYCords->setRange(0, 1000);
	
	ui->polygonAngle->setRange(-360, 360);

	ui->vectorWidth->setRange(1,300);
	ui->vectorWidth->setValue(150);

	ui->shearKoef->setRange(-10, 10);
	ui->shearKoef->setSingleStep(0.2);

	ui->screanWidth->setRange(100, 2000);
	ui->screanHeight->setRange(100, 2000);
	ui->screanWidth->setValue(1000);
	ui->screanHeight->setValue(1000);

	ui->spinBox->setSingleStep(2);

	
	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageViewer::onSpinBoxValueChanged);

	connect(ui->polygonYCords, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageViewer::polygonYCordsValueChanged);
	connect(ui->polygonXCords, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageViewer::polygonXCordsValueChanged);
	connect(ui->shearKoef, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ImageViewer::changeShear);


	connect(ui->polygonScale, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ImageViewer::polygonScaleValueChanged);
	connect(ui->polygonAngle, QOverload<int>::of(&QSlider::valueChanged), this, &ImageViewer::polygonAngleValueChanged);

	connect(ui->vectorWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageViewer::vectorWidthChanged);
	connect(ui->lineTypeVec, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImageViewer::lineTypeVecIndexChanged);



	onSpinBoxValueChanged();
	on_paintButton_clicked();
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setFocusPolicy(Qt::NoFocus);
}

void ImageViewer::onSpinBoxValueChanged()
{
	drawAllPoly();
}

void ImageViewer::loadObjects()
{
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(0);
	ui->tableWidget->setColumnCount(1);
	QStringList headers = { "Objects" };
	ui->tableWidget->setHorizontalHeaderLabels(headers);

	for (int i = 0; i < pW.size();i++) {
		int row = ui->tableWidget->rowCount();
		ui->tableWidget->insertRow(row);

		ui->tableWidget->setItem(row, 0, new QTableWidgetItem("Object" + QString::number(i)));
	}
}

void ImageViewer::on_tableWidget_cellClicked(int row, int column)
{
	objIndex = row;

	if (dynamic_cast<LineVectorWidget*>(pW.at(objIndex))) {
		vW->setMode2(1);
	}
	else if (dynamic_cast<PolygonWidget*>(pW.at(objIndex))) {
		vW->setMode2(0);
	}
	else if (dynamic_cast<CircleWidget*>(pW.at(objIndex))) {
		vW->setMode2(3);
	}
	else {
		vW->setMode2(2);
	}

	ui->checkBox->setChecked(pW.at(objIndex)->getFill());

	ui->polygonXCords->blockSignals(true);
	ui->polygonYCords->blockSignals(true);
	ui->polygonXCords->setValue(pW.at(objIndex)->getFirst().x());
	ui->polygonYCords->setValue(pW.at(objIndex)->getFirst().y());
	ui->polygonXCords->blockSignals(false);
	ui->polygonYCords->blockSignals(false);

	ui->polygonScale->blockSignals(true);
	ui->polygonScale->setValue(pW.at(objIndex)->getScale());
	ui->polygonScale->blockSignals(false);

	ui->shearKoef->blockSignals(true);
	ui->shearKoef->setValue(pW.at(objIndex)->getShear());
	ui->shearKoef->blockSignals(false);
	
	ui->polygonAngle->setValue(pW.at(objIndex)->getAngle());
	ui->checkBox->setChecked(pW.at(objIndex)->getFill());
	ui->fillMode->setCurrentIndex(pW.at(objIndex)->getFillMode());


	if (vW->getMode2() == 1) {
		ui->shearKoef->setEnabled(false);
		ui->checkBox->setEnabled(false);
		ui->fillMode->setEnabled(false);
		ui->polygonScale->setEnabled(false);
		ui->vectorWidth->setEnabled(true);
		ui->lineTypeVec->setEnabled(true);

		ui->vectorWidth->setValue(dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getVecWidth());
		ui->lineTypeVec->setCurrentIndex(dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getTypeL());

		ui->vectorIndex->setRange(0, dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getTangentCount() - 1);
		ui->vectorIndex->setValue(0);

	}
	else if (vW->getMode2() == 0) {
		ui->shearKoef->setEnabled(true);
		ui->checkBox->setEnabled(true);
		ui->fillMode->setEnabled(true);
		ui->polygonScale->setEnabled(true);
		ui->vectorWidth->setEnabled(false);
		ui->lineTypeVec->setEnabled(false);
	}
	else if (vW->getMode2() == 2) {
		ui->shearKoef->setEnabled(false);
		ui->checkBox->setEnabled(false);
		ui->fillMode->setEnabled(false);
		ui->polygonScale->setEnabled(true);
		ui->vectorWidth->setEnabled(false);
		ui->lineTypeVec->setEnabled(false);
	}
	else if (vW->getMode2() == 3) {
		ui->shearKoef->setEnabled(false);
		ui->checkBox->setEnabled(true);
		ui->fillMode->setEnabled(false);
		ui->polygonScale->setEnabled(true);
		ui->vectorWidth->setEnabled(false);
		ui->lineTypeVec->setEnabled(false);
	}
}

void ImageViewer::on_deleteObjButton_clicked()
{
	if (pW.empty()) return;

	pW.erase(pW.begin() + objIndex);
	objIndex = 0;
	drawAllPoly();
	loadObjects();
}

void ImageViewer::on_saveToFile_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "Image Files (*.png)");
	if (fileName.isEmpty()) return;

	if (!vW->getImage()->save(fileName)) {
		QMessageBox::warning(this, "Save Error", "Failed to save the image.");
		return;
	}

	QString dataFileName = fileName;
	dataFileName.replace(".png", ".csv");
	QFile dataFile(dataFileName);
	if (dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&dataFile);
		for (const PolygonWidget* obj : pW) {
			out << obj ->toString() << "\n";
		}
	}
}

void ImageViewer::on_loadFromFile_clicked() {
	on_pushButtonVymazat_clicked();

	QString fileName = QFileDialog::getOpenFileName(this, "Load Data", "", "Data Files (*.csv)");
	if (fileName.isEmpty()) return;

	QFile dataFile(fileName);
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, "Load Error", "Failed to open the file.");
		return;
	}

	QTextStream in(&dataFile);
	QString line;


	while (!in.atEnd()) {
		line = in.readLine().trimmed();
		if (line.isEmpty()) continue;

		QString type = line;
		vector<QPoint> points;
		vector<QPoint> temp_points;


		line = in.readLine().trimmed();
		QStringList pointData = line.split(" ");
		for (int i = 0; i < pointData.size(); i += 2) {
			int x = pointData[i].toInt();
			int y = pointData[i + 1].toInt();
			points.emplace_back(x, y); 
		}
		line = in.readLine().trimmed();
		QStringList temp_pointData = line.split(" ");
		for (int i = 0; i < temp_pointData.size(); i += 2) {
			int x = temp_pointData[i].toInt();
			int y = temp_pointData[i + 1].toInt();
			temp_points.emplace_back(x, y); 
		}

		line = in.readLine().trimmed();
		QStringList params = line.split(" ");
		bool editable = params[0].toInt();
		bool drawing = params[1].toInt();
		QPoint center(params[2].toInt(), params[3].toInt());
		bool move = params[4].toInt();
		double rotationAngle = params[5].toDouble();
		double scale = params[6].toDouble();
		bool fill = params[7].toInt();
		int interType = params[8].toInt();
		QColor color(params[9].toInt(), params[10].toInt(), params[11].toInt(), params[12].toInt());
		double shearX = params[13].toDouble();

		if (type == "PolygonWidget") {
			auto* obj = new PolygonWidget(vW, points, temp_points);
			obj->setEdit(editable);
			obj->setDraw(drawing);
			obj->setCenter(center);
			obj->setMove(move);
			obj->setAngle(rotationAngle);
			obj->setScale(scale);
			obj->setFill(fill);
			obj->setFillMode(interType);
			obj->setColor(color);
			obj->setShear(shearX);
			pW.push_back(obj);
		}
		else if (type == "CircleWidget") {
			int radius = params[14].toInt();
			auto* obj = new CircleWidget(vW, points, temp_points);
			obj->setEdit(editable);
			obj->setDraw(drawing);
			obj->setCenter(center);
			obj->setMove(move);
			obj->setAngle(rotationAngle);
			obj->setScale(scale);
			obj->setFill(fill);
			obj->setFillMode(interType);
			obj->setColor(color);
			obj->setShear(shearX);
			obj->setRadius(radius);
			pW.push_back(obj);
		}
		else if (type == "LineVectorWidget") {
			int vecWidth = params[14].toInt();
			int typeL = params[15].toInt();
			auto* obj = new LineVectorWidget(vW, points, temp_points);
			obj->setEdit(editable);
			obj->setDraw(drawing);
			obj->setCenter(center);
			obj->setMove(move);
			obj->setAngle(rotationAngle);
			obj->setScale(scale);
			obj->setFill(fill);
			obj->setFillMode(interType);
			obj->setColor(color);
			obj->setShear(shearX);
			obj->setVecWidth(vecWidth);
			obj->setTypeL(typeL);
			pW.push_back(obj);
			obj->generateTangents();
		}
		else {
			QMessageBox::warning(this, "Load Error", "Unknown object type: " + type);
		}
	}

	drawAllPoly();
	loadObjects();
}

void ImageViewer::on_screanWidth_valueChanged(int value)
{
	if (!pW.empty()) 
		on_saveToFile_clicked();

	on_pushButtonVymazat_clicked();
	vW->changeSize(value, ui->screanHeight->value());
}

void ImageViewer::on_screanHeight_valueChanged(int value)
{
	if (!pW.empty())
		on_saveToFile_clicked();
	on_pushButtonVymazat_clicked();
	vW->changeSize(ui->screanWidth->value(), value);
}

void ImageViewer::on_rotateAngel_clicked()
{
	if (pW.empty()) return;

	if (vW->getMode2() == 1) {
		dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->rotateTangentAtIndex(ui->vectorIndex->value(), ui->vectorRotateAngel->value());
		drawAllPoly();
	}
	
}

void ImageViewer::on_saveCubeToFile_clicked()
{
	if (pW.empty()) return;

	//if (vW->getMode2() == 4 ) {
	//	dynamic_cast<
	//}
}






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
		case 3: {

			if (pW.empty() || new_object) {
				if (vW->getMode2() == 0 || vW->getMode2() == 2) {
					auto obj = new PolygonWidget(vW);
					obj->setDraw(true);
					pW.push_back(obj);
				}
				else if (vW->getMode2() == 1) {
					auto lineWidget = new LineVectorWidget(vW);
					lineWidget->setTypeL(ui->lineTypeVec->currentIndex());
					lineWidget->setVecWidth(ui->vectorWidth->value());
					lineWidget->setDraw(true);
					pW.push_back(lineWidget);

				}
				else if (vW->getMode2() == 3) {
					auto circleWidget = new CircleWidget(vW);
					circleWidget->setDraw(true);
					pW.push_back(circleWidget);
				}
				objIndex = pW.size() - 1;
				
				new_object = false;
			}

			if (pW.at(objIndex)->getDraw()) {
				w->drawLine(QPoint(e->pos().x() - 1, e->pos().y()), QPoint(e->pos().x() + 1, e->pos().y()), globalColor, 0);
				w->drawLine(QPoint(e->pos().x(), e->pos().y() - 1), QPoint(e->pos().x(), e->pos().y() + 1), globalColor, 0);
				pW.at(objIndex)->addPoint(e->pos());
				if ((vW->getMode2() ==3 || vW->getMode2()==2)  && pW.at(objIndex)->getPointSize()==2) {
					pW.at(objIndex)->setDraw(false);
					pW.at(objIndex)->setColor(globalColor);
					drawAllPoly();
					pW.at(objIndex)->setEdit(true);
					ui->polygonScale->setValue(1);
					ui->polygonAngle->setValue(0);
					ui->shearKoef->setValue(0);

					ui->shearKoef->setEnabled(false);
					ui->fillMode->setEnabled(false);
					if (vW->getMode2() == 2) {
						ui->checkBox->setEnabled(false);
					}
					else {
						ui->checkBox->setEnabled(true);
					}
					ui->polygonScale->setEnabled(true);

					ui->vectorWidth->setEnabled(true);
					ui->lineTypeVec->setEnabled(true);
					loadObjects();

					ui->polygonXCords->blockSignals(true);
					ui->polygonYCords->blockSignals(true);
					ui->polygonXCords->setValue(pW.at(objIndex)->getFirst().x());
					ui->polygonYCords->setValue(pW.at(objIndex)->getFirst().y());
					ui->polygonXCords->blockSignals(false);
					ui->polygonYCords->blockSignals(false);

				}
			}
			else if (pW.at(objIndex)->getEdit()) {
				pW.at(objIndex)->setCenter(e->pos());
				pW.at(objIndex)->setMove(true);
				pW.at(objIndex)->setDraw(false);
			}
				
			
			break;

		}
		}
	}
	else if (e->button() == Qt::RightButton) {
		switch (w->getMode()) {
		case 3:
			if (pW.empty() ) break;
			if (pW.at(objIndex)->getDraw() && (vW->getMode2() == 0 || vW->getMode2() == 1)) {
				if (vW->getMode2() == 1) {
					int typeL = dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getTypeL();
					if (((typeL == 0 || typeL == 1) && pW.at(objIndex)->getPointSize() < 2) || (typeL == 2 && pW.at(objIndex)->getPointSize() < 4))
						break;

					dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->generateTangents();
					ui->vectorIndex->setRange(0, dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getTangentCount() - 1);
					ui->vectorIndex->setValue(0);
				}

				pW.at(objIndex)->setDraw(false);
				pW.at(objIndex)->setColor(globalColor);
				drawAllPoly();
				pW.at(objIndex)->setEdit(true);

				ui->checkBox->setChecked(false);

				ui->polygonXCords->blockSignals(true);
				ui->polygonYCords->blockSignals(true);
				ui->polygonXCords->setValue(pW.at(objIndex)->getFirst().x());
				ui->polygonYCords->setValue(pW.at(objIndex)->getFirst().y());
				ui->polygonXCords->blockSignals(false);
				ui->polygonYCords->blockSignals(false);

				ui->polygonScale->setValue(1);
				ui->polygonAngle->setValue(0);
				ui->shearKoef->setValue(0);
				if (vW->getMode2() == 1) {
					ui->shearKoef->setEnabled(false);
					ui->checkBox->setEnabled(false);
					ui->fillMode->setEnabled(false);
					ui->polygonScale->setEnabled(false);
					ui->vectorWidth->setValue(dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getVecWidth());
					ui->lineTypeVec->setCurrentIndex(dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->getTypeL());

				}
				else if (vW->getMode2() == 0) {
					ui->shearKoef->setEnabled(true);
					ui->checkBox->setEnabled(true);
					ui->fillMode->setEnabled(true);
					ui->polygonScale->setEnabled(true);
				}

				loadObjects();

			}
			else {
				/*objIndex = pW.size();
				switch (vW->getMode2()) {
				case 0: {
					pW.push_back(new PolygonWidget(vW));
					break;
				}
				case 1: {
					pW.push_back(new LineVectorWidget(vW));
					dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->setTypeL(ui->lineTypeVec->currentIndex());
					break;
				}
				}
				pW.at(objIndex)->setDraw(true);*/
				if (vW->getMode2() == 3 && pW.at(objIndex)->getPointSize()<2) {
					break;
				}
				new_object = true;
			}
			break;

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
		case 3:
			if (pW.empty()) break;

			if (pW.at(objIndex)->getEdit()) 
				pW.at(objIndex)->setMove(false);
			break;

		}
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->buttons() == Qt::LeftButton) {
		switch (w->getMode()){
			case 0: {
				if (w->getActivatedFlag()) {
					w->freeDraw(e->pos(), QPen(globalColor));
					w->setBegin(e->pos());
				}
			break;
			}
			case 3: {
				if (pW.empty()) break;

				if (pW.at(objIndex)->getEdit() && pW.at(objIndex)->getMove()) {
					
					pW.at(objIndex)->moveObj(e->pos() - pW.at(objIndex)->getCenter());
					drawAllPoly();
					pW.at(objIndex)->setCenter(e->pos());

					ui->polygonXCords->blockSignals(true);
					ui->polygonYCords->blockSignals(true);
					ui->polygonXCords->setValue(pW.at(objIndex)->getFirst().x());
					ui->polygonYCords->setValue(pW.at(objIndex)->getFirst().y());
					ui->polygonXCords->blockSignals(false);
					ui->polygonYCords->blockSignals(false);
				}
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

	switch (w->getMode()) {
	case 3: {
		if (pW.empty() || vW->getMode2() == 1) break;
		if (pW.at(objIndex)->getEdit()) {
			double delta = ((wheelEvent->angleDelta().y()) > 0 ? 0.25 : -0.25);
			if (pW.at(objIndex)->getScale() + delta <= 10 && pW.at(objIndex)->getScale() + delta >= -10) {
				pW.at(objIndex)->scaleObj(delta);
				
				drawAllPoly();
				ui->polygonScale->blockSignals(true);
				ui->polygonScale->setValue(pW.at(objIndex)->getScale());
				ui->polygonScale->blockSignals(false);
			}
		}

		break;
	}
	}
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
	pW.clear();	
	objIndex = 0;
	loadObjects();
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
	ui->polygonButton->setEnabled(true);
	ui->polyLineButton->setEnabled(true);
	ui->cubePushButton->setEnabled(true);

}

void ImageViewer::on_lineButton_clicked()
{
	vW->setMode(3);
	vW->setMode2(2);
	ui->paintButton->setEnabled(true);
	ui->circleButton->setEnabled(true);
	ui->lineButton->setEnabled(false);
	ui->polyLineButton->setEnabled(true);
	ui->polygonButton->setEnabled(true);
	ui->cubePushButton->setEnabled(true);

}

void ImageViewer::on_polygonButton_clicked()
{
	vW->setMode(3);
	vW->setMode2(0);
	ui->paintButton->setEnabled(true);
	ui->circleButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
	ui->polyLineButton->setEnabled(true);
	ui->polygonButton->setEnabled(false);
	ui->cubePushButton->setEnabled(true);

}

void ImageViewer::on_polyLineButton_clicked()
{
	vW->setMode(3);
	vW->setMode2(1);
	ui->paintButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
	ui->circleButton->setEnabled(true);
	ui->polygonButton->setEnabled(true);
	ui->polyLineButton->setEnabled(false);
	ui->cubePushButton->setEnabled(true);

}

void ImageViewer::on_cubePushButton_clicked()
{
	vW->setMode(3);
	vW->setMode2(4);
	ui->paintButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
	ui->circleButton->setEnabled(true);
	ui->polygonButton->setEnabled(true);
	ui->polyLineButton->setEnabled(true);
	ui->cubePushButton->setEnabled(false);
}

void ImageViewer::on_circleButton_clicked()
{
	vW->setMode(3);
	vW->setMode2(3);
	ui->paintButton->setEnabled(true);
	ui->lineButton->setEnabled(true);
	ui->circleButton->setEnabled(false);
	ui->polygonButton->setEnabled(true);
	ui->polyLineButton->setEnabled(true);
	ui->cubePushButton->setEnabled(true);
}

void ImageViewer::on_pushButtonSetColor_clicked()
{

	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;
		if (pW.empty()) return;
		pW.at(objIndex)->setColor(globalColor);
		if (pW.at(objIndex)->getEdit()) {
			
			drawAllPoly();
		}
	}

}

void ImageViewer::on_checkBox_stateChanged(int state)
{
	if (pW.empty()) return;
	pW.at(objIndex)->setFill(state == Qt::Checked);
	
	drawAllPoly();
}

void ImageViewer::on_fillMode_currentIndexChanged(int index)
{
	if (pW.empty()) return;
	pW.at(objIndex)->setFillMode(index);
	
	drawAllPoly();
}

void ImageViewer::on_reflectPushButton_clicked()
{
	if (pW.empty()) return;
	pW.at(objIndex)->reflectObj(ui->reflectType->currentText());
	drawAllPoly();
}

void ImageViewer::changeShear(double shear)
{
	if (pW.empty()) return;

	if (pW.at(objIndex)->getEdit()) {
		pW.at(objIndex)->setShear(shear);
		pW.at(objIndex)->shearObj();
		
		drawAllPoly();
	}
}

void ImageViewer::polygonYCordsValueChanged(int y)
{
	if (pW.empty()) return;

	if (pW.at(objIndex)->getEdit()) {
		
		pW.at(objIndex)->moveObj(QPoint(pW.at(objIndex)->getFirst().x(), y) - pW.at(objIndex)->getFirst());
		drawAllPoly();
	}
}

void ImageViewer::polygonXCordsValueChanged(int x)
{
	if (pW.empty()) return;

	if (pW.at(objIndex)->getEdit()) {

		
		pW.at(objIndex)->moveObj(QPoint(x, pW.at(objIndex)->getFirst().y()) - pW.at(objIndex)->getFirst());
		drawAllPoly();
	}
}

void ImageViewer::polygonScaleValueChanged(double scale)
{
	if (pW.empty()) return;
	if (pW.at(objIndex)->getEdit()) {

		pW.at(objIndex)->scaleObj(scale - pW.at(objIndex)->getScale());
		
		drawAllPoly();

	}
}

void ImageViewer::drawAllPoly()
{
	on_actionClear_triggered();
	for (auto obj : pW)
		obj->drawGraf(ui->comboBoxLineAlg->currentIndex());
}

void ImageViewer::vectorWidthChanged(int value)
{
	if (pW.empty() || vW->getMode2() != 1) return;
	
	dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->setVecWidth(value);
	dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->generateTangents();

	drawAllPoly();
}

void ImageViewer::lineTypeVecIndexChanged(int index)
{
	if (pW.empty() || vW->getMode2() != 1) return;

	dynamic_cast<LineVectorWidget*>(pW.at(objIndex))->setTypeL(index);
	drawAllPoly();
}


void ImageViewer::polygonAngleValueChanged(int value)
{
	if (pW.empty()) return;
	if (pW.at(objIndex)->getEdit()) {

		pW.at(objIndex)->rotateObj(pW.at(objIndex)->getCenter(),value);
		drawAllPoly();
	}
}
