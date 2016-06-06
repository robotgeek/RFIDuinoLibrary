/***************************************************************************
 *        RFIDuino Demo - Flash Cards
 *   _________________    _________________  
 *  |   _))           |  |                 |
 *  |  > *\     _~    |  |                 |
 *  |  `;'\\__-' \_   |  |                 |
 *  |     | )  _ \ \  |  |                 |
 *  |    / / ``   w w |  |      GOAT       |
 *  |   w w           |  |                 |
 *  |                 |  |                 |
 *  |   ###########   |  |                 |
 *  |_________________|  |_________________|
 *
 * This demo uses the RFIDuino sheild, a Geekduino (or other Arduino Board)
 * and the RFIDuino library to scan for a matching set of tags. The RFIDuino
 * will start up with a Red light on the shield. Scanning the first tag will
 * set the mode to accept only the matching second tag, and shift the light 
 * on the Shield to Green. Upon matching the second card, the system will reset.
 * If an incorrect second card is swiped, the player will have 3 tries to match
 * the correct card. If the correct card is not swiped, the system will reset.
 *
 *
 * This demo supports multiple key tags. To add / remove tags, first change the 
 * #define NUMBER_OF_CARDS to match the number of card pairs you plan to have. Now
 * add/remove the lines to the lowerCaseTag and upperCaseTag arrays.
 * The code will take care of the rest.
 *
 * The RFIDuino library is compatible with both versions of the RFIDuino shield
 * (1.1 and 1.2), but the user must initialize the library correctly. See 
 * 'HARDWARE VERSION' instructions near the beginning of the code.
 *
 *
 * The RFIDuino Shield is designed to be used with 125khz EM4100 family tags. 
 * This includes any of the EM4102 tags sold by Trossen Robotics/ RobotGeek. 
 * The RFIDuino shield may not work with tags outside the EM4100 family
 *
 *
 * Links
 *    RFIDUino Getting Started Guide
 *      http://learn.robotgeek.com/getting-started/41-rfiduino/142-rfiduino-getting-started-guide.html
 *    RFIDUino Library Documentation
 *      http://learn.robotgeek.com/41-rfiduino/144-rfiduino-library-documentation.html
 *    RFIDUino Shield Product Page
 *      http://www.robotgeek.com/rfiduino
 *  
 *
 * User Output pins
 *    myRFIDuino.led1 - Red LED
 *    myRFIDuino.led2 - Green LED
 *    myRFIDuino.buzzer - Buzzer
 *
 ***************************************************************************/

#include <RFIDuino.h> //include the RFIDuino Library

/***************
* HARDWARE VERSION - MAKE SURE YOU HAVE SET THE CORRECT HARDWARE VERSION BELOW
* Version 1.1 has a 4-pin antenna port and no version marking
* Version 1.2 has a 2-pin antenna port and is marked 'Rev 1.2'
*****************/
//RFIDuino myRFIDuino(1.1);     //initialize an RFIDuino object for hardware version 1.1
RFIDuino myRFIDuino(1.2);       //initialize an RFIDuino object for hardware version 1.2

//Define the pins for our outputs
const int OPEN_TIME = 500;          //the time (in milliseconds) that the system will wait before changing modes
const int NUMBER_OF_CARDS = 27;     //number of cards that can be paired with another card.   

int lastTag;           //placeholder for which tag was swiped first.
int wrongCounter = 0;  //holds number of failed attempts to swipe the proper matching card.

byte tagData1[5];                   //Holds the ID numbers from the tag
byte tagData2[5];                   //Holds the ID numbers from the tag
volatile int playState = 0;         // 0 = stopped 1 = playing

