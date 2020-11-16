#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 

/* Pines de Movimiento */
const int X_Pin = A1;
const int Y_Pin = A2;

const int JoyButton = 0;
const int ledRojo = 2;
const int ledVerde = 3;
const int pBuzzer = 4;
/* Tiempo para actualizar la pantalla */
int time = 400;
byte ghostNumber = 6;
int counterA = 0;
int counterB = 0;
unsigned int previousMillis = 0;

byte X = 0;
byte Y = 0;

byte Ghost[6];

byte UnknownGhost01[8] {
  B01110,
  B11111,
  B10101,
  B10101,
  B11111,
  B10001,
  B11111,
  B00000
};

byte Pacman01[8] {
  B00111,
  B11110,
  B10100,
  B11000,
  B11000,
  B11100,
  B01110,
  B00111
};

byte Pacman02[8] {
  B00000,
  B01111,
  B10111,
  B11000,
  B11111,
  B11111,
  B01111,
  B00000
};

/* ------- Definición de los tiempos para el buzzer ------ */
int blanca = 1000 / 2;
int negra = 1000 / 4;
int negra_plus = 1000 / 6;
int corchea = 1000 / 8;
double pausa = 1.3;

/* --------- Notas clave de Sol --------*/
int do3 = 261;
int re3 = 293;
int mi3 = 329;
int fa3 = 349;
int sol3 = 392;
int la3 = 440;
int si3 = 496;

int do4 = 523;
int re4 = 587;
int mi4 = 659;
int fa4 = 698;
int sol4 = 784;
int la4 = 880;
int si4 = 987;
int do5 = 1046;

/* ---- Tonos para el Buzzer 1-2 ---- */
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;
/*-------------------------------------*/
void (*resetFunc)(void) = 0; // Declara la función resetear en la dirección 0

void setup(){
  int set = 14;
  for (int i = 0; i < ghostNumber; i++)
    Ghost[i] = set+=random(2,4); 
  
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(pBuzzer, OUTPUT);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, LOW);
  analogWrite(pBuzzer, 0);
  randomSeed(analogRead(A0));
  lcd.createChar(1, UnknownGhost01);
  lcd.createChar(2, Pacman01);
  lcd.createChar(3, Pacman02);
  lcd.setCursor(0, 0);
  moverEnemigos(X, Y);
  lcd.clear();
}

void loop(){
  checkPosition(X, Y);
  int currentMillis = millis();
  if (currentMillis - previousMillis>=time){
    previousMillis = currentMillis;
    lcd.clear();
    moverPersonaje();
    moverEnemigos(X, Y);
 }
}

void moverEnemigos(int X, int Y){
 lcd.setCursor(X, Y);
 if ((counterA%2)==0) lcd.write((byte)2);
  else lcd.write((byte)3);
 
 for (int i = 0; i < ghostNumber; i++){
   Ghost[i] = Ghost[i] - 1;
   if ((i%2)==0){
     lcd.setCursor(Ghost[i], 0);
     lcd.write((byte)1);
     } else {
       lcd.setCursor(Ghost[i], 1);
       lcd.write((byte)1);
     }
     if (Ghost[i] <= 0) Ghost[i] = 16;
   }
   counterA++;
}

int moverPersonaje(){   
  int X_Axis = analogRead(X_Pin);
  int Y_Axis = analogRead(Y_Pin);
   // Arriba
  if (Y_Axis >= 800 && Y == 1) {
    Y--;
    Serial.println("Arriba");
  }
  
  // Abajo
  if (Y_Axis <= 400 && Y == 0) {
    Y++;
    Serial.println("Abajo");
  }

  // Izquierda
  if (X_Axis <= 400 && X > 0) {
    X--;
    Serial.println("Izquierda");
  }

  // Derecha
  if (X_Axis >= 800 && X >= 0 && X < 15) {
    X++;
    Serial.println("Derecha");
  }

  return X, Y;
}

void Win(){
  while(true){
    lcd.clear();
    lcd.print("Ganaste! ;3");
    youWinTune();

    delay(650);
    for (int i = 3; i >= 0; i--){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press to reset...");
      lcd.setCursor(0,1);
      lcd.print(i);
      if (digitalRead(JoyButton) == LOW) resetFunc();
      delay(1000);
    }
  }
}

void Lose(){
  while(true){
    lcd.clear();
    lcd.print("Perdiste! >:3");
    gameOverTune();
    for (int i = 3; i >= 0; i--){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press to reset...");
      lcd.setCursor(0,1);
      lcd.print(i);
      if (digitalRead(JoyButton) == LOW) resetFunc();
      delay(1000);
    }
  }
}

void checkPosition(int X, int Y){
  // Perder
  for (int i = 0; i < ghostNumber; i++){
    if ((i%2)==0){
      if (Ghost[i] == X && Y==0) Lose();
    } else 
      if (Ghost[i] == X && Y==1) Lose();
    }
     
  // Ganar
  if (X == 15) Win();
}

void beep(int note, int duration){
  tone(pBuzzer, note, duration);

  // Prende diferente LED dependiendo del valor del contador
  if (counterB % 2 == 0){
    digitalWrite(ledVerde, HIGH);
    delay(duration);
    digitalWrite(ledVerde, LOW);
    } else{
      digitalWrite(ledRojo, HIGH);
      delay(duration);
      digitalWrite(ledRojo, LOW);
    }

    noTone(pBuzzer);

    delay(50);

    counterB++;
}

void twinkleTwinkle(){
  beep(do4, negra);
  beep(do4, negra);
  beep(sol4, negra);
  beep(sol4, negra);
  beep(la4, negra);
  beep(la4, negra);
  beep(sol4, blanca);
  beep(fa4, negra);
  beep(fa4, negra);
  beep(mi4, negra);
  beep(mi4, negra);
  beep(re4, negra);
  beep(re4, negra);
  beep(do4, blanca);
}

void gameOverTune(){
    beep(a, 500);
    beep(a, 500);
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);

    delay(500);

    beep(eH, 500);
    beep(eH, 500);
    beep(eH, 500);
    beep(fH, 350);
    beep(cH, 150);
    beep(gS, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);

    delay(500);
}

void youWinTune(){
/* ---- ROLA DE EVANGELION ---- */
  beep(mi3, blanca);
  beep(sol3, blanca);
  beep(la3, blanca);
  beep(sol3, negra);
  beep(la3, negra);
  beep(la3, negra);
  beep(la3, negra);
  beep(re4, negra);
  beep(do4, negra);
  beep(si3, negra_plus);
  beep(la3, negra);
  beep(si3, corchea);
  noTone(pBuzzer);
}