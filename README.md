# ProyectoPrograFinalisimo
Este proyecto implementa un sistema de transporte electrónico basado en tarjetas NFC, utilizando un ESP32. Permite recargar saldo, cobrar pasajes y registrar todas las operaciones en una tarjeta SD con fecha y hora reales.
## Características
- Lectura de tarjetas NFC (PN532)
- Modo Kiosko y Modo Cobro seleccionables por switch
- Recarga y descuento de saldo
- Registro de historial en tarjeta SD
- Fecha y hora mediante RTC DS3231
- Interfaz con LCD I2C, teclado matricial y buzzer
- Arquitectura orientada a objetos
## Hardware utilizado
- ESP32 (38 pines)
- Módulo NFC PN532 (I2C)
- RTC DS3231
- Pantalla LCD 16x2 con I2C
- Teclado matricial 4x4
- Buzzer activo
- Resistencia 1k
- Transistor 2N2222A
- Tarjeta microSD
- Módulo SD
- Switch (2 pines)
## Conexiones principales
| Componente | Pin ESP32 |
|----------|-----------|
| SDA (I2C) | GPIO 21 |
| SCL (I2C) | GPIO 22 |
| NFC IRQ | GPIO 34 |
| NFC RST | GPIO 26 |
| SD CS | GPIO 5 |
| Buzzer | GPIO 32 |
| Switch modo | GPIO 27 |
## Estructura del código
- `SistemaTransporte.h / .cpp`: clases de los componentes fisicos del sistema
- `Modo.h / .cpp`: clase base abstracta para los modos de operación
- `ModoKiosko.h / .cpp`: clase de recarga de tarjetas
- `ModoCobro.h / .cpp`: clase de cobro de pasajes
- `LectorNFC`: manejo del PN532
- `AlmacenSD`: gestión de archivos en la SD
- `Reloj`: manejo del RTC
- `Switch`: manejo del switch
- `Teclado`, `PantallaLCD`, `Buzzer`: periféricos
- `Tarjeta`: clase base abstracta de la que heredan las clases `TarjetaComun`, `TarjetaEstudiante` y `TarjetaAdultoMayor`
- `ControladorSistema.h / .cpp`: clase que maneja el cambio de modos, instancia e inyecta los componentes a `ModoKiosko` y `ModoCobro`
## Funcionamiento general
1. El sistema arranca y detecta el modo mediante un switch (HIGH -> Kiosko / LOW -> Cobro)
2. En modo Kiosko:
   - Se ingresa un monto por el Keypad
   - Se acerca una tarjeta NFC al PN532
   - Se recarga el saldo
3. En modo Cobro:
   - Se acerca una tarjeta NFC al PN532
   - Se descuenta el pasaje
4. Todas las operaciones se registran en la SD con fecha y hora
## Decisiones de diseño
- Se utilizó programación orientada a objetos para desacoplar la lógica del hardware
- El sistema es no bloqueante; se utilizan millis()
- El RTC solo se ajusta si pierde energía para evitar reinicios de hora
- No se utilizan variables globales; los componentes se inyectan por constructor
## Formato de archivos en SD
- tarjetas.txt  
  UID,TIPO,SALDO
- historial.txt  
  FECHA,HORA,UID,TIPO,OPERACION,MONTO,SALDO_FINAL
## Compilación y carga
- IDE: Arduino IDE
- Placa: ESP32 Dev Module (v 2.0.9)
- Velocidad serial: 115200
- Librerías necesarias:
  - Arduino
  - Wire
  - SPI
  - Adafruit_PN532
  - RTClib
  - SdFat
  - LiquidCrystal_I2C
  - Keypad
## Autores
- Andrei Nagel Zelaya
- Maria Belen Satt Rivera
