#include <iostream>
#include <cstdio>
using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    int numSemana, mes;
    cout << "Ingrese el numero de semana a consultar (ej: 1): ";
    cin >> numSemana;
    cout << "Ingrese el numero de mes (ej: 6): ";
    cin >> mes;

    char nombreSemanal[50];
    sprintf(nombreSemanal, "comandas_semana_s%d-%02d.dat", numSemana, mes);

    FILE* f = fopen(nombreSemanal, "rb");

    if (f == NULL) {
        cout << "Error: no se pudo abrir el archivo semanal." << endl;
        return 1;
    }

    Comanda c;
    int leido = fread(&c, sizeof(Comanda), 1, f);

    if (leido == 0) {
        cout << "El archivo esta vacio." << endl;
        fclose(f);
        return 0;
    }

    int totalProdBuffet = 0;
    float totalComBuffet = 0.0;

    cout << endl << "--- RESUMEN SEMANAL DE VENTAS ---" << endl << endl;

    while (leido == 1) {
        int mozoActual = c.idMozo;
        int cantMozo = 0;
        float comMozo = 0.0;

        while (leido == 1 && c.idMozo == mozoActual) {
            cantMozo += c.cantidad;
            comMozo += c.comision;
            
            leido = fread(&c, sizeof(Comanda), 1, f);
        }

        cout << "Mozo Nro: " << mozoActual << endl;
        cout << "  Productos vendidos: " << cantMozo << endl;
        cout << "  Comision a cobrar: $" << comMozo << endl;
        cout << "---------------------------------" << endl;

        totalProdBuffet += cantMozo;
        totalComBuffet += comMozo;
    }

    cout << endl;
    cout << "TOTALES GENERALES DEL BUFFET:" << endl;
    cout << "  Total productos vendidos: " << totalProdBuffet << endl;
    cout << "  Total comisiones a pagar: $" << totalComBuffet << endl;

    fclose(f);

    return 0;
}
