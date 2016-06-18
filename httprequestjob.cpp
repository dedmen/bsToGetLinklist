#include "httprequestjob.h"

httpRequestJob::httpRequestJob(httpRequestJobEnum _type) : type(_type) {

}

httpRequestJob::~httpRequestJob() {

}

void httpRequestJob::run() {
	HTTPdownloader dwnloader;
	if (type == httpRequestJobEnum::postRequest) {
		emit finished(dwnloader.doPost(url, data));
	} else {
		emit finished(dwnloader.doGet(url));
	}
}
httpRequestJobEnum httpRequestJob::getType() const {
	return type;
}

void httpRequestJob::setType(const httpRequestJobEnum& value) {
	type = value;
}

QString httpRequestJob::getUrl() const {
	return url;
}

void httpRequestJob::setUrl(const QString& value) {
	url = value;
}

QByteArray httpRequestJob::getData() const {
	return data;
}

void httpRequestJob::setData(const QByteArray& value) {
	data = value;
}


