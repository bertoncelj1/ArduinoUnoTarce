#include <ATComandHandler.h>

#include <SoftwareSerial.h>


#define serialControl softwareSerial //serial ki se uporabla za debagirat
#define serialWlan Serial //serial ki se uporablja za komunikacijo z wlan modulom
#define WLAN_BAUD 115200


#define AP_SSID   "modulv2"     //ap ssid glavne naprave serverja
#define AP_PASS   "geslo1234"   //ap geslo glavne naprave serverja
#define SRV_IP    "192.168.4.1" //IP TCP serverja
#define SRV_PORT  "80"          //port TCP serverja

//ukazi flags bitne pozicije
#define OK          0
#define TIMEOUT     1
#define NO_ANS      2  sett = 0
#define ODG  3

#define ON       1
#define OFF      0




// software serial: TX = 10, RX = 11
SoftwareSerial softwareSerial(10,11);

int resetPin = 4;
int statusLedPin = 13;
int tarcaSenzorPin = 2; //na ta pin je postavljeni interrupt 0
int tarcaLedPin = 5;
int tarcaState = OFF;
int tarcaScore = 0;
int tarcaScoreOld = 0;//uporablja ko je traca v nacinu echo
volatile int tarcaCas = 0;
int samoOff = 0; //samodejno izklapanje tarce ob zadetku
int tarcaEcho = 0; //sama sporoci serverju da je bila zadeta

char mac[6];
int macExist = 0; //ali je bil mac adress ze kreiran

void tarcaSetOff(){
  digitalWrite(tarcaLedPin, LOW);
  tarcaState = OFF;
  tarcaCas = 0;
}

//prizge tarco za dolocen cas, ce je ta < 0 se ne ugasne (ubistvu se po 65536 ciklih) 
void tarcaSetOn(int cas){
  digitalWrite(tarcaLedPin, HIGH);
  tarcaState = ON;
  tarcaCas = cas;
}

volatile long zadnjicZateta = 0;
void tarcaHit(){
  if(tarcaState == ON){
    if(millis() - zadnjicZateta > 100){
      zadnjicZateta = millis();
      tarcaScore ++;
      if(samoOff)tarcaSetOff();
    }
  }
}

void tarcaOdstevaj(){
  if(tarcaState == ON){
    if(tarcaCas == 0){
      tarcaSetOff();
    }
    if(tarcaCas > 0)tarcaCas--;//zmanjsa samo ce je vecji od nic, ce je tarcaCas -1 bo tudi ostal -1
  }

}
void setup()
{
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    
    pinMode(statusLedPin, OUTPUT);
    digitalWrite(statusLedPin, HIGH);
    
    pinMode(tarcaLedPin, OUTPUT);
    tarcaSetOff();
    
    attachInterrupt(0, tarcaHit, CHANGE); // dada interupt na pin 2
  
    timer2Init(); //zato da prikazuje signalizacijsko lucko in izklaplja tarco
    ATHandlerInit(&softwareSerial);
    
    
    // initialize serial:
    serialControl.begin(9600);
    serialControl.println("\r\n\r\nClient ready for action!");


    
    

    serialWlan.begin(WLAN_BAUD);
    
    //tarcaSetOn(10);

}

//timer 2 se uporablaj za signalizacijsko lucko, ki utripa glede na stanje clienta
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

//timer2 overflow
int statusLed = HIGH;
volatile uint8_t tot_overflow = 0;
ISR(TIMER2_OVF_vect)
{
    // keep a track of number of overflows
    tot_overflow++;
    if(tot_overflow>=10){
      tot_overflow = 0;
      toggleLed();
      tarcaOdstevaj();
    }
    
}

