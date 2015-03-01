//v 1.08 A.B.
#include <Servo.h> 
#include <LCD4Bit_mod.h> 
#include <TarceUNO.h>
#include <EEPROM.h>   //branje in pisanje v spomin

#define OMOGOCI_SPOMIN 0

//osnovne spremenljivke
LCD4Bit_mod lcd = LCD4Bit_mod(2);
int gumbi[5];
uint8_t state;
uint16_t vsiEkrani[MAX_EKRANOV];
uint16_t spomin[MAX_SPOMIN];
s_tarca tarca[3];
s_parameter parametri[MAX_SPOMIN];//opis vsake spremenljivke ki je v spominu. Nafila se v funkciji getSpomin()
int vSpominu[ST_SPOMINA];//dejanske vrednosti indeksov v spominu
char izpis[16] = {ZNAK_LEVO,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',ZNAK_DESNO};

//definicija vseh ekranov
s_ekran mainMenu = //MENI_GLAVNI
{
  "Glavni meni",
  //tipke: 
  //gor dol levo, desno, select
  0,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  3,0,
  "igraj !",      MENI_IGRAJ,
  "nastavitve",   MENI_NASTAVITVE,
  "info",         MENI_INFO,
};

s_ekran infoMenu = //MENI_INFO
{
  "Verzija 1.08",
  //gor dol levo, desno, select
  MENI_GLAVNI, STEJ_PRITISKE, STEJ_PRITISKE, STEJ_PRITISKE, 0,
  1, 0,
  "1.mar.2015",       0,
};

s_ekran madeByMenu = //MENI_MADE_BY
{
  "SPROGRAMIRAL",
  //gor dol levo, desno, select
  MENI_INFO, 0, 0, 0, 0,
  1, 0,
  "Anze  Bertoncelj",       0,
};

s_ekran nastavitveMenu = //MENI_NASTAVITVE
{
  "Nastavitve",
  //gor dol levo, desno, select
  MENI_GLAVNI,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  4,0,
  "motor 1",        NAST_MOTOR1,
  "motor 2",        NAST_MOTOR2,
  "motor 3",        NAST_MOTOR3,
  "sensorji",       NAST_SENZORJI,
};



s_ekranNastavitve motor1Nast =  //NAST_MOTOR1
{
  "Nast. motor1",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  MEM_MOTOR_1_MIN,
  MEM_MOTOR_1_MAX,
};

s_ekranNastavitve motor2Nast =  //NAST_MOTOR2
{
  "Nast. motor2",
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  MEM_MOTOR_2_MIN,
  MEM_MOTOR_2_MAX,
};

s_ekranNastavitve motor3Nast =  //NAST_MOTOR3
{
  "Nast. motor3",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  MEM_MOTOR_3_MIN,
  MEM_MOTOR_3_MAX,
};

s_ekranNastavitve senzorjiNast =  //NAST_SENZORJI
{
  "Nast. senzo",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis,    min   max,  def,  korak
  MEM_SENSOR,

};

s_ekranNastavitveEdit editNast =  //NAST_EDIT
{
  0,
  //gor dol levo, desno, select
  NAST_EDIT_FINISH,0,NAST_EDIT_DEC,NAST_EDIT_INC, 0,
  //return state
  0
};

s_ekran igrajMenu = //MENI_IGRAJ
{
  "Izberi program:",
  //gor dol levo, desno, select
  MENI_GLAVNI,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  3,0,
  "vse prizgane",        MENI_PROG1,
  "nakljucno",        MENI_PROG2,
  "po vrsti",        MENI_PROG3
};

s_ekran prog1Menu = //MENI_PROG1
{
  "Vse prizgane:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_1,
  "nastavitve",        	 NAST_PROG1,
};

s_ekranNastavitve prog1Nast =  //NAST_PROG1
{
  "nastavitve",
  //gor dol levo, desno, select
  MENI_PROG1,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  1,0,OFF,
  MEM_PROG1_CAS,
};

s_ekran prog2Menu = //MENI_PROG2
{
  "Prog. nakljucno:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_2,
  "nastavitve",        	 NAST_PROG2,
};

s_ekranNastavitve prog2Nast =  //NAST_PROG2
{
  "nastavitve",
  //gor dol levo, desno, select
  MENI_PROG2,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  MEM_PROG2_CAS,
  MEM_PROG2_INTERVAL,
};

s_ekran prog3Menu = //MENI_PROG3
{
  "Prog. po vrsti:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_3,
  "nastavitve",        	 NAST_PROG3,
};

s_ekranNastavitve prog3Nast =  //NAST_PROG3
{
  "nastavitve",
  //gor dol levo, desno, select
  MENI_PROG3,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  MEM_PROG3_CAS,
  MEM_PROG3_INTERVAL,
};


s_ekranOdstevaj odstavajOkno =        // OSDSTEVAJ
{
  "Igra se zacne v",
  //gor dol levo, desno, select
  //tuki se poj notr shran tipke ki jih lahko uporabnik
  //uporablja med odstevanjem
  0, 0, 0, 0, 0,
  
};

// def vrednosti odstevalnika
s_odstevajInit odstevalnik = {
  //state, millis, stOdstevanj, returnState
  0, 0, 5, 255
};


s_ekranProgram program1 =
{
  0,
  //gor dol levo, desno, select
  PROG1_KONEC,0,0,0,0,
};

s_ekranProgram program2 =
{
  0,
  //gor dol levo, desno, select
  PROG2_KONEC,0,0,0,0,
};

s_ekranProgram program3 =
{
  0,
  //gor dol levo, desno, select
  PROG3_KONEC,0,0,0,0,
};




void setup() 
{ 
  //Serial.begin(9600); //serijska more bit sklopljena ce hocemo da dela zvok 
  Serial.println("Connection OK !");
  InitialiseIO();        // inicalizacija portov input/output
  InitialiseInterrupt(); // interupt za gumbe
  timer2Init();          // inicalizacije timerja (za piskace in stevec ...)
  lcdInit();             // lcd init
  getSpomin();           // inicializira spomin in shranjene prebere vrednosti
  state = MENI_GLAVNI;
  
  initEkrani();
  
  
} 

void initEkrani(){
  vsiEkrani[MENI_GLAVNI] =       (uint16_t)(&mainMenu);
  vsiEkrani[MENI_IGRAJ] =        (uint16_t)(&igrajMenu);
  vsiEkrani[MENI_NASTAVITVE] =   (uint16_t)(&nastavitveMenu);
  vsiEkrani[MENI_INFO]  =        (uint16_t)(&infoMenu);
  vsiEkrani[MENI_MADE_BY] =      (uint16_t)(&madeByMenu);
  
  vsiEkrani[ODSTEVAJ] =          (uint16_t)(&odstavajOkno);
  
  vsiEkrani[MENI_PROG1] =   	 (uint16_t)(&prog1Menu);
  vsiEkrani[MENI_PROG2] =   	 (uint16_t)(&prog2Menu);
  vsiEkrani[MENI_PROG3] =   	 (uint16_t)(&prog3Menu);
 
  vsiEkrani[NAST_MOTOR1] =       (uint16_t)(&motor1Nast);
  vsiEkrani[NAST_MOTOR2] =       (uint16_t)(&motor2Nast);
  vsiEkrani[NAST_MOTOR3] =       (uint16_t)(&motor3Nast);
  vsiEkrani[NAST_SENZORJI] =     (uint16_t)(&senzorjiNast);
  vsiEkrani[NAST_EDIT] =         (uint16_t)(&editNast);
  
  vsiEkrani[PROGRAM_1] =   	 (uint16_t)(&program1); 
  vsiEkrani[PROGRAM_2] =   	 (uint16_t)(&program2);
  vsiEkrani[PROGRAM_3] =   	 (uint16_t)(&program3);
  
  vsiEkrani[NAST_PROG1] =   	 (uint16_t)(&prog1Nast);
  vsiEkrani[NAST_PROG2] =   	 (uint16_t)(&prog2Nast);
  vsiEkrani[NAST_PROG3] =   	 (uint16_t)(&prog3Nast);
}

int8_t programState = -1;
uint8_t stateReturn = 0;
uint8_t ponavljaj = 0;

char scoreCel[17] = "  1:0     2:0   ";
int i;
int j;
//char score[3];
char casAray[10];


//zato da se zna vrniti nazaj na prejsni state v primeru da trenutni ne obstaja
int prevState;
void loop() {

   if(ponavljaj == 0 || state != prevState) sPrint("state: ", state);
   switch(state){
      case MENI_GLAVNI:
      case MENI_IGRAJ:
      case MENI_NASTAVITVE:
      case MENI_INFO:
      case MENI_MADE_BY:
      case MENI_PROG1:
      case MENI_PROG2:
      case MENI_PROG3:
        drawScreen();
        ponavljaj = 0;
      break;
      
      case NAST_EDIT:
        drawScreenEdit();
        ponavljaj = 0;
      break;
      
      case NAST_MOTOR1:
      case NAST_MOTOR2: 
      case NAST_MOTOR3:
      case NAST_SENZORJI:
      case NAST_PROG1:
      case NAST_PROG2:
      case NAST_PROG3:
        drawScreenNast();
        sPrint("drawScreenNast ", state);
      break;
      
      case PROGRAM_1:
        programFSM1();
      break;
      
      case PROGRAM_2:
        programFSM2();
      break;
      
      case PROGRAM_3:
        programFSM3();
      break;
      
      case ODSTEVAJ:
        odstevaj();
      break;
      
      default:
      //ce ne dobi nc pametnga se vrne na prvotno stanje
      Serial.println("def");
      state = prevState;
      break;
   }
  
  
  prevState = state;
  while(!preveriTipke() && !ponavljaj);
   
        
}

void confOdstevalnik(int retState){
    //skonfigurira odstevalnik
    odstevalnik.state = 1;
    odstevalnik.stOdstevanj = 5;
    odstevalnik.mills = 0;
    odstevalnik.returnState  = state;
    // nastavi vrednost tipke da ko uporabnik pritisne GOR bo su nazanj na meni
    ((s_ekranOdstevaj*)vsiEkrani[ODSTEVAJ])->tipke[btnUP] = retState; //((s_ekranProgram*)vsiEkrani[state])->tipke[btnUP];
    state = ODSTEVAJ;
    ponavljaj = 1;
}


int trajanje;
long zacetniCas;
void programFSM1(){
   char  izpisTocke[16];
  char  tockeSkupajArr[3];
  
  switch (programState) {
      case -1:
        //skonfigurira odstevalnik
        confOdstevalnik(MENI_PROG1);
        programState = 0;
        //odstevalnik premakne programState naprej na 1
        
        tarcaSkrij(0);
        tarcaSkrij(1);
        tarcaSkrij(2);
        break;
  
      case 1:
        programState = 2;
        
        //nikol se ne ugasne
        tarcaPokazi(0,-1);
        tarcaPokazi(1,-1);
        tarcaPokazi(2,-1);
        
        ponavljaj = 1;
        trajanje = spomin[MEM_PROG1_CAS]*10;// tuki se mer v dekasekundah
        
        lcd.clear();
        lcd.printIn("Preostali cas:",-1);
        lcd.cursorTo(2, 0);
        lcd.printIn("         s      ",16);
        zacetniCas = millis();
        
      case 2:
         if(trajanje < (millis()-zacetniCas)/100){
           programState = 3;
           break;
         }
           
         //izpisi cas
         lcd.cursorTo(2, 4);
         intToArr(&casAray[0],trajanje - (millis()-zacetniCas)/100,1,5,' ');
         lcd.printIn(casAray, -1);
        
     break;
  
      case 3:
         tarcaSkrij(0);
         tarcaSkrij(1);
         tarcaSkrij(2);
         piskacOn(3,ZVOK_MED);
         //state = KONCI_ZASLON;
         state = MENI_PROG1;
      break;
  }

}

int interval;
int izbrana;
void programFSM2(){
   char  izpisTocke[16];
  char  tockeSkupajArr[3];
  
  switch (programState) {
      case -1:
        //skonfigurira odstevalnik
        confOdstevalnik(MENI_PROG2);
        programState = 0;
        tarcaSkrij(0);
        tarcaSkrij(1);
        tarcaSkrij(2);
        //odstevalnik premakne programState naprej na 1
        break;
  
      case 1:
        programState = 2;
        interval = spomin[MEM_PROG2_INTERVAL]*10;
        
        izbrana = random(3);
        tarcaPokazi(izbrana, interval);
        
        ponavljaj = 1;
        trajanje = spomin[MEM_PROG2_CAS]*10;//mer v dekasekundah
        
        lcd.clear();
        lcd.printIn("Preostali cas:",-1);
        lcd.cursorTo(2, 0);
        lcd.printIn("         s      ",16);
        zacetniCas = millis();
        
      case 2:
         if(trajanje < (millis()-zacetniCas)/100){
           programState = 3;
           break;
         }
         //ce je izbrana tarca ze ugasnena izbere novo :)
         if(tarca[izbrana].stanje == OFF){
            izbrana = random(3);
            tarcaPokazi(izbrana, interval);
         }
           
         //izpisi cas
         lcd.cursorTo(2, 4);
         intToArr(&casAray[0],trajanje - (millis()-zacetniCas)/100,1,5,' ');
         lcd.printIn(casAray, -1);
        
     break;
  
      case 3:
         tarcaSkrij(0);
         tarcaSkrij(1);
         tarcaSkrij(2);
         piskacOn(3,ZVOK_MED);
         //state = KONCI_ZASLON;
         state = MENI_PROG2;
      break;
  }

}

void programFSM3(){
   char  izpisTocke[16];
  char  tockeSkupajArr[3];
  
  switch (programState) {
      case -1:
        //skonfigurira odstevalnik
        confOdstevalnik(MENI_PROG3);
        programState = 0;
        tarcaSkrij(0);
        tarcaSkrij(1);
        tarcaSkrij(2);
        //odstevalnik premakne programState naprej na 1
        break;
  
      case 1:
        programState = 2;
        interval = spomin[MEM_PROG3_INTERVAL]*10;
        
        izbrana = 0;
        tarcaPokazi(izbrana, interval);
        
        ponavljaj = 1;
        trajanje = spomin[MEM_PROG3_CAS]*10;//mer v dekasekundah
        
        lcd.clear();
        lcd.printIn("Preostali cas:",-1);
        lcd.cursorTo(2, 0);
        lcd.printIn("         s      ",16);
        zacetniCas = millis();
        
      case 2:
         if(trajanje < (millis()-zacetniCas)/100){
           programState = 3;
           break;
         }
         //ce je izbrana tarca ze ugasnena izbere novo :)
         if(tarca[izbrana].stanje == OFF){
            izbrana ++;
            izbrana = izbrana % 3;
            tarcaPokazi(izbrana, interval);
         }
           
         //izpisi cas
         lcd.cursorTo(2, 4);
         intToArr(&casAray[0],trajanje - (millis()-zacetniCas)/100,1,5,' ');
         lcd.printIn(casAray, -1);
        
     break;
  
      case 3:
         tarcaSkrij(0);
         tarcaSkrij(1);
         tarcaSkrij(2);
         piskacOn(3,ZVOK_MED);
         //state = KONCI_ZASLON;
         state = MENI_PROG3;
      break;
  }

}

void spominPisi(int index, int vrednost){
  if(OMOGOCI_SPOMIN){
    EEPROM.write(index, vrednost);
  }
}

int spominBeri(int index){
  if(OMOGOCI_SPOMIN){
    return EEPROM.read(index);
  }else{
    return -1;
  }
}
void getSpomin() {
    
  vSpominu[MEM_MOTOR_1_MIN] 	= INDEX_MOTOR_1_MIN;   
  vSpominu[MEM_MOTOR_1_MAX] 	= INDEX_MOTOR_1_MAX; 
  vSpominu[MEM_MOTOR_2_MIN]	= INDEX_MOTOR_2_MIN;  
  vSpominu[MEM_MOTOR_2_MAX] 	= INDEX_MOTOR_2_MAX;   
  vSpominu[MEM_MOTOR_3_MIN] 	= INDEX_MOTOR_3_MIN;   
  vSpominu[MEM_MOTOR_3_MAX] 	= INDEX_MOTOR_3_MAX;   
  vSpominu[MEM_SENSOR] 		= INDEX_SENSOR;    	
  vSpominu[MEM_PROG1_CAS] 	= INDEX_PROG1_CAS;  	
  vSpominu[MEM_PROG2_INTERVAL]  = INDEX_PROG2_INTERVAL;
  vSpominu[MEM_PROG2_CAS] 	= INDEX_PROG2_CAS;  	
  vSpominu[MEM_PROG3_INTERVAL]  = INDEX_PROG3_INTERVAL;
  vSpominu[MEM_PROG3_CAS] 	= INDEX_PROG3_CAS;  
  
  
  //deklaracija parametrov	opis,	        min,	max,	def,	korak
  parametri[MEM_MOTOR_1_MIN]=	{"zaprt:",	10,	180,	110,	5	};
  parametri[MEM_MOTOR_1_MAX]=   {"odprt:",	10,	180,	35,	5	};
  
  parametri[MEM_MOTOR_2_MIN]=	{"zaprt:",	10,	180,	110,	5	};
  parametri[MEM_MOTOR_2_MAX]=	{"odprt:",	10,	180,	35,	5	};
  
  parametri[MEM_MOTOR_3_MIN]=	{"zaprt:",	10,	180,	110,	5	};
  parametri[MEM_MOTOR_3_MAX]=	{"odprt:",	10,	180,	35,	5	};
  
  parametri[MEM_SENSOR]=	{"obcut.:",	1,	5,	3,	1	};
  
  parametri[MEM_PROG1_CAS]=	{"cas:",	10,	300,	30,	10	};
  
  parametri[MEM_PROG2_INTERVAL]={"interval:",	1,	30,	5,	1	};
  parametri[MEM_PROG2_CAS]=	{"cas:",	10,	300,	30,	10	};
  
  parametri[MEM_PROG3_INTERVAL]={"interval:",	1,	30,	5,	1	};
  parametri[MEM_PROG3_CAS]=	{"cas:",	10,	300,	30,	10	};
      
      
  int vrednost = spominBeri(MEM_PRVIC);
  Serial.println(vrednost);
  //pogleda ali je arduino ze biu prizgan
  if(vrednost != VARNOSTNA_ST){
    Serial.println("Prvic prizgan, pisem def vrednosti");
    spominPisi(MEM_PRVIC,212);
    //nafila ceu spomin iz EEPROM-a
    for (i = 0; i < ST_SPOMINA; i++) {
      Serial.print(i);
      Serial.print(": ");
      spomin[i] = parametri[i].vrednost;
      spominPisi(vSpominu[i], parametri[i].vrednost / parametri[i].korak);
      Serial.print("  ");
      Serial.print("nova:");
      spomin[i];
      Serial.println(spomin[i]);
    }
    
    return;
  }
  sPrint("vrednosti so ze shranjene "VARNOSTNA_ST_STR" == ", vrednost);
  //vrednost = spominBeri(MEM_PRVIC);
  //Serial.println(vrednost);
  
  //nafila ceu spomin iz EEPROM-a
  for (i = 0; i < ST_SPOMINA; i++) {
    spomin[i] = spominBeri(vSpominu[i]) * parametri[i].korak;
    
     Serial.print(i);
     Serial.print(": ");
     
    //vrednost v spominu je izven meja
    if(spomin[i] > parametri[i].max || spomin[i]  < parametri[i].min){
      Serial.print("os:");
      Serial.print(spomin[i]);
      
      //na zacetku ko se ni nc spremenjen je v .vrednosti notr osnovna def vrednost
      spomin[i] = parametri[i].vrednost;
      spominPisi(vSpominu[i], parametri[i].vrednost / parametri[i].korak);
      Serial.print("  ");
      Serial.print("nova:");
    }
   
    parametri[i].vrednost = spomin[i];
    
    Serial.println(spomin[i]);
    
    
  }
}

               
               
               
void tarcaPokazi(uint8_t stTarce, int16_t cas){
  if(cas == -1){
    tarca[stTarce].stanje = VEDNO_ON;
  }else{
    tarca[stTarce].stanje = ON;
  }
  tarca[stTarce].cnt = cas; // cas je v dekasekundah
  //tarca[stTarce].motor.writeMicroseconds(2000);
 switch(stTarce){
    case 0:tarca[stTarce].motor.write(spomin[MEM_MOTOR_1_MAX]);break;
    case 1:tarca[stTarce].motor.write(spomin[MEM_MOTOR_2_MAX]);break;
    case 2:tarca[stTarce].motor.write(spomin[MEM_MOTOR_3_MAX]);break;
  }
  /*switch(stTarce){
    case 0:digitalWrite(MORTOR1_PIN, HIGH);break;
    case 1:digitalWrite(MORTOR2_PIN, HIGH);break;
    case 2:digitalWrite(MORTOR3_PIN, HIGH);break;
    
  }*/
  
  //tarca[stTarce].tocke = 1;
  digitalWrite(3,HIGH);
  sPrint("aktivnaTarca: ", stTarce);
}

void tarcaSkrij(uint8_t stTarce){
  tarca[stTarce].stanje = OFF;
  //tarca[stTarce].motor.writeMicroseconds(1000);
  switch(stTarce){
    case 0:tarca[stTarce].motor.write(spomin[MEM_MOTOR_1_MIN]);break;
    case 1:tarca[stTarce].motor.write(spomin[MEM_MOTOR_2_MIN]);break;
    case 2:tarca[stTarce].motor.write(spomin[MEM_MOTOR_3_MIN]);break;
  }
  /*
  switch(stTarce){
    case 0:digitalWrite(MORTOR1_PIN, LOW);break;
    case 1:digitalWrite(MORTOR2_PIN, LOW);break;
    case 2:digitalWrite(MORTOR3_PIN, LOW);break;
    
  }
  */
  
  //tarca[stTarce].tocke = 0;
  //tarca[stTarce].cnt = 0;
  digitalWrite(3,LOW);
}

void premakniMotor(uint8_t stMotorja, uint16_t mesto){
  //stMotorja se soujema s stevilko tarce
  tarca[stMotorja].motor.write(mesto);
  Serial.print("m");
  Serial.print(stMotorja);
  sPrint(": ", mesto);
}


uint8_t preveriTipke(){
  if(get_LCD_button(btnUP)){
    //Serial.println("UP"); 
    return izvediUkaz(((s_ekran*)vsiEkrani[state])->tipke[btnUP]);
  }
  else if(get_LCD_button(btnDOWN)){
    //Serial.println("DOWN"); 
    return izvediUkaz(((s_ekran*)vsiEkrani[state])->tipke[btnDOWN]);
  }
  else if(get_LCD_button(btnRIGHT)){
    //Serial.println("RIGHT"); 
    return izvediUkaz(((s_ekran*)vsiEkrani[state])->tipke[btnRIGHT]);
  }
  else if(get_LCD_button(btnLEFT)){
    //Serial.println("LEFT"); 
     return izvediUkaz(((s_ekran*)vsiEkrani[state])->tipke[btnLEFT]);
  }
  //else if(get_LCD_button(btnUP));
  return 0;
}

uint32_t zadnjiPritisk;
uint8_t stPritiskov = 0;
uint8_t izvediUkaz(uint8_t ukaz){
  //sPrint("ukaz: ", ukaz);
  if(ukaz == 0)return 0;
  if(ukaz < MAX_EKRANOV && ukaz != NAST_EDIT){
    state = ukaz;
    return 1;
  }
  switch(ukaz){
    case FUN_TIPKE:
    //Serial.println(((s_ekran*)vsiEkrani[state])->elementi[((s_ekran*)vsiEkrani[state])->trenutnoIzbran].funkcija); 
    return izvediUkaz(((s_ekran*)vsiEkrani[state])->elementi[((s_ekran*)vsiEkrani[state])->trenutnoIzbran].funkcija);
    
    case INC_IZBRAN:
      if(((s_ekran*)vsiEkrani[state])->trenutnoIzbran < ((s_ekran*)vsiEkrani[state])->st_elementov-1){
        ((s_ekran*)vsiEkrani[state])->trenutnoIzbran ++;
      }else{
        ((s_ekran*)vsiEkrani[state])->trenutnoIzbran = 0;
      }
    return 1;
    
    case DEC_IZBRAN:
      if(((s_ekran*)vsiEkrani[state])->trenutnoIzbran > 0){
        ((s_ekran*)vsiEkrani[state])->trenutnoIzbran --;
      }else{
        ((s_ekran*)vsiEkrani[state])->trenutnoIzbran = ((s_ekran*)vsiEkrani[state])->st_elementov -1;
      }
    return 1;
    
    
    case NAST_EDIT_DEC:
      d_parameter.vrednost -= d_parameter.korak;
      if(d_parameter.vrednost < d_parameter.min){
        d_parameter.vrednost = d_parameter.min;
      }
    return 1;
    
    case NAST_EDIT_INC:
      
      d_parameter.vrednost += d_parameter.korak;
      if(d_parameter.vrednost > d_parameter.max){
        d_parameter.vrednost = d_parameter.max;
      }
      
    return 1;
    
    case PROG1_KONEC:
      programState = 3;
    break;
    
    case PROG2_KONEC:
      programState = 3;
    break;
    
    case PROG3_KONEC:
      programState = 3;
    break;
    
    //vstop v okno
    case NAST_EDIT:
      Serial.println("NAST_EDIT");
      sPrint("State je ",state);
      // zapovne si state da ve kere podatke ureja
      //to more nardit preden klice d_parameter saj v njem uporablja d_retState
      d_retState = state;
      
      //pogleda ali je vrednost iz spomina znotrej meja
      if ( d_parameter.max < spomin[d_indexParametra] || spomin[d_indexParametra] < d_parameter.min) {
        Serial.println("napacna");
      }else{
        d_parameter.vrednost = spomin[d_indexParametra];
      }
      
      d_retState = state; // zapovne si state da ve kere podatke ureja
      state = NAST_EDIT;
      
    return 1;
    
    
    case NAST_EDIT_FINISH:
      //ali je blia narejena sprememba ? 
      if(spomin[d_indexParametra] != d_parameter.vrednost){
        //zapise vrednost nazaj v spomin
        spomin[d_indexParametra] = d_parameter.vrednost;
        
        spominPisi(vSpominu[d_indexParametra], d_parameter.vrednost / d_parameter.korak);
        sPrint("zapis v spomin:", d_parameter.vrednost / d_parameter.korak);
        
      }else{
        Serial.println("Ni bilo spremembe!");
      }
      
      
      sPrint("koncna:", spomin[d_indexParametra]);
      state = d_retState;
    
      state = d_retState;
      
    return 1;
    
    case STEJ_PRITISKE:
      if(millis() - zadnjiPritisk < 500){
        stPritiskov ++;
        if(stPritiskov > 10){
          state = MENI_MADE_BY;
        }
      }else{
        stPritiskov = 1;
      }
      zadnjiPritisk = millis();
    return 1;
    
  }

}

void sPrint(char* c, int a) {
  Serial.print(c);
  Serial.println(a);
}


uint8_t koncniRezState;
uint8_t nextState;
uint16_t timeOut;
char *koncniIzpis[] = {
  "Koncni rezu2tat:",
  "Koncni rezu2tat:",
  "Koncni rezu2tat:",
  "Koncni rezu2tat:"
};



koncniNapisi napisi1[] = {
  "Koncni rezultat:",5000,
  "Pritisni tipko  ",1000,
  "     gor za     ",1000,
  " nadaljevanje!  ",1000,
};

koncniNapisi napisi2[] = {
  "   Skupaj:      ",5000,
  "                ",3000,
};

uint8_t dolzina = 4;
uint8_t stanje = 0;
uint8_t pozicijaKoncniIzpis = 0;
long zacetniCas2 = 0;

void koncniRezultati(int program){
  //zapise zgornjo vrstico
  if(napisi1[pozicijaKoncniIzpis].zamik < (millis()-zacetniCas)){  
    zacetniCas = millis();
    pozicijaKoncniIzpis ++;
    if(pozicijaKoncniIzpis > dolzina-1)pozicijaKoncniIzpis = 0;
    
    lcd.cursorTo(1, 0);
    lcd.printIn(napisi1[pozicijaKoncniIzpis].besedilo,16);
  }
  
  if(program != 2){
  //zapise spodjno vrstico
  if(napisi2[stanje].zamik < (millis()-zacetniCas2)){
    lcd.cursorTo(2, 0);
    if(stanje){
      stanje = 0;
      lcd.printIn(napisi2[stanje].besedilo,16);
    }else{
      stanje = 1;
      lcd.printIn(napisi2[stanje].besedilo,16);
    }
    zacetniCas2 = millis();
  }
  }
  
}


void odstevaj() {
  //sPrint("ost State:", odstevalnik.state);
  switch (odstevalnik.state) {
    case 1:
      char outStr[16];
      lcd.cursorTo(1,0);
      lcd.printIn(((s_ekranOdstevaj*)vsiEkrani[ODSTEVAJ])->naslov,-1); //"Igra se zacne v"
      char cifra[2];
      odstevalnik.state = 2;
      odstevalnik.mills = 0;
      ponavljaj = 1;

    case 2:
      //sPrint("cas: ",odstevalnik.mills );
      if (millis() - odstevalnik.mills > 960) {
        odstevalnik.mills = millis();
        if (odstevalnik.stOdstevanj-- == 0) {
          odstevalnik.state = 3;
          break;
        }
        lcd.cursorTo(2,0);
        itoa(odstevalnik.stOdstevanj + 1, cifra, 10);
        lcd.printIn(pretvoriBesedilo(outStr,cifra,NASLOV),16);
        piskacOn(3,ZVOK_LOW);
      }
      break;

    case 3:
      //lcd.setCursor(0, 1);
      //lcd.print(pretvoriBesedilo(outStr, "! GO !", NASLOV));
      piskacOn(10,ZVOK_MAX);
      if (millis() - odstevalnik.mills > 100) {
        state = odstevalnik.returnState;
        sPrint("ret State:", state);
        programState = 1;
      }
      break;
  }
}


void drawScreen(){
  char outStr[16];
  
  // izpiše naslov
  lcd.cursorTo(1,0); // postav ga na sredino //TODO: IZRUSI SAMO TAKRAT KO GA se spremeni !!
  lcd.printIn(pretvoriBesedilo(&outStr[0],((s_ekran*)vsiEkrani[state])->naslov, NASLOV),16); 
  
  //ne izrse puscic ce je samo 1 izbor
  int tipIzrisa = BESEDILO;
  if(((s_ekran*)vsiEkrani[state])->st_elementov < 2)tipIzrisa = NASLOV;
  
  //izpiše spodnje besedilo
  lcd.cursorTo(2,0); 
  lcd.printIn(pretvoriBesedilo(&outStr[0],((s_ekran*)vsiEkrani[state])->elementi[((s_ekran*)vsiEkrani[state])->trenutnoIzbran].besedilo, tipIzrisa),16); 
  programState = -1; //TODO premakn to na bolsi mest
}

void drawScreenNast(){
  char outStr[16];
  char vrednostStr[4];
  
  // izpiše naslov
  lcd.cursorTo(1,0); // postav ga na sredino //TODO: IZRUSI SAMO TAKRAT KO GA se spremeni !!
  lcd.printIn(pretvoriBesedilo(&outStr[0],((s_ekranNastavitve*)vsiEkrani[state])->naslov, NASLOV),16); 
  
  //izpiše spodnje besedilo
  lcd.cursorTo(2,0); 
  //if(((s_ekranNastavitve*)vsiEkrani[state])->focus == OFF){
    uint8_t treIzbran = ((s_ekranNastavitve*)vsiEkrani[state])->trenutnoIzbran;
    uint8_t indexParametra = ((s_ekranNastavitve*)vsiEkrani[state])->indexi[treIzbran];
    char *opis = parametri[indexParametra].opis;
    uint32_t vrednost = parametri[indexParametra].vrednost; 
        
    
    int b=0; //<-- stevec ki se premika po poljih
    
    //postavi puscice ce je elementov vec kot 2
    if(((s_ekranNastavitve*)vsiEkrani[state])->stParametrov > 1){
      outStr[b++] = ZNAK_LEVO;
      outStr[15] = ZNAK_DESNO;
    }else{
      outStr[b++] = ' ';
      outStr[15] = ' '; 
    }
    uint8_t dolzina = (getDecLength(vrednost)+getLngth(opis)+1);
    while(b < 8 - (dolzina - dolzina/2)){
      outStr[b++] = ' ';
    }
    

    //zapise opis parametra 
    
    // postavi na sredino
    while(*opis != 0){
      outStr[b++] = *opis;
      opis ++;
    }
    outStr[b++] = ' ';
    //zapise vrednost
    itoa(vrednost,vrednostStr,10);
    //itoa(get_free_memory(),vrednostStr,10);
    for(i = 0;i < getDecLength(vrednost);b++,i++){
      outStr[b] = vrednostStr[i];
    }
    //zapise presledke do konca
    while(b<15){
      outStr[b++] = ' ';
    }
    
    lcd.printIn(outStr,16); 
  //}else{
  //}
}
void drawScreenEdit(){
  
  //ce je izbran taprav index ob spreminjanju nastavitev zraven vrti tudi motorje
  if(d_indexParametra == MEM_MOTOR_1_MAX || d_indexParametra == MEM_MOTOR_1_MIN){
    premakniMotor(0,d_parameter.vrednost);
  }    
  else if(d_indexParametra == MEM_MOTOR_2_MAX || d_indexParametra == MEM_MOTOR_2_MIN){
    premakniMotor(1,d_parameter.vrednost);
  }
  else if(d_indexParametra == MEM_MOTOR_3_MAX || d_indexParametra == MEM_MOTOR_3_MIN){
    premakniMotor(2,d_parameter.vrednost);
  }
  
  
  //naslovne vrstico pustimo pri miru
  //spreminjamo samo ta drugo
    char outStr[16];
    char vrednostStr[4]; // charr arr v katerga zapisemo vrednost
    int b = 0;               // stevec ki se premika po poljih naprej
    
    lcd.cursorTo(2,0);
    char *opis = d_parameter.opis; 
    
    uint8_t dolzina = (getDecLength(d_parameter.vrednost)+getLngth(opis)+5);
    //zapise opis
    while(b < 8 - (dolzina - dolzina/2)){
      outStr[b++] = ' ';
    }
    
    while(*opis != 0){
      outStr[b++] = *opis;
      opis ++;
    }
    
    //postavi puscico levo
    outStr[b++] = ' ';
    outStr[b++] = ZNAK_LEVO;
    outStr[b++] = ' ';
    
    
    //zapise vrednost
    itoa(d_parameter.vrednost,vrednostStr,10);
    for(i = 0;i < getDecLength(d_parameter.vrednost);b++,i++){
      outStr[b] = vrednostStr[i];
    }
    
    outStr[b++] = ' ';
    outStr[b++] = ZNAK_DESNO;
    outStr[b++] = ' ';
    
    //zapise presledke do konca
    while(b<16){
      outStr[b++] = ' ';
    }
    
    
    lcd.printIn(outStr,16); 
  
  


}

//pretvori string v 16 dolg niz znakov ki je primeren za izpis
char* pretvoriBesedilo(char* out, char *str,int tip){
    int dolzina = getLngth(str);
    int dolOuta = 16;                   // dolžina izhodnega stringa
    int i = 0;                          //števec
    if(tip == BESEDILO){
        if(dolzina > 14)dolzina = 14;
        out[i++] = ZNAK_LEVO;                 // na provo mesto zališe puščico levo
        dolOuta = 15;                   // skrajša dolžino izhodnega striga da ne povoz desne puščice
        out[15] = ZNAK_DESNO;                  // na zadnje mesto zapie puščico desno
    }else{
        if(dolzina > 16)dolzina = 16;
    }
    
    int pozStr = 8 - dolzina /2;     // pozicija stringa
    if(dolzina & 0x1)pozStr --;      // če ni deljiv z 2 ga premakne v levo

    while(i<pozStr){
        out[i++] = ' ';             //zapiše presledke pred stringom
    }
    while(i<pozStr+dolzina){
        out[i++] = str[i-pozStr-1];  //zapiše string
    }
    while(i<dolOuta){
        out[i++] = ' ';             //zapiše prtesledke po besedilu
    }
    out[16] = 0;                    // na konec stringa doda nulo
    return &out[0];
}
// vrne dolžino stringa če ima na koncu 0
int getLngth(char * c){
    int dolzina = 0;
    while(*c++ != 0)dolzina ++;
    return dolzina;
}



void lcdInit(){
  lcd.init();
  lcd.clear();
  lcd.cursorTo(2, 0);
  lcd.printIn(izpis,16);
}

void InitialiseIO(){
  pinMode(PISKAC_PIN_JAKOST1, OUTPUT);
  pinMode(PISKAC_PIN_JAKOST2, OUTPUT);
  pinMode(SENZOR1_PIN, INPUT);
  pinMode(SENZOR2_PIN, INPUT);
  pinMode(SENZOR3_PIN, INPUT);
  digitalWrite(SENZOR1_PIN, HIGH);// Configure internal pull-up resistors
  digitalWrite(SENZOR2_PIN, HIGH);
  digitalWrite(SENZOR3_PIN, HIGH);
  
  tarca[0].stanje = tarca[1].stanje = tarca[2].stanje = OFF;
  //pinMode(MORTOR1_PIN, OUTPUT);
  //pinMode(MORTOR2_PIN, OUTPUT);
  //pinMode(MORTOR3_PIN, OUTPUT);
  tarca[0].motor.attach(MORTOR1_PIN);
  tarca[1].motor.attach(MORTOR2_PIN);
  tarca[2].motor.attach(MORTOR3_PIN);
  /*samo test :)
  tarca[0].motor.writeMicroseconds(2000);
  tarca[1].motor.writeMicroseconds(2000);
  tarca[2].motor.writeMicroseconds(2000);
  */
  analogWrite(PISKAC_PIN_JAKOST1, LOW);
  analogWrite(PISKAC_PIN_JAKOST2, LOW);
}


void InitialiseInterrupt(){
  cli();		// switch interrupts off while messing with their settings  
  PCICR =0x02;          // Enable PCINT1 interrupt
  PCMSK1 = 0b00001111;  // keri pini bojo sprožil interrupt
  sei();		// turn interrupts back on
}



void timer2Init(){
  cli();//stop interrupts

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  //OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)

  //  TCCR2A |= (1 << WGM21);   // turn on CTC mode

  TIMSK2 |= (1 << TOIE2);// enable overflow interrupt
  TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20);    // Set CS21 bit for 1024 prescaler
  //TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  sei();//allow interrupts

}



