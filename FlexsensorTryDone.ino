const int ADC_pin = 14;              //Analog-Digital-Converter
const float VCC = 5;                 //Quellspannung
const float fixed_resistor = 22000;  //Vorwiderstand Spannungsteiler

void setup() {
  Serial.begin(9600);       //Serial mit 9600Bits/s
  pinMode(ADC_pin, INPUT);  //Analog-Digital-Converter als Input setzen
}

void loop() {
  int flex_ADC = analogRead(ADC_pin);                                            //Spannungsteiler wert messen
  float flex_voltage = flex_ADC * VCC / 1023.0;                                  //Spannungsdifferenz berechnen
  float flex_resistance = (fixed_resistor * (VCC / flex_voltage - 1.0)) / 1000;  //Widerstandswert berechnen
  Serial.print("ADC value: ");
  Serial.println(flex_ADC);  //Gemessene Spannung Ausgeben
  Serial.print("Volatge value: ")
  Serial.println(flex_voltage);  //Berechnete Spannung ausgeben

  Serial.println();
  delay(500);  //Warte 500ms
}