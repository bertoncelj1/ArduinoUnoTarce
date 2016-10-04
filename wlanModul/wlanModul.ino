#include <SoftwareSerial.h>


// software serial #1: TX = 10, RX = 11
SoftwareSerial softwareSerial(10,11);

//serial ki se uporabla za posiljat ukaze nanotu
#define serialControl softwareSerial

//serial ki se uporablja za komunikacijo z wlan modulom
#define serialWlan Serial

long serialWlanBaud = 115200;
int resetPin = 13;

void setup()
{
    // initialize serial:
    serialControl.begin(9600);
    serialControl.println("I'm ready !");

    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);

    serialWlan.begin(serialWlanBaud);

}
char vhod[100];   //sem notr shranja vhod dokler ne pride ukaz \n ali \r
int indexVhod = 0; //index do arreja vhod
int izpisiNL = 0; // ce je to 1 bo izpisal New line
int ukazSend = 0; // izvrsen je bil ukaz, to je zato da se poj nakonc doda delay
long zadnjiUkaz = 0;
long zakasnitev = 1000; // 1s zakasnitve

//vse ukaze shrani v ta arr in jih poj po vrst izvaja
char *ukazArr[30];
int ukazArrIndex = 0;

//mesto kamor se sghrani prebrano sporocilo ki ga je posal modul
char prebrano[1000];

void loop()
{

    // if there's any serial available, read it:
    while (serialControl.available() > 0){

        // look for the next valid integer in the incoming serial stream:
        //sPrint("indexVhod: ",indexVhod);
        vhod[indexVhod] = serialControl.read();
        //serialControl.write(vhod[indexVhod]);
        //ce je special znak ga ne izpise
        if(vhod[indexVhod] != 27){
          serialControl.print(vhod[indexVhod]);
        }
        //sPrint("vhod[indexVhod] = ", vhod[indexVhod-1]);
        // do it again:

        // look for the newline. That's the end of your
        // sentence:
        if (vhod[indexVhod] == '\n' || vhod[indexVhod] == '\r'){
            if (vhod[indexVhod] == '\r')izpisiNL = 0;
            if (vhod[indexVhod] == '\n')izpisiNL = 1;
            //ce je vnos dolg natancno 1 znak: t.j. znak brez \n

            if(indexVhod == 1){
                //sPrint("program st. ", vhod[0]);
                switch (vhod[0]){
                case '1':
                    ukaz("AT");
                    break;
                case '2':
                    //pokaze vidne wlane
                    ukaz("AT+CWLAP");
                    break;
                case '3':
                    //izpise IP
                    ukaz("AT+CIFSR");
                    break;
                case '4':
                    //izpise status
                    ukaz("AT+CIPSTATUS");
                    //2: Got IP
                    //3: Connected
                    //4: Disconnected
                    break;
                case '5':
                    //delno izpise ukaz za poslati
                    ukaz("AT+CIPSEND=0,");
                    break;
                case '6':
                    //verzija modula
                    ukaz("AT+GMR");
                    break;
                case '7':
                    //poslje serverju get ukaz
                    getUkaz();
                    break;
                case 'r':
                    //resetira
                    ukaz("AT+RST");
                    break;
                case 'f':
                    //resetira ga s pinom
                    serialControl.println("full reseting!");
                    digitalWrite(resetPin, LOW);
                    delay(1000);
                    digitalWrite(resetPin, HIGH);
                    delay(500);
                    
                    break;
                case 'c':
                    //poveze se na poljcha
                    //ukaz("AT+CWMODE=1"); // je ze po def
                    ukaz("AT+CIPMUX=1");
                    ukaz("AT+CWJAP=\"Poljch\",\"kwatebr1ga123\"");
                    break;
                case 's':
                    //poveze se na server
                    //ukaz("AT+CIPSTART=0,\"TCP\",\"192.168.1.14\",80");
                    ukaz("AT+CIPSTART=0,\"TCP\",\"google.com\",80");
                    break;
                case 'd':
                    //prekine povezavo s serverjem
                    ukaz("AT+CIPCLOSE=0");
                    break;
                default:
                    //pobrisat more komando \n z niclo zato ne povecuje indexsa
                    vhod[indexVhod] = '\0';
                    ukaz(&vhod[0]);
                    break;

                }
            }
            else{
                if(vhodZacneZ("setbr")){
                    long in = readInt(5);
                    serialControl.println();
                    serialControl.print("baud rate ->'");
                    serialControl.print(in);
                    serialControl.print("', ");
                    if(in > 0){
                        serialWlan.end();
                        serialWlanBaud = in;
                        serialWlan.begin(serialWlanBaud);
                        serialControl.println("OK");
                    }
                    else{
                        serialControl.println("napacen vnos");
                    }
                }
                else if(vhodZacneZ("readbr"))
                {
                    serialControl.println();
                    serialControl.print("baud rate:'");
                    serialControl.print(serialWlanBaud);
                    serialControl.println("'");
                }
                else{
                    //pobrisat more komando \n z niclo zato ne povecuje indexsa
                    vhod[indexVhod] = '\0';
                    ukaz(&vhod[0]);
                }
            }
            // -1 zato ker ga odspodi poveca za 1 in ga poj sprav na 0
            indexVhod = -1;

        }
        else if(vhod[indexVhod] == 27){   // escape character kateremu sledijo posebni znaki
            //serialControl.println("special");
            
            //zato da zbriše zapis 27
            indexVhod--;
            
            //za tem ukazu sledita vedno dva znaka
            int znakiIndex = 0;
            char znaki[2];
            //na posebne znake caka samo 1s
            long zacetekPoslusanja = millis();
            while(znakiIndex < 2 && (millis() - zacetekPoslusanja < 1000)){
              if(serialControl.available() > 0){
                znaki[znakiIndex] = serialControl.read();
                znakiIndex++;
              }
            }
            if(znakiIndex == 2){
              switch(znaki[0]){
                case 91:
                  switch(znaki[1]){
                    case 65://gor
                      
                      break;
                    
                    case 66://dol
                      
                      break;
                      
                    case 67://levo
                      
                      break;
                      
                    case 68://desno
                      
                      break;
                    
                    default:
                      serialControl.print("neznan special:");
                      serialControl.print((unsigned int)znaki[0]);
                      serialControl.print(", ");
                      serialControl.println((unsigned int)znaki[1]);
                  }
               break;
                  
               default:
                  serialControl.print("neznan special:");
                  serialControl.print((unsigned int)znaki[0]);
                  serialControl.print(", ");
                  serialControl.println((unsigned int)znaki[1]);
               }
            }
            
            


        }
        else if(vhod[indexVhod] == 8){  // pogleda ali je biu pritisnjen backspase '8'
            //zmanjsa index za 2 prvic za backspace drugic pa za pobrisano crko
            if(indexVhod > 0)
            {
                indexVhod -= 2;
                //namesto pobrisane crke izpise presledek
                serialControl.print(' ');
                //na kuncu se izpise backspace zato da kazalec premakne na izpisan presledek
                serialControl.write(8);
            }

        }
        indexVhod ++;
    }


    //pogleda ce ma wlan modul kej za povedat
    //serialWlan.listen();
    int dolzinaVhoda = 0;
    long lastRead = 0;
    if((dolzinaVhoda = serialWlan.available()) > 0){


        lastRead = millis();
        //nastavi pomozn pointer na zacetek arreja
        char *p_prebrano = prebrano;
        //bere tolk cajt k lahko
        //preden neha brat more minit 50 ms od tazadnga ukaza
        while(millis() - lastRead < 50){
            while (serialWlan.available() > 0){
                *p_prebrano++ = serialWlan.read();
                lastRead = millis();
                //serialControl.write(serialWlan.read());
            }
        }
        //while((*p_prebrano++ = serialWlan.read()) != -1);
        //Serial.print(dolzinaVhoda);
        /*while (dolzinaVhoda -- || (dolzinaVhoda = serialWlan.available()) > 0) {
            *p_prebrano++ = serialWlan.read();
        }*/

        //na koncu prebranega doda nulo
        *p_prebrano = 0;
        //postavi pointer neazaj na zacetek
        p_prebrano = prebrano;
        //izpisuje tok cajta dokler ne naleti na nic
        while(*p_prebrano){
            serialControl.write(*p_prebrano++);
        }

        /*
        delay(1);
        serialControl.write('X');
        */
    }

    //po izvedenem ukazu pocaka 1s
    //izvede ukaz
    while(ukazArrIndex && millis() - zadnjiUkaz > zakasnitev){
        zadnjiUkaz = millis();
        ukazArrIndex --;
        serialControl.println();
        serialControl.print("ukaz:'");
        serialControl.print(ukazArr[ukazArrIndex]);
        serialControl.println("'");
        serialWlan.println(ukazArr[ukazArrIndex]);
    }

}

