
#include "Arduino.h"
#include "connection.h"

Connection::Connection()
  {
    Initialize();    
  }

void Connection::Initialize()
  {
    set_state("initializing");
    send_delay = 0;
    debug = true;
    PIN_A = 0;
    PIN_B = 1;
    //set all used PIN as LOW
    stop_transmission();
  }


void Connection::stop_transmission()
  {
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_C, OUTPUT);
    digitalWrite(PIN_A, LOW);
    digitalWrite(PIN_B, LOW);    
    digitalWrite(PIN_C, LOW);    
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);
    pinMode(PIN_C, INPUT);
    int c = get_array_count(PIN_COMM);
            for(int z=0; z<c; z++)
          {
            info("Set PIN: "+String(get_array_item(PIN_COMM,z))+" as OUTPUT and LOW");
            pinMode(get_array_item(PIN_COMM,z), OUTPUT);       
            digitalWrite(get_array_item(PIN_COMM,z), LOW);  
            pinMode(get_array_item(PIN_COMM,z), INPUT);       
          }

  }

String Connection::receive_data()
  {
    int c = get_array_count(PIN_COMM);
    String received = "";    
    
    pinMode(PIN_A, OUTPUT);
    digitalWrite(PIN_A, LOW);
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);

    if(digitalRead(PIN_B) == HIGH)
      {
      for(int z=0; z<c; z++)
        {
          info("Set PIN: "+String(get_array_item(PIN_COMM,z))+" as INPUT");
          pinMode(get_array_item(PIN_COMM,z), INPUT);          
        }
        
      pinMode(PIN_C, OUTPUT);
      digitalWrite(PIN_C, LOW);
      info("I found that someone is trying to send data");          

      while(digitalRead(PIN_A) == HIGH)
        {
  info("OK");
          for(int z=0; z<c; z++)
            {
              info(",");
              if(digitalRead(get_array_item(PIN_COMM,z)) == HIGH)
                {
                info("Get PIN: "+String(get_array_item(PIN_COMM,z))+" = HIGH (1)");   
                received = received + "1"; 
                }
              else
                {
                info("Get PIN: "+String(get_array_item(PIN_COMM,z))+" = LOW (0)");    
                received = received + "0";
                }
            }

        info("I will tell him that I received data");
        pinMode(PIN_C, OUTPUT);
        digitalWrite(PIN_C, HIGH);
        delay(500);
        digitalWrite(PIN_C, LOW);
        check_if_ready_to_send();
          {
            //return "ERROR to resend";
          }
            
        // check if transmission is complete ?
        if(digitalRead(PIN_A) == LOW)  
          {
            //set all used PIN as LOW
            stop_transmission();
            //return byte2string(received);
            return received;
          }
        }        
     }
    //else
    //  {
    //    info("There is no status for receiving data, exiting"); 
    //    //set all used PIN as LOW
    //    stop_transmission();
    //  }
  }

bool Connection::send_data(String string_to_send)
  {
    info("Trying to connect...");
    set_state("trying_connect");

    for(int i=0; i<get_array_count(PIN_COMM); i++)
      {
        info("PIN to communicate: "+String(get_array_item(PIN_COMM, i)));
      }

    pinMode(PIN_A, INPUT);
    info("Checking PIN_A");
    
    if(digitalRead(PIN_A) == HIGH)
      {
        set_state("someone_is_sending_data");
        info("Checking PIN_A = HIGH");
        info("Cant't connect to send data, other Arduino is sending data");
        info("Changing status to receive");
        receive_data();
      }
    else
      {
      pinMode(PIN_A, OUTPUT);
      digitalWrite(PIN_A, HIGH);
        
      // sending data
      info("PIN_A has state HIGH");
      for(int i=0; i<SEND_LOOP_NO; i++)
        {
        info(i+" try to send package");  
        delay(SEND_SLEEP_DATA);
          if(send_splited(string_to_send) == "OK")
            {
            info("Communicate ended correct");  
            set_state("data_sended");            
            }
          else
            {
            info("Sending Timeout");  
            set_state("error_sended");  
            }            
          }          
        }
      if(STATE == "data_sended")
        {
          //set all used PIN as LOW
          stop_transmission();
          set_state("correct_send");  
          info("Communicate ended correct");
        }
      else
        {
          //set all used PIN as LOW
          stop_transmission();
          set_state("error_send");
          
        }
      
  }


