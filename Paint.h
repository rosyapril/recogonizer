#ifndef PAINT_H
#define PAINT_H

#include <QWidget>
#include <QVector>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>

namespace Ui {
class Paint;
}

class Paint : public QWidget {
	Q_OBJECT

public:
	explicit Paint(QWidget *parent = 0);
	~Paint();

private slots:
	void on_confirmButton_clicked();

	void on_clearButton_clicked();

private:

	//Variables
	Ui::Paint *ui;

	QVector<QPoint> stroke;
	QVector<QVector<QPoint> > lines;

	bool painting;
	bool painted;



	//Functions:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
};

#endif // PAINT_H
