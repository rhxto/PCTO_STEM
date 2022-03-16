void setup() {
  // 1 volta a inizio programma
  pinMode(13, OUTPUT); // usiamo il pin 13 in modalità output (è il led)
}

void loop() {
  // ripete a manetta
  digitalWrite(13, HIGH); // imposta il led a acceso (5V)
  delay(2000); // aspetta 2secondi
  digitalWrite(13, LOW); // imposta il led a spento (0V)
  delay(2000); // aspetta 2 secondi
  // ripeti
}
