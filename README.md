# Client-Server Anwendung

Senden und Empfangen von internen Mails mithilfe von Socket Kommunikation

1. Der Client wird mit einer IP Adresse und einem Port als Parameter gestartet
2. Der Server wird mit einem Port und einem Verzeichnispfad
(Mailspoolverzeichnis) als Parameter gestartet und soll als iterativer Server
ausgelegt werden (keine gleichzeitigen Requests)
3. Der Client connected mittels Stream Sockets über die angegebene IP-Adresse /
Port Kombination zum Server und schickt Requests an den Server.
4. Der Server erkennt und reagiert auf folgende Requests des Clients:
SEND: Senden einer Nachricht vom Client zum Server.
  * LIST: Auflisten der Nachrichten eines Users. Es soll die Anzahl der
Nachrichten und pro Nachricht die Betreff Zeile angezeigt werden. o READ: Anzeigen einer bestimmten Nachricht für einen User.
  * DEL: Löschen einer Nachricht eines Users.
  * QUIT: Logout des Clients

Die gesendeten Nachrichten sollen pro User im Mailspoolverzeichnis permanent gespeichert werden. Die Struktur des Verzeichnisses und der Speicherung bleibt ihnen überlassen (z.B. pro User eine Datei mit allen Nachrichten, oder pro User ein Unterverzeichnis mit einer Datei pro Nachricht).
Als Absender und Empfängeradressen werden nur Usernamen (ohne @domain) verwendet, es handelt sich also um Strings mit max. 8 Zeichen, z.B. if17b001.
Der Protokollaufbau des SEND Befehls ist wie folgt definiert:
```
SEND\n
<Sender max. 8 Zeichen>\n
<Empfänger max. 8 Zeichen>\n
<Betreff max. 80 Zeichen>\n
<Nachricht, beliebige Anzahl an Zeilen\n> .\n
```
D.h. die Ende Kennung der Nachricht ist ein Newline, gefolgt von einem Punkt und noch einem Newline.
Der Server antwortet mit `OK\n` oder `ERR\n` im Fehlerfall.

Der Protokollaufbau des LIST Befehls ist wie folgt definiert:
```
LIST\n
<Username max. 8 Zeichen>\n
Der Server antwortet mit:
<Anzahl der Nachrichten für den User, 0 wenn keine Nachrichten vorhanden sind>\n <Betreff 1>\n
<Betreff 2>\n
...
<Betreff N>\n
```

Der Protokollaufbau des READ Befehls ist wie folgt definiert:
```
READ\n
<Username max. 8 Zeichen>\n
<Nachrichten-Nummer>\n
```
Der Server antwortet bei korrekten Parametern mit:
```
OK\n
<kompletter Inhalt der Nachricht wie beim SEND Befehl>
```
Der Server antwortet im Fehlerfall (Nachricht nicht vorhanden) mit:
```
ERR\n
```

Der Protokollaufbau des DEL Befehls ist wie folgt definiert:
```
DEL\n
<Username max. 8 Zeichen>\n
<Nachrichten-Nummer>\n
```
Der Server antwortet bei korrekten Parametern und erfolgreichem Löschen mit:
```
OK\n
```
Der Server antwortet im Fehlerfall (Nachricht nicht vorhanden, Fehler beim Löschen, etc.) mit:
```
ERR\n
```
Achten Sie auf korrekte Fehlerabfragen und beachten Sie die Richtlinien der C- Programmierung unter Linux!
Tutorials
Verwenden Sie zum Lesen aus dem Socket bis zum nächsten Newline z.B. die Funktion readline() aus dem Tutorial tcpip_linux-prog-details.pdf
