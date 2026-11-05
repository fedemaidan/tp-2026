#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

const float TASA_COMISION = 0.10f;
const int K = 5;
const int MAX_MOZOS = 100;
const int MAX_PRODUCTOS = 100;
const int MAX_COMANDAS = 1000;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int cargarMozo(Mozo lista[]) { // CARGAR LOS MOZOS de "mozos.dat"
    FILE* archivoMozo = fopen("datos/mozos.dat", "rb"); // SE TIENE QUE CAMBIAR
    if(archivoMozo == NULL) {
        cout << "Error al abrir mozos.dat" << endl;
        return -1;
    }

    int MozosCargados = 0;

    while(MozosCargados < MAX_MOZOS && fread (&lista[MozosCargados], sizeof(Mozo), 1, archivoMozo)== 1) {
        MozosCargados++;
    }

    fclose (archivoMozo);
    return MozosCargados;
}

int cargarProducto(Producto lista[]) { // CARGAR LOS PRODUCTOS DE "inventario.dat"
    FILE* archivoProducto = fopen("datos/inventario.dat", "rb");
    if(archivoProducto == NULL) {
        cout << "Error al abrir inventario.dat" << endl;
        return -1;
    }

    int ProductosCargados = 0;

    while(ProductosCargados < MAX_PRODUCTOS && fread(&lista[ProductosCargados], sizeof(Producto), 1, archivoProducto) == 1) {
        ProductosCargados++;
    }

    fclose(archivoProducto);
    return ProductosCargados;
}
bool comprobandoFecha (char fecha[]) { // COMPROBAR QUE LA FECHA ESTE BIEN PUESTA
    if (strlen(fecha) != 10) {
        return false;
    }
    if (fecha[2] != '-' ) {
        return false;
    }
    if (fecha [5] != '-') {
        return false;
    }
    
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            continue;
        }
        if (fecha[i] < '0' || fecha[i] > '9') {
            return false;
        }
    }

    int dia = (fecha[0] - '0') * 10 + (fecha[1] - '0');
    int mes = (fecha[3] - '0') * 10 + (fecha[4] - '0');

    if (dia < 1 || dia > 31) {
        return false;
    }
    if (mes < 1 || mes > 12) {
        return false;
    }

    return true;
}

void encriptar (char clave[]) { // ENCRIPTAR LAS CLAVES DE LOS MOZOS
    for (int i = 0; clave[i] != '\0'; i++) {
        clave[i] = clave[i] + K;
    }
}

int buscarMozo(int idBuscado, Mozo lista[], int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (lista[i].idMozo == idBuscado) {
            return i;
        }
    }
    return -1;
}

int login(Mozo lista[], int cantidad) { // PIDE ID Y CLAVE DE LOS MOZOS
    int id;
    char clave[20];

    cout << "ingrese su ID de mozo: ";
    cin >> id;

    int pos = buscarMozo(id, lista, cantidad);
    if (pos == -1) {
        cout << "ID de mozo invalido." << endl;
        return -1;
    }

    cout << "ingrese su clave: ";
    cin.ignore();
    cin.getline(clave, 20);
    encriptar(clave);

    if (strcmp(clave, lista[pos].password) != 0) {
        cout << "clave incorrecta." << endl;
        return -1;
    }
    return pos;
}

int buscarProducto(int codigoBuscado, Producto lista[], int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (lista[i].codigo == codigoBuscado) {
            return i;
        }
    }
    return -1;
}

float calcularComision(float precio, int cantidad) {
    return precio * cantidad * TASA_COMISION;
}


bool guardarComanda(char nombreArchivo[], Comanda c) { // GENERA O REESCRIBE LA COMANDA 
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "datos/%s", nombreArchivo);
    FILE* archivo = fopen(ruta, "ab");
    if (archivo == NULL) {
        cout << "Error al abrir " << nombreArchivo << endl;
        return false;
    }
 
    fwrite(&c, sizeof(Comanda), 1, archivo);
    fclose(archivo);
    return true;
}

bool actualizarInventario(int posicion, int nuevoStock) { // ACTUALIZA LOS PRODUCTOS DE "inventario.dat"
    FILE* archivo = fopen("datos/inventario.dat", "r+b");
    if (archivo == NULL) {
        cout <<"Error al abrir inventario.dat" << endl;
        return false;
    }

    Producto p;

    fseek(archivo, posicion * sizeof(Producto), SEEK_SET);
    if (fread(&p, sizeof(Producto), 1, archivo) != 1) {
        cout << "Error al leer el producto." << endl;
        fclose(archivo);
        return false;
    }

    p.stockActual = nuevoStock;

    fseek(archivo, posicion * sizeof(Producto), SEEK_SET);
    fwrite(&p, sizeof(Producto), 1, archivo);
    fclose(archivo);
    return true;
}
 
