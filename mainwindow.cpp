#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

#include <QDebug>
#include <QSignalMapper>

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	entryLayout = dynamic_cast<QVBoxLayout*>(ui->lines->layout());
	this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

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
	entry_t* e = new entry_t(ui->name_v->text());

	connect(e->box, &QGroupBox::customContextMenuRequested,
	        this,   [ = ] { editEntry(e); });


//	QSignalMapper* signalMapper = new QSignalMapper(this);
//	connect(e->box,       &QGroupBox::customContextMenuRequested,
//	        signalMapper, &QSignalMapper::map);
//	signalMapper->setMapping(e->box, e);
//	connect(signalMapper, &QSignalMapper::mapped(entry_t*),
//	        this,         &MainWindow::editEntry(entry_t*));

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
MainWindow::editEntry(entry_t* e)
{
	e->setName(ui->name_v->text());
}

MainWindow::entry_t::entry_t(const QString& _n)
        : name(_n), num(0), sum(0),
          avg(1), final(1)
{
	box = new QGroupBox(name);
	hbox = new QHBoxLayout(box);
	gradesWidget = new QWidget(box);
	gradesBox = new QHBoxLayout(gradesWidget);
	addGrade_b = new QPushButton(this);
	grade_sb = nullptr;

	addGrade_b->setText("+");
	QFont f = addGrade_b->font();
	f.setBold(1);
	f.setPixelSize(22);
	addGrade_b->setFont(f);
	addGrade_b->setMaximumWidth(25);
	addGrade_b->setMaximumHeight(25);

	hbox->addWidget(gradesWidget);
	hbox->addWidget(addGrade_b);
	hbox->addStretch(1);
	box->setLayout(hbox);
	gradesWidget->setLayout(gradesBox);
	//                      box->setStyleSheet("QGroupBox { background-color: #444; }");


	avg_l = new MufLabel(QString::number(avg));
	final_l = new MufLabel(QString::number(final));
	avg_l->setFrameStyle(QFrame::StyledPanel);
	final_l->setFrameStyle(QFrame::StyledPanel);

	hbox->addWidget(avg_l);
	hbox->addWidget(final_l);

	connect(addGrade_b, &QPushButton::clicked,
	        this,       &entry_t::showGradeSB);

	box->setContextMenuPolicy(Qt::CustomContextMenu);
}

MainWindow::entry_t::~entry_t()
{
	delete box;
	if (addGrade_b != nullptr) {
		delete addGrade_b;
		addGrade_b = nullptr;
	}
	if (grade_sb != nullptr) {
		delete grade_sb;
		grade_sb = nullptr;
	}
}

void
MainWindow::editName()
{
}

void
MainWindow::entry_t::showGradeSB()
{
	if (grade_sb != nullptr) {
		delete grade_sb;
		grade_sb = nullptr;
	}
	grade_sb = new QSpinBox(box);

	grade_sb->setValue(3);

	hbox->insertWidget(1, grade_sb);
	grade_sb->setFocus();

	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::showGradeSB);
	connect(addGrade_b, &QPushButton::clicked,
	        this,       &entry_t::addGradeSB);

	disconnect(grade_sb, &QSpinBox::editingFinished,
	           this,     &entry_t::saveGradeSB);
	connect(grade_sb, &QSpinBox::editingFinished,
	        this,     &entry_t::addGradeSB);
}

void
MainWindow::entry_t::addGradeSB()
{
	addGrade(grade_sb->value());

	delete grade_sb;
	grade_sb = nullptr;
	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::addGradeSB);
	connect(addGrade_b, &QPushButton::clicked,
	        this,       &entry_t::showGradeSB);
}

void
MainWindow::entry_t::editGradeSB()
{
	if (grade_sb != nullptr) {
		//                              grade_sb->hide();
		emit grade_sb->editingFinished();
	}
	grade_sb = new QSpinBox;

	grade_t to = dynamic_cast<QLabel*>(sender())->text().toInt();
	grade_sb->setValue(to);

	int i = gradesBox->indexOf(dynamic_cast<QWidget*>(sender()));
	gradesBox->removeWidget(dynamic_cast<QWidget*>(sender()));
	delete sender();

	gradesBox->insertWidget(i, grade_sb);
	grade_sb->setFocus();

	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::showGradeSB);
	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::addGradeSB);
	connect(addGrade_b, &QPushButton::clicked,
	        this,       &entry_t::saveGradeSB);

	disconnect(grade_sb, &QSpinBox::editingFinished,
	           this,     &entry_t::addGradeSB);
	connect(grade_sb, &QSpinBox::editingFinished,
	        this,     &entry_t::saveGradeSB);
}

void
MainWindow::entry_t::saveGradeSB()
{
	editGrade(gradesBox->indexOf(grade_sb), grade_sb->value());

	MufLabel* l = new MufLabel(QString::number(grade_sb->value()));
	l->setFrameStyle(QFrame::StyledPanel);
	gradesBox->replaceWidget(grade_sb, l);
	//                      gradesBox->insertWidget(gradesBox->indexOf(grade_sb), l);

	connect(l,    &MufLabel::clicked,
	        this, &entry_t::editGradeSB);

	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::addGradeSB);
	disconnect(addGrade_b, &QPushButton::clicked,
	           this,       &entry_t::saveGradeSB);
	connect(addGrade_b, &QPushButton::clicked,
	        this,       &entry_t::showGradeSB);

	delete grade_sb;
	grade_sb = nullptr;
}

void
MainWindow::entry_t::addGrade(const MainWindow::grade_t& grade)
{
	grades.append(grade);
	sum += grade;
	++num;
	avg = (double)sum / num;
	final = qRound(avg);
	drawGrade(grade); // TODO: remove

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));
}

void
MainWindow::entry_t::removeGrade(const int& index)
{
	grades.removeAt(index);
	sum -= grades.at(index);
	--num;
	avg = (double)sum / num;
	final = qRound(avg);

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));
}

void
MainWindow::entry_t::editGrade(
        const int& index,
        const MainWindow::grade_t& to)
{
	sum -= grades.at(index);
	sum += to;
	grades.replace(index, to);
	avg = (double)sum / num;
	final = qRound(avg);

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));
}

void
MainWindow::entry_t::setName(const QString& _n)
{
	name = _n;
	box->setTitle(name);
}

void
MainWindow::entry_t::drawGrade(const MainWindow::grade_t& grade)
{
	MufLabel* l = new MufLabel(QString::number(grade));
	l->setFrameStyle(QFrame::StyledPanel);
	//                      l->setStyleSheet("MufLabel { color: red; }");
	//                      qDebug() << l->styleSheet();
	//                      QHBoxLayout* bl = dynamic_cast<QHBoxLayout*>(box->layout());
	//                      bl->insertWidget(bl->count() - 1, l);
	gradesBox->addWidget(l);
	//                      gradesBox->insertWidget(gradesBox->count() - 1, l);
	//                      box->layout()->addWidget(l);

	connect(l,    &MufLabel::clicked,
	        this, &entry_t::editGradeSB);
}
