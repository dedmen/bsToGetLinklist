#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QMap>
#include <QCache>
#include "httprequestjob.h"
#include <QVector3D>
#include <QSettings>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		HTTPdownloader mainHTTP;
		QStringList bsToCache;
	signals:



	private slots:
void titleChanged(const QString&);

		void on_button_newTab_clicked();

		void on_tabWidget_tabCloseRequested(int index);

	private:
		Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
