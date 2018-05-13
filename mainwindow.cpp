#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	entryLayout = dynamic_cast<QVBoxLayout*>(ui->lines->layout());

	connect(ui->addEntry_b, &QPushButton::clicked,
	        this,           &MainWindow::addEntry);
}

MainWindow::~MainWindow()
{
	delete ui;
	for (auto& el : entries) {
		delete el;
	}
	entries.clear();
}

void
MainWindow::addEntry()
{
	entry_t* e = new entry_t();
	entries.append(e);

//	e->addGrade(2);
//	e->addGrade(3);
//	e->addGrade(5);
//	e->addGrade(e->final);
	QVBoxLayout* l = dynamic_cast<QVBoxLayout*>(ui->lines->layout());
//	qDebug() << l->count();
	l->insertWidget(l->count() - 2, e->box);
}

void
MainWindow::editEntry()
{
	Q_UNIMPLEMENTED();
}
