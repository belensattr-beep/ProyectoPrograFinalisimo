#include <Wire.h>
#include "ControladorSistema.h"

ControladorSistema& obtenerSistema() {
    static ControladorSistema instancia(27);
    return instancia;
}
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  
  obtenerSistema().iniciar();
}

void loop() {
  obtenerSistema().ejecutar();
}
