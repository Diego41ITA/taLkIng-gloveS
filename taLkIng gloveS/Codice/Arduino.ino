#include <SD.h>                 //libreria scheda SD
#include <TMRpcm.h>             //libreria lettura file audio 
#include <string.h>
#include <math.h>               //libreria potenze 
#define SD_ChipSelectPin 10    // pin n da collegare al terminale CS della scheda SD   

TMRpcm audio;                 // crea l'oggetto della libreria TMRpcm per riprodurre l'audio

int p;
int V[5] = {0, 0, 0, 0, 0};

float pos[] = { //Array con i secondi corrispondenti alle parole
  1.052828,
  2.682206,
  4.336651,
  5.715355,
  7.144193,
  8.472762,
  9.826399,
  11.180036,
  12.282999,
  13.611568,
  15.015339,
  16.444178,
  18.775441,
  21.106704,
  23.287563,
  25.017210,
  27.097800
};

char c [] = "FRASIAV";

void riproduci_par(int p)
{
  unsigned long d1 = (pos[p + 1] - pos[p]) * 1000; // durata parola rilevata
  audio.play(c, pos[p]);                           // riproduce la parola rilevata
  delay(d1);                                       // durata riproduziond
  audio.stopPlayback();                            // stop riproduzione
}


void setup() {
  // messaggi relativi alla scheda SD inviati sulla seriale
  Serial.begin(9600);
  for (int i = 2; i < 7; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  pinMode(8, INPUT);    //prima era 8
  digitalWrite(8, LOW); // prima del cambio era 8

  audio.speakerPin = 9; // pin uscita audio
  audio.quality(1);

  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.print("Errore lettura scheda SD!");
    return;
  }
  else {
    Serial.print("Scheda SD riconosciuta... \n");
  }
}
// conversione numero decimale a numero binario
void converti () {
  int j = 0;
  for (int i = 2; i < 7; i++) {
    if (digitalRead(i) == HIGH) {
      V[4 - j] = 1;
    }
    j++;
  }
  //conversione
  p = V[0] * pow(2, 4) + V[1] * pow(2, 3) + V[2] * pow(2, 2) + V[3] * pow(2, 1) + V[4];
  Serial.print(p);
}

void loop() {
  if (digitalRead(8) == HIGH) {
    converti();
    riproduci_par(p);
    p = 0;
  }
  delay(500);
}
