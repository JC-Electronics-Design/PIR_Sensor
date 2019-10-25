//#define DEBUG                                         //Uncomment this line if you want serial debugging enabled

const char* ssid = "ssid";                              //Put in your network ssid
const char* password = "password";                      //Put in your network password

const char* server = "192.168.0.10";                    //Change to your MQTT broker ip-address
const char* mqttDeviceID = "PIR_Sensor";                //Unique device id in your MQTT network

char* outTopic1 = "Home/PIR_Sensor/PIR";                //Set desired topic name for the PIR sensor
char* outTopic2 = "Home/PIR_Sensor/TEMP";               //Set desired topic name for the TEMP sensor
char* outTopic3 = "Home/PIR_Sensor/RH";                 //Set desired topic name for the RH sensor
char* outTopic4 = "Home/PIR_Sensor/BAT";                //Set desired topic name for the BAT value

/* Use a static ip-address for faster connection to your network
 * The software uses the BSSID of your router to quickly connect to your wifi network
 * Thereby minimizing the time the module is powered on and thus saving battery life
 */ 
IPAddress ip(192,168,0,25);                             //Set desired static IP Address
IPAddress gateway(192,168,0,1);                         //Set gateway to match your network
IPAddress subnet(255,255,255,0);                        //Set subnet mask to match your network

/* Battery Levels
 * The type of battery that is used is Lithium-Ion, 3.7V nominal, 4.2V max, 3.2V min (The minimum voltage I like to use)
 * The minimum voltage of 3.2 volt corresponds with an analog value of 715
 * The maximum voltage of 4.2 volt corresponds with an analog value of 1024
 * 
 * To calculate the analog value, instead of measuring, for the MAXIMUM voltage level of your battery use the following equation:
 * MAX_BATTERY = ((10k / (10k + 33k)) * Max_bat_voltage) * 1024 
 *  where:
 *      - Max_bat_voltage = the maximum voltage of your battery
 * 
 * To calculate the analog value, instead of measuring, for the MINIMUM voltage level of your battery use the following equation:
 * MIN_BATTERY = ((10k / (10k + 33k)) * Min_bat_voltage) * 1024 
 *  where: 
 *      - Min_bat_voltage = the minimum voltage of your battery
 * 
 * Example:
 * When using a Lithium-Ion battery the maximum voltage is 4.2V
 * The desired minimum voltage is 3.2V
 * 
 * When you replace these values in the equations you get the following analog values:
 * MAX_BATTERY = ((10k / (10k + 33k)) * 4.2) * 1024 = 1000
 * MIN_BATTERY = ((10k / (10k + 33k)) * 3.2) * 1024 = 762
 */
#define MIN_BATTERY 762                                 //Define the minimum battery level
#define MAX_BATTERY 1000                                //Define the maximum battery level