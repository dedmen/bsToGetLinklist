#ifndef BSWIDGET_H
#define BSWIDGET_H

#include <QWidget>
#include <QThreadPool>
#include <QCache>
#include "httprequestjob.h"
#include <QVector3D>
#include <QDebug>
QT_BEGIN_NAMESPACE
namespace Ui {
class bsWidget;
}
QT_END_NAMESPACE
class bsWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit bsWidget(QWidget *parent = 0, QStringList& bsToCache = QStringList());
		~bsWidget();


		QMap<quint8, void* > AllEpisodes; //QMap<quint8, QMap<quint8,episodeInfoStruct>  >
		quint8 getHoster(QString hoster,bool addEpisode = false);
		QList<QString> hosters;
		QMap<quint8,quint16> hosterCount;
		QMap<quint8,QMap<quint8,QString>*> episodeDownloadLinks;
		QThreadPool threadPool;
		HTTPdownloader mainHTTP;
		QCache<QString,requestStruct> httpCache;
		void setMovie(const QString&);
	public slots:
		void returnSeasonPage(requestStruct);
		void returnEpisodePage(requestStruct);
	private slots:
		void on_button_start_clicked();
		void on_button_startGetEpisodesList_clicked();
		void on_button_startGetLinks_clicked();
		void on_inputMovieTitle_currentIndexChanged(const QString &arg1);
		void on_button_linksToJD_clicked();
		void on_inputMovieTitle_currentTextChanged(const QString &arg1);

	private:
		Ui::bsWidget *ui;
};

#endif // BSWIDGET_H