void intToArr(char *c, long n, int stDecMest, int dolChara,char znak){
  int i = 0;
  if(stDecMest == 0)stDecMest = -1; // tko pac program dela
  //izpisuje iz desne proti levi
  c += dolChara;
  *c-- = 0;
  //    if(n == 0){
  //        *c-- = '0';
  //        i++;
  //    }

  //izpise stevilke
  while(n){
    if(stDecMest == i){
      *c-- = '.';//izpise decimalno piko
    }
    else{
      *c-- = n%10+0x30;//izpise stevilko
      n /= 10;
    }
    i++;
  }
  //izpise dodatne nicle
  if(stDecMest+1>i){
    while(stDecMest>i){
      *c-- = '0';
      i++;
    }
    *c-- = '.';
    *c-- = '0';
    i+=2;
  }
  //izpise predsledke do konca
  while(dolChara-i>0){
    *c-- = znak;
    i++;
  }

}

uint8_t toggle = 0;
void toggleLed(){
  if(toggle){
    digitalWrite(3,HIGH);
  }
  else{
    digitalWrite(3,LOW);
  }
  toggle = !toggle;
}

int piskacCnt = 0;
int piskac = OFF;
void piskacOn(int cas,int zvok){
  if(piskac == OFF){
    sPrint("piskac ON ", zvok);
    piskacCnt = cas;
    //Timer1.attachInterrupt(100, piskacOff);
    //Timer1.attachInterrupt(piskacOff); 
    switch(zvok){
      case 0:
        digitalWrite(PISKAC_PIN_JAKOST1, LOW);
        digitalWrite(PISKAC_PIN_JAKOST2, LOW);
      break;
      
      case 1:
        digitalWrite(PISKAC_PIN_JAKOST1, HIGH);
        digitalWrite(PISKAC_PIN_JAKOST2, LOW);
      break;
      
      case 2:
        digitalWrite(PISKAC_PIN_JAKOST1, LOW);
        digitalWrite(PISKAC_PIN_JAKOST2, HIGH);
      break;
      
      case 3:
        digitalWrite(PISKAC_PIN_JAKOST1, HIGH);
        digitalWrite(PISKAC_PIN_JAKOST2, HIGH);
      break;
    }
    piskac = ON;
  }
}

