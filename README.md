# bsToGetLinklist
Holt alle Streamlinks einer Staffel oder einer ganzen Serie, gibt ihnen einen regelmäßigen Titel und sendet sie an JDownloader.

Um das automatische umbennennungs Feature zu nutzen muss in Jdownloader eine Packetverwaltungsregel hinzugefügt werden.
Die Einstellungen der Regel sind:
  Dateiname enthält *.*
  QuellURL enthält dump=*
  Datei ist online
  Dateiname <jd:source:1>.<jd:orgfilename:2>
  Kommentar <jd:source:1>
  
  Außerdem müssen in den Jdownloader "Profieinstellungen" folgende einträge eingestellt werden:
    "RemoteAPI: Extern Interface Localhost Only" aktiviert
    "RemoteAPI: Extern Interface Auth" "["127.0.0.1"]"
    "RemoteAPI: Extern Interface" aktiviert
    
