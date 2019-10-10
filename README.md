# Thronix- A monitoring and watering system!
 In this final master project a monitoring and watering system, that can be used for domestic use as for large scale, has been developed. The system is based on the CC3200, which uses FreeRTOS, 3 sensors interfaces have been developed (soil moisture, temperature and humidity ratio and water level) as well as a way to water the plant via a water pump powered by an external battery and controlled by a relay. A PC application has also been developed consisting of a graphic user interface in where the user can visualize the sensors data, both the latest measure as well as a graphic containing the history of the variable. The user interface also allows to water the plan in two ways, normal watering or fixing a temperature and soil moisture threshold, if those thresholds are surpassed the plant will water by itself. Data is also stored in a file at the computer in CSV format, just in case, any further processing is needed. The communication between PC and microcontroller is wireless and done via Wi-Fi, using MQTT, which makes scaling the project very easy thanks to the publish-subscribe method. Besides, a websyte has been developed in order to be able to access the data and interact with the system even when not in the same Wi-Fi network.
 
 ![Prototype]( https://i.imgur.com/fQ1lwO4.jpg "Prototype")
 
 * Microcontroller code can be found at CSSWorkspace directory.
 * Qt Interface can be found at QtWorkspace directory.
 * Web Aplicantion can be found at [Losant website developed for this pourpose](https://tfm-sergiogasquez.onlosant.com/), credentials are needed (test.user@example.com ; 12345678)
 * Documentation directory includes the datasheet and documents of every module of the system.
 * The report directory has the report created for the proyec which, sadly, is in Spanish (dont hessitate to reach me if you have any question)
 * The file mosquito.conf hold the configuration used for the Mosquitto broker which includes the bridge to Losant broker.
 
 

 
