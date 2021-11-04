// Connecting to thingspeak server
#include <ThingSpeak.h>;
unsigned long myChannelNumber = 1559619; //Your Channel Number (Without Brackets)
const char * myWriteAPIKey = "2J2UTY8NUZ81BYO4"; //Your Write API Key

// Other important libraries
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include<Keypad.h>

// Defining the Keypad matrix
const byte n_rows = 4;
const byte n_cols = 4; 

int flag = 0;
String data = "---------"; 
String result;

char keys[n_rows][n_cols] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
}; 

byte colPins[n_rows] = {D4, D3, D2, D8};
byte rowPins[n_cols] = {D7, D6, D5, D9}; 

Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);  

// Connecting to Firebase server
#define FIREBASE_HOST "tictactoe-c6682-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "tvs0QRZLP6AqdaAOl4RcYOMo0uRgGx9BkdUG9BBR"

// Connecting to the Internet
#define WIFI_SSID "Y9_2019"
#define WIFI_PASSWORD "10201020"

WiFiClient client;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

// Connecting to WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP()); 

// Initializing the Data to default values
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString("Matrix",data);
  Firebase.setInt("Player 1", flag);
  Firebase.setInt("Player 2", flag);
  Firebase.setString("Winner","-");

  ThingSpeak.begin(client);

  pinMode(D0,OUTPUT);
}


void loop() {

// Input the Key pressed
  char myKey = myKeypad.getKey();
  
  if (myKey != NULL){    
    Serial.print("Key pressed:");
    Serial.println(myKey);
    int key = myKey - '0';

// Alternate play due to single device developed
    if(flag%2 == 0){

// Update the Firebase and Thingspeak database
      Firebase.setInt("Player 1", key);
      data[key-1] = 'x';
      
      ThingSpeak.setField(1, key);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

// Check the status, Thingspeak might face errors to update. So wait for sometime.
      while(x!=200){
        Serial.println("Problem updating channel.Trying to connect, pls wait");
        ThingSpeak.setField(1, key);
        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      }
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      
    }

// Alternate play due to single device developed    
    else{

// Update the Firebase and Thingspeak database
      Firebase.setInt("Player 2", key);
      data[key-1] = 'o';
      
      ThingSpeak.setField(2, key);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);


// Check the status, Thingspeak might face errors to update. So wait for sometime.
      while(x!=200){
        Serial.println("Problem updating channel.Trying to connect, pls wait");
        ThingSpeak.setField(2, key);
        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      }
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      
    }

// Update the matrix on Firebase
    flag = flag + 1;
    Firebase.setString("Matrix",data);
    result = Firebase.getString("Matrix");

// Print the Matrix to visualize the game
    Serial.print(result[0]);Serial.print(result[1]);Serial.println(result[2]);
    Serial.print(result[3]);Serial.print(result[4]);Serial.println(result[5]);
    Serial.print(result[6]);Serial.print(result[7]);Serial.println(result[8]);

// Checking for Player 1 victory    
    if (result[0] == 'x' and result[1] == 'x' and result[2] == 'x' or
        result[3] == 'x' and result[4] == 'x' and result[5] == 'x' or
        result[6] == 'x' and result[7] == 'x' and result[8] == 'x' or
        result[0] == 'x' and result[3] == 'x' and result[6] == 'x' or
        result[1] == 'x' and result[4] == 'x' and result[7] == 'x' or
        result[2] == 'x' and result[5] == 'x' and result[8] == 'x' or
        result[0] == 'x' and result[4] == 'x' and result[8] == 'x' or
        result[2] == 'x' and result[4] == 'x' and result[6] == 'x' ){
          
      Serial.println("Game over");
      Serial.println("Player 1 wins");
      digitalWrite(D0,HIGH);
      
// Update the game status on firebase and thingspeak server
      Firebase.setString("Matrix","---------");
      data = "---------"; 
      result = "---------"; 
      Firebase.setInt("Player 1", 0);
      Firebase.setInt("Player 2", 0);
      Firebase.setString("Winner", "Player 1");
      
      ThingSpeak.setField(3, 1);

      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      while(x!=200){
        Serial.println("Problem updating channel.Trying to connect, pls wait");
        ThingSpeak.setField(3, 1);
        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      }
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      digitalWrite(D0,LOW);
    }

// Checking for Player 2 victory
    else if (result[0] == 'o' and result[1] == 'o' and result[2] == 'o' or
        result[3] == 'o' and result[4] == 'o' and result[5] == 'o' or
        result[6] == 'o' and result[7] == 'o' and result[8] == 'o' or
        result[0] == 'o' and result[3] == 'o' and result[6] == 'o' or
        result[1] == 'o' and result[4] == 'o' and result[7] == 'o' or
        result[2] == 'o' and result[5] == 'o' and result[8] == 'o' or
        result[0] == 'o' and result[4] == 'o' and result[8] == 'o' or
        result[2] == 'o' and result[4] == 'o' and result[6] == 'o' ){
          
      Serial.println("Game over");
      Serial.println("Player 2 wins");
      
// Update the game status on firebase and thingspeak server      
      Firebase.setString("Matrix","---------");
      data = "---------"; 
      result = "---------"; 
      Firebase.setInt("Player 1", 0);
      Firebase.setInt("Player 2", 0);
      Firebase.setString("Winner", "Player 2");
      ThingSpeak.setField(3, 2);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      while(x!=200){
        Serial.println("Problem updating channel.Trying to connect, pls wait");
        ThingSpeak.setField(3, 2);
        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      }
      if(x == 200){
        Serial.println("Channel update successful.");
      }
    }

// Checking for a draw
   else if(result[0] != '-' and result[1] != '-' and result[2] != '-' and
        result[3] != '-' and result[4] != '-' and result[5] != '-' and
        result[6] != '-' and result[7] != '-' and result[8] != '-'){
          
          Serial.println("Game over");
          Serial.println("Its a Draw :|");

// Update the game status on firebase and thingspeak server          
          Firebase.setString("Matrix","---------");
          data = "---------"; 
          result = "---------"; 
          Firebase.setInt("Player 1", 0);
          Firebase.setInt("Player 2", 0);
          Firebase.setString("Winner", "Draw");
          ThingSpeak.setField(3, 0);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      while(x!=200){
        Serial.println("Problem updating channel.Trying to connect, pls wait");
        ThingSpeak.setField(3, 0);
        x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      }
      if(x == 200){
        Serial.println("Channel update successful.");
      }
   }
  } 
}
