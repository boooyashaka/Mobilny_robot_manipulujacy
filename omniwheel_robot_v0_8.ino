#include <AccelStepper.h>
#include <ESP32Servo.h>
#include "BluetoothSerial.h"

#define s1_dirPin 12
#define s1_stepPin 13
#define s2_dirPin 14
#define s2_stepPin 27
#define s3_dirPin 25
#define s3_stepPin 26
#define s4_dirPin 32
#define s4_stepPin 33
#define czlon1_pin 16
#define czlon2_pin 17
#define chwytak_pin 18
#define redLed_pin 15
#define yellowLed_pin 4
#define greenLed_pin 5
#define pi 3.14159
#define minUs 500   //minimalna szerokość impulsu dla seromechanizmów w mikrosekundach oznaczająca minimalne wychylenie - 0 stopni
#define maxUs 2500  //maksymalna szerokość impulsu dla seromechanizmów w mikrosekundach oznaczająca maksymalne wychylenie - 180 stopni
#define motorInterfaceType 1

// Stworzenie obiektów klasy AccelStepper do sterowania każdym z silników krokowych
AccelStepper stepper1 = AccelStepper(motorInterfaceType, s1_stepPin, s1_dirPin);
AccelStepper stepper2 = AccelStepper(motorInterfaceType, s2_stepPin, s2_dirPin);
AccelStepper stepper3 = AccelStepper(motorInterfaceType, s3_stepPin, s3_dirPin);
AccelStepper stepper4 = AccelStepper(motorInterfaceType, s4_stepPin, s4_dirPin);
// Stworzenie obiektu klasy BluetoothSerial w celu otwarcia komunikacji bluetooth
BluetoothSerial SerialBT;
// Stworzenie obiektów klasy Servo do sterowania serwomechanizmami manipulatora
Servo servo1;
Servo servo2;
Servo servo3;

char frame[25]; //zmienna przechowująca otrzymaną ramkę
char command[25];  //zmienna przechowująca odczytaną komendę
int i = 0; //zmienna używana do indeksowania w pętlach for
int j = 0; //zmienna używana do indeksowania w pętlach for
char par1[25]; //zmienna przechowująca odczytany parametr 1
char par2[25]; //zmienna przechowująca odczytany parametr 2
char par3[25]; //zmienna przechowująca odczytany parametr 3
int s_target[3] = {90, 90, 90}; //tablica zmiennych przechowująca docelowe pozycje wszystkich trzech serw
int s_pos[3] = {-1, -1, -1};  //tablica zmiennych przechowująca aktualne pozycje wszystkich trzech serw
int velocity = 200; //zmienna określająca prędkość z jaką poruszają się silniki krokowe przy standardowych ruchach
bool homed = false; //zmienna pamiętająca czy manipulator został zbazowany
float s1_speed ; //zmienna przechowująca prędkość silnika 1
float s2_speed;  //zmienna przechowująca prędkość silnika 2
float s3_speed;  //zmienna przechowująca prędkość silnika 3
float s4_speed;  //zmienna przechowująca prędkość silnika 4
float joystick_angle; //zmienna przechowująca otrzymany kąt nachylenia joysticka
float joystick_strength; //zmienna przechowująca odległość (siłę) wychylenia joysticka
int joystick_ping = 0;
float vel_par = 2; //parametr wpływający na prędkość obrotu silników platformy jeżdżącej w ruchu sterowanym joystickiem

void setup() {
  // Na czas uruchamiania się robota zapala się czerwony LED
  pinMode(redLed_pin, OUTPUT);
  pinMode(yellowLed_pin, OUTPUT);
  pinMode(greenLed_pin, OUTPUT);
  digitalWrite(redLed_pin, HIGH);
  delay(250);
  
  Serial.begin(115200);
  delay(250);
  SerialBT.begin("Omnibot"); //nazwa, pod którą widoczne bęzie urządzenie
  delay(250);
  Serial.println("Bluetooth initiated");

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);
}

