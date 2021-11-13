#include <EEPROM.h>

#define EEPROM_SIZE 255

class Storage {
public:
  char buffer[EEPROM_SIZE];
  char* wiFiSSID;
  char* wiFiPassword;
  char* timeBetweenPubs;
  char* mqttServer;
  char* mqttPort;
  
};
