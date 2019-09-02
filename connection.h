#ifndef Connection_h
#define Connection_h
#include "Arduino.h"

class Connection
  {
    public:
    String STATE = "none";
    String PIN_COMM; // pins array/string to communication between Arduino_A and Arduino_B (default 5) min 1, max unlimited
    int PIN_A = 3; // pin number (default 0) to communication, Arduino_A::PIN_A <-------> Arduino_B::PIN_A, communication pin, send
    int PIN_B = 1; // pin number (default 1) to communication, Arduino_A::PIN_B <-------> Arduino_B::PIN_B, data sended pin
    int PIN_C = 2; // pin number (default 2) to communication, Arduino_A::PIN_B <-------> Arduino_B::PIN_B, data received pin
    bool debug; // true,false
    bool send_data(String string_to_send); // call other arduino for connection
    String receive_data(); // this method returning received data
    byte send_delay; // delay between send data, min 0
    int SEND_LOOP_NO = 10000; // no try to send when error, timeout
    byte TIMEOUT_LOOP_NO = 10; // timeout for waiting for answer ( there is no clock so this is loop no)
    byte SEND_SLEEP_DATA = 10; // timeout [ms] for sleep to wait for answer
    int timeout; // timeout waitin for connection (in facts loops numbers, default 500 );
    bool check_if_ready_to_send();
    void Initialize();    
    Connection();
    void info(String str);
    String byte2string(String str);
    
    private:
    void stop_transmission();
    bool confirm_received();
    void set_state(String new_state);
    String str2bin(String str);
    String send_splited(String str);
    int get_array_count(String str);
    int get_array_item(String str, int number);
    
  };

#endif
