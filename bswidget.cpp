#include "bswidget.h"
#include "ui_bswidget.h"
#include <QRegularExpression>
bsWidget::bsWidget(QWidget *parent, QStringList& bsToCache) :
	QWidget(parent),
	ui(new Ui::bsWidget) {
	ui->setupUi(this);

	threadPool.setMaxThreadCount(20);
	ui->progressBar->setRange(0, threadPool.maxThreadCount());

	if (bsToCache.isEmpty()) {	//#TODO set global cache .. maybe function static variable?
		requestStruct seriesIndex = mainHTTP.doGet("https://bs.to/serie-alphabet");
		QRegularExpression expression("<ul id='serSeries'>([\\s\\S]*?)<\\/ul>");
		QString seriesPart = expression.match(seriesIndex.data).captured(1);
		expression.setPattern("<a href=\"serie\\/(.*?).>");
		QRegularExpressionMatchIterator seriesIterator = expression.globalMatch(seriesPart);
		while (seriesIterator.hasNext()) {
			QRegularExpressionMatch match = seriesIterator.next();
			ui->inputMovieTitle->addItem(match.captured(1));
		}
	} else {
		for (QString it : bsToCache) {
			ui->inputMovieTitle->addItem(it);
		}
	}

}

bsWidget::~bsWidget() {
	delete ui;
}

quint8 bsWidget::getHoster(QString hoster, bool addEpisode) {
	if (!hosters.contains(hoster))
		hosters.append(hoster);
	if (addEpisode)
		hosterCount.insert(hosters.indexOf(hoster), hosterCount.value(hosters.indexOf(hoster), 0) + 1);
	return hosters.indexOf(hoster);
}

void bsWidget::setMovie(const QString& title) {
	ui->inputMovieTitle->setCurrentText(title);
}

void bsWidget::returnSeasonPage(requestStruct strct) {
	QRegularExpression expression("(?:<div>[\\s\\S]*?<table>)([\\s\\S]*?)(?:<\\/table>[\\s\\S]*?<\\/div>)");
	QString episodesPart = expression.match(strct.data).captured(1);
	expression.setPattern("<a class=.*title=.*[^<]*href=.serie\\/.*\\/(\\d*)\\/(\\d*)-(.*)\\/(.*).>(.*)<\\/a>");
	QRegularExpressionMatchIterator episodesIterator = expression.globalMatch(episodesPart);

	while (episodesIterator.hasNext()) {
		QRegularExpressionMatch match = episodesIterator.next();
		episodeInfoStruct episode;
		episode.season = match.captured(1).toInt();
		episode.episode = match.captured(2).toInt();
		episode.title = match.captured(3);
		episode.hosterLink = match.captured(4);
		episode.hoster = getHoster(match.captured(5), true);
		if (AllEpisodes.value(episode.season, NULL) == nullptr)
			AllEpisodes.insert(episode.season, new QMap<quint8, QMap<quint8, episodeInfoStruct>*>());
		if (!static_cast<QMap<quint8, QMap<quint8, episodeInfoStruct>*>*>(AllEpisodes.value(episode.season))->contains(episode.episode)) {
			QMap<quint8, QMap<quint8, episodeInfoStruct>*>* seasonMap = ((QMap<quint8, QMap<quint8, episodeInfoStruct>*>*)AllEpisodes.value(episode.season));
			seasonMap->insert(episode.episode, new QMap<quint8, episodeInfoStruct>());
		}
		QMap<quint8, episodeInfoStruct>* episodeMap = ((QMap<quint8, QMap<quint8, episodeInfoStruct>*>*)AllEpisodes.value(episode.season))->value(episode.episode);
		episodeMap->insert(episode.hoster, episode);
		qDebug() << episode.title;
	}
	qDebug() << __FUNCTION__ << threadPool.activeThreadCount();
	if (threadPool.activeThreadCount() <= 1) {
		qDebug() << "finishedSeason";


		//ui->list_Episodes->resizeColumnToContents(1);

		ui->button_startGetEpisodesList->setEnabled(true);
		ui->button_startGetLinks->setEnabled(true);
		ui->inputMovieTitle->setEnabled(true);
		for (QString hoster : hosters) {
			//QTreeWidgetItem* item = new QTreeWidgetItem();
			//item->setText(0,hoster);
			//item->setText(1,QString::number(hosterCount.value(getHoster(hoster))));
			ui->list_Hosters->addItem(hoster + " | " + QString::number(hosterCount.value(getHoster(hoster))));
			//ui->list_Hosters->addTopLevelItem(item);
		}
		ui->list_Episodes->clear();
		QMapIterator<quint8, void*> itSeason(AllEpisodes);
		int episodeCount = 0;
		while (itSeason.hasNext()) {
			itSeason.next();
			if (itSeason.value() == nullptr)
				continue;
			//qDebug() << itSeason.key();
			QTreeWidgetItem* currentSeasonItem = new QTreeWidgetItem();
			currentSeasonItem->setText(0, QString::number(itSeason.key()));
			QMapIterator<quint8, QMap<quint8, episodeInfoStruct>*> itEpisode(*static_cast<QMap<quint8, QMap<quint8, episodeInfoStruct>*>*>(itSeason.value()));
			while (itEpisode.hasNext()) {
				itEpisode.next();
				QMap<quint8, episodeInfoStruct>* mapEpisode = itEpisode.value();
				mapEpisode->value(mapEpisode->firstKey());
				QTreeWidgetItem* currentEpisodeItem = new QTreeWidgetItem();
				currentEpisodeItem->setText(0, QString::number(mapEpisode->value(mapEpisode->firstKey()).episode));
				currentEpisodeItem->setText(1, mapEpisode->value(mapEpisode->firstKey()).title);
				currentSeasonItem->addChild(currentEpisodeItem);
				episodeCount++;
			}
			ui->list_Episodes->addTopLevelItem(currentSeasonItem);
			currentSeasonItem->setExpanded(true);
		}
		ui->progressBar->setRange(0, episodeCount);
		ui->episodesInfoLabel->setText("Anzahl der Episoden: " + QString::number(episodeCount));

	}
}

