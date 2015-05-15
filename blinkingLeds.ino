int led = 13;
int ledDelayAmount = 200;

void setup() {
  pinMode(led,OUTPUT);
}


void displayLedOnce(int time) {
  digitalWrite(led,HIGH);
  delay(time);
  digitalWrite(led,LOW);
  delay(time);
}


void loop() {
 displayLedOnce(ledDelayAmount);
}
