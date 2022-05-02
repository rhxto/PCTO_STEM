int millisecondi = 1000;
int temporaneo; // il led aspetta temporaneo centinaia di millisecondi
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(13, HIGH);
  temporaneo = Serial.read()-'0'; // da "x" (char) a x (int). Ad es '3' - '0' = 3 oppure '7' - 2 = '5'
  if (temporaneo > 0 && temporaneo<10) {
    millisecondi = temporaneo*100;
  }
  
  Serial.print("Luce accesa per ");
  Serial.print(millisecondi);
  Serial.println(" millisecondi");
  delay(millisecondi);
  
  digitalWrite(13, LOW);
  Serial.print("Luce spenta per ");
  Serial.print(millisecondi);
  Serial.println(" millisecondi");
  delay(millisecondi);
}