void bsWidget::returnEpisodePage(requestStruct strct) {
	if (strct.httpStatusCode != 200) {
		ui->progressBar->setValue(ui->progressBar->value() + 1);
		//ui->progressBar->setValue(-(threadPool.activeThreadCount()-threadPool.maxThreadCount()));
		//qDebug() << threadPool.activeThreadCount();
		if (threadPool.activeThreadCount() < 2) { // dont know why this ends when ThreadCount == 1
			qDebug() << "finishedEpisode";
			ui->button_startGetLinks->setEnabled(true);
			ui->button_linksToJD->setEnabled(true);
			ui->inputMovieTitle->setEnabled(true);
			//		QMapIterator<quint8,QMap<quint8,QString>*> iterator (episodeDownloadLinks);
			//		while (iterator.hasNext()){
			//			iterator.next();
			//			QMapIterator<quint8,QString> iterator2(*iterator.value());
			//			while (iterator2.hasNext()){
			//				iterator2.next();
			//				qDebug() << iterator2.key() << iterator2.value();
			//			}
			//		}
		}
		return;
	}

	QRegularExpression expression("<a target='_blank' href='([^']*)'>");
	QString episodesLink = expression.match(strct.data).captured(1);
	expression.setPattern("https?:\\/\\/bs.to\\/serie\\/.*?\\/(\\d*)\\/(\\d*)");
	QRegularExpressionMatch urlMatch = expression.match(strct.url);
	if (!episodeDownloadLinks.contains(urlMatch.captured(1).toInt()))
		episodeDownloadLinks.insert(urlMatch.captured(1).toInt(), new QMap<quint8, QString>);
	episodeDownloadLinks.value(urlMatch.captured(1).toInt())->insert(urlMatch.captured(2).toInt(), episodesLink);
	for (int var = 0; var < ui->list_Episodes->topLevelItemCount(); ++var) {
		if (ui->list_Episodes->topLevelItem(var)->text(0) == urlMatch.captured(1)) {
			for (int var2 = 0; var2 < ui->list_Episodes->topLevelItem(var)->childCount(); ++var2) {
				if (ui->list_Episodes->topLevelItem(var)->child(var2)->text(0) == urlMatch.captured(2)) {
					ui->list_Episodes->topLevelItem(var)->child(var2)->setBackgroundColor(0, QColor(0xFF, 0x66, 0x0));
					ui->list_Episodes->topLevelItem(var)->child(var2)->setBackgroundColor(1, QColor(0xFF, 0x66, 0x0));
					break;
				}
			}
			break;
		}
	}
	ui->progressBar->setValue(ui->progressBar->value() + 1);
	//ui->progressBar->setValue(-(threadPool.activeThreadCount()-threadPool.maxThreadCount()));
	//qDebug() << threadPool.activeThreadCount();
	if (threadPool.activeThreadCount() < 2) { // dont know why this ends when ThreadCount == 1
		qDebug() << "finishedEpisode";
		ui->button_startGetLinks->setEnabled(true);
		ui->button_linksToJD->setEnabled(true);
		ui->inputMovieTitle->setEnabled(true);
		//		QMapIterator<quint8,QMap<quint8,QString>*> iterator (episodeDownloadLinks);
		//		while (iterator.hasNext()){
		//			iterator.next();
		//			QMapIterator<quint8,QString> iterator2(*iterator.value());
		//			while (iterator2.hasNext()){
		//				iterator2.next();
		//				qDebug() << iterator2.key() << iterator2.value();
		//			}
		//		}
	}
}

