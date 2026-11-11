#include <iostream>
#include <cstdio> 
using namespace std;

struct Comanda { 
	int idMozo; 
	int codigoProducto; 
	int cantidad; 
	float comision; 
};

const int MAX_DIAS = 7;

int main() {
    int diaLunes, mes, anio, numSemana;
    
    cout << "--- CREAR PLANILLA SEMANAL ---" << endl;
    cout << "Ingrese la fecha del lunes de la semana (DD MM AAAA): ";
    cin >> diaLunes >> mes >> anio;
    cout << "Ingrese el numero de semana (ejemplo: 1): ";
    cin >> numSemana;

    FILE* archivosDelDia[MAX_DIAS] = {NULL};
    Comanda ventasLeidas[MAX_DIAS];
    bool hayVentasParaLeer[MAX_DIAS] = {false};

    // 1. Abrimos los archivos de cada dia de la semana
    for (int i = 0; i < MAX_DIAS; ++i) {
        char nombreDelDia[50];
        // Calculamos la fecha de cada dia sumandole 'i' al lunes
        sprintf(nombreDelDia, "comandas_%02d-%02d-%04d.dat", diaLunes + i, mes, anio);
        
        archivosDelDia[i] = fopen(nombreDelDia, "rb");
        
        if (archivosDelDia[i] != NULL) {
            // Si encontramos el archivo del dia, leemos la primera venta
            if (fread(&ventasLeidas[i], sizeof(Comanda), 1, archivosDelDia[i]) == 1) {
                hayVentasParaLeer[i] = true;
                cout << "Se encontro el archivo del dia: " << nombreDelDia << endl;
            } else {
                hayVentasParaLeer[i] = false; 
            }
        } else {
            hayVentasParaLeer[i] = false;
            cout << "No hubo ventas o no se encontro el dia: " << nombreDelDia << endl;
        }
    }

    // 2. Creamos el archivo nuevo que va a juntar toda la semana
    char nombreSemanal[50];
    sprintf(nombreSemanal, "comandas_semana_s%d-%02d.dat", numSemana, mes);
    
    FILE* archivoDeLaSemana = fopen(nombreSemanal, "wb");
    if (archivoDeLaSemana == NULL) {
        cout << "Hubo un error al intentar crear el archivo semanal: " << nombreSemanal << endl;
        // Cerramos lo que se haya abierto para no dejar archivos colgados
        for (int i = 0; i < MAX_DIAS; ++i) {
            if (archivosDelDia[i] != NULL) fclose(archivosDelDia[i]);
        }
        return 1;
    }

    cout << "\nArmando la planilla de la semana: " << nombreSemanal << "..." << endl;

    // 3. Juntamos las ventas ordenandolas por mozo
    while (true) {
        int diaConElMozoMasChico = -1;

        // Buscamos cual es la venta que tiene el numero de mozo mas bajo
        for (int i = 0; i < MAX_DIAS; ++i) {
            if (hayVentasParaLeer[i]) {
                // Si todavia no guardamos a ningun mozo (-1) 
                // O si el mozo de este archivo es MENOR al que ya teniamos guardado
                if (diaConElMozoMasChico == -1 || ventasLeidas[i].idMozo < ventasLeidas[diaConElMozoMasChico].idMozo) {
                    diaConElMozoMasChico = i; 
                }
            }
        }

        // Si revisamos todos los dias y sigue siendo -1, significa que no hay mas ventas
        if (diaConElMozoMasChico == -1) {
            break; 
        }

        // Guardamos esa venta ordenada en el archivo de la semana
        fwrite(&ventasLeidas[diaConElMozoMasChico], sizeof(Comanda), 1, archivoDeLaSemana);

        // Leemos la SIGUIENTE venta solamente del dia de donde sacamos la anterior
        if (fread(&ventasLeidas[diaConElMozoMasChico], sizeof(Comanda), 1, archivosDelDia[diaConElMozoMasChico]) != 1) {
            hayVentasParaLeer[diaConElMozoMasChico] = false; 
        }
    }

    // 4. Cerramos todos los archivos para dejar todo prolijo
    for (int i = 0; i < MAX_DIAS; ++i) {
        if (archivosDelDia[i] != NULL) {
            fclose(archivosDelDia[i]);
        }
    }
    fclose(archivoDeLaSemana);

    cout << "Planilla semanal creada y ordenada con exito." << endl;

    return 0;
}