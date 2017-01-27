#include "Paint.h"
#include "ui_Paint.h"
#include <QPainter>
#include <QPen>

Paint::Paint(QWidget *parent) : QWidget(parent), ui(new Ui::Paint) {
	ui->setupUi(this);

	// clear everything;
	stroke.clear();
	lines.clear();

	painting = false;
	painted = false;
}

Paint::~Paint()
{
	delete ui;
}

void Paint::mousePressEvent(QMouseEvent *event) {

	// track the mouse location
	if (event->x() > 0 && event->y() > 0 && event->x() < 1024 && event->y() < 768){
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

		lines.push_back(stroke);
		stroke.clear();
	}

	repaint();
}

void Paint::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	// set pen for painting;
	painter.setPen(QPen(Qt::black, 3));

	// paint the lines first
	for (size_t i = 0; i < lines.size() ; ++i){
		for (size_t j = 0; j < lines[i].size() ; ++j){
			painter.drawPoint(lines[i][j].x() , lines[i][j].y() );
		}
	}

	// then paint the stroke;
	for (size_t i = 0; i < stroke.size() ; ++i) painter.drawPoint(stroke[i].x() , stroke[i].y());

}

void Paint::on_confirmButton_clicked() {
	// recogonize in video 2;
}

void Paint::on_clearButton_clicked() {

	stroke.clear();
	lines.clear();

	painting = false;
	painted = false;

	repaint();

}
