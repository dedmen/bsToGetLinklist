#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegularExpression>
#include <QDebug>
#include "bswidget.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	qRegisterMetaType<requestStruct>("requestStruct");


	requestStruct seriesIndex = mainHTTP.doGet("https://bs.to/serie-alphabet");
	qDebug() << seriesIndex.data;
	QRegularExpression expression("<ul id='series-alphabet-list'>([\\s\\S]*?)<\\/ul>");
	QString seriesPart = expression.match(seriesIndex.data).captured(1);
	qDebug() << seriesPart.size();
	expression.setPattern("<a href=\"serie\\/(.*?).>");
	QRegularExpressionMatchIterator seriesIterator = expression.globalMatch(seriesPart);
	while (seriesIterator.hasNext()){
		QRegularExpressionMatch match = seriesIterator.next();
		bsToCache.append(match.captured(1));
		//ui->inputMovieTitle->addItem(match.captured(1));
	}
	qDebug() << bsToCache.size();
	QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
	qDebug() << settings.fileName() << QCoreApplication::applicationDirPath()+"/settings.ini";
	for (QString it : settings.value("openPages",QStringList()).toStringList()){
		bsWidget* wdg = new bsWidget(this,bsToCache);
		ui->tabWidget->addTab(wdg,"");
		connect(wdg,&bsWidget::windowTitleChanged,this,&MainWindow::titleChanged);
		wdg->setMovie(it);
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::titleChanged(const QString& title)
{
	QStringList tabs;
	for (int var = 0; var < ui->tabWidget->count(); ++var) {
		ui->tabWidget->setTabText(var,ui->tabWidget->widget(var)->windowTitle());
		tabs.append(ui->tabWidget->widget(var)->windowTitle());
	}
	tabs.removeDuplicates();
	QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
	settings.setValue("openPages",tabs);
}

void MainWindow::on_button_newTab_clicked()
{
	bsWidget* wdg = new bsWidget(this,bsToCache);

	int idx = ui->tabWidget->addTab(wdg,"");

	ui->tabWidget->setCurrentIndex(idx);
	ui->tabWidget->setTabText(idx,QString::number(idx));
	connect(wdg,&bsWidget::windowTitleChanged,this,&MainWindow::titleChanged);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
	delete ui->tabWidget->widget(index);
	ui->tabWidget->removeTab(index);
}