int time = 0;//trenutni preostali cas 
int statusLedOn = 0;
int statusLedOff = 0;
volatile int spreminjam = 0; 
//preklaplja med prizgano in ugasnjeno statusLed glede na podana parametra 
//statusLedOn in statusLedOff, ki povesta koliko ciklov bo statusLed prizgana in ugasnjena
//ce je parameter statusLedOn ali statusLedOff postavljen na vrednost ki je < 0 bo vedno v nasprotnem 
//stanju. Npr ce je statusLedOn < 0 bo statusLed vedno off. Ce sta oba paremetra na < 0 bo vrednost random
void toggleLed(){
  if(!spreminjam){//pogleda  ce se nastavitve ravno spreminjajo
    time--;
    //serialControl.println(time);
    if(time == 0){
      time = (statusLed == HIGH)? statusLedOff : statusLedOn;
      if(time > 0){
        //SerialControl.println('#');
        statusLed = (statusLed == HIGH)? LOW: HIGH;
        digitalWrite(statusLedPin,statusLed);
      }else{
        //if(statusLed == HIGH)serialControl.println("vedno bo HIGH");
        //else if(statusLed == LOW)serialControl.println("vedno bo LOW");
        //else serialControl.println("vedno bo ???");
      }
      // ce je time manjsi od nic pomeni da se statusLed ne sme zamenjati
      // if(time == 0) ne bo vec klical in se stanje led ne bo spreminjalo
    }
  }
}

//nastima koliko casa bo status led on in koliko cas off
// ce je parameter < 0 led nikoli ne bo v tem stanju
//ce sta oba < 0 se stanje led ne bo spreminjalo iz prejsnega nastavljenega stanja  

void setStatusLed(int onTime, int offTime){
  spreminjam = 1;
  statusLedOn = onTime;
  statusLedOff = offTime;
  if(onTime > 0){
    statusLed = HIGH;
    time=statusLedOn;
  }else{
    statusLed = LOW;
    time=statusLedOff;
  }
  time += 1;
  digitalWrite(statusLedPin,statusLed);
  spreminjam = 0;
}


void hardReset(){
    serialControl.println("#######hard rst!#######");
    digitalWrite(resetPin, LOW);
    delay(100);
    digitalWrite(resetPin, HIGH);

}

//nastavitve potrebne za clienta
s_nastParam cipmux = {
  "AT+CIPMUX?",
  "+CIPMUX:_",
  0,
  "AT+CIPMUX=0"
};

s_nastParam cwmode = {
  "AT+CWMODE?",
  "+CWMODE:_",
  1,
  "AT+CWMODE=1"
};

s_nastParam *serverSettings[2] = {&cipmux, &cwmode};


int preveriNastavitve(int tries, s_nastParam **setArray, int setLen){
  int odg, sett, i;
  ;
  while(tries--){
    sett = 1;
    //gre cez vse podane nastavitve
    for(i=0; i<setLen; i++){
      if(nastaviNastavitve(setArray[i]) == 0){
        sett = 0;
      }
    }
    if(sett == 1)return 1;
    
  }
  
  return 0;
}

//posiskusi se povezati z AP-jem x-krat
//vrne 1 ce se je uspesno povezal ali 0 ce se mu v x korakih ni ratalo povezati
char *naborOdgovorov[5] = {"NO AP", "No AP", AP_SSID, "+CWJAP:", "ERROR"};//odgovora ki jih pricakuje po ukazu 'AT+CWJAP?'
char *naborOdgovorov2[3] = {"OK", "FAIL", "ERROR"};//odgovori ki jih  pricakuje po ukazu za povezavo z AP-jem
int connectToAP(int tries){
  int odgConn;
  
  while(tries--){
    //najprej preveri stanje povezave
    posljiUkaz("AT+CWJAP?");
    int odg = getOdgovorMulty(naborOdgovorov, 5, 2000);
    if(odg > 0){
      serialControl.println(naborOdgovorov[odg]);
    }else{
      serialControl.print(odg);
    }
    
    
    switch(odg){
      case 2://connection OK
      case 3:
        return 1;
        break;
      
      case 0://NO AP, ali pa je povezan z napacnim AP-jem
      case 1:
        // ni se povezan na AP zato se poveze ! 
        posljiUkaz("AT+CWJAP=\""AP_SSID"\",\""AP_PASS"\"");
        //5s caka na odgovor 'OK'
        odgConn = getOdgovorMulty(naborOdgovorov2, 3, 20000);//10s
        serialControl.println(naborOdgovorov2[odgConn]);
        if(odgConn == 0){//OK
          return 1;
          
        }else  if(odgConn == 1){//FAIL  
          //FAIL pomeni da ni nasel AP s podanim SSID-je ali pa je geslo napacno      

        }else  if(odgConn == 2){//ERROR
          //ERROR vrne samo takrt, ko je napacen vnos
          return 0;
        }
     
        break;
        
      case 5://ERROR
        return 0;
        break;
      
      case -1://noben odgovor se ne ujema
        serialControl.println(" NO MATCH");
        break;
      
      case -2://nic ni prispelo
        serialControl.println(" ERROR");
        break;
    
    }
    
  }
  
  return 0;

}

