//librerie mkr
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Relay.h>

//Librerie per il wi-fi
#include <SPI.h> 
#include <WiFiNINA.h>

float x, y, z; // valori dell'accelerazione sui vari assi
MKRIoTCarrier carrier; // shield

void mandaAccelerazioni(client) {

}

// funzione chiamata quando si visita PERCORSO_DATI
// legge l'accelerazione 100 volte ogni 100 ms ed invia una pagina web col grafico a/t
void leggiAccelerometro(WiFiClient paginaWeb) {
  int tempi[100]; //Array contenente i tempi
  float acc[100]; //Array contenente le accelerazioni
  int tempo = 100; //valore da attribuire al delay
  float accX[100]; //array contenente le accelerazioni sull'asse x
  float accY[100]; //array contenente le accelerazioni sull'asse y
  float accZ[100]; //array contenente le accelerazioni sull'asse z
  for (int i=0; i<100;i++){ //Legge l'accelerazione e inserisce i valori ottenuti in acc[i] in linea con il tempo 100 volte
    if (carrier.IMUmodule.accelerationAvailable()){ 
      carrier.IMUmodule.readAcceleration(x,y,z);
    }
    tempi[i]= i*tempo; //tempo che corrisponde all'accelerazione
    acc[i] = sqrt(x*x + y*y + z*z); //formula matematica
    accX[i] = x;
    accY[i] = y;
    accZ[i] = z;
    delay(tempo); // quanto aspettare tra una misurazione e l'altra
  }


  // debugging in seriale per controllare i valori misurati e quelli inviati
  Serial.print("T = [");
  for(int i=0;i<100;i++){
    Serial.print(tempi[i]);
    Serial.print(", ");
  } 
  Serial.println("]");
  Serial.print("A = [");
  for(int i=0;i<100;i++){
    Serial.print(acc[i]);
    Serial.print(", ");
  } 
  Serial.println("]");
        // fine debugging
 
  // inviamo al client una pagina web, stampando riga per riga i tag html statici e il codice javascript (variabile)
  paginaWeb.println("<html>");
  // la libreria Chart.js
  paginaWeb.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>");
  // il canvas dove andremo a disegnare il grafico
  paginaWeb.println("<canvas id=\"Grafico\" style=\"width:100%;max-width:600px\"></canvas>");
  paginaWeb.println("<canvas id=\"GraficoSeparato\" style=\"witdh:100%;max-width:600px\"></canvas>");
  // inizio codice javascript da eseguire per generare il grafico, si deve inserire all'interno del tag script
  paginaWeb.println("<script>");
  paginaWeb.print("var tempi = [");

  // all'interno della sezione script dichiariamo due array e li riempiamo:
  /*
   * var tempi = [a, b, c, d];
   * var acc = [x, y, z, w];
   */
  for (int i = 0; i < 100; i++) {
    paginaWeb.print(tempi[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");
  
  paginaWeb.print("var acc = [");
  for (int i=0; i<100; i++){
    paginaWeb.print(acc[i]);
    paginaWeb.print(", ");
  } paginaWeb.println("];");
 
  paginaWeb.print("var accX = [");
  for (int i=0; i<100; i++){
    paginaWeb.print(accX[i];
    paginaWeb.print(", ");
  } paginaWeb.println("];");
  
  paginaWeb.print("var accY = [");
  for (int i=0; i<100; i++){
    paginaWeb.print(accY[i];
    paginaWeb.print(", ");
  } paginaWeb.println("];");
  
  paginaWeb.print("var accZ = [");
  for (int i=0; i<100; i++){
    paginaWeb.print(accZ[i];
    paginaWeb.print(", ");   
  } paginaWeb.println("];");
  

  // generiamo un grafico con l'array tempi come asse x e acc come asse y, attraverso la libreria Chart.js
  // il grafico viene inserito nel canvas con id "Grafico", è di tipo "line" e l'unica serie di dati (dataset) presente è quella delle accelerazioni
  // mettendo più datasets, magari di più misurazioni si generano più linee
  paginaWeb.println("new Chart(\"Grafico\",{type: \"line\", data: {labels: tempi,datasets: [{label: \"Accelerazione\", fill: false, lineTension: 0, backgroundColor: \"rgba(0,0,0, 1.0)\", data: acc}]}});");
  paginaWeb.println("new Chart(\"GraficoSeparato\",{type: \"line\", data: {labels: tempi, datasets: [{label: \"Accelerazione X\", fill: false, lineTension:0, backgroundColor: \"rgba(0,0,255, 1.0)\", borderColor: \"rgba(0,0,255,0.1)\", data: accX},{ label: \"Accelerazione Y\", fill: false, lineTension:0, backgroundColor: \"rgba(0,255,0, 1.0)\", borderColor: \"rgba(0,255,0, 0.1)\", data: accY},{label: \"Accelerazione Z\", fill: false, lineTension:0, backgroundColor: \"rgba(255,0,0, 1.0)\", borderColor(255,0,0, 0.1)\", data: accZ}]}});"); 

  // chiusura dei tag per la pagina
  paginaWeb.println("</script>");
  paginaWeb.println("</html>");
}

char ssid[]="Blueface_dawe";
char pass[]="rustigay";
int status = WL_IDLE_STATUS;


WiFiServer server(80);
void setup() {
  // inizializzazione seriale
  Serial.begin(9600);
  while (!Serial) {}

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
    
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  
  Serial.println("Connesso 8)");

  // stampiamo l'indirizzo ip da usare quando si vuole visitare la pagina web
  // ad esempio se l'ip è 172.20.10.4 -> http://172.20.10.4/
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
// http://ipArduino/accelerazioni
String PERCORSO_ACCELERAZIONI = "POST /accelerazioni HTTP/1.1";
// istruzione ricevuta
String lineaCorrente = "";                
void loop() {
  WiFiClient client = server.available();         // controlla se c'è un dispositivo a cui consegnare una pagina web
  if (client) {                                   // Se il client esiste, le sue informazioni vengono inserite all'interno di una stringa per conservarle
    while (client.connected()) {
      if (!client.available()) { // se il client non è disponibile chiudiamo la connessione
        break;
      }

      lineaCorrente = ""; // reset lineaCorrente
      // possiamo leggere le istruzioni ricevute solo un carattere per volta
      // leggi un carattere e aggiungilo a lineaCorrente finché non incontri un a capo
      char c = client.read();
      while (c != '\n' && c != '\r') {
        lineaCorrente += c;
        c = client.read();
      }

      // se non abbiamo ricevuto alcuna istruzione chiudiamo la connessione
      if (lineaCorrente.length() == 0) {
        break;
      } else {
        Serial.print("Pagina richiesta: ");
        Serial.println(lineaCorrente);
      }

      // una volta ricevuta la prima riga dal client, cancelliamo il resto dei dati ricevuti (dato che non li usiamo)
      client.flush();
      
      if (lineaCorrente.equals(PERCORSO_DATI)) { // percorso della pagina richiesta
        Serial.println("Pagina dati");                       
        client.println("HTTP/1.1 200 OK");    // Protocollo HTTP da seguire
        client.println("Content-type: text/html"); // stiamo inviando dell'html
        client.println(); // nuova riga per iniziare a mandare l'html (sono finiti gli header)
        leggiAccelerometro(client); // la funzione leggiAccelerometro genera il grafico e lo manda
        break;
      } else if (lineaCorrente.equals(PERCORSO_ROOT)) {
        Serial.println("Pagina di base");                       
        client.println("HTTP/1.1 200 OK");    // Protocollo HTTP da seguire
        client.println("Content-type:text/html"); // stiamo inviando dell'html
        client.println(); // nuova riga per iniziare a mandare l'html (sono finiti gli header)
        client.println("<html><h1>Pagina web funzionante</h1></html>");
        break;
      } else if (lineaCorrente.equals(PERCORSO_ACCELERAZIONI)) {
        mandaAccelerazione(client);
        break;
      } else {
        break;
      }
    }
    // chiusura della connessione
    client.stop();
   }
}
