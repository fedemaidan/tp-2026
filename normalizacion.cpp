#include <iostream>
#include <cstdio>
using namespace std;


/* struct de archivos existente comandas_histroicas.dat */
struct ComandaHistorica{
	char fecha[11];  // "DD-MM-AAAA"
	char nombreMozo[50];
	int codigoProducto;
	int cantidad;
	float comision;
};


/* struct de archivo nuevo mozos.dat */
struct Mozo{
	int idMozo;
	char nombre[50];
	char password[20];
	float totalComision;
};


/* struct del nodo de cada mozo */
struct NodoMozo{
	Mozo info;
	NodoMozo* sig;
};


/* struct que define la nueva forma en la que se registraran las comandas */
struct Comanda{
	int idMozo;
	int codigoProducto;
	int cantidad;
	float comision;
};


/* struct del nodo de cada comanda */
struct NodoComanda{
	Comanda info;
	NodoComanda* sig;
};


/* struct del nodo de cada fecha en que se realizaron comandas */
struct NodoFecha {
	char fecha[11];
	NodoComanda* listaComandas;
	NodoFecha* sig;
};


/* struct de archivo "inventario.dat" */
struct Producto {
	int codigo;
	char descripcion[50];
	float precio;
	int stockActual;
};


/* funcion que crea modifica la contraseña de los mozos para su posterior verificacion,
usando su ID como texto (char) y una constante K definida arbitrariamente*/
void encriptarContra (char* contrasenia, char* destino){
	int k=8; // K = 8 (valor arbitrario)
	int i=0;
	while (contrasenia[i] != '\0'){
		destino[i] = contrasenia[i]+k; // se suma 8 a cada carcter de la contraseña
		i++;
	}
	destino[i] = '\0';
}


/* funcion de comapracion entre dos parametros A y B */
bool comparacion(char cadenaA[], char cadenaB[]){
	int i=0;
	while (cadenaA[i] != '\0' && cadenaB[i] != '\0'){
		if (cadenaA[i] != cadenaB[i]){
			return false;
		}
		i++;
	}
	if (cadenaA[i] == '\0' && cadenaB[i] == '\0'){
		return true;
     }
     return false;
}


/* funcion que lee el archivo existente "comandas_historicas.dat", identifica a todos los mozos y los guarda
de manera ordenada (por ID) en un nuevo archivo "mozos.dat" */
void guardarYordenarMozos (){
	FILE* f = fopen("datos/comandas_historicas.dat", "rb");
	if (f==NULL){
		cout<<"error, no se pudo abrir el archivo ´comandas_historicas.dat´"<<endl;
		return;
	}
	
	NodoMozo* lista = NULL; // puntero a el primer nodo 
	int len = 0; // cantidad de nodos en la lista
	
	ComandaHistorica comanda;
	while(fread(&comanda, sizeof(ComandaHistorica), 1, f)==1){
		
		NodoMozo* punteroAux = lista; // puntero a el nodo de comapracion 
		NodoMozo* encontrado = NULL;
		
		while (punteroAux != NULL){
			bool sonIguales = comparacion(comanda.nombreMozo, punteroAux->info.nombre); // verificamos si el mozo ya esta en la lista
		     if (sonIguales){
			     encontrado = punteroAux;
			     break;
		}
		punteroAux = punteroAux->sig;
		}
		
		// accion consecuente 
		if (encontrado != NULL){ // cuando si existe, se suma la comision al mozo
			encontrado->info.totalComision += comanda.comision;
		}
		else { // cuando no existe el mozo, se crea un nuevo nodo 
			NodoMozo* nuevoMozo = new NodoMozo();
			len++;
			
			// agregamos los datos pertenecientes al mozo
			nuevoMozo->info.idMozo = len; // agregar id
			
			// agregar nombre
			int i=0; 
			while (comanda.nombreMozo[i] != '\0'){
				nuevoMozo->info.nombre[i] = comanda.nombreMozo[i];
				i++;
			}
			nuevoMozo->info.nombre[i] = '\0';
			
			// agregar contraseña encriptada ????? 
			char contraChar[20]; 
			sprintf(contraChar, "%d", nuevoMozo->info.idMozo); // Estructrua auxiliar! convierte dato int en dato char
			encriptarContra(contraChar, nuevoMozo->info.password); 
			
			nuevoMozo->info.totalComision = comanda.comision; // agregar comision
			
			// se enlaza el neuvo mozo/nodo a la lista (al principio)
			nuevoMozo->sig = lista; 
			lista = nuevoMozo;
		}
	}
	fclose(f);
	
	/* recorrer la lista creada con mozos y guardar a cada uno (cada nodo) en un nuevo archivo llamado 
	"mozos.dat" . Hacer "delete" para cada "new" realizado */
	FILE* mozos = fopen("mozos.dat", "wb");
	if (mozos == NULL){
		cout<<"error, no se pudo crear el archivo ´mozos.dat´"<<endl;
		return;
	}
	
	while (lista != NULL){
		fwrite(&(lista->info), sizeof(Mozo),1, mozos); // escribimos en el archivo solo los datos de la struct "mozo" (se encuentran en "info"
		NodoMozo* temp = lista; // guardamos puntero en un puntero auxiliar
		lista = lista->sig; // avanzar puntero valido para la funcion (siguiente nodo)
		delete temp; // borrar/liberar memoria de puntero (nodo) ya usado
	}
	fclose(mozos);
	
}


