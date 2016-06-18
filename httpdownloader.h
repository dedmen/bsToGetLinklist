#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QObject>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QEventLoop>
#include <QProgressBar>

struct requestStruct{
		int httpStatusCode;
		QByteArray data;
		QString url;
		QByteArray postData;
};

struct episodeInfoStruct {
		quint8 season;
		quint8 episode;
		QString title;
		QString hosterLink;
		quint8 hoster;
};

class HTTPdownloader : public QNetworkAccessManager
{
		Q_OBJECT
	public:
		explicit HTTPdownloader();
		~HTTPdownloader();
		requestStruct doGet(QString url);
		requestStruct doPost(QString url, QByteArray data);
	signals:
	public slots:
	private:
};

#endif // HTTPDOWNLOADER_H
