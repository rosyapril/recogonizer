#include "Paint.h"
#include "ui_Paint.h"
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QDebug>

Paint::Paint(QWidget *parent) : QWidget(parent), ui(new Ui::Paint) {
	ui->setupUi(this);

	// clear everything;
	on_clearButton_clicked();

	SAMPLESIZE = 50;
	boundingBoxSize = 200;
}

Paint::~Paint() {
	delete ui;
}

void Paint::mousePressEvent(QMouseEvent *event) {

	if(painted){
		painted = false;
		on_clearButton_clicked();
	}

	// track the mouse location
	if (event->x() > 0 && event->y() > 0 && event->x() < 1024 && event->y() < 768 && !painted){
		painting = true;
		stroke.push_back(event->pos());

	}

	repaint();

	// push the pressed location to the stroke;

}

void Paint::mouseMoveEvent(QMouseEvent *event) {

	if (event->x() > 0 && event->y() > 0 && event->x() < 1024 && event->y() < 768 && painting){
		stroke.push_back(event->pos());
	}

	repaint();

}

void Paint::mouseReleaseEvent(QMouseEvent *event) {
	if (painting){
		painting = false;

		painted = true;

//		lines.push_back(stroke);
//		stroke.clear();
	}

	// do oneDollarCalculate after mouse release;
	if (stroke.size() < 10 ) on_clearButton_clicked();
	else oneDollarCalculate();

	repaint();
}

void Paint::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	// set pen for painting;
	painter.setPen(QPen(Qt::black, 3));

	// paint the lines first
//	for (size_t i = 0; i < lines.size() ; ++i){
//		for (size_t j = 0; j < lines[i].size() ; ++j){
//			painter.drawPoint(lines[i][j].x() , lines[i][j].y() );
//		}
//	}

	// then paint the stroke;
	for (size_t i = 0; i < stroke.size() ; ++i) painter.drawPoint(stroke[i]);

	QPainter painter2(this);
	painter2.setPen(QPen(Qt::red , 5));
	for (size_t i = 0 ; i < resampledStroke.size() ; ++i){
		painter2.drawPoint(resampledStroke[i]);
//		qDebug() << resampledStroke[i].x() << " <-x y-> " << resampledStroke[i].y();
	}
//	qDebug() << "Size of new stroke -> " <<resampledStroke.size();

}

void Paint::oneDollarCalculate() {
	// step 1 - RESIZE
	int xTotal = 0, yTotal = 0;

	// 1.1 calculate the total length of the stroke;
	double strokeLength = 0;
	QVector<double> stepLength;
	for(size_t i = 1; i < stroke.size() ; ++i){
		// sqrt( (x1-x0)^2 + (y1 - y0)^2);
		xTotal += stroke[i].x() ;
		yTotal += stroke[i].y();
		double currentLength = qSqrt( qPow(stroke[i].x() - stroke[i-1].x() , 2) + qPow(stroke[i].y() - stroke[i-1].y() , 2) );
		stepLength.push_back(currentLength);
		strokeLength += currentLength;
	}

	double unitLength = strokeLength / SAMPLESIZE;

	// 1.2 generate new QVector<QPointF> to store the new point;

	resampledStroke.push_back(QPointF(stroke[0]));

	double stock = 0;
	double residual = 0;
	QVector<double>::iterator it = stepLength.begin();
	for (size_t i = 1; i < SAMPLESIZE-1 ; ++i ){
		while( stock < unitLength ){
			stock += *it++;
		}

		residual = stock - unitLength;

		int pointIndex = it - stepLength.begin()+1;
		double xLength = stroke[pointIndex-1].x() - stroke[pointIndex].x();
		double yLength = stroke[pointIndex-1].y() - stroke[pointIndex].y();
		double percent = residual / (*it);

		resampledStroke.push_back(QPointF(stroke[pointIndex].x() +xLength * percent , stroke[pointIndex].y() + yLength * percent ));

		stock = residual;
	}

	resampledStroke.push_back(stroke[stroke.size()-1]);

	// step 2 - ROTATE
	// file the center
	double centerX = (double)xTotal / stroke.size();
	double centerY = (double)yTotal / stroke.size() * (-1);

	double rotateDegree = qAtan2( ( (-1)*stroke[0].y() - centerY  ) , (stroke[0].x() - centerX) );

	// move the center point to origin, and the rest point to same distance;

	double largestX = 0 , largestY = 0, smallestX = 1024 , smallestY = 768;

	for (size_t i = 0 ; i < SAMPLESIZE ; ++i){
		// move x = -centerX , move y = -centerY;
		double newX, newY , newXX, newYY ;
		newX = resampledStroke[i].x() - centerX ;
		newY = (-1)*resampledStroke[i].y() - centerY ;

		// rotate and move the point back
		newXX = newX * qCos(rotateDegree) + newY * qSin(rotateDegree) + centerX;
		newYY = (newX * (-qSin(rotateDegree)) + newY * cos(rotateDegree) + centerY) * (-1);
		resampledStroke[i].setX( newXX );
		resampledStroke[i].setY( newYY );
		if (largestX < newXX) largestX = newXX;
		if (largestY < newYY) largestY = newYY;
		if (smallestX > newXX) smallestX = newXX;
		if (smallestY > newYY) smallestY = newYY;
	}

	// step 3 - SCALE

	// 3.1 find the smallest and largest x, y; scale it to 100*100;
	// move every point by the samllest(x, y), and x scale is xlargest / 100, y scale is ylargest/100;
	double xRatio = (largestX - smallestX) / boundingBoxSize;
	double yRatio = (largestY - smallestY) / boundingBoxSize;
	for (size_t i = 0 ; i < SAMPLESIZE ; ++i) {
		// move
		resampledStroke[i].setX( (resampledStroke[i].x() - smallestX) / xRatio);
		resampledStroke[i].setY( (resampledStroke[i].y() - smallestY) / yRatio);
		//scale;
	}

}

void Paint::on_confirmButton_clicked() {
	// store this one as template
}

void Paint::on_clearButton_clicked() {

	stroke.clear();
	lines.clear();
	resampledStroke.clear();

	painting = false;
	painted = false;

	repaint();

}
