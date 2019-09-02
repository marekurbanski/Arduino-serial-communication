#include "connection.h" 
Connection comm;

String received_data = "";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  comm.send_delay = 100;

  //Setup PIN connections, this is set for Arduino UNO
  //Pin for send status signal
  comm.PIN_A = 8;
  comm.PIN_B = 2;
  comm.PIN_C = 4;
  
  //Pins to communicate, put then splitted via comma
  comm.PIN_COMM = "6,7";
  


}


void loop() {

  //comm.send_data("This is message from first Arduino");

  received_data = comm.receive_data();
  if(received_data != "")
    {
    Serial.println("This is what I received: '"+received_data+"'");
    Serial.println("This is what I received: '"+comm.byte2string(received_data)+"'");    
    }

  Serial.print(".");
  delay(100);
}
