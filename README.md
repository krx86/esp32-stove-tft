
Projekta Dokumentācija: esp32-stove-tft

Projekta Apraksts

Šis projekts ir centrālās krāsns vadības bloks, kas regulē krāsns gaisa padevi, vadoties pēc ūdens temperatūras. Tas izmanto ESP32 kontrolieri ar TFT displeju, lai nodrošinātu lietotājam draudzīgu interfeisu, kontrolētu un pārraudzītu dažādas krāsns funkcijas, piemēram, temperatūru un darbības režīmus.

Uzstādīšanas Instrukcijas

Sagatavošana:

Lejupielādējiet projekta kodu no GitHub repozitorija.

Pārliecinieties, ka jums ir uzstādīts PlatformIO vai Arduino IDE.

Nepieciešamās Bibliotēkas:

Instalējiet nepieciešamās bibliotēkas, piemēram, TFT_eSPI, ESPAsyncWebServer, ArduinoJson.

Kompilācija un Augšupielāde:

Atveriet projektu savā IDE un konfigurējiet platformio.ini failu atbilstoši jūsu ESP32 aparatūrai.

Kompilējiet un augšupielādējiet kodu ESP32 mikrokontrolierī.

Lietošana

Kad augšupielāde ir veiksmīga, pievienojiet krāsns sensorus un TFT displeju atbilstoši projekta specifikācijām.

ESP32 tiks restartēts, un TFT displejā parādīsies krāsns kontroles interfeiss.

Izmantojiet pieskārienu ekrānam, lai izvēlētos un pielāgotu krāsns iestatījumus.

Projekta Fails

Platformio.ini: Satur visas projekta konfigurācijas un atkarības.

Galvenais kods: Ietver krāsns kontroles loģiku un mijiedarbību ar lietotāju caur TFT displeju.

Licencēšana

Šim projektam vēl nav pievienota licence. Ieteicams pievienot piemērotu atvērtā pirmkoda licenci, piemēram, MIT vai GPL, lai citi varētu to izmantot un modificēt atbilstoši savām vajadzībām.

Papildu Informācija

Autors: krx86

GitHub Repozitorijs: esp32-stove-tft

Kontakti: Lai saņemtu palīdzību vai sniegtu atsauksmes, lūdzu, sazinieties ar projekta autoru caur GitHub.


gdn = melns;
+5vin = sarkan;
servoPort = 5   // servo pin dzeltens;
OneWire oneWire=6 // temp pin balts;
touch1 = 4;
touch2 = 1;
touch3 = 2;
relay = 17 // zals;
buzzer = 14;
relay2 =16 //oranzs;



