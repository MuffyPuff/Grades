#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

#include <QDebug>
#include <QSignalMapper>

#include <algorithm>

#define ALL(c) (c).begin(),(c).end()
#define IN(x, c) (find(c.begin(), c.end(), x) != (c).end())
#define REP(i, n) for (int i = 0; i < (int)(n); ++i)
#define FOR(i, a, b) for (int i = (a); i <= (b); ++i)

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
	connect(e,      &entry_t::gradeUpdated,
	        this,   &MainWindow::updateGrades,
	        Qt::QueuedConnection);


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

entry_t::entry_t(const QString& _n)
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

entry_t::~entry_t()
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
MainWindow::updateGrades()
{
//	qDebug() << "test";
	QList<grade_t> grades;
	avg = 0;
	for (auto& el : entries) {
		avg += el->avg;
		grades.append(el->final);
	}
	avg /= entries.size();

//	qSort(grades);
//	std::sort(grades.begin(), grades.end());
	std::sort(ALL(grades));
	qint8 cnt[6];
	cnt[5] = grades.count(5);
	cnt[4] = grades.count(4);
	cnt[3] = grades.count(3);
	cnt[2] = grades.count(2);
	cnt[1] = grades.count(1);
	cnt[0] = grades.size();

	if (cnt[1] > 0) {
		gpa = 1;
	} else if (cnt[5] >= cnt[0] / 2 and cnt[3] <= 1 and cnt[2] == 0) {
		gpa = 5;
	} else if (cnt[5] + cnt[4] >=  cnt[0] / 2 and cnt[2] <= 1) {
		gpa = 4;
	} else if (cnt[5] + cnt[4] + cnt[3] >=  cnt[0] / 2) {
		gpa = 3;
	} else {
		gpa = 2;
	}

	ui->gpa_v->setText(QString::number(gpa));
	ui->avg_v->setText(QString::number(avg, 'g', 3));

}

void
entry_t::showGradeSB()
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
entry_t::addGradeSB()
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
entry_t::editGradeSB()
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
entry_t::saveGradeSB()
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
entry_t::addGrade(const grade_t& grade)
{
	grades.append(grade);
	sum += grade;
	++num;
	avg = (double)sum / num;
	final = qRound(avg);
	drawGrade(grade); // TODO: remove

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));

	emit gradeUpdated();
}

void
entry_t::removeGrade(const int& index)
{
	grades.removeAt(index);
	sum -= grades.at(index);
	--num;
	avg = (double)sum / num;
	final = qRound(avg);

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));


	emit gradeUpdated();
}

void
entry_t::editGrade(
        const int& index,
        const grade_t& to)
{
	sum -= grades.at(index);
	sum += to;
	grades.replace(index, to);
	avg = (double)sum / num;
	final = qRound(avg);

	final_l->setText(QString::number(final));
	avg_l->setText(QString::number(avg, 'g', 3));

	emit gradeUpdated();
}

void
entry_t::setName(const QString& _n)
{
	name = _n;
	box->setTitle(name);
}

void
entry_t::drawGrade(const grade_t& grade)
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
