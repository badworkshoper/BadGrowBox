#define LEDPIN 3
#define IRPIN 2
#define work_min 3

unsigned long work_time = 1000L * 60 * work_min;
unsigned long timer = 0L;
bool isLight = false;

void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, INPUT);
  attachInterrupt(0, onChange, CHANGE);
}

void onChange(){
  timer = millis() + work_time;
  if(!isLight){
    isLight = true;
    digitalWrite(LEDPIN, HIGH);
    }
}

void loop(){
  if(millis() >= timer){
    if(isLight){
      digitalWrite(LEDPIN, LOW);
      isLight = false;
    }
  }
  delay(50);
}
