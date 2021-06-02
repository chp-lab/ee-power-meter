#include "chp.h"
#include <PZEM004Tv30.h>
#include <ESP8266HTTPClient.h>
#include <SD.h>

#define OPT_PWD "administrator"
#define ROBOT_BUILDING 10
#define ROBOT_ROOM 2.2
#define A_BUILDING 2.8
#define INNO_EM 2.2
#define CUR_SCALE INNO_EM

#define ALARM_LED 10
#define RESET_ENERGY_PIN 13

PZEM004Tv30 pzemC(D2,D1);
PZEM004Tv30 pzemB(D4,D3);
PZEM004Tv30 pzemA(D6,D5);

String inString;
int alarm_led = ALARM_LED;
int reset_energy_btn = RESET_ENERGY_PIN;
int count = 0;

File myFile;
const int chipSelect = D1;
bool sd_log = true;

String ReadDataAll(){
  float voltageA = pzemA.voltage();
  float currentA = CUR_SCALE*pzemA.current();
  float powerA = CUR_SCALE*pzemA.power();
  float energyA = CUR_SCALE*pzemA.energy();
  float frequencyA = pzemA.frequency();
  float pfA = pzemA.pf();

  float voltageB = pzemB.voltage();
  float currentB = CUR_SCALE*pzemB.current();
  float powerB = CUR_SCALE*pzemB.power();
  float energyB = CUR_SCALE*pzemB.energy();
  float frequencyB = pzemB.frequency();
  float pfB = pzemB.pf();

  float voltageC = pzemC.voltage();
  float currentC = CUR_SCALE*pzemC.current();
  float powerC = CUR_SCALE*pzemC.power();
  float energyC = CUR_SCALE*pzemC.energy();
  float frequencyC = pzemC.frequency();
  float pfC = pzemC.pf();

  String V = String("") + "\"V\":[" + voltageA + "," + voltageB + "," + voltageC + "]";
  String I = String("") + "\"I\":[" + currentA + "," + currentB + "," + currentC + "]";
  String P = String("") + "\"P\":[" + powerA + "," + powerB + "," + powerC + "]";
  String E = String("") + "\"E\":[" + energyA + "," + energyB + "," + energyC + "]";
  String F = String("") + "\"f\":[" + frequencyA +"," + frequencyB + "," + frequencyC + "]";
  String PF = String("") + "\"pf\":[" + pfA + "," + pfB + "," + pfC + "]";

  String dataAll_1 = "{" + V + "," + I + "," + P + ",";
  String dataAll_2 = E + "," + F + "," + PF + "}";
 
  Serial.print(dataAll_1);
  Serial.println(dataAll_2);
  String tmp_data = dataAll_1 + dataAll_2;
  return tmp_data;
}

void log_to_file(String log_msg)
{
  #ifdef LOG_TO_FILE
  if(sd_log)
  {
    if(myFile)
    {
      myFile.print(get_time_format() + " " + log_msg);
    }
    else 
    {
      Serial.println("error opening test.txt");
    }
  }
  #endif
}

void setup() {
  Serial.begin(115200);
  
  Serial.print("Initializing SD card...");
  
  #ifdef LOG_TO_FILE
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("initialization failed!");
    sd_log = false;  
  }
  else
  {
    Serial.println("initialization done.");
    myFile = SD.open("log.txt", FILE_WRITE);
  }
  #endif
  
  log_to_file("Welcome");
  
  pinMode(alarm_led, OUTPUT);
  digitalWrite(alarm_led, LOW);
  pinMode(reset_energy_btn, INPUT);

//  use_saved_config();
  String my_topic;
//  String my_server = "18.140.173.239";
//  int my_port = 1883;
//  String my_username = "chp-lab";
//  String my_password = "atop3352";
//  set_mqtt(my_server, my_port, get_client_name(), my_username, my_password);
//  
  get_client_name();
  set_ota_pwd(OPT_PWD);
  log_to_file("WiFi begining");
  chp_wifi_begin();
  log_to_file("WiFi begin success");
  chp_init(true);
  set_sync_time(30*1000);
  
  Serial.println("Sync time=" + String(get_sync_time()));
  Serial.println("Interval=" + String(get_interval()));
}

void loop() {
  String tmp_msg;
  String msg;
  String res = "";

  chp_wifi_handle();
  listen_for_fw();
  chp_loop();
  time_to_sync();
  
  if(time_to_send())
  {
    log_to_file("Time to send");
    // read from meter
    msg = ReadDataAll();
    // simulation
//    msg = uart_read();
    Serial.println("msg=" + msg);
    res = influx_inline(msg, false);
    String mytopic = "influx/" + device_id();
    Serial.println("topic="  + mytopic);
    Serial.println("res=" + res);
    pubData(res, mytopic);
  }
  if(real_time_req())
  {
    log_to_file("Real time request recv");
    Serial.println("res_rt=" + msg);
    pubData(msg, "rt_res/" + device_id());
  }

  if(time_to_reboot("0:0") || time_to_reboot("12:00"))
  {
    reboot_now();
  }

  if(digitalRead(reset_energy_btn) == LOW)
  {
      pzemA.resetEnergy();
      pzemB.resetEnergy();
      pzemC.resetEnergy();
      Serial.println("Reset energy");
  }
  delay(10);
}
