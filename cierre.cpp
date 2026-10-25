#include <iostream>
#include <cstdio>
#include <iomanip>
#include <string>
using namespace std;
 
#define MAX_ARCHIVOS 7 // un archivo por dia de la semana
 
struct comanda
{
    int idmozo;     // id del mozo
    int codprod;    // num de 3 digitos
    int cantidad;   // cantidad de comandas
    float comision; // cantidad ganada
};
 
int main()
{
    // ---------------------------------------------------------
    // CUANTOS ARCHIVOS DIARIOS SE VAN A UNIR
    // ---------------------------------------------------------
    int cant;
    cout << "Cantidad de archivos diarios a unir (1-" << MAX_ARCHIVOS << "): ";
    cin >> cant;
 
    if (cant < 1 || cant > MAX_ARCHIVOS)
    {
        cout << "Cantidad invalida." << endl;
        return 1;
    }
 
    FILE *archivos[MAX_ARCHIVOS]; // un FILE* por archivo diario
    comanda actual[MAX_ARCHIVOS]; // registro actual de cada archivo
    bool hay[MAX_ARCHIVOS];       // true si el archivo todavia tiene un registro disponible
 
    // ---------------------------------------------------------
    // APERTURA DE LOS ARCHIVOS + LECTURA ANTICIPADA
    // ---------------------------------------------------------
    for (int i = 0; i < cant; i++)
    {
        int dia, mesArchivo, anio;
 
        cout << "\nArchivo " << i + 1 << " de " << cant << endl;
        cout << "Ingrese el dia: ";
        cin >> dia;
        cout << "Ingrese el mes: ";
        cin >> mesArchivo;
        cout << "Ingrese el anio: ";
        cin >> anio;
 
        if (dia < 1 || dia > 31 || mesArchivo < 1 || mesArchivo > 12 || anio < 1)
        {
            cout << "La fecha ingresada no es valida." << endl;
            for (int k = 0; k < i; k++)
                fclose(archivos[k]);
            return 1;
        }
 
        string nombrearchivo = "datos/comandas_" + to_string(dia) + "-" + to_string(mesArchivo) + "-" + to_string(anio) + ".dat";
 
        archivos[i] = fopen(nombrearchivo.c_str(), "rb");
 
        if (archivos[i] == NULL)
        {
            cout << "No se pudo abrir el archivo: " << nombrearchivo << endl;
            for (int k = 0; k < i; k++)
                fclose(archivos[k]);
            return 1;
        }
 
        // lectura anticipada: primer registro de cada archivo
        hay[i] = (fread(&actual[i], sizeof(comanda), 1, archivos[i]) == 1);
    }
 
    // ---------------------------------------------------------
    // ARCHIVO DE SALIDA
    // ---------------------------------------------------------
    int semana = 0;
    int mes = 0;
    cout << "\nNumero de mes: ";
    cin >> mes;
    cout << "Numero de semana: ";
    cin >> semana;
    string nombre = "datos/comandas_semana_S" + to_string(semana) + "-" + to_string(mes) + ".dat";
 
    FILE *salida = fopen(nombre.c_str(), "wb");
 
    if (salida == NULL)
    {
        cout << "No se pudo crear el archivo de la planilla." << endl;
        for (int i = 0; i < cant; i++)
            fclose(archivos[i]);
        return 1;
    }
 
    // ---------------------------------------------------------
    // APAREO DE LOS ARCHIVOS (ordenado por idmozo)
    // Imprime la planilla y escribe el archivo de salida a la vez.
    // ---------------------------------------------------------
    cout << "\n"
         << left << setw(12) << "MOZO" << setw(10) << "CODPROD"
         << setw(10) << "CANTIDAD" << setw(10) << "COMISION" << "\n";
 
    while (true)
    {
        // buscar entre los archivos que todavia tienen registros el de menor idmozo
        int menor = -1;
        for (int i = 0; i < cant; i++)
        {
            if (hay[i] && (menor == -1 || actual[i].idmozo < actual[menor].idmozo))
                menor = i;
        }
 
        if (menor == -1) // ningun archivo tiene mas registros
            break;
 
        comanda c = actual[menor];
 
        cout << left << setw(12) << c.idmozo
             << setw(10) << c.codprod
             << setw(10) << c.cantidad
             << fixed << setprecision(2) << setw(10) << c.comision
             << "\n";
 
        fwrite(&c, sizeof(comanda), 1, salida);
 
        // avanzar solo en el archivo del que salio el registro
        hay[menor] = (fread(&actual[menor], sizeof(comanda), 1, archivos[menor]) == 1);
    }
 
    // ---------------------------------------------------------
    // CIERRE
    // ---------------------------------------------------------
    for (int i = 0; i < cant; i++)
        fclose(archivos[i]);
    fclose(salida);
 
    return 0;
}