//librerie mkr
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Relay.h>

//Librerie per il wi-fi
#include <SPI.h> 
#include <WiFiNINA.h>

float x, y, z;
MKRIoTCarrier carrier;
void leggiAccelerometro(WiFiClient paginaWeb) {
  int tempi[100]; //Array contenente i tempi
  float acc[100]; //Array contenente le accelerazioni
  int tempo = 100; //valore da attribuire al delay
  
for(int i=0; i<100;i++){ //Legge l'accelerazione e inserisce i valori ottenuti in acc[i] in linea con il tempo
    if (carrier.IMUmodule.accelerationAvailable()){ 
      carrier.IMUmodule.readAcceleration(x,y,z);
    }
    tempi[i]= i*tempo; //tempo che corrisponde all'accelerazione
    acc[i] = sqrt(x*x + y*y + z*z); //formula matematica
    delay(tempo); //ogni quanto i valori vengono inseriti negli array (in millisecondi)
  }


  
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
 
  
  paginaWeb.println("<html>");
  paginaWeb.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>");
  paginaWeb.println("<canvas id=\"Grafico\" style=\"width:100%;max-width:600px\"></canvas>");
  paginaWeb.println("<script>");
  paginaWeb.print("var tempi = [");
  for (int i = 0; i < 100; i++) {
    paginaWeb.print(tempi[i]);
    paginaWeb.print(", ");
  }
  paginaWeb.println("];");
  paginaWeb.print("var acc = [");
  for (int i=0; i<100; i++){
    paginaWeb.print(acc[i]);
    paginaWeb.print(", ");
  }
  paginaWeb.println("];");
  paginaWeb.println("new Chart(\"Grafico\",{type: \"line\", data: {labels: tempi,datasets: [{fill: false, lineTension: 0, data: acc}]}});");
  paginaWeb.println("</script>");
  paginaWeb.println("</html>");
}

char ssid[]="Blueface_dawe";
char pass[]="rustigay";
int status = WL_IDLE_STATUS;


WiFiServer server(80);
void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Modulo wifi non trovato");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Aggiornare il firmware");
  } else {
    Serial.print("Versione firmware: ");
    Serial.println(fv);
  }
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  
  Serial.println("Connesso 8)");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // inizializza webserver in porta 80
  server.begin();
  CARRIER_CASE = false;
  carrier.begin();
}


String PERCORSO_DATI = "GET /dati HTTP/1.1";
String PERCORSO_ROOT = "GET / HTTP/1.1";
String lineaCorrente = "";                
void loop() {
  WiFiClient client = server.available();         // controlla se c'è un dispositivo a cui consegnare una pagina web
  if (client) {                                   // Se il client esiste, le sue informazioni vengono inserite all'interno di una stringa per conservarle
    while (client.connected()) {
      if (!client.available()) {
        break;
      }

      lineaCorrente = ""; // reset lineaCorrente
      char c = client.read();
      while (c != '\n' && c != '\r') {
        lineaCorrente += c;
        c = client.read();
      }

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
        leggiAccelerometro(client);
        client.println();
        // TODO: generazione grafico
        break;
      } else if (lineaCorrente.equals(PERCORSO_ROOT)) {
        Serial.println("Pagina di base");                       
        client.println("HTTP/1.1 200 OK");    // Protocollo HTTP da seguire
        client.println("Content-type:text/html"); // stiamo inviando dell'html
        client.println(); // nuova riga per iniziare a mandare l'html (sono finiti gli header)
        client.println("<html><h1>Pagina web funzionante</h1></html>");
        // chiusura della connessione
        break;
      } else {
        break;
      }
    }
    // chiusura della connessione
    client.stop();
   }
}
