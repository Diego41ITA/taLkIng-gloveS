#include <Adafruit_BMP280.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <SPI.h>

Adafruit_BMP280 bmp;
MPU6050 mpu6050(Wire);

#define SIZE 5
#define greenled 2
#define redled 3
#define bluled 4

// definisco variabili globali
int k = 0;
int VAL[SIZE];
int POS[SIZE];
int PIN[SIZE] = {A0 , A1, A2, A3, A4};
float altezza;

float Xrilevato, Yrilevato, Zrilevato;
int Xmappato, Ymappato, Zmappato;

//definisco struttura su cui scrivere i dati rilevati
struct datir {
  int VAL[SIZE]; //valore rilevato dal flessiometro
  int POS[SIZE]; //valore trasformato dalla funzione map
  float altezza;
  int angoloX;
  int angoloY;
  int angoloZ;
};
struct datir datirilevati;

//struttura delle parole salvate
struct parolesalvate {
  int POS[SIZE];
};
// vettore di struct delle parole salvate--> valori campionati
struct parolesalvate parole[10] = {{{3, 4, 4, 3, 3}},
  {{3, 4, 4, 3, 3}},
  {{2, 3, 3, 3, 1}},
  {{3, 4, 4, 3, 2}},
  {{3, 4, 4, 3, 3}},
  {{1, 2, 4, 3, 3}},
  {{1, 2, 4, 3, 2}},
  {{1, 2, 3, 3, 3}},
  {{1, 2, 4, 3, 4}},
  {{3, 4, 4, 3, 3}}
};

//funzione che rileva i dati dal guanto-->
void rilevadati() {
  for (byte i = 0; i < SIZE; i++) {
    datirilevati.VAL[i] = analogRead(PIN[i]);
  }

  for (byte i = 0; i < SIZE; i++) {
    datirilevati.POS[i] = map(datirilevati.VAL[i] , 360 , 590, 1 , 5);
  }
  datirilevati.altezza = bmp.readAltitude(1013.25);
  //Serial.print(datirilevati.altezza);
}
//converte da decimale a binario e invia ad arduino 1
void trasmetti(int n) {
  int i = 0;
  while (n > 0) {
    if (n % 2 == 0) {
      digitalWrite(i, LOW);
      Serial.print("0");
    } else {
      digitalWrite(i, HIGH);
      Serial.print("1");
    }
    n = n / 2;
    i++;
  }
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5 , LOW);
}
//confronta i dati della mano con quelli del database

void confrontadati() {
  bool check = true; //check delle parole
  bool finito = false; // check della fine
  //Serial.print("inizio confronto \n");
  for (int i = 0; i < 10  && finito == false ; i++) { // giro le parole
    check = true;  // imposto il  check a true per ogni parola
    //Serial.print("for esterno \n");
    for (int j = 0; j < SIZE && check == true; j++) { // giro le dita e confronto i valori per la i-esima parola
      /*Serial.print("for interno \n");
        Serial.print(datirilevati.POS[j]);
        Serial.print("\n");
        Serial.print(parole[i].POS[j]);
        Serial.print("\n");*/
      if (datirilevati.POS[j] != parole[i].POS[j]) {  // se anche solo un dito non corrisponde metto il check a false
        check = false;
      }
    }
    if (check == true && i < 2) {                  //se check==true allora ho corrispondenza per tutti i flex, la condizione su i serve per discriminare l'altezza
      
      if (datirilevati.altezza > altezza) {
        trasmetti(0); // buongiorno --> alto
        finito = true;
      }
      else {
        trasmetti(1);
        finito = true; // buonasera--> basso
      }
      }
    if ( check == true && i>1) { //se chedck ==true e sonoo dopo la seconda parola allora trasmetto k che va di pari passo a i
      trasmetti(k);
      Serial.print(k);
      Serial.print("\n");
      finito = true;
    }
    k++;
  }
  k = 0;
}

void printdati() {
  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m \n");

  mpu6050.update();
  Xrilevato = mpu6050.getAngleX();
  Xmappato = map(Xrilevato, 0 , 360, 0, 80);
  Yrilevato = mpu6050.getAngleY();
  Ymappato = map(Yrilevato, 0 , 360, 0, 80);
  Zrilevato = mpu6050.getAngleZ();
  Zmappato = map(Zrilevato, 0 , 360, 0, 80);

  Serial.print("angleX : ");
  Serial.print(Xmappato);
  Serial.print("\tangleY : ");
  Serial.print(Ymappato);
  Serial.print("\tangleZ : ");
  Serial.println(Zmappato);

  Serial.println("\n");

  for (byte i = 0; i < SIZE; i++) {
    VAL[i] = analogRead(PIN[i]);
  }
  Serial.print("VAL1: " + String(VAL[0]) + "\t VAL2: " + String(VAL[1]) + "\t VAL3: " + String(VAL[2]) + "\t VAL4: " + String(VAL[3]) + "\t VAL5: " + String(VAL[4]) + "\n");

  for (byte i = 0; i < SIZE; i++) {
    POS[i] = map(VAL[i] , 360 , 590, 1 , 5);
  }
  Serial.print("POS1: " + String(POS[0]) + "\t POS2: " + String(POS[1]) + "\t POS3: " + String(POS[2]) + "\t POS4: " + String(POS[3]) + "\t POS5: " + String(POS[4]) + "\n");
  delay(3000);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  Serial.println("BMP280 test");

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring or "
                   "try a different address!");
    delay(1000);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  printdati();

  Serial.print("RILEVAMENTO ALTEZZA\n");
  trasmetti(14);
    /*delay(500);
    for (int i = 0; i < 5; i++) {
    digitalWrite(i, LOW);
    }
    delay(2000);
  */
  delay(3000);
  altezza = bmp.readAltitude(1013.25);
  Serial.print(altezza);
  Serial.print("\n \n");
  Serial.print("Inizio movimento\n");
   trasmetti(15);
    /*delay(500);
    for (int i = 0; i < 5; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
    }*/
    delay(1000);
}

void loop() {
   for (int i = 0; i < 6; i++) {
    digitalWrite(i, LOW);
  }
  digitalWrite(8,LOW);
  rilevadati();
  confrontadati();
  printdati();

  delay(3000);

}