void loop() {
  digitalWrite(redLed_pin, LOW);
  digitalWrite(yellowLed_pin, LOW);
  digitalWrite(greenLed_pin, HIGH);
  // Jeśli przyszła nowa ramka to czytamy jej zawartość
  if (SerialBT.available())
  {
    read_frame();
  }
  //Jeżeli komenda została wykryta to szukamy jej w poniższej bazie komend
  if (command[0] > 0)
  {
    // KOMENDA: Ruch do przodu
    if (strcmp(command, "forward") == 0)
    {
      Serial.println("Command recived: forward");
      move_platform(-1, 1,-1, 1);
    }
    // KOMENDA: Ruch do tyłu
    else if (strcmp(command, "backward") == 0)
    {
      Serial.println("Command recived: backward");
      move_platform(1,-1, 1,-1);
    }
    // KOMENDA: Obrót zgodnie z ruchem wskazówek zegara
    else if (strcmp(command, "spincw") == 0)
    {
      Serial.println("Command recived: spincw");
      move_platform(1, 1, 1, 1);
    }
    // KOMENDA: Obrót przeciwnie do ruchu wskazówek zegara
    else if (strcmp(command, "spinccw") == 0)
    {
      Serial.println("Command recived: spinccw");
      move_platform(-1,-1,-1,-1);
    }
    // KOMENDA: Ruch w prawo
    else if (strcmp(command, "right") == 0)
    {
      Serial.println("Command recived: right");
      move_platform(1, 1,-1,-1);
    }
    // KOMENDA: Ruch w lewo
    else if (strcmp(command, "left") == 0)
    {
      Serial.println("Command recived: left");
      move_platform(-1,-1, 1, 1);
    }
    // KOMENDA: Ruch po przekątnej w prawo/do przodu
    else if (strcmp(command, "upright") == 0)
    {
      Serial.println("Command recived: upright");
      move_platform(0, 1, -1, 0);
    }
    // KOMENDA: Ruch po przekątnej w prawo/do tyłu
    else if (strcmp(command, "downright") == 0)
    {
      Serial.println("Command recived: downright");
      move_platform(1, 0, 0, -1);
    }
    // KOMENDA: Ruch po przekątnej w lewo/do tyłu
    else if (strcmp(command, "downleft") == 0)
    {
      Serial.println("Command recived: downleft");
      move_platform(0, -1, 1, 0);
    }
    // KOMENDA: Ruch po przekątnej w lewo/do przodu
    else if (strcmp(command, "upleft") == 0)
    {
      Serial.println("Command recived: upleft");
      move_platform(-1, 0, 0, 1);
    }
    // KOMENDA: Zwiększenie kąta na członie 1
    else if (strcmp(command, "cz1up") == 0)
    {
      move_link(0,1,60,180);
    }
    // KOMENDA: Zmniejszenie kąta na członie 1
    else if (strcmp(command, "cz1down") == 0)
    {
      move_link(0,-1,60,180);
    }
    // KOMENDA: Zwiększenie kąta na członie 2
    else if (strcmp(command, "cz2up") == 0)
    {
      move_link(1,1,0,110);
    }
    // KOMENDA: Zmniejszenie kąta na członie 2
    else if (strcmp(command, "cz2down") == 0)
    {
      move_link(1,-1,0,110);
    }
    // KOMENDA: Zwiększenie kąta rozłożenia chwytaka
    else if (strcmp(command, "chwup") == 0)
    {
      move_link(2,1,58,150);
    }
    // KOMENDA: Zmniejszenie kąta rozłożenia chwytaka
    else if (strcmp(command, "chwdown") == 0)
    {
      move_link(2,-1,58,150);
    }
    // KOMENDA: Ruch wszystkimi członami manipulatora i chwytakiem
    else if (strcmp(command, "arm") == 0 && homed)
    {
      move_arm();
    }
    // KOMENDA: Wybazowanie manipulatora
    else if (strcmp(command, "home") == 0)
    {
      home_arm();
    }
    else if (strcmp(command, "joystick") == 0)
    {
      move_platform_joystick();
    }
    else if (strcmp(command, "200") == 0)
    {
      Serial.println("STOP");
    }
    else
    {
      Serial.println("Command not recognized");
    }
    // Wyczyszczenie ramki
    clear_frame();
  }
  delay(20);
}