//The following tags are the first set of tags the system will be expecting. These must be swiped first.
byte lowerCaseTag[NUMBER_OF_CARDS][5] ={
{128,0,72,35,76},   //a, Apple,       0
{128,0,72,75,111},  //b, Basketball,  1
{128,0,72,28,7},    //c, Cake,        2
{128,0,72,94,95},   //d, Duck,        3
{128,0,72,54,217},  //e, Elephant,    4
{128,0,72,38,53},   //f, Fish,        5
{128,0,72,102,167}, //g, Goat,        6
{128,0,72,15,87},   //h, House,       7
{128,0,72,19,166},  //i, Iguana,      8
{128,0,72,101,23},  //j, Jar,         9
{128,0,72,31,162},  //k, Kite,       10
{128,0,72,88,241},  //l, Lion,       11
{128,0,72,20,240},  //m, Mitten,     12
{128,0,72,90,77},   //n, Nest,       13
{128,0,72,30,148},  //o, Octopus,    14
{128,0,72,24,77},   //p, Pizza,      15
{128,0,72,32,18},   //q, Quilt,      16
{128,0,72,35,104},  //r, Rope,       17
{128,0,72,60,250},  //s, Sandwich,   18
{128,0,72,18,240},  //t, Television, 19
{128,0,72,20,163},  //u, Umbrella,   20
{128,0,72,41,107},  //v, Violin,     21
{128,0,72,99,13},   //w, Wagon,      22
{128,0,72,25,229},  //x, Xylophone,  23
{128,0,72,77,241},  //y, Yo-Yo,      24
{128,0,72,90,50},   //z, Zebra,      25
{128,0,72,41,110},  //lowercase,Void,26
};  

//The following tags are the second set of tags. These will be expected when the green light is lit, and must match with the first card.
byte upperCaseTag[NUMBER_OF_CARDS][5] ={
{128,0,72,75,152},  //A, Apron,       0
{128,0,72,70,93},   //B, Butterfly,   1
{128,0,72,14,0},    //C, Car,         2
{128,0,72,83,19},   //D, Dog,         3
{128,0,72,69,213},  //E, Easel,       4
{128,0,72,38,110},  //F, Fox,         5
{128,0,72,76,216},  //G, Guitar,      6
{128,0,72,45,220},  //H, Hammer,      7
{128,0,72,20,48},   //I, Ice Cream,   8
{128,0,72,93,166},  //J, Jump Rope,   9 
{128,0,72,16,183},  //K, Key,        10
{128,0,72,31,251},  //L, Lemon,      11
{128,0,72,33,237},  //M, Monkey,     12
{128,0,72,98,221},  //N, Nail,       13
{128,0,72,85,32},   //O, Overalls,   14
{128,0,72,82,189},  //P, Pig,        15
{128,0,72,73,118},  //Q, Quarters,   16
{128,0,72,50,141},  //R, Rabbit,     17
{128,0,72,62,112},  //S, Socks,      18
{128,0,72,10,52},   //T, Turtle,     19
{128,0,72,73,44},   //U, Unicorn,    20
{128,0,72,13,34},   //V, Vacuum,     21
{128,0,72,26,137},  //W, Watermelon, 22
{128,0,72,26,88},   //X, X-Ray,      23
{128,0,72,34,230},  //Y, Yarn,       24
{128,0,72,69,123},  //Z, Zipper,     25
{128,0,72,53,151},  //uppercase,Void,26
};  

boolean verifyKey1 = false;         //true when a verified tag has been determined to match the key tag, false otherwise
boolean verifyKey2 = false;         //true when a verified tag has been determined to match the key tag, false otherwise
int i;                              //global i variable

void setup()
{
  //The RFIDUINO hardware pins and user outputs(Buzzer / LEDS) are all initialized via pinMode in the library

  // start serial
  Serial.begin(9600);
  Serial.println("Starting Flash Cards - Alphabet");
  Serial.println("Swipe a lowercase card to begin");
}