void bsWidget::on_button_start_clicked() {
	AllEpisodes.clear();
	ui->list_Seasons->clear();
	ui->list_Episodes->clear();
	ui->button_start->setEnabled(false);
	ui->inputMovieTitle->setEnabled(false);
	ui->button_startGetEpisodesList->setEnabled(false);
	requestStruct mainPage = mainHTTP.doGet("https://bs.to/serie/" + ui->inputMovieTitle->currentText());
	QRegularExpression expression("(?:<ul class=\"pages\">)([\\s\\S]*?)<\\/ul>");
	QString seasonsPart = expression.match(mainPage.data).captured(1);
	expression.setPattern("<a href=.*>([\\d]*?)<\\/a>");
	QRegularExpressionMatchIterator seasonsIterator = expression.globalMatch(seasonsPart);
	while (seasonsIterator.hasNext()) {
		QRegularExpressionMatch match = seasonsIterator.next();
		//qDebug() << "Season " << match.captured(1).toInt();
		ui->list_Seasons->addItem(QString::number(match.captured(1).toInt()));
		ui->list_Seasons->item(ui->list_Seasons->count() - 1)->setSelected(true);
		AllEpisodes.insert(match.captured(1).toInt(), nullptr);
	}
	ui->button_start->setEnabled(true);
	ui->button_startGetEpisodesList->setEnabled(true);
	ui->inputMovieTitle->setEnabled(true);
}

void bsWidget::on_button_startGetEpisodesList_clicked() {
	QMapIterator<quint8, void*> itSeason(AllEpisodes);
	while (itSeason.hasNext()) {
		itSeason.next();
		if (itSeason.value() == nullptr)
			continue;
		QTreeWidgetItem* currentSeasonItem = new QTreeWidgetItem();
		currentSeasonItem->setText(0, QString::number(itSeason.key()));
		QMapIterator<quint8, QMap<quint8, episodeInfoStruct>*> itEpisode(*static_cast<QMap<quint8, QMap<quint8, episodeInfoStruct>*>*>(itSeason.value()));
		while (itEpisode.hasNext()) {
			itEpisode.next();
			if (itEpisode.value())
				delete itEpisode.value();
		}
		static_cast<QMap<quint8, QMap<quint8, episodeInfoStruct>*>*>(itSeason.value())->clear();
	}
	hosters.clear();
	hosterCount.clear();
	for (auto ptr : episodeDownloadLinks)
		if (ptr) delete ptr;
	episodeDownloadLinks.clear();
	ui->list_Hosters->clear();
	ui->inputMovieTitle->setEnabled(false);
	ui->button_startGetEpisodesList->setEnabled(false);
	episodeDownloadLinks.clear();
	//QMapIterator<quint8,void*> seasonsIterator(AllEpisodes);
	for (QListWidgetItem* item : ui->list_Seasons->selectedItems()) {
		quint8 season = item->text().toInt();
		httpRequestJob *job = new httpRequestJob(httpRequestJobEnum::getRequest);
		job->setUrl("https://bs.to/serie/" + ui->inputMovieTitle->currentText() + "/" + QString::number(season));
		job->setAutoDelete(true);
		connect(job, &httpRequestJob::finished, this, &bsWidget::returnSeasonPage);
		threadPool.start(job);
	}
}

