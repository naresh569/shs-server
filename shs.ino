
#include <SPI.h>
#include <Ethernet.h>
#include "Config.hpp"
#include "ArduinoJson.h"
#include "Utilities.hpp"
#include "Switch.hpp"
#include "Block.hpp"
#include "Users.hpp"
#include "Sessions.hpp"
#include "HTTP.hpp"
#include "Actions.hpp"

EthernetServer server(SERVER_PORT);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 90);

void shsInitialize() {

  // User :: addUser("Dharani Dhanasekhara", "dharanij2", "123456", "c60aa0446a1484e0");

  /* 
  addSwitch(
    Switch Type - Char
    Assigned Pin - Number
    Title - String
    Desc - String
  )
   */
  
  Block* block1 = Block :: addBlock("Block 1", "Contains the fisrt set of 12 Switches..");
  block1->addSwitch('1',  2, "Main Light", "The CFL (Compact Fluroscent Light) Bulb located in the Hall center");
  block1->addSwitch('2',  3, "Fan", "Fan in the bedr room");
  block1->addSwitch('2',  3, "Fan", "Fan in the bedr room");

  Block* block2 = Block :: addBlock("Block 2", "Contains the second set of 12 Switches..");
  block2->addSwitch('1',  4, "Light 1", "Light in the hall back");
  block2->addSwitch('1',  5, "Light 2", "Light in the hall front");
  block2->addSwitch('1',  6, "Bed Light", "Attached to right side wall of hall");
  block2->addSwitch('2',  7, "Fan 1", "Main fan in the hall just above the sofa");

  Block* block3 = Block :: addBlock("Block 3", "Contains the third set of 12 Switches..");
  block3->addSwitch('2',  8, "Fan 2", "Main fan in the hall before stairs");
  block3->addSwitch('3', 14, "Socket 1", "Triple way plug socket in hall near sofa");
  block3->addSwitch('3', 15, "Socket 2", "Triple way plug socket in the bedroom");
  block3->addSwitch('3', 16, "Socket 3", "Triple way plug socket for heavey use");
  block3->addSwitch('3', 17, "Socket 4", "Triple way plug socket in the kichen");
  block3->addSwitch('2', 18, "Water Motor", "Will pump the water from ground tank to top tank");

  Block :: addBlock("Block 4", "Contains the forth set of 12 Switches..");

}
  
void setup() {

  randomSeed(analogRead(0));

  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  Serial.println(" > Initializing SHS.. ");
  shsInitialize();

  // Start the ethernet connection..
  Serial.println(" > Initializing Ethernet.. ");
  Ethernet.begin(mac, ip);

  // Start the webserver
  Serial.println(" > Initializing Web Server.. ");
  server.begin();
  Serial.print(" > Server is running at ");
  Serial.print(Ethernet.localIP());
  Serial.print(":");
  Serial.print(SERVER_PORT);
  Serial.println("..");
  
}

void loop() {
  // Connectivity - webserver
  EthernetClient client = server.available();
  if (client) {
    HTTP http(&client);
    client.stop();
  }
}