void piskacCount(){

  if(piskac == ON){
    sPrint("p ", piskacCnt);
    if(piskacCnt == 0){
      //Timer1.detachInterrupt();
        digitalWrite(PISKAC_PIN_JAKOST1, LOW);
        digitalWrite(PISKAC_PIN_JAKOST2, LOW);
      piskac = OFF;
    }
    else piskacCnt --;
  }
}
void tarcaCount(){
  int k;
  for(k=0;k<3;k++){
    if(tarca[k].stanje == ON){
      //toggleLed();
      //Serial.print("1");
      //if(k == 0)sPrint("t0 ", tarca[k].cnt);
      if(tarca[k].cnt == 0)tarcaSkrij(k);
      else tarca[k].cnt --; 
    }
    //Serial.print("0");
  }
  //Serial.println();
}

unsigned long tipkeDelay = 0;
int adc_key_in;
uint8_t up = 0;
void set_LCD_buttons()
{

    if(millis()-tipkeDelay > 200){// med vsak klikom tipke more bit saj 100 ms zamika
      tipkeDelay = millis();
      adc_key_in = analogRead(GUMBI_PIN);      // read the value from the sensor 
      //toggleLed();
      Serial.print(adc_key_in);

      // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
      // we add approx 50 to those values and check to see if we are close
      if (adc_key_in > 1000) return; // We make this the 1st option for speed reasons since it will be the most likely result
      // For V1.1 us this threshold
      // For V1.0 comment the other threshold and use the one below:
      if (adc_key_in < 50) {  gumbi[btnRIGHT] = 1 ;        Serial.println(" R");}
      else if (adc_key_in < 195){   gumbi[btnUP] = 1;       Serial.println(" U");}
      else if (adc_key_in < 380){   gumbi[btnDOWN] = 1;      Serial.println(" D");}
      else if (adc_key_in < 555){   gumbi[btnLEFT] = 1;      Serial.println(" L");}
      else if (adc_key_in < 790){   gumbi[btnSELECT] = 1;    Serial.println(" S");}
    }


}
int get_LCD_button(int btn)
{

  if(gumbi[btn]){
    //sPrint("gumb ", btn);
    //sPrint("vrednost ", gumbi[btn]);
    gumbi[btn] = 0;
    return 1;
  }
  return 0;
}

