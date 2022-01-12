#include <Arduino.h>

const int SERVO = 9;
const int DIR0 = 2;
const int DIR1 = 4;
const int STEP0 = 3;
const int STEP1 = 11;
const int PHR1 = A0;
const int PHR2 = A1;
const int PHR_ANS = 650;
const int RPM_5 = 22;
const int RPM_90 = 307;
const int BACK = 100;
const int JET = 333;
const int SLP0 = 7;
const int SLP1 = 8;

int ans1;
int ans2;
int p = 1;

void setup()
{
  // cli();
  // TCCR1A  = 0;
  // TCCR1B  = 0;  //12ビット
  // TCNT1 = 0;  //???
  // TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);  //CTC 分周1024  
  // OCR1A   = 46875-1;
  // TIMSK1 |= (1 << OCIE1A);
  // sei();

  Serial.begin(9600); // 9600bpsでシリアル通信のポートを開きます
  pinMode(PHR1, INPUT);
  pinMode(PHR2, INPUT);
  pinMode(SERVO,OUTPUT);
  pinMode(DIR0, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP0, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(SLP0,OUTPUT);
  pinMode(SLP1,OUTPUT);
  digitalWrite(SLP0, HIGH);
  digitalWrite(SLP1, HIGH);
}

void cw()
{
  for(int i = 500; i <= 1900; i += 100){
    {
    digitalWrite(SERVO,HIGH);
    delayMicroseconds(i);
    }
    delay(1);
    {
    digitalWrite(SERVO,LOW);
    delayMicroseconds(20000-i);
    }
    delay(1);
  }
}

void ccw()
{
  for(int i = 1900; i >= 500; i -= 100){
    {
    digitalWrite(SERVO,HIGH);
    delayMicroseconds(i);
    }
    delay(1);
    {
    digitalWrite(SERVO,LOW);
    delayMicroseconds(20000-i);
    }
    delay(1);
  }
}

void jet_stop()
{
  digitalWrite(SERVO,HIGH);
  delayMicroseconds(500);
  digitalWrite(SERVO,LOW);
  delayMicroseconds(19500);
}

void jet()
{
  cw();
  delay(10);
  ccw();
  delay(10);
}

void stop()
{
  digitalWrite(DIR0, HIGH);
  digitalWrite(DIR1, HIGH);
  digitalWrite(STEP0, HIGH);
  digitalWrite(STEP1, HIGH);
}

void go_forward()
{
  for (int j = 0; j < JET; j++)
  {
    digitalWrite(DIR0, LOW);
    digitalWrite(DIR1, HIGH);
    digitalWrite(STEP0, HIGH);
    digitalWrite(STEP1, HIGH);
    delay(5);
    digitalWrite(STEP0, LOW);
    digitalWrite(STEP1, LOW);
    delay(4);
  }
  jet();
}

void go_back()
{
  digitalWrite(DIR0, HIGH);
  digitalWrite(DIR1, LOW);
  for (int i = 0; i < BACK; i++)
  {
    digitalWrite(STEP0, HIGH);
    digitalWrite(STEP1, HIGH);
    delay(5);
    digitalWrite(STEP0, LOW);
    digitalWrite(STEP1, LOW);
    delay(4);
  }
}

void turn_right_5()
{
  digitalWrite(DIR0, HIGH);
  digitalWrite(DIR1, HIGH);
  digitalWrite(STEP1, HIGH);

  for (int i = 0; i < RPM_5; i++)
  {
    digitalWrite(STEP0, HIGH);
    delay(6);
    digitalWrite(STEP0, LOW);
    delay(6);
  }
}

void turn_right_90()
{
  digitalWrite(DIR0, HIGH);
  digitalWrite(DIR1, HIGH);
  digitalWrite(STEP0, HIGH);

  for (int i = 0; i < RPM_90; i++)
  {
    digitalWrite(STEP1, HIGH);
    delay(6);
    digitalWrite(STEP1, LOW);
    delay(6);
  }
}

void turn_left_5()
{
  digitalWrite(DIR0, LOW);
  digitalWrite(DIR1, LOW);
  digitalWrite(STEP0, HIGH);

  for (int i = 0; i < RPM_5; i++)
  {
    digitalWrite(STEP1, HIGH);
    delay(6);
    digitalWrite(STEP1, LOW);
    delay(6);
  }
}

void turn_left_90()
{
  digitalWrite(DIR0, LOW);
  digitalWrite(DIR1, LOW);
  digitalWrite(STEP0, HIGH);

  for (int i = 0; i < RPM_90; i++)
  {
    digitalWrite(STEP1, HIGH);
    delay(6);
    digitalWrite(STEP1, LOW);
    delay(6);
  }
}



void read_sensor(){
  ans1 = analogRead(PHR1);
  ans2 = analogRead(PHR2);
  Serial.println(ans1);
  Serial.println(ans2); // シリアルモニターに表示させる
}

void run()
{
  if (ans1 < PHR_ANS && ans2 < PHR_ANS)
  {
    go_forward();
    // read_sensor();
  }
  else if (ans1 < PHR_ANS && ans2 >= PHR_ANS)
  {
    turn_left_5();
    // read_sensor();
  }
  else if (ans1 >= PHR_ANS && ans2 < PHR_ANS)
  {
    turn_right_5();
    // read_sensor();
  }
}

void rotate_right()
{
      turn_left_90();
      
      if (ans1 < PHR_ANS && ans2 < PHR_ANS)
      {
        turn_left_90();
      }

      else if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
      {
        turn_left_90();
        run();
        if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
        {
          stop();
          jet_stop();
        }
      } 
}

void rotate_left()
{
  turn_right_90();

      if (ans1 < PHR_ANS && ans2 < PHR_ANS)
      {
        turn_right_90();
      }

      else if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
      {
        turn_right_90();
        run();
        if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
        {
          stop();
          jet_stop();
        }
      } 
}

/*
ISR (TIMER1_COMPA_vect) {
  jet();
}
*/

void loop()
{
  /*
  while(1);
  // put your main code here, to run repeatedly:
  delay(50);           // 250ms時間待ちで繰り返す
  */

  
  while(1){
      run();
      read_sensor();
  }
  

  /*
  if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
  {
    stop();
  }
  */

  
  if (ans1 >= PHR_ANS && ans2 >= PHR_ANS)
  {
    go_back();

    if(p==1)
    {
      {
        rotate_right();
      }   
      p++;
    }

    if(p==2)
    {
      {
        rotate_left();
      }
      p--;
    }
  }
  
}
