#include "httpdownloader.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QEventLoop>
#include <QNetworkSession>

HTTPdownloader::HTTPdownloader() {

}

HTTPdownloader::~HTTPdownloader() {}

requestStruct HTTPdownloader::doGet(QString url) {
	int tries = 0;
	requestStruct strct;
	do {
		qDebug() << "get" << url;
		QNetworkRequest req;
		req.setUrl(QUrl(url));
		//req.setRawHeader("User-Agent", "bsToGetLinklist/1.0 (Nokia; Qt)"); //was a test because bs.to wasnt returning valid results
		QNetworkReply *reply = this->get(static_cast<const QNetworkRequest>(req));
		QEventLoop loop;
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();

		strct.httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		strct.data = reply->readAll();
		strct.url = url;
		qDebug() << "getDone" << strct.httpStatusCode << strct.data.size() << url << tries;
		reply->deleteLater();
		if (strct.httpStatusCode == 200)
			return strct;
		++tries;
	} while (tries < 4);
	return strct;
}

requestStruct HTTPdownloader::doPost(QString url, QByteArray data) {
	QNetworkRequest *NR = new QNetworkRequest(QUrl(url));
	NR->setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	QNetworkReply *reply = this->post(*NR, data);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	requestStruct strct;
	strct.httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	strct.data = reply->readAll();
	strct.url = url;
	strct.postData = data;
	reply->deleteLater();
	delete NR;
	return strct;
}
