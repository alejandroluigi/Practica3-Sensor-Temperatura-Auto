const int pinVout = A0;
const int RELE1 = 8;
const int RELE2 = 9;

const float VCC = 5.08;

// CALIBRACION
const int N = 21;

float Rcal[N] = {100.0, 102.0, 104.0, 106.0, 108.0, 110.0,
  112.0, 114.0, 116.0, 118.0, 120.0, 122.0, 124.0, 126.0,
  128.0, 130.0, 132.0, 134.0, 136.0, 138.0, 138.5
};

float Vcal[N] = { 2.180, 2.204, 2.228, 2.252, 2.276, 2.300,
  2.319, 2.338, 2.358, 2.377, 2.397, 2.416, 2.436, 2.455,
  2.468, 2.487, 2.500, 2.519, 2.548, 2.568, 2.577
};

/*
const int N = 8;

float Rcal[N] = {
  102.0,
  110.0,
  116.0, 118.0, 120.0, 122.0,
  123.7,
  126.0
};

float Vcal[N] = {
  2.204,
  2.300,
  2.377, 2.397, 2.416,
  2.443,
  2.455
};*/

// MODO
bool modoSistema = true;

float obtenerResistencia(float Vout) {

  // Si está por debajo del primer punto
  if (Vout <= Vcal[0]) {
    return Rcal[0];
  }

  // Si está por encima del último punto
  if (Vout >= Vcal[N - 1]) {
    return Rcal[N - 1];
  }

  // Buscar los dos puntos que rodean Vout
  for (int i = 0; i < N - 1; i++) {
    if (Vout >= Vcal[i] && Vout <= Vcal[i + 1]) {

      // Interpolación lineal
      float resistencia = Rcal[i] + (Vout - Vcal[i]) *
        (Rcal[i + 1] - Rcal[i]) / (Vcal[i + 1] - Vcal[i]);

      return resistencia;
    }
  }
  // Seguridad
  return Rcal[N - 1];
}

// CONVERSIÓN PT100 a TEMPERATURA
// Para 0 a 100 °C
float obtenerTemperatura(float resistencia) {
  // Aproximación lineal del Pt100:
  // R = 100 + 0.385*T
  // T = (R - 100) / 0.385
  float temperatura = (resistencia - 100.0) / 0.385;
  return temperatura;
}

void setup() {

  Serial.begin(9600);

  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);

  // Iniciar en modo SISTEMA
  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, LOW);
}

void loop() {

  // RECIBIR COMANDOS DE PYTHON
  if (Serial.available() > 0) {

    char comando = Serial.read();

    if (comando == 'S' || comando == 's') {
      modoSistema = true;
      digitalWrite(RELE1, LOW);
      digitalWrite(RELE2, LOW);
      delay(100);
    }

    if (comando == 'M' || comando == 'm') {
      modoSistema = false;
      digitalWrite(RELE1, LOW);
      digitalWrite(RELE2, LOW);
      delay(100);
      digitalWrite(RELE1, HIGH);
      digitalWrite(RELE2, HIGH);
      delay(100);
    }
  }

  // MODO SISTEMA
  if (modoSistema) {

    int lecturaADC = analogRead(pinVout);
    float Vout = lecturaADC * VCC / 1023.0;

    // Interpolacion
    float resistencia = obtenerResistencia(Vout);
    if(resistencia > 122){
      resistencia = obtenerResistencia(Vout) - 1.2;
    } else if(resistencia < 115.8 && resistencia > 108) {
      resistencia = obtenerResistencia(Vout) + 0.2;
    } else if(resistencia > 116 && resistencia < 121){
      resistencia = obtenerResistencia(Vout) - 4;
    } else if(resistencia > 121 && resistencia < 127){
      resistencia = obtenerResistencia(Vout) - 2.5;
    } else if(resistencia > 127 && resistencia < 138){
      resistencia = obtenerResistencia(Vout) - 2.0;
    } else if(resistencia > 100 && resistencia < 108){
      resistencia = obtenerResistencia(Vout) + 1.4;
    }
    resistencia = obtenerResistencia(Vout) - 10;
    if(Vout > 2.46){
      resistencia = obtenerResistencia(Vout) - 13;
    } else if(Vout > 2.23 && Vout < 2.46){
      resistencia = obtenerResistencia(Vout) - 11;
    }
    

    // Temperatura
    float temperatura = obtenerTemperatura(resistencia);

    // Limitar rango
    if (resistencia < 100.0){
      resistencia = 100.0;
    }
    if (resistencia > 138.5){
      resistencia = 138.5;
    }
    if (temperatura < 0.0) {
      temperatura = 0.0;
    }
    if (temperatura > 100.0) {
      temperatura = 100.0;
    }
    
    // ENVIAR A PYTHON
    Serial.print("SISTEMA,");
    Serial.print(Vout, 3);
    Serial.print(",");
    Serial.print(resistencia, 2);
    Serial.print(",");
    Serial.println(temperatura, 2);

  } else {
    // MODO MULTIMETRO
    Serial.println("MULTIMETRO");
  }
  delay(500);
}