// funcion para mostrar por pantalla el contenido gaurdado en "mozos.dat"
void leerYmostrarMozos() {
    FILE* f = fopen("mozos.dat", "rb"); // abrimos el archivo recién creado
    if (f == NULL) {
          cout << "error, no se pudo abrir mozos.dat" << endl;
          return;
    }

    cout << "CONTENIDO DEL ARCHIVO MOZOS.DAT " << endl;

    Mozo m;
    while (fread(&m, sizeof(Mozo), 1, f) == 1) { 
          cout<<"- Id: "<<m.idMozo<<endl;
          cout<<"- Nombre: "<<m.nombre<<endl;
          cout<<"- Contraseña encriptada: "<<m.password<<endl;
	     cout<<"- Comisiones acumuladas: "<<m.totalComision<<endl;
          cout<<" "<<endl;
    }

    fclose(f); 
}



/* funcion que retorna el id del mozo de acuerdo al nombre ingresado, buscandolo en mozos.dat */
int buscarIdMozo (char nombreMozo[]){
	FILE* f = fopen("mozos.dat", "rb");
	int encontrado = -1;
	
	if (f == NULL){
		cout<<"error, no se pudo abrir el archivo ´mozos.dat´"<<endl;
		return encontrado;
	}
	
	Mozo m;
	while (fread(&m, sizeof(Mozo), 1, f)==1){
		bool sonIguales = comparacion(nombreMozo, m.nombre);
		if (sonIguales){
			encontrado = m.idMozo;
			break;
	     }
	}
	fclose(f);
	return encontrado;
}


