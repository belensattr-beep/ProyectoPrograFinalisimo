#include "ModoKiosko.h"
ModoKiosko::ModoKiosko(PantallaLCD& l, LectorNFC& n, AlmacenSD &s, Reloj &r, Buzzer &b, Teclado& t) : Modo(l, n, s, r, b), teclado(t), estado(ESPERANDO_MONTO), monto(0), tipo('0'), tMensaje(0) {}

void ModoKiosko::alEntrar() {
    estado = ESPERANDO_MONTO;
    monto = 0;
    uidPendiente = "";
    lcd.mensaje("Ingrese monto", "#: OK");
    nfc.reset();
}

void ModoKiosko::ejecutar() {
    if (estado == MOSTRANDO_OK) {
        if (millis() - tMensaje >= 3000){
          alEntrar();
        }
        return;
    }

    if (estado == ESPERANDO_MONTO) {
        if (teclado.leerMontoNoBloqueante(monto, lcd)) {
            estado = ESPERANDO_TARJETA;
            lcd.mensaje("Monto: " + String(monto, 2), "Acerque tarjeta");
        }
        return;
    }

    if (estado == ESPERANDO_TARJETA) {
        String uid;
        if (nfc.leerUID(uid)) {
            Tarjeta* t = sd.cargarTarjeta(uid);
            if (t) {
                t->recargar(monto);
                sd.guardarTarjeta(t);
                registrarYFinalizar(uid, t);
                delete t;
            } else {
                uidPendiente = uid;
                estado = ESPERANDO_TIPO;
                lcd.mensaje("Nueva: 1, 2 o 3", "1:C 2:E 3:A");
            }
        }
        return;
    }

    if (estado == ESPERANDO_TIPO) {
        if (teclado.leerTipoNoBloqueante(tipo)) {
            Tarjeta* t = nullptr;
            if (tipo == '2')      t = new TarjetaEstudiante(uidPendiente, 0);
            else if (tipo == '3') t = new TarjetaAdultoMayor(uidPendiente, 0);
            else                  t = new TarjetaComun(uidPendiente, 0);

            t->recargar(monto);
            sd.guardarTarjeta(t);
            registrarYFinalizar(uidPendiente, t);
            delete t;
        }
    }
}

void ModoKiosko::registrarYFinalizar(String uid, Tarjeta* t) {
    String f, h;
    reloj.ahora(f, h);
    sd.guardarOperacion(f + "," + h + "," + uid + "," + t->tipo() + ",RECARGA," + String(monto,2) + "," + String(t->getSaldo(),2));
    buzzer.ok();
    lcd.mensaje("RECARGA OK", "Saldo:" + String(t->getSaldo(), 2));
    tMensaje = millis();
    estado = MOSTRANDO_OK;
}