// Dichiarazione di costanti e variabili globali

// Queste costanti non cambieranno. Sono i pin dell’Arduino per l' uscita e dell' ingresso da collegare al sensore:
  const int trigPin = 9;
  const int echoPin = 10;
  const float vSuono = 340;

// Dichiarazioni variabili  
  float duration, distance; // durata del segnale (andata-ritorno) e la distanza in cm

// Istruzioni che vengono effettuati solo una volta all’inizio dell’esecuzione
void setup() 
{
  // Uso dei pin
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);

  // Inizializzare la comunicazione seriale:
    Serial.begin(9600);
}

void loop() 
{
  // Il trasmittore ad ultrasuoni è attivato da un impulso HIGH di 2 o più microsecondi.
  // Effettuare un corto impulso LOW in anticipo, per garantire una pulita HIGH impulso:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Il pin “echo” viene utilizzato per leggere il segnale dal sensore: un impulso HIGH, la cui durata è il tempo 
  // (in microsecondi) dall'invio del segnale alla ricezione della sua eco da un oggetto. Il variabile "duration" è in s.
  
  duration = pulseIn(echoPin, HIGH)/1000000.0; // convertito in secondi da microsecondi

  // La seguente istruzione manca nel programma degli alunni; il compito degli alunni è di
  // capire cosa (e come) si deve calcolare; incluso la conversione in cm.

  distance =  100*0.5*(vSuono*duration);
  //0.5* = distanza totale (avanti-indietro)/2 (solo indietro)
  //100* = conversione metri in cm
  //vSuono*duration = legge della velocità (s = v*t)
  // convertire il tempo in una distanza in cm

  // Il risultato viene comunicato al Monitor (e Plotter) seriale
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);            // L’istruzione delay inserisce una pausa do 100 ms.
}