void cargarVenta(Mozo listaMozos[], int cantidadMozos, Producto listaProductos[], int cantidadProductos, char nombreArchivo[]) { // BUSCA, MUESTRA Y EDITA EL STOCK DE LOS PRODUCTOS PEDIDOS
    int posMozo = login(listaMozos, cantidadMozos);
    if (posMozo == -1) {
        cout << "Venta descartada." << endl;
        return;
    }
 
    int codigo;
    cout << "Ingrese el codigo del producto: ";
    cin >> codigo;
 
    int posProducto = buscarProducto(codigo, listaProductos, cantidadProductos);
    if (posProducto == -1) {
        cout << "El producto no existe." << endl;
        return;
    }
 
    cout << "Producto: " << listaProductos[posProducto].descripcion
         << " - Precio: $" << listaProductos[posProducto].precio
         << " - Stock: " << listaProductos[posProducto].stockActual << endl;
 
    int cantidad;
    cout << "Ingrese la cantidad: ";
    cin >> cantidad;
 
    if (cantidad <= 0) {
        cout << "Cantidad invalida." << endl;
        return;
    }
    if (cantidad > listaProductos[posProducto].stockActual) {
        cout << "Stock insuficiente." << endl;
        return;
    }
 
    Comanda c;
    c.idMozo = listaMozos[posMozo].idMozo;
    c.codigoProducto = listaProductos[posProducto].codigo;
    c.cantidad = cantidad;
    c.comision = calcularComision(listaProductos[posProducto].precio, cantidad);
 
    if (!guardarComanda(nombreArchivo, c)) {
        cout << "No se pudo guardar la venta." << endl;
        return;
    }
 
    listaProductos[posProducto].stockActual = listaProductos[posProducto].stockActual - cantidad;
    if (!actualizarInventario(posProducto, listaProductos[posProducto].stockActual)) {
        cout << "Aviso: No se pudo actualizar el stock en el inventario." << endl;
    }
 
    cout << "Venta guardada. Comision: $" << c.comision << endl;
}

void ordenarMozo(Comanda lista[], int cantidad) {
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - 1 - i; j++) {
            if (lista[j].idMozo > lista[j + 1].idMozo) {
                Comanda aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
            }
        }
    }
}

void ordenarPlanilla(char nombreArchivo[]) {
    FILE* archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL) {
        cout << "No hay ventas para ordenar en la planilla." << endl;
        return;
    }

    Comanda lista[MAX_COMANDAS];
    int cantidad = 0;

    while (cantidad < MAX_COMANDAS && fread(&lista[cantidad], sizeof(Comanda), 1, archivo) == 1) {
        cantidad++;
    }

    Comanda sobrante;
    if (cantidad == MAX_COMANDAS && fread(&sobrante, sizeof(Comanda), 1, archivo) == 1) {
        cout << "La planilla tiene demasiadas ventas para ordenarla. No se modifico el archivo." << endl;
        fclose(archivo);
        return;
    }
    fclose(archivo);
 
    ordenarMozo(lista, cantidad);
 
    archivo = fopen(nombreArchivo, "wb");
    if (archivo == NULL) {
        cout << "Error al reescribir " << nombreArchivo << endl;
        return;
    }
    for (int i = 0; i < cantidad; i++) {
        fwrite(&lista[i], sizeof(Comanda), 1, archivo);
    }
    fclose(archivo);
 
    cout << "Planilla ordenada (" << cantidad << " ventas)." << endl;
}

int main () {
    Mozo listaMozos[MAX_MOZOS];
    Producto listaProductos[MAX_PRODUCTOS];

    int cantidadMozos = cargarMozo(listaMozos);
    if (cantidadMozos == -1) {
        return 1;
    }

    int cantidadProductos = cargarProducto(listaProductos);
    if (cantidadProductos == -1) {
        return 1;
    }

    cout << "Se cargaron: " << cantidadMozos << " Mozos y " << cantidadProductos << " Productos." << endl;

    int posMozo = login(listaMozos, cantidadMozos);
    while (posMozo == -1) {
        cout << "Error al iniciar sesion." << endl;
        posMozo = login(listaMozos, cantidadMozos);
    } 
    cout << "Bienvenido, " << listaMozos[posMozo].nombre << endl;

    char fecha[11];

    cout << "Ingrese la fecha de hoy (DD-MM-AAAA)" << endl;
    cin >> fecha;

    while (!comprobandoFecha(fecha)) {
        cout << "Fecha incorrecta, compruebe que coincida con lo puesto entre parentesis ()." << endl;
        cout << "Ingrese la fecha de hoy (DD-MM-AAAA)" << endl;
        cin >> fecha;
    }

    char nombreArchivo[30];
    strcpy(nombreArchivo, "comandas_");
    strcat(nombreArchivo, fecha);
    strcat(nombreArchivo, ".dat");

    cout << "Archivo del dia: " << nombreArchivo << endl;
    
    int opcion = -1;
    while (opcion != 0) {
        cout << endl;
        cout << "===== MENU =====" << endl;
        cout << "1. Cargar venta" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
 
        if (opcion == 1) {
            cargarVenta(listaMozos, cantidadMozos, listaProductos, cantidadProductos, nombreArchivo);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }

    ordenarPlanilla(nombreArchivo);

    cout << "Carga terminada." << endl;
    
    return 0;  
}