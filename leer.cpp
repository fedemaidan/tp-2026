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
    // Cambiá el nombre por el de la planilla que quieras verificar
    FILE* f = fopen("datos/comandas_03-06-2025.dat", "rb"); 
    
    if (f == NULL) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return 1;
    }

    Comanda c;
    cout << "--- CONTENIDO DE LA PLANILLA ---" << endl;
    
    while (fread(&c, sizeof(Comanda), 1, f) == 1) {
        cout << "Mozo ID: " << c.idMozo 
             << " | Prod: " << c.codigoProducto 
             << " | Cant: " << c.cantidad 
             << " | Comision: $" << c.comision << endl;
    }

    fclose(f);
    return 0;
}