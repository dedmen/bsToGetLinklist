#ifndef HTTPREQUESTJOB_H
#define HTTPREQUESTJOB_H

#include <QObject>
#include <QRunnable>
#include "httpdownloader.h"
enum class httpRequestJobEnum {
	postRequest = 1,
	getRequest = 0
};

class httpRequestJob : public QObject, public QRunnable
{
		Q_OBJECT
	public:
		httpRequestJob(httpRequestJobEnum _type);
		~httpRequestJob();
		void run() override;

		QByteArray getData() const;
		void setData(const QByteArray& value);
		QString getUrl() const;
		void setUrl(const QString& value);

		httpRequestJobEnum getType() const;
		void setType(const httpRequestJobEnum& value);

	signals:
		void finished(requestStruct);
private:
	QByteArray data;
	QString url;
	httpRequestJobEnum type;
};

#endif // HTTPREQUESTJOB_H
