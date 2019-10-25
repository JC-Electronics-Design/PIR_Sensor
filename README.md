# PIR_Sensor

PIR_Sensor is a software that is written for a TPL5111 and ESP8266 based battery powered PIR sensor that I developed. I developed this board to be able to sense movement in my house and then publish the status via MQTT to my home automation system. With the status of these PIR sensors I can active certain rules to for instance turn on the light in that specific room or to notify me that someone invaded my house.

The purpose of this software is to quickly connect to your wifi network using your routers BSSID. Once connected the PIR sensor status, temperature, relative humidity and battery level will be published to your MQTT broker.
To learn more about what your routers BSSID is, you can read the article found here: https://www.juniper.net/documentation/en_US/junos-space-apps/network-director3.7/topics/concept/wireless-ssid-bssid-essid.html

This board will be soon available on my Tindie store.
