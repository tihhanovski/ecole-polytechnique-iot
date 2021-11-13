#include <iostream>

const char* msg = "setDelay 123";
const char* CMD_SETDELAY = "setDelay";
const size_t CMD_SETDELAY_LENGTH = 8;

using namespace std;

int main() {
    
    const char* test = "proov1\0proov2\0proov3\0"
    
    
    if(!memcmp(msg, CMD_SETDELAY, CMD_SETDELAY_LENGTH)) {
        cout << "new delay req\n";
        
        int i = atoi(msg + CMD_SETDELAY_LENGTH);
        
        cout << "i = " << i << "\n";
        
        
      
        /*
        unsigned long newDelayRequested = messageTemp.substring(9).toInt();
        if(newDelayRequested > MAX_TIME_BETWEEN_PUBS_SEC) {
        Serial.print("delay ");
        Serial.print(newDelayRequested);
        Serial.print(" is too long, max ");
        Serial.print(MAX_TIME_BETWEEN_PUBS_SEC);
        requestBlink = true;
        }
        */
    }
    
    return 0;
}