/* funcion que lee comandas_hisotricas.dat y crea por cada fecha un archivo con las comandas realizadas ese dia */
void separarComandasPorFecha (){
	FILE* f = fopen("datos/comandas_historicas.dat", "rb");
	if (f == NULL){
		cout<<"error, no se pudo abrir el archivo ´comandas_historicas.dat´"<<endl;
		return;
	}
	
	NodoFecha* listaFechas = NULL;
	ComandaHistorica comanda;
	
	while (fread(&comanda, sizeof(ComandaHistorica), 1, f) == 1){
		
		NodoFecha* fechaAux = listaFechas; // puntero de comparacion
		NodoFecha* encontrado = NULL;
		
		// busca si la fecha ya existe entre todos las que hay en la lista, comaprando caracter por caracter
		while (fechaAux != NULL){ 
		     bool sonIguales = comparacion(comanda.fecha, fechaAux->fecha);
		     if(sonIguales){
		     	encontrado = fechaAux;
		     	break;
			}
			fechaAux = fechaAux->sig;
		}
		
		// accion consecuente
		if (encontrado == NULL){ // no existe la fecha todavia. Se agrega un nuevo nodo
			NodoFecha* nuevaFecha = new NodoFecha;
			
			// copia y agrega fecha
			int i=0; 
			while (comanda.fecha[i] != '\0'){
				nuevaFecha->fecha[i] = comanda.fecha[i];
				i++;
			}
			nuevaFecha->fecha[i] = '\0';
			
			nuevaFecha->listaComandas = NULL; // asigna como vacio al puntero de su sublista
			
			// se enlaza el nuevo nodo al inicio de la lista
			nuevaFecha->sig = listaFechas;
			listaFechas = nuevaFecha;
			encontrado = nuevaFecha;
		}
		
		// crear el nodo de la comanda
		NodoComanda* nuevaComanda = new NodoComanda;
		
		// cargar datos del neuvo nodo
		nuevaComanda->info.idMozo = buscarIdMozo(comanda.nombreMozo);
		nuevaComanda->info.codigoProducto = comanda.codigoProducto;
		nuevaComanda->info.cantidad = comanda.cantidad;
		nuevaComanda->info.comision = comanda.comision;
		nuevaComanda->sig = NULL;
		
		// agregar la nueva comanda a la sublista de la fecha en que se realizo , de manera ordenada segun idMozo
		if (encontrado->listaComandas == NULL || encontrado->listaComandas->info.idMozo > nuevaComanda->info.idMozo){
			nuevaComanda->sig = encontrado->listaComandas;
			encontrado->listaComandas = nuevaComanda;
		}
		else{
			NodoComanda* actual = encontrado->listaComandas;
			while (actual->sig != NULL && actual->sig->info.idMozo <= nuevaComanda->info.idMozo){
				actual = actual->sig;
			}
			nuevaComanda->sig = actual->sig;
			actual->sig = nuevaComanda;
		}
	}
	fclose(f);
	
	// guardamos todos los datos pertenecientes a la lista en los archivos correspondientes
	NodoFecha* fechaActual = listaFechas;
	
	while (fechaActual != NULL){ // ciclo que recorre cada nodo (cada fecha)
		
		// codigo que copia el nombre del archivo correspondiente en uan variable llamasa nombreArchivo para usarla al momento de crear el archivo
		char nombreArchivo[30];
		int caracteres = 0;
		
		char texto1[] = "comandas_";
		int i = 0;
		while (texto1[i] != '\0'){
			nombreArchivo[caracteres] = texto1[i];
			caracteres++;
			i++; 
		}
		
		int j = 0;
		while (fechaActual->fecha[j] != '\0'){
			nombreArchivo[caracteres] = fechaActual->fecha[j];
			caracteres++;
			j++;
		}
		
		char texto2[] = ".dat";
		int k = 0;
		while (texto2[k] != '\0'){
			nombreArchivo[caracteres] = texto2[k];
			caracteres++;
			k++;
		}
		
		nombreArchivo[caracteres] = '\0';
		
		// craer archivo correspondiente
		FILE* f = fopen(nombreArchivo, "wb");
		if (f == NULL){
			cout<<"error, no se pudo abrir un nuevo archivo"<<endl;
		}
		else{
			NodoComanda* comandaActual = fechaActual->listaComandas; // puntero al primer nodo (comanda) de la fecha
			
			while (comandaActual != NULL){ // ciclo que recorre cada nodo (cada comanda) de esta fecha
			     fwrite(&(comandaActual->info), sizeof(Comanda), 1, f);
			     NodoComanda* temp = comandaActual;
			     comandaActual = comandaActual->sig;
			     delete temp;
		     }
		     fclose(f);
		}	
		NodoFecha* temp = fechaActual;
		fechaActual = fechaActual->sig;
		delete temp;
	}
}


