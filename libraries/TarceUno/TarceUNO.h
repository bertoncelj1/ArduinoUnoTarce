//v 1.08 A.B.

#ifndef tarceUNO_h
#define tarceUNO_h

#define BESEDILO 0
#define NASLOV 1

#define ZNAK_DESNO  126
#define ZNAK_LEVO   127

#define ZVOK_OFF  0
#define ZVOK_LOW  1
#define ZVOK_MED  2
#define ZVOK_MAX  3

#define btnUP     0
#define btnDOWN   1
#define btnLEFT   2
#define btnRIGHT  3
#define btnSELECT 4

//serijska more bit sklopljena ce hocemo da dela zvok 
#define PISKAC_PIN_JAKOST1  1
#define PISKAC_PIN_JAKOST2  2
//vezava pinov za zvok:
//pin1 je prek 200 Ohm vezan na piskacev plus
//pin2 je prek 500 Ohm vezan na piskacev plus
#define GUMBI_PIN    A0
#define SENZOR1_PIN  A1
#define SENZOR2_PIN  A2
#define SENZOR3_PIN  A3
#define MORTOR1_PIN  13
#define MORTOR2_PIN  12
#define MORTOR3_PIN  11


#define VEDNO_ON  2 //uporablja se da je tarca skoz przgana
#define ON  1
#define OFF  0

#define MAX_EKRANOV	25
//ekrani in dejanska stanja
#define MENI_GLAVNI 	1
#define MENI_IGRAJ 		2
#define MENI_INFO 	    3
#define MENI_MADE_BY	4
#define MENI_NASTAVITVE 5
#define MENI_PROG1 		6
#define MENI_PROG2 		7
#define MENI_PROG3 		8
#define NAST_MOTOR1     9
#define NAST_MOTOR2     10
#define NAST_MOTOR3 	11
#define NAST_SENZORJI 	12
#define PROGRAM_1		13
#define PROGRAM_2		14
#define PROGRAM_3		15
#define NAST_PROG1 		16
#define NAST_PROG2 		17
#define NAST_PROG3 		18
#define ODSTEVAJ		19
#define NAST_EDIT   	20// ja ta oken ... 
//fora tega okna je d aje edin oken ki uporablja inicializacijo .. pac da prebere podatke iz spomina pa nalozi def vrednosti pa to
//in ta inicalizacija je shranjena kr pod "ukazi", kar pomen da obstaja v switchu ukazi NAST_EDIT, kjer naredi omenjeno inicalizacijo
//posledica tega je da se obnasa kot ekran in ukaz hkrati kar je ZELO slabo in motece. Zaeenkrat je ta problem resen tako da je dodan
//pogoj if(ukaz < MAX_EKRANOV && ukaz != NAST_EDIT), ki gre izvajat ukaze tudi ce je st manjsa od MAX_EKRANOV.
//uprašanje: zakaj mu ne dodelim vecje stevilke? O:ce je st > MAX_EKRANOV se ne bo pravilno shranila v arr "vsi_Ekrani"
//ta problem se bi dal resit tako da bi inicalizacijo premaknil drugam, naprimer kar v glaven switch stavek
//ali pa da ze nardim poljso fucking state masino !
//to bo vse

//ukazi
#define INC_IZBRAN        30
#define DEC_IZBRAN        31
#define NAST_EDIT_DEC     32
#define NAST_EDIT_INC     33
#define NAST_EDIT_FINISH  34 
#define STEJ_PRITISKE     35 
#define PROG1_KONEC		  36
#define PROG2_KONEC		  37
#define PROG3_KONEC		  38


//spomin
#define MAX_SPOMIN			20 //velikost arr v katerega se prepišejo podatki 
#define ST_SPOMINA 		 	12		//dejansko st spremenljivk


#define MEM_PRVIC			123	//na to mesto shrani ali je biu prvic prizgan
#define VARNOSTNA_ST 		212 //stevilka s katero preverja ali je v spominu vse OK
#define VARNOSTNA_ST_STR 	"212"