//preveri ce se vhod zacne z nizom znakov
int vhodZacneZ(char *c)
{
    //pogleda ali je c prazen
    if(*c == 0 || indexVhod == 0)return 0;
    
    int i = 0;
    while(*c && indexVhod > i){
        if(vhod[i] != *c)return 0;
        c ++;
        i ++;
    }

    return 1;
}

//prebere int iz vhoda
//brati zacne sele pri shift znaku
//vse presledke pred sporocilom ignorira
long readInt(int shift)
{
    int i = shift;
    //ignorira vse presledke
    while(vhod[i] == ' ' && i < indexVhod)i++;

    //bere vhod tolk cajta dokler je na vhodu cifra
    int stevke[10];
    int indexStevke = 0;
    while(vhod[i] >= '0' && vhod[i] <= '9' && i < indexVhod){
        stevke[indexStevke] = vhod[i] - '0';
        indexStevke ++;
        i++;
    }

    //dobljeno cifro shrani v long
    long stevilo = 0;
    long dst = 1;
    while(indexStevke --){
        stevilo += stevke[indexStevke] * dst;
        dst *= 10;
    }

    return stevilo;

}

//shrani ukaz na stack
void ukaz(char* ukaz)
{

    ukazArr[ukazArrIndex] = ukaz;
    ukazArrIndex ++;
    /*
    serialWlan.println(ukazArr[0]);
    Serial.println("Serial:");
    Serial.println(ukazArr[0]);
    Serial.println("END");
    */
    /*
    if(izpisiNL){
      serialWlan.println(ukaz);
    }else{
      serialWlan.print(ukaz);
    }
    ukazSend = 1;
    */
}
void getUkaz()
{
    String page = "/?gws_rd=cr&ei=sk33VOzHE4nnUsnNg6AJ";
    String host = "www.google.si";
    String port = "80";
    String http_req = "GET " + page + " HTTP/1.1\r\nHost: " + host + ":" + port + "\r\n\r\n";
    String cmd = "AT+CIPSEND=0,";
    cmd = cmd + http_req.length();

    serialWlan.println(cmd);
    delay(2000);
    serialWlan.print(http_req);
    ukazSend = 1;

}
/*
void sPrintLong(long l){
  if(l == 0){
    serialControl.print(0);
    return;
  }

  while(l >= 1){
    serialControl.print(l%10);
    l /= 10;
  }
}
*/
void sPrint(char* c, long a)
{
    serialControl.print(c);
    serialControl.println(a);
}