void bsWidget::on_button_startGetLinks_clicked() {
	ui->inputMovieTitle->setEnabled(false);
	ui->button_startGetLinks->setEnabled(false);
	ui->button_linksToJD->setEnabled(false);
	ui->progressBar->setValue(0);
	QMapIterator<quint8, void*> itSeason(AllEpisodes);
	while (itSeason.hasNext()) {
		itSeason.next();
		if (itSeason.value() == nullptr)
			continue;
		QMapIterator<quint8, QMap<quint8, episodeInfoStruct>*> itEpisode(*static_cast<QMap<quint8, QMap<quint8, episodeInfoStruct>*>*>(itSeason.value()));
		while (itEpisode.hasNext()) {
			itEpisode.next();
			QMap<quint8, episodeInfoStruct>* mapEpisode = itEpisode.value();
			const episodeInfoStruct episode = mapEpisode->value(getHoster(
				ui->list_Hosters->item(0)->text().split(" | ").first()
				//ui->list_Hosters->topLevelItem(0)->text(0)
			));

			httpRequestJob *job = new httpRequestJob(httpRequestJobEnum::getRequest);
			job->setUrl("https://bs.to/serie/" + ui->inputMovieTitle->currentText() + "/" + QString::number(episode.season) + "/" + QString::number(episode.episode) + "-" + episode.title + "/" + episode.hosterLink);
			connect(job, &httpRequestJob::finished, this, &bsWidget::returnEpisodePage);
			threadPool.start(job);
		}
	}
}

void bsWidget::on_inputMovieTitle_currentIndexChanged(const QString &) {
	AllEpisodes.clear();
	ui->list_Seasons->clear();
	ui->list_Episodes->clear();
	ui->list_Hosters->clear();
	episodeDownloadLinks.clear();
}

void bsWidget::on_button_linksToJD_clicked() {
	qDebug() << "##########OUT##########";
	//QFile cj("T:/bs.crawljob");	//can create JDownloader crawljob files
	//cj.open(QFile::WriteOnly);


	for (int var = 0; var < ui->list_Episodes->topLevelItemCount(); ++var) {
		for (int var2 = 0; var2 < ui->list_Episodes->topLevelItem(var)->childCount(); ++var2) {
			QString season = ui->list_Episodes->topLevelItem(var)->text(0);
			QString episode = ui->list_Episodes->topLevelItem(var)->child(var2)->text(0);
			QString title = ui->list_Episodes->topLevelItem(var)->child(var2)->text(1);
			QString url = episodeDownloadLinks.value(ui->list_Episodes->topLevelItem(var)->text(0).toInt())->value(ui->list_Episodes->topLevelItem(var)->child(var2)->text(0).toInt());
			httpRequestJob *job = new httpRequestJob(httpRequestJobEnum::postRequest);
			job->setUrl("http://127.0.0.1:9666/flash/add");
			connect(job, &httpRequestJob::finished, this, &bsWidget::returnEpisodePage);
			job->setData(QString("passwords=myPassword&source=" + QUrl::toPercentEncoding(
				url +
				"&dump=" +//&dump
				ui->inputMovieTitle->currentText().replace("-", ".") +
				"." +
				QString("S%1E%2").arg(season, 2, QChar('0')).arg(episode, 2, QChar('0')) +
				"." + title.replace("-", ".")
			) +

				"&submit=Add%2BLink%2Bto%2BJDownloader&urls=" +
				QUrl::toPercentEncoding(url)).toUtf8());
			qDebug() << QUrl::toPercentEncoding(
				url +
				"&dump=" +//&dump
				ui->inputMovieTitle->currentText().replace("-", ".") +
				"." +
				QString("S%1E%2").arg(season, 2, QChar('0')).arg(episode, 2, QChar('0')) +
				"." + title.replace("-", ".")
			);
			QString fulltitle = ui->inputMovieTitle->currentText().replace("-", ".") +
				"." +
				QString("S%1E%2").arg(season, 2, QChar('0')).arg(episode, 2, QChar('0')) +
				"." + title.replace("-", ".");
			//cj.write("\n->NEW ENTRY<-\n");
			//cj.write(QString("text="+url+"&dump="+fulltitle+"\n").toUtf8());
			//cj.write(QString("filename="+fulltitle+"\n\n").toUtf8());
			threadPool.start(job);
		}
	}
	qDebug() << "##########END##########";
	//cj.close();
}

void bsWidget::on_inputMovieTitle_currentTextChanged(const QString &arg1) {
	this->setWindowTitle(arg1);
}
