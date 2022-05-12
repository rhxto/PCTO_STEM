//librerie per i sensori
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Relay.h>

//Librerie per il wi-fi
#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
// se opera con batteria (3.7V) è necessaria questa libreria per poter fare andare il wifi
// altre informazioni qua https://www.arduino.cc/en/Guide/MKRWiFi1010/powering-with-batteries
#include <ArduinoLowPower.h>


float x, y, z; // valori dell'accelerazione sui vari assi
MKRIoTCarrier carrier; // oggetto con cui possiamo utilizzare i sensori dello shield

// numero di misurazioni che si vogliono effettuare
const int N_MISURAZIONI = 100;
//tempo che intercorre tra una misurazione e l'altra (in millisecondi)
const int TEMPO = 100;

// funzione chiamata quando si visita PERCORSO_DATI
// legge l'accelerazione 100 volte ogni 100 ms ed invia una pagina web col grafico a/t
// questa funzione manda sulla seriale wifi (paginaWeb) una, appunto, pagina web
// vedere nei files della documentazione un esempio di output di questa funzione (telnet.jpg)
void leggiAccelerometro(WiFiClient paginaWeb) {
  int tempi[N_MISURAZIONI]; //Array contenente i tempi
  float acc[N_MISURAZIONI]; //Array contenente le accelerazioni
  float accX[N_MISURAZIONI]; //array contenente le accelerazioni sull'asse x
  float accY[N_MISURAZIONI]; //array contenente le accelerazioni sull'asse y
  float accZ[N_MISURAZIONI]; //array contenente le accelerazioni sull'asse z
  
  //Legge l'accelerazione e inserisce i valori ottenuti in acc[i], accX[i], accY[i] ed accZ[i] in linea con il tempo N_MISURAZIONI volte e a distanza di TEMPO millisecondi
  for (int i = 0; i < N_MISURAZIONI; i++) { 
    if (carrier.IMUmodule.accelerationAvailable()) {
      carrier.IMUmodule.readAcceleration(x, y, z);
    }

    // il tempo della misurazione corrente corrisponde al numero di misurazioni già effettuate (i) per l'attesa ad ogni misurazione (TEMPO)
    tempi[i] = i * TEMPO;
    // accelerazione totale
    acc[i] = sqrt(x * x + y * y + z * z);
    accX[i] = x;
    accY[i] = y;
    accZ[i] = z;
    delay(TEMPO); // attendi TEMPO secondi prima di effettuare un'altra misurazione
  }


  /* debugging in seriale per controllare i valori misurati e quelli inviati
    Serial.print("T = [");
    for(int i=0;i<100;i++){
    Serial.print(tempi[i]);
    Serial.print(", ");
    }
    Serial.println("]");
    Serial.print("A = [");
    for(int i=0;i<100;i++){
    Serial.print(accX[i]);
    Serial.print(", ");
    }
    Serial.println("]");
    Serial.print("A = [");
    for(int i=0;i<100;i++){
    Serial.print(accY[i]);
    Serial.print(", ");
    }

    Serial.println("]");
        // fine debugging */

  // inviamo al client una pagina web, stampando riga per riga i tag html statici e il codice javascript (variabile)
  paginaWeb.println("<html>");
  // includiamo la libreria Chart.js
  paginaWeb.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>");
  // pulsante per tornare indietro
  paginaWeb.println("<a href=\"/\">Torna indietro</a><br>");
  // i canvas dove andremo a disegnare i grafici
  paginaWeb.println("<canvas id=\"Grafico\"></canvas>");
  paginaWeb.println("<canvas id=\"GraficoSeparato\"></canvas>");
  // inizio codice javascript da eseguire per generare il grafico, si deve inserire all'interno del tag script
  paginaWeb.println("<script>");
  // creo (in javascript, stiamo mandando una pagina web) array dei tempi
  paginaWeb.print("var tempi = [");
  // stampo i valori che lo compongono
  for (int i = 0; i < N_MISURAZIONI; i++) {
    paginaWeb.print(tempi[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");

  // creo in javascript array delle accelerazioni totali
  paginaWeb.print("var acc = [");
  // stampo i valori che lo compongono
  for (int i = 0; i < N_MISURAZIONI; i++) {
    paginaWeb.print(acc[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");

  // creo in javascript array delle accelerazioni sull'asse X
  paginaWeb.print("var accX = [");
  for (int i = 0; i < N_MISURAZIONI; i++) {
    paginaWeb.print(accX[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");

  // creo in javascript array delle accelerazioni sull'asse Y
  paginaWeb.print("var accY = [");
  // stampo i valori che lo compongono
  for (int i = 0; i < N_MISURAZIONI; i++) {
    paginaWeb.print(accY[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");

  // creo in javascript array delle accelerazioni sull'asse Z
  paginaWeb.print("var accZ = [");
  // stampo i valori che lo compongono
  for (int i = 0; i < N_MISURAZIONI; i++) {
    paginaWeb.print(accZ[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");

  // il grafico viene inserito nel canvas con id "Grafico", è di tipo "line" e l'unica serie di dati (dataset) presente è quella delle accelerazioni
  paginaWeb.println("new Chart(\"Grafico\",{type: \"line\", data: {labels: tempi,datasets: [{label: \"Accelerazione\", fill: false, lineTension: 0, backgroundColor: \"rgba(0,0,0, 1.0)\", data: acc}]}});");
  // mettendo più datasets, magari di più misurazioni si generano più linee
  // è ciò che facciamo nel canvas con id "GraficoSeparato", ci sono 3 dataset (3 linee) con rispettivi colori, nomi, e array di dati
  // i dataset sono un array di oggetti es. [dataset1, dataset2] dove dataset1 è l'oggetto {label: "Accelerazioni", data: arrayCreatoInPrecedenza, color: red, ...}
  paginaWeb.println("new Chart(\"GraficoSeparato\",{type: \"line\", data: {labels: tempi, datasets: [{label: \"Accelerazione X\", fill: false, lineTension:0, backgroundColor: \"rgba(0,0,255, 1.0)\", borderColor: \"rgba(0,0,255,0.1)\", data: accX},{ label: \"Accelerazione Y\", fill: false, lineTension:0, backgroundColor: \"rgba(0,255,0, 1.0)\", borderColor: \"rgba(0,255,0, 0.1)\", data: accY},{label: \"Accelerazione Z\", fill: false, lineTension:0, hidden: true, backgroundColor: \"rgba(255,0,0, 1.0)\", borderColor: \"rgba(255,0,0, 0.1)\", data: accZ}]}});");

  // chiusura del tag per javascript
  paginaWeb.println("</script>");
  // pulsante per tornare indietro
  paginaWeb.println("<br><a href=\"/\">Torna indietro</a>");
  paginaWeb.println("</html>");
}

// nome della rete
char ssid[] = "F";
// password della rete, se non c'è password lasciare vuoto
char pass[] = "uisabopdoe";
int status = WL_IDLE_STATUS;

// un oggetto di tipo server (per poter ricevere connessioni in entrata da altri dispositivi, i quali sono i client)
WiFiServer server(80);
void setup() {
  // inizializzazione seriale
  Serial.begin(9600);
  delay(100); // tempo di inizializzazione della seriale se connessa al pc

  // pin per controllare le luci RGB
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  // impsoto led a rosso
  WiFiDrv::analogWrite(25, 70);     // ROSSO
  WiFiDrv::digitalWrite(26, LOW);   // VERDE
  WiFiDrv::digitalWrite(27, LOW);   // BLU
 
  // rilevazione del modulo wifi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Modulo wifi non trovato");
    // il programma si ferma nel caso non ci sia il modulo wifi
    while (true);
  }

  // controllo che la versione del firmware non sia minore dell'ultima, in caso contrario va aggiornato
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Aggiornare il firmware");
  } else {
    Serial.print("Versione firmware: ");
    Serial.println(fv);
  }

  // finché non siamo connessi al wifi riproviamo dando 5 secondi di tempo ogni volta
  // quando arduino sarà connesso la condizione del while sarà falsa e procederà
  while (status != WL_CONNECTED) {
    Serial.print("Connessione a: ");
    Serial.println(ssid);

    if (sizeof(pass) != 0) { // se password è dichiarato ci connettiamo ad una rete con password
      status = WiFi.begin(ssid, pass); // usare per reti con password
    } else { // altrimenti tentiamo la connessione a rete libera
      status = WiFi.begin(ssid);
    }
    WiFi.lowPowerMode(); // per usare la low power mode del wifi, nel caso l'alimentazione sia con batteria
    delay(5000); // aspetta 5 secondi prima di controllare se la connessione è stata effettuata
  }

  Serial.println("Connesso 8)");
  // imposto il led a verde
  WiFiDrv::digitalWrite(25, LOW); // ROSSO
  WiFiDrv::analogWrite(26, 70);   // VERDE
  WiFiDrv::digitalWrite(27, LOW); // BLU

  // stampiamo l'indirizzo ip da usare quando si vuole visitare la pagina web
  // ad esempio se l'ip è 172.20.10.4 -> http://172.20.10.4/ è la pagina da visitare
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // inizializza webserver in porta 80 (standard HTTP)
  server.begin();

  // non abbiamo il case dello shield
  CARRIER_CASE = false;
  // avvio shield
  carrier.begin();
}

// istruzioni che arduino riceve dal broswer a seconda della pagina richiesta
// http://ipArduino/dati
String PERCORSO_DATI = "GET /dati HTTP/1.1";
// http://ipArduino/
String PERCORSO_ROOT = "GET / HTTP/1.1";
// istruzione ricevuta dal broswer
String comandoRicevuto = "";
void loop() {
  WiFiClient client = server.available(); // controlla se c'è un dispositivo a cui consegnare una pagina web
  if (client) {                           // Se il client esiste
    while (client.connected()) {
      if (!client.available()) {          // se il client non è disponibile chiudiamo la connessione
        break;
      }

      comandoRicevuto = ""; // reset comandoRicevuto
      // possiamo leggere le istruzioni ricevute solo un carattere per volta
      // leggi un carattere e aggiungilo a comandoRicevuto finché non incontri un a capo
      char c = client.read();
      while (c != '\n' && c != '\r') {
        comandoRicevuto += c;
        c = client.read();
      }

      // se non abbiamo ricevuto alcuna istruzione chiudiamo la connessione
      if (comandoRicevuto.length() == 0) {
        break;
      } else {
        Serial.print("Pagina richiesta: ");
        Serial.println(comandoRicevuto);
      }

      // una volta ricevuta la prima istruzione dal client, cancelliamo il resto dei dati ricevuti (dato che non li usiamo, ad esempio ci sono cookie ed informazioni tecniche che non ci interessano)
      client.flush();

      if (comandoRicevuto.equals(PERCORSO_DATI)) { // se il broswer ha richiesto la pagina PERCORSO_DATI
        client.println("HTTP/1.1 200 OK");    // Protocollo HTTP da seguire
        client.println("Content-type: text/html"); // stiamo inviando dell'html
        client.println(); // nuova riga per iniziare a mandare l'html (sono finiti i metadati per il broswer)
        leggiAccelerometro(client); // la funzione leggiAccelerometro raccoglie i dati e manda la pagina web
        break;
      } else if (comandoRicevuto.equals(PERCORSO_ROOT)) {
        client.println("HTTP/1.1 200 OK");    // Protocollo HTTP da seguire
        client.println("Content-type: text/html"); // stiamo inviando dell'html
        client.println(); // nuova riga per iniziare a mandare l'html (sono finiti i metadati per il broswer)
        // stampo la pagina home, il codice javascript su onmousedown serve a scrivere "Misurazione in corso" quando si preme sul link per misurare i dati
        client.println("<html><body style=\"text-align:center;\"><h1>Benvenuto</h1><br><a id=\"link\" onmousedown=\"document.getElementById('link').style.display = 'none'; document.getElementById('caricamento').innerHTML = 'Misurazione in corso... attendere';\" href=\"/dati\">Inizia misurazione</a><p id=\"caricamento\"></p><img src=\"https://www.orientamentoravenna.it/images/00061/liceolugo_small.png\"></body></html>");
        break;
      } else {
        // se la pagina richiesta non è né PERCORSO_ROOT né PERCORSO_DATI mandiamo 404
        client.println("HTTP/1.1 404 NOT FOUND");
        break;
      }
    }
    // chiusura della connessione
    client.stop();
  }
}
