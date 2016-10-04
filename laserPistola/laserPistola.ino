

int gumbPin = A7;
int laserPin = A5;
int resetGumbPin = A6;

//nastavitve
int gumbDolTime = 50;
int casIztrelka = 150;
int stNabojev = 0;

//  4     5   gnd    6    7
//  |_____|____|_____|____|
//  |                     |
//  |     ----------      |
//  |  :       6      :   |
//  |  :              :   |
//  |  : 5          7 :   |
//  |  :              :   |
//  |  :       4      :   |
//  |     ----------      |
//  |  :              :   |
//  |  :              :   |
//  |  : 3          1 :   |
//  |  :              :   |
//  |  :       2      :   |
//  |     ----------      |
//  |_____________________|
//  |     |    |     |    |
//  3     2   gnd    1    0


int cifre[] = {
//  01234567
   B01110111,	//"0": 1, 2, 3, 5, 6, 7
   B01000001,	//"1": 1, 7
   B00111011,	//"2": 2, 3, 4, 6, 7
   B01101011,	//"3": 1, 2, 4, 6, 7
   B01001101,	//"4": 1, 4, 5, 7
   B01101110,	//"5": 1, 2, 4, 5, 6
   B01111110,	//"6": 1, 2, 3, 4, 5, 6
   B01000011,	//"7": 1, 6, 7
   B01111111,	//"8": 1, 2, 3, 4, 5, 6, 7
   B01101111,	//"9": 1, 2, 4, 5, 6, 7
};

//kateri pini so na katero nogico zvezani: npr. pin 2 je zvezan na 0, 3 na 1, 4 na 2, 10 na 0 od drugega displaya...
int pini[][8] = {
  //  0   1   2   3   4   5   6   7 --> nogce na ekranu
    { 2,  3,  4,  5,  6,  7,  8,  9},  //led display 1
    {10, 11, 12, A0, A1, A2, A3, A4}, //led display 2
}; 

int stPinov = 8;


int trenutnoStNabojev = stNabojev;
int prizgan = 1;
long zadnjaSprememba = 0;
long zadnjiReset = 0;
int gumbStanje = 0;
int gumbRstStanje = 0;
int laserStanje = 0;
long laserTime = 0;

// the setup routine runs once when you press reset:
void setup() {
 
  // initialize the digital pins as an output.
  int i;
  for(i=0; i<stPinov; i++){
    pinMode(pini[0][i], OUTPUT);
    digitalWrite(pini[0][i], LOW);
    
    pinMode(pini[1][i], OUTPUT);
    digitalWrite(pini[1][i], LOW);
  }
  pinMode(gumbPin, INPUT);
  digitalWrite(gumbPin, LOW);
  
  pinMode(resetGumbPin, INPUT);
  digitalWrite(resetGumbPin, LOW);
  
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
  

  
  //Serial.begin(9600);
}

void loop() {
  
  //pogleda ali je bil pritisnjen "sprozilec"
  if(analogRead(gumbPin) > 600){
    if(millis() - zadnjaSprememba > gumbDolTime && gumbStanje == 0){
      zadnjaSprememba = millis() - 1000;
        //if(trenutnoStNabojev > 0){
          digitalWrite(laserPin, HIGH);
          laserStanje = 1;
          laserTime = millis();
          trenutnoStNabojev ++;
        //}
      }
      gumbStanje = 1;
  }else{
    gumbStanje = 0;
  }

  //pogleda ali je lasar prizgan in ce mi je ze potekel cas
  if(laserStanje == 1){
    if(millis() - laserTime > casIztrelka){
      digitalWrite(laserPin, LOW);
      laserStanje = 0;
    }
  }
  
  //gleda ali je bil pritisnjen gumb reset
  if(analogRead(resetGumbPin) > 600){
    if(millis() - zadnjiReset > 50 && gumbRstStanje == 0){
      zadnjiReset = millis();
      trenutnoStNabojev  = stNabojev;
      laserStanje = 0;
    }
    gumbRstStanje = 1;
  }else{
    gumbRstStanje = 0;
  }
  
  
  //priziga in ugasa display za ohranjanje energije
  pokaziStevilo(trenutnoStNabojev);
  delay(1);
  ugasniVse();
  delay(10);
  
}
void ugasniVse(){
  int i;
  for(i=0; i<stPinov; i++){
    digitalWrite(pini[0][i], LOW);
    digitalWrite(pini[1][i], LOW);
  }
}
void pokaziStevilo(uint8_t stevilo){
  //if(stevilo > 99) return;
  stevilo = stevilo % 100;
  int i, maska;
  
  int enice = stevilo % 10;
  int desetice = stevilo / 10;
  
  maska = 1 << 7; // 128
  for(i=0; i < 8; i ++){
    //izpise enice
    if((maska & cifre[enice]) != 0)digitalWrite(pini[0][i], HIGH);
    else digitalWrite(pini[0][i], LOW);
    
    //izpise desetice
    if((maska & cifre[desetice]) != 0)digitalWrite(pini[1][i], HIGH);
    else digitalWrite(pini[1][i], LOW);
    
    maska = maska >> 1;
  }
  
  
}