// move_platform - porusza każdym z silników krokowych platformy jeżdżącej w zadanym kierunku
void move_platform(int s1, int s2, int s3, int s4)
{
  digitalWrite(greenLed_pin, LOW);
  digitalWrite(yellowLed_pin, HIGH);
  while (1)
  {
    stepper1.setSpeed(s1*velocity);
    stepper2.setSpeed(s2*velocity);
    stepper3.setSpeed(s3*velocity);
    stepper4.setSpeed(s4*velocity);
    stepper1.runSpeed();
    stepper2.runSpeed();
    stepper3.runSpeed();
    stepper4.runSpeed();
    if (SerialBT.available())
    {
      break;
    }
  }
}

void move_platform_joystick()
{
  Serial.println("inside");
  digitalWrite(greenLed_pin, LOW);
  digitalWrite(yellowLed_pin, HIGH);
  //wysyłany jest kąt w stopniach więc należy zamienić go na radiany
  joystick_angle = atof(par1)*2*pi/360;  
  //aplituda wysyłana jest w procentach więc trzeba podzielić ją przez 100
  joystick_strength = atof(par2)/100;  
  s1_speed = cos(joystick_angle+pi/4)*joystick_strength*vel_par;
  s2_speed = sin(joystick_angle+pi/4)*joystick_strength*vel_par;
  joystick_ping = 0;
  while (1)
  {
    stepper1.setSpeed(s1_speed*velocity);
    stepper2.setSpeed(s2_speed*velocity);
    stepper3.setSpeed(-s2_speed*velocity);
    stepper4.setSpeed(-s1_speed*velocity);
    stepper1.runSpeed();
    stepper2.runSpeed();
    stepper3.runSpeed();
    stepper4.runSpeed();
    joystick_ping++;
    Serial.println(joystick_ping);
    if (joystick_ping > 1000) {break;}
    if (SerialBT.available())
    {
      clear_frame();
      read_frame();
      if (strcmp(command, "joystick") == 0)
      {
        joystick_angle = atof(par1)*2*pi/360;
        joystick_strength = atof(par2)/100;
        s1_speed = cos(joystick_angle+pi/4)*joystick_strength*vel_par;
        s2_speed = sin(joystick_angle+pi/4)*joystick_strength*vel_par;
        joystick_ping = 0;
        Serial.println("update");
      }
      else
      {
        Serial.println("break");
        break;
      }
    } 
  }
}

// home_arm - ustawienie serwonapędów w pozycji startowej (neutralnej)
void home_arm()
{
  Serial.println("Command recived: home");
  digitalWrite(greenLed_pin, LOW);
  digitalWrite(yellowLed_pin, HIGH);
  servo1.attach(czlon1_pin, minUs, maxUs);
  servo2.attach(czlon2_pin, minUs, maxUs);
  servo3.attach(chwytak_pin, minUs, maxUs);
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  s_pos[0] = 90;
  s_pos[1] = 90;
  s_pos[2] = 90;
  homed = true;
}

// move_link - porusza jednym z serwomechanizmów ramienia robota w kierunku dodatnim lub ujemnym
void move_link(int link_number, int dir, int lower_limit, int upper_limit)
{
  if (dir == 1)
  {
    while((s_pos[link_number] < upper_limit) && homed)
    {
      digitalWrite(greenLed_pin, LOW);
      digitalWrite(yellowLed_pin, HIGH);
      if (link_number == 0)
      {
        servo1.write(s_pos[0]+1);
        s_pos[0] = s_pos[0] + 1;
      }
      else if (link_number == 1)
      {
        servo2.write(s_pos[1]+1);
        s_pos[1] = s_pos[1] + 1;
      }
      else if (link_number == 2)
      {
        servo3.write(s_pos[2]+1);
        s_pos[2] = s_pos[2] + 1;
      }
      delay(25);
      if (SerialBT.available())
      {
        break;
      }
    }
  }
  else if (dir == -1)
  {
    while((s_pos[link_number] > lower_limit) && homed)
    {
      digitalWrite(greenLed_pin, LOW);
      digitalWrite(yellowLed_pin, HIGH);
      if (link_number == 0)
      {
        servo1.write(s_pos[0]-1);
        s_pos[0] = s_pos[0] - 1;
      }
      else if (link_number == 1)
      {
        servo2.write(s_pos[1]-1);
        s_pos[1] = s_pos[1] - 1;
      }
      else if (link_number == 2)
      {
        servo3.write(s_pos[2]-1);
        s_pos[2] = s_pos[2] - 1;
      }
      delay(25);
      if (SerialBT.available())
      {
        break;
      }
    }
  }  
}