unsigned long tockeClk[3];
void povecajTocke(int stTarce){
  if(millis()-tockeClk[stTarce]>200){
    //if(tarca[stTarce].stanje == ON){
      tarca[stTarce].tocke += 1;
      tockeClk[stTarce] = millis();
   // }
  }
}


ISR(PCINT1_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
  // will generate an interrupt: but this will always be the same interrupt routine
  if (digitalRead(A0)==0) { 
    //if( (PINB & (1 << PINB1)) == 0 ){
        /* LOW to HIGH pin change */
         set_LCD_buttons();
    //}
  }
  else if (digitalRead(A1)==0)  povecajTocke(0);
  else if (digitalRead(A2)==0)  povecajTocke(1);
  else if (digitalRead(A3)==0)  povecajTocke(2);
}

volatile uint8_t tot_overflow;
ISR(TIMER2_OVF_vect)
{
  // keep a track of number of overflows
  tot_overflow++;
  if(tot_overflow>=10){
    tot_overflow = 0;
    //toggleLed();
    piskacCount();
    tarcaCount();
    //toggleLed();
  }
}


int getDecLength(long n){
  if (n < 100000)
  {
    // 5 or less
    if (n < 100)
    {
      // 1 or 2
      if (n < 10)
        return 1;
      else
        return 2;
    }
    else
    {
      // 3 or 4 or 5
      if (n < 1000)
        return 3;
      else
      {
        // 4 or 5
        if (n < 10000)
          return 4;
        else
          return 5;
      }
    }
  }
  else
  {
    // 6 or more
    if (n < 10000000)
    {
      // 6 or 7
      if (n < 1000000)
        return 6;
      else
        return 7;
    }
    else
    {
      // 8 to 10
      if (n < 100000000)
        return 8;
      else
      {
        // 9 or 10
        if (n < 1000000000)
          return 9;
        else
          return 10;
      }
    }
  }
}
