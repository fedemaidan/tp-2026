#include <iostream>
#include <cstdio>
using namespace std;

struct ComandaHistorica {
	char  fecha[11];         // "DD-MM-AAAA" 
	char  nombreMozo[50];   // el nombre completo, repetido en cada venta 
	int codigoProducto;
	int cantidad;
	float comision;
};

struct Producto {
	int   codigo;
	char descripcion[50];
	float precio;
	int stockActual;
};
struct Mozo {
	int idMozo;
	char nombre[50];
	char password[20];
	float totalComision;
};
struct Comanda {
	int idMozo;
	int codigoProducto;
	int cantidad;
	float comision;
};
struct Nodo {
	Comanda info;
	Nodo* sig;
};

Nodo* crearNodo(Comanda valor) {
	Nodo* nuevoNodo = new Nodo();
	nuevoNodo->info = valor;
	nuevoNodo->sig = NULL;
	return nuevoNodo;
};


const float TASA_COMISION = 0.10f;   // la comision de cada venta es el 10% de lo vendido

float precio(const char* Archivo, int cod); // busca el pecio
void actualizar(const char* Archivo, int cod, int cantidadVendida); // actualiza el stock
void pedirventa(const char* Archivo); // pide la venta
bool valido(int cod, const char* Archivo);// verifica el mozo
bool password(char pass[], char mpass[]); // verifica la contraseña del mozo
void ordenar(const char* Archivo); // ordena la planilla por mozo
void planilla(int dia, int mes, int anio, char nombre[]); // crea la planilla del día
bool validarFecha(int dia, int mes, int anio); // valida la fecha ingresada
Nodo* insertarOrdenado(Nodo* inicio, Comanda valor); // inserta las comandas en una lista ordenada por mozo
void borrarLista(Nodo*& lista);
int busquedaBinaria(const char* Archivo, int valor);


int main() {
	// funcion fecha 
	int dia, mes, anio; // Pide la fecha del día
	cout << "Ingrese la fecha (DD MM AAAA): ";
	cin >> dia >> mes >> anio;
	bool fecha = validarFecha(dia, mes, anio);
	while (!fecha) {
		cout << "fecha incorrecta, ingresela nuevamente" << endl;
		cin >> dia >> mes >> anio;
		fecha = validarFecha(dia, mes, anio);
	}
	char planillaDia[30];
	planilla(dia, mes, anio, planillaDia); // crea la planilla del día
	pedirventa(planillaDia);

	return 0;
}

float precio(const char* Archivo, int cod) {
	int pos = busquedaBinaria(Archivo, cod); // busco la posicion del producto en el archivo
	if (pos == -1) { // si no se encuentra el producto
		return -1;
	}
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo de inventario." << endl; // si no se puede abrir el archivo
		return -1; // para que no se ejecute el resto de la funcion
	}
	Producto prod;
	fseek(f, pos * sizeof(Producto), SEEK_SET); // me muevo a la posicion del producto
	fread(&prod, sizeof(Producto), 1, f);// leo el producto
	fclose(f);
	return prod.precio;
}


void actualizar(const char* Archivo, int cod, int cantidadVendida) {
	int pos = busquedaBinaria(Archivo, cod); // busco la posicion del producto en el archivo
	if (pos == -1) { // si no se encuentra el producto
		return;
	}
	FILE* f = fopen(Archivo, "rb+"); // abro el archivo
	if (f == NULL) {
		cout << "Error al abrir el archivo de inventario." << endl; // si no se puede abrir el archivo
		return; // para que no se ejecute el resto de la funcion
	}
	Producto prod;
	fseek(f, pos * sizeof(Producto), SEEK_SET); // me muevo a la posicion del producto
	fread(&prod, sizeof(Producto), 1, f);// leo el producto
	prod.stockActual -= cantidadVendida; // resto la cantidad vendida
	fseek(f, -sizeof(Producto), SEEK_CUR); // vuelvo al producto a modificar
	fwrite(&prod, sizeof(Producto), 1, f); // escribo el producto actualizado
	cout << "Stock actual: " << prod.stockActual << endl; // muestro el stock actual
	if (prod.stockActual <= 0) {
		cout << "stock agotado" << endl;// si no hay mas stock

	}
	fclose(f);// cierro el archivo 
	cout << "Stock actualizado con exito" << endl;
}


void pedirventa(const char* Archivo) {
	FILE* f = fopen(Archivo, "ab+");
	if (f == NULL) {
		cout << "Error al abrir el archivo de comandas." << endl;
		return;
	}
	int n = 1;
	Comanda c;
	while (n != 0) {

		cout << "Ingrese el codigo de mozo: ";
		cin >> c.idMozo;
		bool esValido = valido(c.idMozo, "mozos.dat"); // FUNCION verifica el mozo 
		while (!esValido) {
			cout << "Codigo de mozo inválido" << endl;
			cout << "ingrese un codigo de mozo valido" << endl;
			cin >> c.idMozo;
			esValido = valido(c.idMozo, "mozos.dat");
		}
		cout << "Ingrese codigo del producto: ";
		cin >> c.codigoProducto;
		cout << "Ingrese cantidad vendida: ";
		cin >> c.cantidad;
		while (c.cantidad <= 0) {
			cout << "ingesaste una cantidad incorrectar, vuelve a ingresarla " << endl;
			cin >> c.cantidad;
		}
		float p = precio("datos/inventario.dat", c.codigoProducto);
		while (p == -1) {
			cout << "Producto inexistente" << endl;
			cout << "Ingrese un codigo de producto válido: ";
			cin >> c.codigoProducto;
			p = precio("datos/inventario.dat", c.codigoProducto);
		}
		c.comision = p * c.cantidad * TASA_COMISION;
		fwrite(&c, sizeof(Comanda), 1, f);
		actualizar("datos/inventario.dat", c.codigoProducto, c.cantidad);
		cout << "Ingrese 0 para terminar o 1 para continuar: ";
		cin >> n;

	}
	fclose(f); // cierro el archivo para poder usar ordenar 
	ordenar(Archivo);
}