/*
ukaz:'AT+CIFSR'
AT+CIFSR
+CIFSR:APIP,"192.168.4.1"
+CIFSR:APMAC,"1a:fe:34:a0:bb:66"

OK
*/

//od wlan modula zahteva mac adress, ce ta ne podpira te funkcije, sa ga sam izmisli
int getMacAdress(int tries){
  int odgmac;
  
  //pogleda ce je ze pridobil mac adress
  if(macExist)return 1;
  
  while(tries --){
    posljiUkaz("AT+CIFSR");
    
    char podatki[18]; //ker na koncu doda se 0, sem notr zapise mac v string obliki
    odgmac = preberiPodatke("+CIFSR:STAMAC,\"%", 17, podatki, 1000);
    
    switch(odgmac){
      case 1: //uspesno prebral
        if(pretovoriMacvInt(podatki, mac)){
          macExist = 1;
          createMACsendmsg();
          serialControl.println(" OK");
          return 1;
        }else{
          serialControl.println(" npaka pri pretvorbi MAC");
        }
        break;
      
      case -1: //podakti so bili prekratki
        serialControl.println("SHOTRT");
        break;
      
      case 0:  //nic ni preral, mac se generira na random
        //na zadnja dva mesta zapise R R zato da se ve da je random
        //random zacne z 32 zato da spusit posebne znake kot so new line...
        serialControl.println("NO MAC");
        
        mac[0] = random(32, 255);
        mac[1] = random(32,255);
        mac[2] = random(32,255);
        mac[3] = random(32,255);
        mac[4] = 82;//R
        mac[5] = 82;//R
        
        macExist = 1;
        createMACsendmsg();
        return 1;
        break;
    }

  }
    
  return 0;
}

char macStrSend[11]; //{'m','a','c',':',mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],0};
//zapise mac v sporocilo
void createMACsendmsg(){
  if(!macExist)return;
  
  macStrSend[0] = 'm';
  macStrSend[1] = 'a';
  macStrSend[2] = 'c';
  macStrSend[3] = ':';
  macStrSend[4] = mac[0];
  macStrSend[5] = mac[1];
  macStrSend[6] = mac[2];
  macStrSend[7] = mac[3];
  macStrSend[8] = mac[4];
  macStrSend[9] = mac[5];
  macStrSend[10] = 0;
}

