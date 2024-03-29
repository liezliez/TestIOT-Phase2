#include <RTClib.h>
#include <Wire.h>
#include <dht.h>
#define sensor A0
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial module_bluetooth(0, 1);

File myFile;
dht DHT;
File root; // buat cek isi directory
RTC_DS1307 rtc; //sensor rtc

char namaHari[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"}; //rtc template hari indo

void setup() {
  Serial.begin(9600);

  // INISIASI BLUETOOTH
  Serial.println("Input command AT : ");
  module_bluetooth.begin(38400);
  

  // INISIASI RTC
  if (! rtc.begin()) {
    Serial.println("RTC TIDAK TERBACA");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//update rtc dari waktu komputer
  }

  //INISIASI SD CARD
  if (!SD.begin(4)) {
    Serial.println("inisiasi SD card gagal");
    return;
  }
  Serial.println("Sukses inisiasi SD card");

  //INISIASI RTC
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("RTC TIDAK TERBACA");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//update rtc dari waktu komputer
  }
  
  
}

void printDirectory(File dir, int numTabs){
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  } 
}

void loop() {

  
  //baca terhubungnya koneksi HC05 dengan arduino pada serial Monitor
  if (module_bluetooth.available())
  Serial.write(module_bluetooth.read()); 
  //baca data dari Arduino Serial monitor yang akan dikirim ke HC05
  if (Serial.available())
  module_bluetooth.write(Serial.read());
  
  myFile = SD.open("test.txt", FILE_WRITE);
   
  float kelembaban = DHT.humidity;
  float suhu = DHT.temperature;
  
  DHT.read11(sensor);
  DateTime now = rtc.now();
  myFile.print(namaHari[now.dayOfTheWeek()]);  
  myFile.print(',');    
  myFile.print(now.day(), DEC);
  myFile.print('/');
  myFile.print(now.month(), DEC);
  myFile.print('/');
  myFile.print(now.year(), DEC);
  myFile.print(" ");   
  myFile.print(now.hour(), DEC);
  myFile.print(':');
  myFile.print(now.minute(), DEC);
  myFile.print(':');
  myFile.print(now.second(), DEC);
  myFile.println();
  delay(3000);

  // BIKIN FILE DAN WRITE FILE HASIL SENSOR
  
  if (myFile){
    Serial.println("----------------------------------");
    myFile.print("Kelembaban : ");
    myFile.println(kelembaban);
    myFile.print("Suhu : ");
    myFile.println(suhu);
    myFile.close();
  } else {
    Serial.println("error open test.txt");
  }
  
  // READ FILE HASIL SENSOR
  myFile = SD.open("test.txt");
  if(myFile){
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Error baca SD card");
  }
  
  delay(400);


  
  
}