#define MEM_MOTOR_1_MIN        0
#define MEM_MOTOR_1_MAX        1
#define MEM_MOTOR_2_MIN        2
#define MEM_MOTOR_2_MAX        3
#define MEM_MOTOR_3_MIN        4
#define MEM_MOTOR_3_MAX        5
#define MEM_SENSOR    	   	   6
#define MEM_PROG1_CAS  	   	   7
#define MEM_PROG2_INTERVAL 	   8
#define MEM_PROG2_CAS  	   	   9
#define MEM_PROG3_INTERVAL	   10
#define MEM_PROG3_CAS  	   	   11

//dejanski indexi shranjenih parametrov
#define INDEX_MOTOR_1_MIN        0
#define INDEX_MOTOR_1_MAX        1
#define INDEX_MOTOR_2_MIN        2
#define INDEX_MOTOR_2_MAX        3
#define INDEX_MOTOR_3_MIN        4
#define INDEX_MOTOR_3_MAX        5
#define INDEX_SENSOR    	   	   6
#define INDEX_PROG1_CAS  	   	   7
#define INDEX_PROG2_INTERVAL 	   8
#define INDEX_PROG2_CAS  	   	   9
#define INDEX_PROG3_INTERVAL	   10
#define INDEX_PROG3_CAS  	   	   11
	

	
#define FUN_TIPKE 200

#define MAX_PARAMETROV 2 // kolk parametrov je lahko v posameznem oknu ekranNastavitve

#define d_retState   	 (((s_ekranNastavitveEdit*)vsiEkrani[NAST_EDIT])->returnState)		//int v katermu je shranjen ret state
#define d_tretIzbran	 (((s_ekranNastavitve*)vsiEkrani[d_retState])->trenutnoIzbran)		//ekran[ret_state] trenutno izbrani element
#define d_indexParametra (((s_ekranNastavitve*)vsiEkrani[d_retState])->indexi[d_tretIzbran])//od trenutno izbranega elementa dobi index parametra
#define d_parameter  	 (parametri[d_indexParametra])										//parameter




typedef struct
{
char *besedilo;
uint8_t funkcija;
}s_izbirnaVrstica;



typedef struct 
{
char* naslov;
uint8_t tipke[5];
uint8_t st_elementov;
uint8_t trenutnoIzbran;
s_izbirnaVrstica elementi[5];
}s_ekran;

typedef struct 
{
char* opis;
uint16_t min;
uint16_t max;
int16_t vrednost; // def vrednost-> uporablja samo takrat ce je v spominu shranjena napacna vrednost in takrat ko se ureja na ekranu eddit
uint8_t  korak;
}s_parameter;


typedef struct 
{
char* naslov;
uint8_t tipke[5];
uint8_t stParametrov;
uint8_t trenutnoIzbran;
uint8_t focus;
uint8_t indexi[MAX_PARAMETROV]; //indexi parametrov ki so shranjeni v areju "spomin"
}s_ekranNastavitve;

typedef struct 
{
char* naslov; //nima ga
uint8_t tipke[5];
uint16_t returnState;
}s_ekranNastavitveEdit;

typedef struct 
{
char* naslov;
uint8_t tipke[5];
}s_ekranProgram;


typedef struct 
{
uint8_t stanje;
uint16_t cnt;
uint8_t tocke;
Servo motor;
}s_tarca;

struct koncniNapisi 
{
  char *besedilo;
  uint16_t zamik;
};

typedef struct 
{
char* naslov;
uint8_t tipke[5];
}s_ekranOdstevaj;

typedef struct 
{
uint8_t state;
uint32_t mills;
uint8_t stOdstevanj;
uint8_t returnState;

}s_odstevajInit;

void odstevaj();//uint8_t stOdstevanj, void (*initProgram)());
#endif
