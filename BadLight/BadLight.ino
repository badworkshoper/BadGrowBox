#define LEDPIN 3
#define IRPIN 4

void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, INPUT);
}

void loop(){
  int value = digitalRead(IRPIN);
   
  switch(bool(value)){
    case true:
      digitalWrite(LEDPIN, HIGH);
      delay(20000);
      break;
    case false:
      digitalWrite(LEDPIN, LOW);
      delay(50);
      break;
  }
  
}