void loop()
{   
  
    digitalWrite(myRFIDuino.led1,HIGH);         //turn red LED on. This indicates the first mode.
    //scan for a tag - if a tag is sucesfully scanned, return a 'true' and proceed
    if(myRFIDuino.scanForTag(tagData1) == true)
    {
      verifyKey1 = false;  //set verfiy key to false incase previous ones have made it true
    
      i = 0; //reset tag counter

      while(i<NUMBER_OF_CARDS && verifyKey1 == false)
      {
        verifyKey1 = myRFIDuino.compareTagData(tagData1, lowerCaseTag[i]);//run the comparetagData to check the tag just read against the 'key' tag we defined.
        lastTag = i; //save the last integer before moving to the next. Used to match the second tag to the first.
        i++;         
      }

      if(verifyKey1 == true)//if a 'true' is returned by comparetagData, the current read is a key tag
      {              
        digitalWrite(myRFIDuino.led2,HIGH);         //turn green LED on
        myRFIDuino.successSound();                  //Play a 'success' sound from the buzzer, 3 notes acsending 
        delay(OPEN_TIME);                           //the amount of time that the solenoid will remain open for
        digitalWrite(myRFIDuino.led2,LOW);          //turn the green LED off
        Serial.println("First Card Swiped!");
        Serial.print("It was Tag ");
        Serial.print(lastTag, DEC);                 //display the array number for the tag that was swiped
        Serial.println("\n");
        secondTag();                                //begin looking for a match
      }
  
      else //otherwise the tag is not a key tag
      {        
        Serial.println("No, use a lowercase card.");
        digitalWrite(myRFIDuino.led1,HIGH);         //turn red LED on
        myRFIDuino.errorSound();                    //Play a 'error' sound from the buzzer, 3 notes acsending 
        delay(OPEN_TIME);                           //wait /delay before turning the led off
        digitalWrite(myRFIDuino.led1,LOW);          //turn the red LED off        
      }
    }
}//end loop()

void secondTag()
{
  digitalWrite(myRFIDuino.led1,LOW);             //turn red LED off
  digitalWrite(myRFIDuino.led2,HIGH);            //turn green LED on. This indicates the second mode.
  Serial.println("Now find the Matching Card.");
  playState = 1;                                 //set playState to 1 as the sequence is now playing
  wrongCounter = constrain(wrongCounter, 0, 2);  //limit the wrongCounter to values between zero and two
  while(playState == 1)
  {
    //scan for a tag - if a tag is sucesfully scanned, return a 'true' and proceed
    if(myRFIDuino.scanForTag(tagData2) == true)
    {
      verifyKey2 = false;  //set verfiy key to false incase previous ones have made it true

      verifyKey2 = myRFIDuino.compareTagData(tagData2, upperCaseTag[lastTag]);//run the comparetagData to check the tag just read against the 'key' tag we defined.

      if(verifyKey2 == true)//if a 'true' is returned by comparetagData, the current read is a key tag
      {                       
        Serial.println("Match Found!");
        Serial.println("\n");
        digitalWrite(myRFIDuino.led2,HIGH);         //turn green LED on
        myRFIDuino.successSound();                  //Play a 'success' sound from the buzzer, 3 notes acsending 
        delay(OPEN_TIME);                           //the amount of time that the solenoid will remain open for
        digitalWrite(myRFIDuino.led2,LOW);          //turn the green LED off
        playState = 0;
        Serial.println("Pick a new lowercase card to begin again!");
        loop();
      }
  
      else//otherwise the tag is not a key tag
      {        
        if(wrongCounter == 2) //if the player has tried 3 times to match the tag, reset the counters and go back to first mode.
        {
          Serial.println("That's not it... Let's try something else!");
          digitalWrite(myRFIDuino.led1,HIGH);         //turn red LED on
          myRFIDuino.errorSound();                    //Play a 'error' sound from the buzzer, 3 notes acsending 
          delay(OPEN_TIME);                           //wait /delay before turning the led off
          digitalWrite(myRFIDuino.led1,LOW);          //turn the red LED off 
          wrongCounter = 0;                           //set the wrongCounter back to zero
          playState = 0;                              //set the playState back to zero
          Serial.println("Pick a new lowercase card to begin again!");
          loop();                                     //return to loop
        }
        else
        {
          wrongCounter++;                             //add one to wrongCounter
          Serial.println("That's not it... Try Again!");
          digitalWrite(myRFIDuino.led1,HIGH);         //turn red LED on
          myRFIDuino.errorSound();                    //Play a 'error' sound from the buzzer, 3 notes acsending 
          delay(OPEN_TIME);                           //wait /delay before turning the led off
          digitalWrite(myRFIDuino.led1,LOW);          //turn the red LED off        
        }
      }
    }  
  }  
}//end secondTag()