bool valido(int cod, const char* Archivo) {
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo de mozos." << endl;
		return false;
	}
	Mozo m;
	char pass[20];
	int leido = fread(&m, sizeof(Mozo), 1, f);
	while (leido == 1) {
		if (cod == m.idMozo) {
			cout << "Ingrese password: ";
			cin >> pass;
			fclose(f);
			return password(pass, m.password);
		}
		leido = fread(&m, sizeof(Mozo), 1, f);
	}
	fclose(f);
	cout << "Mozo no encontrado." << endl;
	return false;
}

bool password(char pass[], char mpass[]) {
	int k = 8;
	int i = 0;
	while (pass[i] != '\0' && mpass[i] != '\0') {
		if (pass[i] + k != mpass[i]) {
			cout << "contraseña incorrecta" << endl;
			return false;
		}
		i++;
	}
	if (pass[i] != '\0' || mpass[i] != '\0') {
		cout << "contraseña incorrecta" << endl;
		return false;
	}
	return true;
}

void ordenar(const char* Archivo) { // funcion que pasa a una lista ordenada por mozo
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo." << endl;
		return;
	}
	Nodo* lista = NULL;
	Comanda c;
	int leido = fread(&c, sizeof(Comanda), 1, f);
	while (leido == 1) {
		lista = insertarOrdenado(lista, c); // inserto en la lista ordenada por mozo
		leido = fread(&c, sizeof(Comanda), 1, f);
	}
	fclose(f);

	FILE* f2 = fopen(Archivo, "wb");
	if (f2 == NULL) {
		cout << "Error al abrir el archivo." << endl;
		borrarLista(lista);
		return;
	}
	Nodo* actual = lista;
	while (actual != NULL) {
		fwrite(&actual->info, sizeof(Comanda), 1, f2); // paso la lista a un archivo otra vez
		actual = actual->sig;
	}
	fclose(f2);
	borrarLista(lista);
}

void planilla(int dia, int mes, int anio, char nombre[]) {
	// Abre o crea la planilla de ese día 
	sprintf(nombre, "comandas_%02d-%02d-%04d.dat", dia, mes, anio);
	FILE* f = fopen(nombre, "ab+");
	if (f == NULL) {
		cout << "Error al abrir el archivo de comandas." << endl;
		return;
	}
	fclose(f);
}

bool validarFecha(int dia, int mes, int anio) {
	if (dia < 1 || dia > 31) {
		return false;
	}
	if (mes < 1 || mes > 12) {
		return false;
	}
	if (anio < 2025) {
		return false;
	}
	switch (mes) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return dia <= 31;

	case 4:
	case 6:
	case 9:
	case 11:
		return dia <= 30;

	case 2:
		return dia <= 29;
	}
	return false;
}

Nodo* insertarOrdenado(Nodo* inicio, Comanda valor) {
	Nodo* nuevoNodo = crearNodo(valor);

	// Caso especial: lista vacia o el valor va primero
	if (inicio == NULL || valor.idMozo < inicio->info.idMozo) {
		nuevoNodo->sig = inicio;
		return nuevoNodo;
	}

	Nodo* actual = inicio;
	while (actual->sig != NULL && actual->sig->info.idMozo < valor.idMozo) {
		actual = actual->sig;
	}
	nuevoNodo->sig = actual->sig;
	actual->sig = nuevoNodo;
	return inicio;
}
void borrarLista(Nodo*& lista) {
	while (lista != NULL) {
		Nodo* borrar = lista;
		lista = lista->sig;
		delete borrar;
	}
}
int busquedaBinaria(const char* Archivo, int cod) {
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo." << endl;
		return -1;
	}
	Producto prod;
	// calcular cantidad de registros
	fseek(f, 0, SEEK_END);// me muevo al final del archivo
	long tamanioArchivo = ftell(f);//cuantos bytes tiene el archivo
	int item = tamanioArchivo / sizeof(Producto);// cuantos productos hay en el archivo

	int medio;
	int izquierda = 0; // posicion inicial
	int derecha = item - 1; // posicion final
	while (izquierda <= derecha) {
		medio = (izquierda + derecha) / 2; // defino el medio 
		fseek(f, medio * sizeof(Producto), SEEK_SET); // me muevo a la mitad del archivo
		fread(&prod, sizeof(Producto), 1, f); // leo el producto en la mitad y lo asigna a prod
		if (prod.codigo == cod) { // si el codigo coincide
			fclose(f);
			return medio; // encontrado 
		}
		else if (prod.codigo < cod) {
			izquierda = medio + 1; // buscar en la mitad derecha 
		}
		else {
			derecha = medio - 1;
		}
	}
	cout << "Producto no encontrado." << endl;
	fclose(f);
	return -1;  // no encontrado 
}

