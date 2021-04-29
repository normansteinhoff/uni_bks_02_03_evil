
// --------------------------------------------------------------------------------------

// Autor:
    // Norman Steinhoff

// Jahr
    // 2021

// --------------------------------------------------------------------------------------

// Verwendete Abkürzungen:
    // Vor. == Voraussetzung
    // Eff. == Effekt
    // Erg. == Ergebnis
    // Anm. == Anmerkung

// --------------------------------------------------------------------------------------

// neueren POSIX_Standard festlegen
// Wird für { sigaction() } und { nanosleep() } benötigt.
#define _POSIX_C_SOURCE 199309L

// Standard_Header einbinden
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Konstanten
#define OK 0

// Vor.:
// -
// Eff.:
// -
// Erg.:
// -
// Anm.:
// *    Diese Funktion soll als Reaktion auf den Erhalt eines Signals ausgeführt werden.
// *    Vorher muss diese Funktion aber mittels der Funktion { connect_signal_with_function }
//      mit einem Signal verknüpft werden.
void my_signal_handler(int signal_number) {

    //den Wert { signal_number } ignorieren
    (void)signal_number;
}

// Vor.:
// -
// Eff.:
// *    Nach dem Aufruf dieser Funktion wird bei dem eingehenden Signal { signal }
//      ab sofort immer die Funktion { signalHandler } ausgeführt.
// Erg.:
// -
// Anm.:
// -
void connect_signal_with_function(int signal, void (*signalHandler)(int)) {

    // notwendige Daten sammeln
    struct sigaction sa_register = {
        .sa_handler = signalHandler,
        .sa_flags = 0,
    };
    sigemptyset(&sa_register.sa_mask);

    // eigentlicher Verknüpfungs_Schritt
    sigaction(signal, &sa_register, NULL);
}

// Vor.:
// -
// Eff.:
// *    Die Prozess-ID wird in der Standard-Ausgabe angezeigt.
// Erg.:
// -
// Anm.:
// -
void print_process_id(void) {

    // Prozess_Nummer ermitteln
    int pid = getpid();

    // Prozess_Nummer anzeigen
    printf("ProcessId = %d\n", pid);
}

// Vor.:
// *    Die Ziel_Datei mit dem Deskriptor { destinationFileDescriptor } muss zum schreiben geöffnet sein.
// *    { byteCount <= sizeof(buffer) }
// Eff.:
// *    Die ersten { byteCount } vielen Bytes von { buffer } werden in die Datei mit dem
//      Datei-Deskriptor { destinationFileDescriptor } geschrieben.
// Erg.:
// -
// Anm.:
// -
void write_to_file(int destinationFileDescriptor, char buffer[], ssize_t byteCount) {
    
    // Anzahl geschriebener Bytes initialisieren.
    ssize_t bytesWritten = 0;

    // solange versuchen in die Ziel_Datei zu schreiben, bis alle Bytes geschieben wurden
    char *startPos;
    int bytesToWrite;
    while (bytesWritten < byteCount) {
        startPos = buffer + bytesWritten;
        bytesToWrite = byteCount - bytesWritten;
        
        // hier kann es passieren, das weniger Bytes geschrieben werden als angefragt wurde. Daher die Schleife.
        bytesWritten += write(destinationFileDescriptor, startPos, bytesToWrite);
    }
}

// Vor.:
// -
// Eff.:
// *    Der String { text } wird in der Standard-Ausgabe ausgegeben.
// Erg.:
// -
// Anm.:
// -
void print_string(char text[]) {

    //weiterleiten
    write_to_file(STDOUT_FILENO, text, strlen(text));
}

// Vor.:
// -
// Eff.:
// *    Das Programm pausiert { sec } viele Sekunden.
// Erg.:
// -
// Anm.:
// -
void sleep_seconds(int sec) {

    // Bei einer unsinnigen Eingabe wird die Funktion direkt beendet
    if (sec <= 0) {
        return;
    }

    // Notwendige Instanzen der Struktur { timespec } werden erzeugt und initialisiert.
    // Diese werden für die Funktion { nanosleep } benötigt.
    struct timespec requestedTime;
    requestedTime.tv_sec = sec;
    requestedTime.tv_nsec = 0;
    struct timespec remainingTime;

    // Gewünschte Zeit warten und bei Misserfolg nochmal versuchen.
    int cont = -1;
    do {
        cont = nanosleep(&requestedTime, &remainingTime); // { = 0 } if time is over
        requestedTime = remainingTime;
    }
    while (0 != cont);
}

// Vor.:
// -
// Eff.:
// *    Eine Endlosschleife wird gestartet, welche alle 2 Sekunden die
//      Nachricht "Endlosschleife" in der Standard-Ausgabe anzeigt.
// Erg.:
// -
// Anm.:
// -
void start_endless_loop(void) {

    //Endlosschleife starten
    while (true)
    {
        sleep_seconds(2);
        print_string("Endlosschleife\n");
    }
}

// Vor.:
// -
// Eff.:
// *    Ein fieser Plan wird ausgeführt :)
// Erg.:
// *    { OK }
// Anm.:
// *    { OK } ist eine Makro_Konstanten.
int main(void) {

    // Ergebnis initialisieren
    int result = OK;

    // Den Signalen eine Funktion zuordnen
    connect_signal_with_function(SIGINT, my_signal_handler);
    connect_signal_with_function(SIGTERM, my_signal_handler);
    
    // Prozess_Nummer anzeigen
    print_process_id();

    // Endlosschleife starten
    start_endless_loop();
    
    // Ergebnis liefern
    return result;
}
