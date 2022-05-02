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
  if (carrier.IMUmodule.accelerationAvailable()){ 
    carrier.IMUmodule.readAcceleration(x,y,z);
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" ");
    Serial.println(z);
  }
  
  paginaWeb.println("<html>");
  paginaWeb.println("<p>");
  paginaWeb.println(x);
  paginaWeb.println("</p>");
  paginaWeb.println("<p>");
  paginaWeb.println(y);
  paginaWeb.println("</p>");
  paginaWeb.println("<p>");
  paginaWeb.println(z);
  paginaWeb.println("</p>");
  paginaWeb.println("</html>");
}

char ssid[]="HotWaifu";
char pass[]="12345678";
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
  
  Serial.println("Creazione AP");
  status = WiFi.beginAP(ssid, pass);
  while (status != WL_AP_LISTENING) {
    Serial.print("Waiting for access point: ");
    Serial.println(status);
    delay(2000);
  }
  
  Serial.println("AP creato 8)");
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
      
      char c = client.read();
      while (c != '\n' && c != '\r') {
        lineaCorrente += c;
        c = client.read();
      }

      if (lineaCorrente.length == 0) {
        break;
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
      }
    }
    // chiusura della connessione
    client.stop();
   }
}