/* funcion auxiliar para leer las comandas realizadas en una fecha especifica y mostrarlo 
por pantalla */
void mostrarComandas(const char fecha[]) {
    char nombreArchivo[50];
    sprintf(nombreArchivo, "comandas_%s.dat", fecha);
    
    FILE* f = fopen(nombreArchivo, "rb");
    if(f == NULL) {
          cout<<"No existe el archivo "<<nombreArchivo<<endl;
          return;
    }
    
    cout<<"COMANDAS DEL DIA  "<<fecha<< endl;
    Comanda c;
    while(fread(&c, sizeof(Comanda), 1, f) == 1) {
          cout<<"Id mozo: "<< c.idMozo<<endl;
          cout<<"codigo producto: " << c.codigoProducto<<endl;
          cout<<"cantidad: "<< c.cantidad<<endl;
          cout<<"comision: "<<c.comision<<endl;
          cout<<" "<<endl;
    }
    fclose(f);
}


/* funcion que el archivo existente "inventario.dat" y lo actualiza segun las ventas realizadas registradas 
"comandas_histroicas.dat" */
void actualizarInventario (){
	//abrimos "comandas_hisoricas" en modo lectura
	FILE* f = fopen("datos/comandas_historicas.dat", "rb");
	if (f == NULL){
		cout<<"error, no se pudo abrir el archivo ´comandas_historicas.dat´"<<endl;
		return;
	}
	
	// abrimos "inventario" en modo lectura ye scritura para reescribirlo ya atcualizarlo
	FILE* f2 = fopen("datos/inventario.dat", "rb+");
	if (f2 == NULL){
		cout<<"error, no se pudo abrir el archivo ´inventario.dat´"<<endl;
		return;
	}
	
	// recorremos los registros del archivo "comandas_hoistroicas"
	ComandaHistorica comanda;
	while (fread(&comanda, sizeof(ComandaHistorica), 1, f)==1){
		
		// "devolvemos el puntero del archivo "inventario" al inicio
		fseek(f2, 0, SEEK_SET); 
		
		// recorremos archivo "inventario"
		Producto produ;
		while (fread(&produ, sizeof(Producto), 1, f2)==1){
			if (comanda.codigoProducto == produ.codigo){
				produ.stockActual -= comanda.cantidad; // restamos al stock la cantidad vendida
				fseek(f2, -(long)sizeof(Producto), SEEK_CUR); // retrocedemos el puntero un registro hacia atras
				fwrite(&produ, sizeof(Producto), 1, f2); // reescribimos con la actualizacion implementada
				break;
			}
		}
	}
	fclose(f);
	fclose(f2);
	cout<<"inventario correctamente actualizado"<<endl;	
}

void mostrarInventario(){
	FILE* f = fopen("datos/inventario.dat", "rb");
	if (f == NULL){
		cout<<"error, no se pudo abrir ela rchivo ´inventario.dat´"<<endl;
		return;
	}
	cout<<"CONTENIDO DEL INVENTARIO"<<endl;
	Producto produ;
	while (fread(&produ, sizeof(Producto), 1, f)==1){
		cout<<"- Codigo: "<<produ.codigo<<endl;
		cout<<"- Descripcion: "<<produ.descripcion<<endl;
		cout<<"- Precio: $"<<produ.precio<<endl;
		cout<<"- Stock: "<<produ.stockActual<<endl;
		cout<<" "<<endl;
	}
	fclose(f);
}



int main() {
    guardarYordenarMozos(); // creacion de mozos.dat
    cout<<"La operacion se realizo con exito"<<endl;
    cout<<" "<<endl;
    leerYmostrarMozos(); // mostrar contenido guardado en mozos.dat 
    
    cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    cout<<" "<<endl;
    
    separarComandasPorFecha(); // creacion de varios archivos
    cout<<"La operacion se realizo con exito"<<endl;
    cout<<" "<<endl; 
    mostrarComandas("02-06-2025"); // mostar contenido guardado ena rchivo de la fecha indicada
    mostrarComandas("03-06-2025");
    mostrarComandas("04-06-2025");
    mostrarComandas("05-06-2025");
    mostrarComandas("06-06-2025");
    mostrarComandas("07-06-2025");
    cout<<" "<<endl;
    
    cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    cout<<" "<<endl;
    
    actualizarInventario(); // actualizacion de "inventario.dat"
    cout<<" "<<endl;
    mostrarInventario(); // mostrar contendio del archivo "inventario.dat"

    return 0;
}