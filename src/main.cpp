#include "WiFi.h"
#include <esp32fota.h>
#include "esp_ota_ops.h"

esp32FOTA esp32FOTA("esp32-fota-http", 1, false, true);

void setup(){

    Serial.begin(115200);

    for(int i=0;(i<10);i++) {
        delay(1000);
        Serial.println(i);
    }
 
    Serial.printf("Wifi SSD: %s\n",WIFI_SSID);
    Serial.printf("Wifi PWD: %s\n",WIFI_PWD);
    

    Serial.print("Attempting to connect to Wi-Fi");
    WiFi.begin(WIFI_SSID,WIFI_PWD);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    

    Serial.printf("Starting OTA from URL: %s\n", MANIFEST_URL);

    esp32FOTA.setUpdateBeginFailCb( [](int partition) {
        Serial.printf("Update could not begin with %s partition\n", partition==U_SPIFFS ? "spiffs" : "firmware" );
    });
    esp32FOTA.setProgressCb( [](size_t progress, size_t size) {
      if( progress == size || progress == 0 ) Serial.println();
    

       Serial.printf(".");
       //%d\n",int(10 * progress / size ));
    });

    Serial.println("Please Wait it takes some time ...");


    const esp_partition_t *running = esp_ota_get_running_partition();
    Serial.printf("partition: %d, %d\n",running->address,running->size);
    if ( running->address == 0x10000 ) {
        Serial.println("We are in the update partition");
        esp32FOTA.setManifestURL(URL_MAIN_MANIFEST);
    } else {
        Serial.println("We are in the main partition");
        esp32FOTA.setManifestURL(URL_UPDATER_MANIFEST);
    }

    Serial.println("Dumping config");
    esp32FOTA.printConfig();

}

void loop(){

    bool updatedNeeded = esp32FOTA.execHTTPcheck();
    if (updatedNeeded)
    {
        Serial.println("Update needed");
        esp32FOTA.execOTA();
    } else {
        Serial.println("No update needed");
    }

    delay(10000);
}