char *odgovoriCipstart[5] = {"CONNECT", "FAIL", "CLOSED", "ALREADY", "no ip"};
//poskusi se povezati s TCP streznikom 
int connectToTCPServer(int tries){
  
  while(tries --){
    //najprej preveri CIPSTATUS ce je slucajno ze povezan in na kaj je povezan
    posljiUkaz("AT+CIPSTATUS");
    int stanje = getNastavitve("STATUS:_", 200);
    serialControl.print(stanje);
    
    if(stanje == 2){//Got IP
      //zaradi cudne napaka lahko naprava vrne stanje = 2 tudi ce je ze povezana na streznik ... :(
      if(getOdgovor(":0,\"TCP\",\""SRV_IP"\","SRV_PORT, 500)){
        serialControl.println(" Connected ");  
        return 1;
      }
      //more vspostavt povezavo
      serialControl.println(" Got IP");
      posljiUkaz("AT+CIPSTART=\"TCP\",\""SRV_IP"\","SRV_PORT);
      
      int odgConn = getOdgovorMulty(odgovoriCipstart, 5, 5000);
      serialControl.print(" ");
      if(odgConn != -1){
        serialControl.println(odgovoriCipstart[odgConn]);
        if(odgConn == 0){//CONNECT
          return 1;
        }if(odgConn == 1){//FAIL
          delay(1000);
        }if(odgConn == 2){//CLOSED
          //zaradi nepojasnenega razloga po neuspeli povezavi pozabi da ima IP (na ukaz 'AT+CIPSTATUS' vrne 4 -> disconected) 
          //zato je nasa naloga da ga spovnimo da ima sekar IP tako da poklicemo ukaz: (AT+CWJAP ga tudi spovni)
          serialWlan.println("AT+CIFSR");
          if(!getOdgovor("OK", 1000))return 0;
          delay(5000);
        }if(odgConn == 3){//ALERADY CONNECTED
          //je ze povezan 
          serialWlan.println("AT+CIFSR");
          if(!getOdgovor("OK", 1000))return 0;
          delay(1000);
          return 1;
        }if(odgConn == 4){//no ip
          return 0;
        }
      }else{
        serialControl.println("neznan odgovor!");
        return 0;
      }
      
    }else if(stanje == 3){//Connected
      //vse je ze v redu :)
      //preveri ce je povezan na pravi streznik
      if(getOdgovor(":0,\"TCP\",\""SRV_IP"\","SRV_PORT, 500)){
        serialControl.println(" Connected");  
        return 1;
      }
      //ni nujno da smo povezani s pravim serverjem ampak se itak ne more povezat na drug server
      //tko da je vse ok
      serialControl.println(" Connected?");  
      return 1;
      
    }else if(stanje == 4){//4:Disconected,
      //sploh se nima IP-ja in se ni povezan na AP
      //TODO: poglej katero komando mu mors tuki poslt da bo vedu da ni v resnic disconected ampak da ze ima IP
      //tudi na tej tocki se probaj povezati
      serialControl.println(" Disconected");
      return 0;
      
      
    }else if(stanje == 5){ //lahko je tudi 5 samo to v spec. nc ne pise kva to tocn pomen
      //mozno da caka na IP zato je tuki ta delay
      //ce ka samo sprasujemo 'AT+CIPSTATUS', bo sekar naprej vracal 5, zato ga moramo vprasati nekaj drugega
      serialControl.println(" ???");
      serialWlan.println("AT+CIFSR");
      if(!getOdgovor("OK", 1000))delay(1000);
      
    }else{
      serialControl.println(" ERROR");
      return 0;
    }
  }
  
  return 0;

}



