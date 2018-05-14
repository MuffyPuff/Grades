#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QList>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>

#include <QDebug>

namespace Ui
{
class MainWindow;
}

class MufLabel : public QLabel
{
	Q_OBJECT

public:
	explicit MufLabel(QWidget* parent = Q_NULLPTR,
	                  Qt::WindowFlags f = Qt::WindowFlags()): QLabel(parent, f) {}
	explicit MufLabel(QString text,
	                  QWidget* parent = Q_NULLPTR,
	                  Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(text, parent, f) {}
	~MufLabel() {}

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton) {
			emit clicked();
		}
	}
};

class entry_t : public QWidget
{
	Q_OBJECT

	typedef qint8   grade_t;

public:
	entry_t(const QString& _n = "");
	~entry_t();

	QString name;
	QList<grade_t> grades;
	qint8 num;
	qint8 sum;
	qreal avg;
	grade_t final;
	QGroupBox* box;
	QHBoxLayout* hbox;
	QWidget* gradesWidget;
	QHBoxLayout* gradesBox;
	QPushButton* addGrade_b;

	MufLabel* final_l;
	MufLabel* avg_l;

	QSpinBox* grade_sb;

signals:
	void gradeUpdated();

public slots:

	void showGradeSB();
	void addGradeSB();
	void editGradeSB();
	void saveGradeSB();
	void addGrade(const grade_t& grade);
	void removeGrade(const int& index);
	void editGrade(const int& index,
	               const grade_t& to);
	void setName(const QString& _n);
	void drawGrade(const grade_t& grade);
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

	typedef qint8   grade_t;

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

private slots:
	void            addEntry();
	void            editEntry(entry_t* e);
	void            editName();
	void            updateGrades();

private:
	Ui::MainWindow* ui;
	QVBoxLayout*    entryLayout;

	QList<entry_t*> entries;
	qint8           gpa;
	qreal           avg;
};

#endif // MAINWINDOW_H
