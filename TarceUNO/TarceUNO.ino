#include <Timer.h>
#include <Servo.h> 
#include <LCD4Bit_mod.h> 
#include <TarceUNO.h>






//Timer Timer1;

LCD4Bit_mod lcd = LCD4Bit_mod(2);
volatile uint8_t tot_overflow;
uint8_t toggle = 0;
int piskac = OFF;
int stoparica = OFF;
int stevec = 0;
int piskacCnt = 0;
int gumbi[5];
int adc_key_in;
int funInt = 1;
uint8_t state;
uint8_t puscice = 0; // ali so narisane puščice ? 
uint16_t vsiEkrani[MAX_EKRANOV];
long zacetniCas = 0;
long zacetniCas2 = 0;
char *(besedila)[3] = {
  "Igra","Start ","Info"};
char izpis[16] = {
  ZNAK_LEVO,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',ZNAK_DESNO};
//int tocke[3];


void piskacOn(int cas,int zvok);
void piskacOff();
int get_LCD_button();





s_ekran mainMenu = //MENI_GLAVNI
{
  "Glavni meni",
  //tipke: 
  //gor dol levo, desno, select
  0,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  4,0,
  "igraj !",      MENI_IGRAJ,
  "nastavitve",   MENI_NASTAVITVE,
  "info",         0,
  "random :)",    0,
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
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve motor2Nast =  //NAST_MOTOR2
{
  "Nast. motor2",
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve motor3Nast =  //NAST_MOTOR3
{
  "Nast. motor3",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve senzorjiNast =  //NAST_SENZORJI
{
  "Nast. senzo",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis,    min   max,  def,  korak
  "obcut.:", 1000, 2000, 1500, 100,
  "zamik:",  100,  1000, 400,  100,
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
  "program 1",        MENI_PROG1,
  "program 2",        MENI_PROG2,
  "program 3",        MENI_PROG3
};

s_ekran prog1Menu = //MENI_PROG1
{
  "Program 1:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_1,
  "nastavitve",        	 NAST_PROG1,
};

s_ekranNastavitve prog1Nast =  //NAST_PROG1
{
  "prog1 nastavitve",
  //gor dol levo, desno, select
  MENI_PROG1,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis,    min   max,  def,  korak
  "tezavnost:", 1, 5, 3, 1,
  "cas:",  10,  200, 60,  10,
  //"hitrost:",  100,  1000, 500,  50

};


s_ekran prog2Menu = //MENI_PROG2
{
  "Program 2:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  1,0,
  "zacni z igro",        PROGRAM_2,
  "nastavitve",        	 0,
};

s_ekran prog3Menu = //MENI_PROG3
{
  "Program 3:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_3,
  "nastavitve",        	 0,
};

s_ekranProgram program1 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG1,0,0,0,0,
};

s_ekranProgram program2 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG2,0,0,0,0,
};

s_ekranProgram program3 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG3,0,0,0,0,
};

s_tarca tarca[3];
//tarca[0] = {OFF,0,0};
//tarca[1] = {OFF,0,0};
//tarca[2] = {OFF,0,0};


void setup() 
{ 
  InitialiseIO();        // inicalizacija portov input/output
  InitialiseInterrupt(); // interupt za gumbe
  timer2Init();          // inicalizacije timerja (za piskace in stevec ...)
  lcdInit();             // lcd init
  state = MENI_GLAVNI;
  //Serial.begin(9600); 
  //Serial.println("Connection OK !"); 
  
  vsiEkrani[MENI_GLAVNI] =       (uint16_t)(&mainMenu);
  vsiEkrani[MENI_IGRAJ] =        (uint16_t)(&igrajMenu);
  vsiEkrani[MENI_NASTAVITVE] =   (uint16_t)(&nastavitveMenu);
  vsiEkrani[MENI_PROG1] =   	 (uint16_t)(&prog1Menu);
  vsiEkrani[MENI_PROG2] =   	 (uint16_t)(&prog2Menu);
  vsiEkrani[MENI_PROG3] =   	 (uint16_t)(&prog3Menu);
 
  vsiEkrani[NAST_MOTOR1] =       (uint16_t)(&motor1Nast);
  vsiEkrani[NAST_MOTOR2] =       (uint16_t)(&motor2Nast);
  vsiEkrani[NAST_MOTOR3] =       (uint16_t)(&motor3Nast);
  vsiEkrani[NAST_SENZORJI] =     (uint16_t)(&senzorjiNast);
  vsiEkrani[NAST_EDIT] =         (uint16_t)(&editNast);
  
  vsiEkrani[PROGRAM_1] =   	 (uint16_t)(&program1);
  vsiEkrani[NAST_PROG1] =   	 (uint16_t)(&prog1Nast);
  
  vsiEkrani[PROGRAM_2] =   	 (uint16_t)(&program2);
  vsiEkrani[PROGRAM_3] =   	 (uint16_t)(&program3);
  
} 

int odstevajState = 0;
uint8_t stOdstevanj = 0;
int8_t programState = -1;
uint8_t stateReturn = 0;
uint8_t ponavljaj = 0;
void (*initProgram)();
char scoreCel[17] = "  1:0     2:0   ";
int i;
int j;
int cas = 10;
char score[3];
char casAray[10];
uint8_t pozicijaKoncniIzpis = 0;
uint8_t nastEditReturn;
int tipPrograma = -1;

koncniNapisi napisi2[] = {
  "   Skupaj:      ",5000,
  "                ",3000,
};

void loop() {

  
   switch(state){
      case MENI_GLAVNI:
        drawScreen();
      break;
      
      case MENI_IGRAJ:
        drawScreen();
      break;
      
      case MENI_NASTAVITVE:
        drawScreen();
      break;

      case MENI_PROG1:
        drawScreen();
      break;

      case MENI_PROG2:
        drawScreen();
      break;

      case MENI_PROG3:
        drawScreen();
      break;
      
      case NAST_EDIT:
        drawScreenEdit();
      break;
      
      case NAST_MOTOR1:
        drawScreenNast();
      break;
      
      case NAST_MOTOR2:
        drawScreenNast();
      break;
      
      case NAST_MOTOR3:
        drawScreenNast();
      break;
      
      case NAST_SENZORJI:
        drawScreenNast();
      break;
      
      case NAST_PROG1:
        drawScreenNast();
      break;
      
      case PROGRAM_2:
        switch (programState){
        case -1:
          odstevajState = 1;
          programState = 0;
          stOdstevanj = 3; 
          tarcaSkrij(0);
          tarcaSkrij(1);
          tarcaSkrij(2);
          
          
        case 0:
          //ko se odstevanje konca premakne programState na 1
          odstevaj();pis
          ponavljaj = 10;
        break;
        
        case 1:
          programState = 2;
          cas = 300;
          
          
          lcd.clear();
          tarca[0].tocke = tarca[1].tocke = tarca[2].tocke = 0;
          lcd.cursorTo(1, 0);
          lcd.printIn("   Cas:       ",-1);
          //lcd.cursorTo(2, 0);
          //lcd.printIn(scoreCel,16);
          zacetniCas = millis();
          
        case 2:


          
          for(i=0;i<3;i++){
            if(random(1000)<60 && tarca[i].stanje == OFF){
              tarcaPokazi(i,random(5,20));
              //toggleLed();
            }
          }
           
           if(cas < (millis()-zacetniCas)/100){
             programState = 3;
             tarcaSkrij(0);
             tarcaSkrij(1);
             tarcaSkrij(2);  
             
             zacetniCas = millis(); // ta je za ta zgrno vrstico 
             zacetniCas2 = millis(); // ta je pa za ta spodno
             pozicijaKoncniIzpis = 3;
             for(i=0;i<16;i++){
               napisi2[1].besedilo[i] = scoreCel[i];
             }
             itoa(tarca[0].tocke + tarca[1].tocke + tarca[2].tocke,score,10);
             for(i=0;i<getDecLength(tarca[0].tocke + tarca[1].tocke + tarca[2].tocke);i++){
               napisi2[0].besedilo[i+11] = score[i];
             }
    
             break; 
           }
           //izpisi cas
           lcd.cursorTo(1, 13-5);
           intToArr(&casAray[0],cas - (millis()-zacetniCas)/100,1,5,' ');
           lcd.printIn(casAray, 5);
           
           break;
           case 3:
             //koncniRezultati(2);
             piskacOn(3,ZVOK_LOW);
             state = MENI_PROG2;
           break;
        }

      break;
      
      case PROGRAM_1:
        switch (programState){
        case -1:
          odstevajState = 1;
          programState = 0;
          stOdstevanj = 3; 
          tarcaSkrij(0);
          tarcaSkrij(1);
          tarcaSkrij(2);
          
        case 0:
          //ko se odstevanje konca premakne programState na 1
          odstevaj();
          ponavljaj = 10;
        break;
        
        case 1:
          programState = 2;
          cas = 300;
          
          
          lcd.clear();
          tarca[0].tocke = tarca[1].tocke = tarca[2].tocke = 0;
          lcd.cursorTo(1, 0);
          lcd.printIn("   Cas:       ",-1);
          lcd.cursorTo(2, 0);
          lcd.printIn(scoreCel,16);
          zacetniCas = millis();
          
        case 2:


          
          for(i=0;i<3;i++){
            if(random(1000)<60 && tarca[i].stanje == OFF){
              tarcaPokazi(i,random(5,20));
              //toggleLed();
            }
          }
           
           if(cas < (millis()-zacetniCas)/100){
             programState = 3;
             tarcaSkrij(0);
             tarcaSkrij(1);
             tarcaSkrij(2);  
             
             zacetniCas = millis(); // ta je za ta zgrno vrstico 
             zacetniCas2 = millis(); // ta je pa za ta spodno
             pozicijaKoncniIzpis = 3;
             for(i=0;i<16;i++){x  

extern char *__bss_start;
extern char *__bss_end;
//extern char *__brkval;
extern char *__heap_start;
extern char *__heap_end;
//extern char *__malloc_heap_end;
//extern size_t __malloc_margin;


	int	data_size	=	(int)&__data_end - (int)&__data_start;
	int	bss_size	=	(int)&__bss_end - (int)&__data_end;
	int	heap_end	=	(int)&stack - (int)&__malloc_margin;
//	int	heap_size	=	(int)__brkval - (int)&__bss_end;
	int	heap_size	=	heap_end - (int)&__bss_end;
	int	stack_size	=	RAMEND - (int)&stack + 1;
	int	available	=	(RAMEND - (int)&__data_start + 1);
	
	available	-=	data_size + bss_size + heap_size + stack_size;
}*/
void tarcaPokazi(uint8_t stTarce, uint8_t cas){
  tarca[stTarce].stanje = ON;
  tarca[stTarce].cnt = cas; // cas je v dekasekundah
  tarca[stTarce].motor.writeMicroseconds(2000);
  //tarca[stTarce].tocke = 1;
  digitalWrite(3,HIGH);
}

void tarcaSkrij(uint8_t stTarce){
  tarca[stTarce].stanje = OFF;
  tarca[stTarce].motor.writeMicroseconds(1000);
  //tarca[stTarce].tocke = 0;
  //tarca[stTarce].cnt = 0;
  digitalWrite(3,LOW);
}

uint8_t preveriTipke(){
  if(get_LCD_button(btnUP)){
    //Serial.println(((s_ekran*)vsiEkrani[state])->tipke[btnUP]); 
    return izvediUkaz(((s_ekran*)vsiEkrani[state])->tipke[btnUP]);
  }
  else if(get_LCD_button(btnDOWN)){
    //Serial.println(((s_ekran*)vsiEkrani[state])->tipke[btnDOWN]); 
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

uint8_t izvediUkaz(uint8_t ukaz){
  if(ukaz == 0)return 0;
  if(ukaz < 15){
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
    

    case NAST_EDIT:
      d_retState = state; // zapovne si state da ve kere podatke ureja
      state = NAST_EDIT;
      
    return 1;
    
    case NAST_EDIT_FINISH:
      state = d_retState; 
    return 1;
    
    
  }

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



uint8_t dolzina = 4;
uint8_t stanje = 0;

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

long mills = 0;
void odstevaj(){
  switch(odstevajState){
    case 1:
      char outStr[16];
      lcd.cursorTo(1,0);
      lcd.printIn("Igra se zacne v",-1);
      char cifra[2];
      piskac = OFF;
      odstevajState = 2;
      mills = millis()-960;
    
    case 2:
    if(millis() - mills > 960){
      mills = millis();
      if(stOdstevanj-- == 0){
        odstevajState = 3;
        break;
      }
      lcd.cursorTo(2,0);
      itoa(stOdstevanj+1,cifra,10);
      lcd.printIn(pretvoriBesedilo(outStr,cifra,NASLOV),16);
      piskacOn(3,ZVOK_LOW);
      tarcaPokazi(0,10);
     }
     break; 
     
     case 3:
      lcd.cursorTo(2,0);
      lcd.printIn(pretvoriBesedilo(outStr,"! GO !",NASLOV),16);
      piskacOn(10,ZVOK_MAX);
      if(millis() - mills > 1500){
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
  
  //izpiše spodnje besedilo
  lcd.cursorTo(2,0); 
  lcd.printIn(pretvoriBesedilo(&outStr[0],((s_ekran*)vsiEkrani[state])->elementi[((s_ekran*)vsiEkrani[state])->trenutnoIzbran].besedilo, BESEDILO),16); 
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
    char *opis = ((s_ekranNastavitve*)vsiEkrani[state])->parameter[treIzbran].opis;
    uint32_t vrednost = ((s_ekranNastavitve*)vsiEkrani[state])->parameter[treIzbran].vrednost; 
    int b=0; //<-- stevec ki se premika po poljih
    
    //postavi puscice
    outStr[b++] = ZNAK_LEVO;
    outStr[15] = ZNAK_DESNO; 
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
  puscice = 1;
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
  tarca[0].motor.attach(MORTOR1_PIN);
  tarca[1].motor.attach(MORTOR2_PIN);
  tarca[2].motor.attach(MORTOR3_PIN);
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
void toggleLed(){
  if(toggle){
    digitalWrite(3,HIGH);
  }
  else{
    digitalWrite(3,LOW);
  }
  toggle = !toggle;
}
void piskacOn(int cas,int zvok){
  if(piskac == OFF){
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
      if(tarca[k].cnt == 0){
          tarcaSkrij(k);
      }else tarca[k].cnt --; 
    }
  }
}

unsigned long tipkeDelay = 0;
uint8_t up = 0;
void set_LCD_buttons()
{

    if(millis()-tipkeDelay > 200){// med vsak klikom tipke more bit saj 100 ms zamika
      tipkeDelay = millis();
      adc_key_in = analogRead(GUMBI_PIN);      // read the value from the sensor 
      //toggleLed();
      // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
      // we add approx 50 to those values and check to see if we are close
      if (adc_key_in > 1000) return; // We make this the 1st option for speed reasons since it will be the most likely result
      // For V1.1 us this threshold
      // For V1.0 comment the other threshold and use the one below:
      if (adc_key_in < 50)   gumbi[btnRIGHT] = 1 ;  
      else if (adc_key_in < 195)   gumbi[btnUP] = 1; 
      else if (adc_key_in < 380)   gumbi[btnDOWN] = 1;
      else if (adc_key_in < 555)   gumbi[btnLEFT] = 1;
      else if (adc_key_in < 790)   gumbi[btnSELECT] = 1;  
    }


}
int get_LCD_button(int btn)
{
  if(gumbi[btn]){
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