bool Connection::check_if_ready_to_send()
  {
    //pinMode(PIN_B, INPUT);
    
    for(int wait_for_answer=0; wait_for_answer<=TIMEOUT_LOOP_NO; wait_for_answer++)
      {
      if(digitalRead(PIN_B) == HIGH)
        {
        return true;        
        }      
      delay(100);  
      }
    set_state("error_wait_for_answer_timeout");  
    info("error_wait_for_answer_timeout");  
    return false;  
  }
    

bool Connection::confirm_received()
  {
    //pinMode(PIN_B, INPUT);
    pinMode(PIN_C, INPUT);
    
    for(int wait_for_answer=0; wait_for_answer<=TIMEOUT_LOOP_NO; wait_for_answer++)
      {
      if(digitalRead(PIN_C) == HIGH)
        {

              for(int wait_for_answer2=0; wait_for_answer2<=TIMEOUT_LOOP_NO; wait_for_answer2++)
                {
                if(digitalRead(PIN_C) == LOW) 
                  {
                  return true;
                  }
                delay(100);  
                }
        }
      delay(100);
      }
    set_state("error_wait_for_answer_timeout");  
    info("error_wait_for_answer_timeout");  
    return false;  
  }

String Connection::send_splited(String data)
  {
    int i,j;
    String res = "OK";
    int c = get_array_count(PIN_COMM);

    String str = str2bin(data);
        
    // setup pins to output
    for(int z=0; z<c; z++)
      {
        info("Set PIN: "+String(get_array_item(PIN_COMM,z))+" as OUTPUT");
        pinMode(get_array_item(PIN_COMM,z), OUTPUT);          
      }
      
    j=0;
    
    for(i=0; i<str.length(); i++)
      {
        j++;  
        res = res + str[i];
        if(j == c)
          {
            info("Splitted: "+res);
            for(int z=0; z<c; z++)
              {
                if(String(str[i-c+z+1]) == "1")
                  {
                  info("Set PIN for loop "+String(z)+"): "+(get_array_item(PIN_COMM,z))+" = HIGH");
                  digitalWrite(get_array_item(PIN_COMM,z), HIGH);
                  }
                else
                  {
                  info("Set PIN for loop "+String(z)+"): "+(get_array_item(PIN_COMM,z))+" = LOW");
                  digitalWrite(get_array_item(PIN_COMM,z), LOW);
                  }
              }

              delay(1000);
            pinMode(PIN_B, OUTPUT);
            digitalWrite(PIN_B, HIGH);
  
            delay(send_delay);
            if(confirm_received() == false)
              {
                return "ERROR";
              }
           pinMode(PIN_B, OUTPUT);
           digitalWrite(PIN_B, LOW);
  
            delay(1000);
            j = 0;            
          }
      }
    return res;  
  }

String Connection::byte2string(String str)
  {
    int value = 0;
    for (int i=0; i< str.length(); i++) 
      {
      value *= 2; // double the result so far
      if (str[i] == '1') value++;  //add 1 if needed
      }
    char chr = value;
    return String(chr);
  }
  

String Connection::str2bin(String str)
  {
    String res;
    String string_byte;
    char inputChar;

    for(int i=0; i<str.length(); i++)
    {
      inputChar = str[i];
      string_byte = "";
      for ( uint8_t bitMask = 128; bitMask != 0; bitMask = bitMask >> 1 ) 
        {
          if ( inputChar & bitMask ) 
            {
            string_byte = string_byte + "1";
            } 
          else 
            {
            string_byte = string_byte + "0";
            }
        }
      res = res + string_byte;
      info("Decoded '"+string_byte+"' = '"+(byte2string(string_byte))+"'");
    }

    info("Encode '"+str+"' to '"+res+"'");
         
    return res;
  }

void Connection::info(String str)
  {
    if(debug)
      {
      Serial.println("LOG: "+str);  
      }
  }



int Connection::get_array_item(String str, int number)
  {
  int this_number = 0;
  String return_str = "";
    
  for (int i = 0; i < str.length(); i++) 
    {
    if(number == this_number)
            {
              return_str = return_str + str.substring(i, i+1);
            }

      if (str.substring(i, i+1) == ",") 
        {
          this_number++;
        }
  }
  if(return_str.substring(return_str.length()-1, return_str.length()) == ",")
    {
      return_str = return_str.substring(0, return_str.length()-1);
    }
    return return_str.toInt();
  }



int Connection::get_array_count(String str)
{
  int no = 0;
  for (int i = 0; i < str.length(); i++) 
    {
      if (str.substring(i, i+1) == ",") 
        {
          no++;
        }
  }
  return ++no;
}  

void Connection::set_state(String new_state)
  {
    info("State: "+new_state);
    STATE = new_state;
  }

