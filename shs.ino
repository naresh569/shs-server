void initUsers();
void saveUsers();

#include <SPI.h>
#include <Ethernet.h>
#include "Config.hpp"
#include "ArduinoJson.h"
#include "Utilities.hpp"
#include "Switch.hpp"
#include "Block.hpp"
#include "Users.hpp"
#include "Storage.hpp"
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

void initUsers() {
  Storage* s = new Storage();
  char* users = s->get();
  
  StaticJsonBuffer<1536> jsonBuffer;
  JsonArray& ulist = jsonBuffer.parseArray(users);

  for (int i = 0; i < ulist.size(); i++) {
    User :: addUser(ulist[i]["userId"], ulist[i]["name"], ulist[i]["username"],  ulist[i]["pin"],  ulist[i]["deviceUUId"], ulist[i]["accessLevel"], ulist[i]["deactivated"]);
  }

  Serial.print(" > Total no. of users: ");
  Serial.println(User :: total);
}

void saveUsers() {
  Storage s;
  int count = 0;
  s.write("[");
  for (int i = 0; i < MAXUSERS; i++) {
    if (users[i] == NULL)
    continue;
    count++;
    s.write("{\"userId\":");
    s.write(users[i]->_id);
    s.write(",\"name\":\"");
    s.write(users[i]->name);
    s.write("\",\"username\":\"");
    s.write(users[i]->username);
    s.write("\",\"pin\":\"");
    s.write(users[i]->pin);
    s.write("\",\"deviceUUId\":\"");
    s.write(users[i]->deviceUUId);
    s.write("\",\"accessLevel\":");
    s.write(users[i]->accessLevel);
    s.write(",\"deactivated\":");
    if (users[i]->deactivated)
    s.write("true");
    else
    s.write("false");
    s.write('}');
    if (count != User :: total)
    s.write(',');
  }
  s.write(']');
  s.write("");
} 

void shsInitialize() {

  Serial.println(" > Initializing Users.. ");
  initUsers();

  Block* block1 = Block :: addBlock("Block 1", "Contains the fisrt set of 12 Switches..");
  block1->addSwitch('1',  2, "Main Light", "The CFL (Compact Fluroscent Light) Bulb located in the Hall center");
  block1->addSwitch('2',  3, "Fan", "Fan in the bedr room");
  block1->addSwitch('2',  3, "Fan", "Fan in the bedr room");
  block1->addSwitch('1',  4, "Light 1", "Light in the hall back");
  block1->addSwitch('1',  5, "Light 2", "Light in the hall front");
  block1->addSwitch('1',  6, "Bed Light", "Attached to right side wall of hall");
  block1->addSwitch('2',  7, "Fan 1", "Main fan in the hall just above the sofa");
  block1->addSwitch('2',  8, "Fan 2", "Main fan in the hall before stairs");
  block1->addSwitch('3', 14, "Socket 1", "Triple way plug socket in hall near sofa");
  block1->addSwitch('3', 15, "Socket 2", "Triple way plug socket in the bedroom");
  block1->addSwitch('3', 16, "Socket 3", "Triple way plug socket for heavey use");
  block1->addSwitch('3', 17, "Socket 4", "Triple way plug socket in the kichen");

}
  
void setup() {
  randomSeed(analogRead(0));

  Storage s;
  s.clear();

  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  Serial.println();
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