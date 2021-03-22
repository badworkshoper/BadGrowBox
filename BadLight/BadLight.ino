#define LEDPIN 3
#define IRPIN 4
#define work_min 3

unsigned long work_time = 1000L * 60 * work_min;
unsigned long time = 0;
bool isChanged = false;
bool isLight = false;



void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRPIN), onChange, CHANGE);
}

void onChange(){
  isChanged = true;
}

void loop(){
  if(millis() > time){
    if(isLight){
      digitalWrite(LEDPIN, LOW);
      isLight = false;
    }
  }
  if(isChanged){
    time = millis() + work_time;
    if(!isLight){
      digitalWrite(LEDPIN, HIGH);
      }
    isChanged = false;
  }
  
}