int state = 0;
char *odgovoriConnected[2] = {"+IPD,", "CLOSED"}; //mozni odgovoi potem ko je ze povezan s serverjem
long zadnjiConnChk = 0;//cas ko smo zadnjickrat preverili povezavo
void loop(){
  
 
  switch(state){
    case -1:
      hardReset();
      
    case 0://prever ali je naprava sploh priklopljena 
      setStatusLed(5,10);
      preveriOdzivnost();

    
    case 1://preveri nastavitve naprave
      setStatusLed(1,5);
      serialControl.println("chk set.");
      if(!preveriNastavitve(5, serverSettings, 2)){
        serialControl.println("set. ERROR");
        state = -1;
        break;
      }
      serialControl.println("set. OK");
    
    case 2://poiskusi se povezati z AP sttreznikom
      setStatusLed(1,10);
      serialControl.println("try conn.");
      if(!connectToAP(5)){
        serialControl.println("conn. ERROR");
        state = -1;
        break;
      }
      serialControl.println("conn. OK");
  
    case 3://poraba se povezati na TCP streznik
      setStatusLed(1,1);
      serialControl.println("try tcp srv.");
      if(!connectToTCPServer(5)){
        serialControl.println("tcp srv. ERROR");
        state = -1;
        break;
      }
      serialControl.println("tcp srv. OK");
      state = 4;
  
    case 4://prebere mac, ali pa si ga izmisli
      serialControl.println("try get MAC");
      if(!getMacAdress(1)){
        serialControl.println("MAC ERROR");
        state = -1;
        break;
      }
      serialControl.println("get MAC OK");
      state = 5;
      
    case 5://pozdravi server
      setStatusLed(0, 1);
      //zacne posiljati
      if(!macExist){
        serialControl.println("MAC ne obstaja!");
        state = 4;
        break;
      }
      
      if(!posljiSporocilo(macStrSend)){
        //napaka pri posiljanju sporocila
        state = -1;
        break;
      }
      state = 6;
   
   case 6://poslusa server 
   
      //preveri ali je povezava sekar vspostavljena na vsake 15s
      if(millis() - zadnjiConnChk > 11000){
        zadnjiConnChk = millis();
        //TODO: najdi kaksen bolj zanesljiv nacin za testiranje povezave
        if(connectToTCPServer(1) && posljiSporocilo("chk")){
          //zakomentiran zato da prevec casa ne porab za izpisovanje
          //serialControl.println("conn. ok");
        }else{//izgubili smo povezavo !!
          serialControl.println("conn. lost");
          state = -1;
          break;
        }
      }
      
      //ce je tarca v echo nacinu poslje sporocilo v primeru da je bila zadeta
      if(tarcaEcho && tarcaScoreOld < tarcaScore){
          char scoreStr[3];
          itoa(tarcaScore - tarcaScoreOld, scoreStr, 10);
          posljiSporocilo(scoreStr);
          tarcaScoreOld = tarcaScore;
      }
      
      int odg = getOdgovorMulty(odgovoriConnected, 2, 500);
      if(odg == 0){//connected
        char *prejetMsg = getMessage(100);
        if(prejetMsg){
          serialControl.println();
          serialControl.print("msg:");
          serialControl.println(prejetMsg);
          
          zadnjiConnChk = millis();
          
          if(primerjajStr(prejetMsg, "odg")){//pogleda ali je tarca se kar odzivna
            posljiSporocilo("active");
            
          }else if(primerjajStr(prejetMsg, "mac")){//ponovno poslje svoj mac naslov
            posljiSporocilo(macStrSend);
            
          }else if(primerjajStr(prejetMsg, "on")){//prizge tarco za doloceno stevilo casa
            int cas = getSt(prejetMsg, "on%");
            serialControl.println(cas);
            tarcaSetOn(cas);
            
          }else if(primerjajStr(prejetMsg, "off")){//izklopi tarco
            tarcaSetOff();
            
          }else if(primerjajStr(prejetMsg, "score")){//prebere trenutne tocke
            char scoreStr[3];
            itoa(tarcaScore, scoreStr, 10);
            posljiSporocilo(scoreStr);
            
          }else if(primerjajStr(prejetMsg, "samoff")){//samodejno izklaplanje tarce ob zadetku "samodejno off"
            int value = getSt(prejetMsg, "samoff%");
            if(value == 0 || value == 1)samoOff = value;
            serialControl.print("samoOff set to ");
            serialControl.println(samoOff);
            
          }else if(primerjajStr(prejetMsg, "state")){
            if(tarcaState == ON)posljiSporocilo("ON");
            else posljiSporocilo("OFF");
            
          }else if(primerjajStr(prejetMsg, "echo")){//sama odgovori takoj ko je zadeta
            int value = getSt(prejetMsg, "echo%");
            if(value == 0 || value == 1)tarcaEcho = value;
            serialControl.print("tarcaEcho set to ");
            serialControl.println(tarcaEcho);
          }
        }
        else{
          serialControl.print("_");
        }
      }else if(odg == 1){//CLOSED
        serialControl.println(odgovoriConnected[odg]);
        state = 3;
        break;
      }else if(odg == -2){//v podanem casu ni nic prispelo
        //serialControl.print(".");
      }else{
        //nekaj neprepoznavnega smo prijeli 
        //serialControl.println("??");
      }

  }
}

//vrne stevilo ki ga najde v stringu s1
//*poz je pozicija stevila v besedilu s1
//poz je naprimer "visina%", funkcija bo poiskala stevilo ki je na mestu znaka %
//npr: s1="visina1234" bo funkcija vrnila 1234
int getSt(char *s1, char *poz){
  int st = 0;
  int state = 0;
  while(*s1 && *poz){
    switch(state){
      case 0:
        if(*poz == '%'){
          state = 1;
          break;
        }
        if(*s1 != *poz)return -2;
        s1++;
        poz++;
        break;
        
      case 1:
        if(*s1 >= '0' && *s1 <= '9'){
          st *= 10;
          st += *s1 - '0';
        }else{
          return st;
        }
        s1 ++;
        break;

    }
  }
  
  return st;
  
}
int primerjajStr(char *s1, char *s2){
  while(*s1 && *s2){
    if(*s1++ != *s2++)return 0;
  }
  return 1;
}