// move_arm - ustawia wszystkie trzy serwomechanizmy ramienia robota na konkretne położenia (kąty)
void move_arm()
{
  Serial.println("Command recived: arm");
  s_target[0] = atoi(par1);
  s_target[1] = atoi(par2);
  s_target[2] = atoi(par3);
  if(s_target[0] >= 60 && s_target[0] <= 180 && s_target[1] >= 0 && s_target[1] <= 110 && s_target[2] >= 58 && s_target[2] <= 150)
  {
    while (s_pos[0] != s_target[0] || s_pos[1] != s_target[1] || s_pos[2] != s_target[2])
    {
      digitalWrite(greenLed_pin, LOW);
      digitalWrite(yellowLed_pin, HIGH);
      
      // Ruch członu 1
      if (s_target[0] > s_pos[0])
      {
        servo1.write(s_pos[0]+1);
        s_pos[0] = s_pos[0] + 1;
      }
      if (s_target[0] < s_pos[0])
      {
        servo1.write(s_pos[0]-1);
        s_pos[0] = s_pos[0] - 1;
      }

      // Ruch członu 2
      if (s_target[1] > s_pos[1])
      {
        servo2.write(s_pos[1]+1);
        s_pos[1] = s_pos[1] + 1;
      }
      if (s_target[1] < s_pos[1])
      {
        servo2.write(s_pos[1]-1);
        s_pos[1] = s_pos[1] - 1;
      }

      // Ruch chwytaka
      if (s_target[2] > s_pos[2])
      {
        servo3.write(s_pos[2]+1);
        s_pos[2] = s_pos[2] + 1;
      }
      if (s_target[2] < s_pos[2])
      {
        servo3.write(s_pos[2]-1);
        s_pos[2] = s_pos[2] - 1;
      }
      delay(18);
    }
  }
  else
  {
    Serial.println("Target position is out of limits");
  }
}

// read_frame - analizuje otrzymaną ramkę i rozdziela ją na podstawie przecinków na poszczególne części: komendę oraz trzy parametry
void read_frame()
{
  // Odczytanie przysłanych informacji z portu szeregowego bluetooth
  while (SerialBT.available())
  {
    frame[i] = SerialBT.read();
    // Usuwamy znak 'Carriage return'(13) oraz 'Line feed'(10), aby móc porównywać tylko otrzymane litery polecenia
    if (frame[i] == 10 or frame[i] == 13)
    {
      frame[i] = 0;
    }
    i++;
  }
  // Nowa wiadomość została wykryta
  if (frame[0] > 0)
  {
      for (i = 0; int(frame[i]) != 44 && i < 24; i++) //dopóki znak nie jest przecinkiem. i < 24 a nie 25 dlatego, że w ostatniej pętli gdy przekroczymy indeks to próbuje sprawdzić frame[i] i jest błąd
    {
      command[i] = frame[i];
    }
    i++;
    j = 0;
  
    for (i; int(frame[i]) != 44 && i < 24; i++)
    {
      par1[j] = frame[i];
      j++;
    }
    i++;
    j = 0;
  
    for (i; int(frame[i]) != 44 && i < 24; i++)
    {
      par2[j] = frame[i];
      j++;
    }
    i++;
    j = 0;
  
    for (i; int(frame[i]) != 44 && i < 24; i++)
    {
      par3[j] = frame[i];
      j++;
    }
  }
  //Wyświetlenie otrzymanej ramki
  Serial.print("Recived frame: ");
  Serial.println(frame);
  Serial.print("Command: ");
  Serial.print(command);
  Serial.print("      Par1: ");
  Serial.print(par1);
  Serial.print("      Par2: ");
  Serial.print(par2);
  Serial.print("      Par3: ");
  Serial.println(par3);
}

// clear_frame - wyczyszczenie odczytanej ramki, wykonanej komendy i jej parametrów
void clear_frame()
{
  i = 0;
  for (int j = 0; j < sizeof(frame); j++)
  {
    frame[j] = (char)0;
    command[j] = (char)0;
    par1[j] = (char)0;
    par2[j] = (char)0;
    par3[j] = (char)0;
  }
}
