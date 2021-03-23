#define LEDPIN 3
#define IRPIN 2
#define work_min 3

unsigned long work_time = 1000L * 60 * work_min;
unsigned long tim = 0;
bool isChanged = false;
bool isLight = false;



void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, INPUT);
  attachInterrupt(INT0, onChange, CHANGE);
}

void onChange(){
  isChanged = true;
}

void loop(){
  if(millis() > tim){
    if(isLight){
      digitalWrite(LEDPIN, LOW);
      isLight = false;
    }
  }
  if(isChanged){
    tim = millis() + work_time;
    if(!isLight){
      digitalWrite(LEDPIN, HIGH);
      }
    isChanged = false;
  }
  
}
