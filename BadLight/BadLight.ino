#define LEDPIN 3
#define IRPIN 4

int work_time = 1000*60*2;
unsigned long time = 0;
bool isLight = false;



void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, INPUT);
}

void loop(){
  if(!isLight){
    if(digitalRead(IRPIN)){
    time = millis() + work_time
    }
  }
  
  if(millis() <= time){
    digitalWrite(LEDPIN, HIGH);
  }
  
  
  if(millis() >= time_now + t){
      time_now += t;
    }
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
