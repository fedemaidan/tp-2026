#include<iostream>
#include<cstdio>
#include<cstring>


using namespace std;

struct Producto {    // estructura del stock 
	int   codigo;   
	char descripcion[50];   
	float precio;   
	int stockActual; 
};


struct mozo{
	
	int id;
	char nombre[50];
	char password[20];  
	float TotalComision;
	
	
};

struct ComandaHistorica{               //struct del archivo binario con el mismo nombre
	
	char fecha[11];
	char nombreMozo[50];
	int codProd;  //codigo del producto
	int cantidad;
	float comision;
};

struct Comanda {  // creamos el struct para poder leer y escribir las ventas del dia 
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

//DECLARCION DE FUNCIONES------------------------------------------------------------------------------
void CalcularRegistros(const char* nombre, int& x); 
void CrearArchiAux(const char* nombre);
void OrdenarArchivo(const char* nombre,int x);
void IncripitarClave(mozo& m);
void CorteDeControlArchivo();  //Corte de control con el archivo auxiliar  para generar el  archivo "Mozos.dat"
// PLANILLAS Y STOCK 
long busquedabinaria(const char* nombre, int codigo, Producto& r); //busqueda binaria para encontrar el stock (mediante las casillas fisicas)
void actualizarStock();
int obtenerIdMozo(const char* nombreBuscado); // para poder generar las plinllas por dia
void generarPlanillasPorDia();
void ordenarPlanillaDia(const char* nombreArchivo);


//MAIN -----------------------------------------------------------------------------------------------
int main(){
	
	int cantidad_registros=0;
	const char nombre[] = "datos/comandas_historicas.dat";
	
	
	//funciones
	CalcularRegistros(nombre,cantidad_registros);
	CrearArchiAux(nombre);
    OrdenarArchivo("auxiliar.dat",cantidad_registros);
	CorteDeControlArchivo();
	
	//PLANILLAS Y STOCK
	actualizarStock();
	generarPlanillasPorDia();
	
	
	
	
	

	return 0;
}

//DESARROLLLO DE FUNCIONES-----------------------------------------------------------------------------------------
void CalcularRegistros(const char* nombre, int& x){ 
	
	FILE*f = fopen(nombre,"rb"); //abrimos el archivo en modo lectura binaria
	
	if(f ==NULL){
		
		cout<<"No se pudo abrir el archivo"<<endl;
		return;
	}
	
	fseek(f,0,SEEK_END);                 //nos posicionamos al final del archivo
	
   long y = ftell(f)/ sizeof(ComandaHistorica);  //con ftell  obtenemos la cantidad de bytes del archivo 
                                                //  con sizeof  obtenemos la cantidad de bytes de cada registro
                                                //al dividirlo obtendremos la cantidad de registros del archivo
	x=y;  
	fclose(f);
}
void CrearArchiAux(const  char* nombre){
	
	ComandaHistorica m;
	
	FILE*a = fopen(nombre,"rb");    //abrimos el archivo en lectura binaria
	FILE*b = fopen("auxiliar.dat","wb");   //abrimos el archivo en escritura binaria
	
	if(a == NULL ){
		cout<<"No se pudo abrir el archivo"<<endl;
		return;
	}
	if(b == NULL){
		
		cout<<"No se pudo crear/abrir el archivo"<<endl;
		return;
	}
	
	while(fread(&m,sizeof(ComandaHistorica),1,a)==1){     //usamos fread para que mientra el archivo "a" siga teniendo registros el bucle seguira dando vueltas
		
		fwrite(&m,sizeof(ComandaHistorica),1,b);        //En cada vuelta mandamos los datos del respectivo registro del archivo "a" hacia el archivo "b"
	}
	
	fclose(a);
	fclose(b);
}

void OrdenarArchivo(const char* nombre,int x){
	
	FILE*a = fopen(nombre,"rb+"); //abrimos el archivo  en modo lectura y escritura
	
	if(a == NULL){
		
		cout<<"No se pudo abrir el archivo"<<endl;
		return;
	}
	
	ComandaHistorica  m,n; //declaramos variables
	
	
	//Odenamos por nombre
	
	for(int i=0;i<x-1;i++){
			int pos=i;    //asumimos al primer registro  desordenado en cada vuelta del primer for como la posicion del minimo
			
		fread(&m,sizeof(ComandaHistorica),1,a); //avanzamos una posicion  y guardamos  la variable m el primer registro
		
		char minimo[50];
		strcpy(minimo,m.nombreMozo);  //copiamos en minimo el contenido del segundo parametro
        
		for(int j=i+1;j<x;j++){
			fread(&m,sizeof(ComandaHistorica),1,a); //avanzamos una posicion mas  y guardamos  en m otro valor, lo que nos permite ir comparando los nombres
			if(strcmp(minimo,m.nombreMozo)>0){     //Comparamos para ver si el valor del parametro 1 es menor que el del parametro 2
			                                       //si el resultado de esa comparacion nos da que es mayor a 1 entonces el paramametro 1 es mayor que el parametro 2
				
			    strcpy(minimo,m.nombreMozo);    //entonces reemplazamos el valor de minimo por el de la otra variable
			                                 
				pos=j;                          //guardamos la posicion del minimo
			}
		}
		
		if(pos!=i){    //Si la posicion del minimo resulta ser distinta que el valor de "i" entonces intercambiamos variables
	
		fseek(a,pos*sizeof(ComandaHistorica),SEEK_SET); // Nos posicionamos en el registro con  el menor valor
		fread(&m,sizeof(ComandaHistorica),1,a);      // guardamos los datos del registro con menor valor
		
		fseek(a,i*sizeof(ComandaHistorica),SEEK_SET); //Nos posicionamos en  nuestro primer registro desordenado
		fread(&n,sizeof(ComandaHistorica),1,a);   //guardamos los datos del registro
		
		
		 //INTERCAMBIO DE VALORES
		fseek(a,i*sizeof(ComandaHistorica),SEEK_SET);   //nos posicionamos en el primer registro desordenado
		fwrite(&m,sizeof(ComandaHistorica),1,a);    //guardamos los datos del registro con el menor valor en el primer registro desordenado
		
	    fseek(a,pos*sizeof(ComandaHistorica),SEEK_SET);   //nos posicionamos en la posicion del registro con menor valor
	    fwrite(&n,sizeof(ComandaHistorica),1,a);         // guardamos los datos del registro desordenado en esa posicion
		}
		
		fseek(a,(i+1)*sizeof(ComandaHistorica),SEEK_SET);  //como ya intercambiamos los valores, debemos posicionarnos en el siguiente registro  no ordenado
		
		
		
	}
		
	
	
    fclose(a); //cerramos  el archivo
}

void IncripitarClave(mozo& m){
	
	int posicion=0;
	
		//Usaremos los primeros 4 letras de los nombres de los mozos como clave inicial 
		for(int i=0; m.nombre[i]!= '\0' && posicion<5;i++){   
			
			if(m.nombre[i] != ' '){   //Si el elemento es distinto de espacio entonces copiaremos ese elemento hacia la variable m.password para formar una clave 
				
				m.password[posicion]=m.nombre[i];
				posicion++;
			}	
		}

      m.password[posicion]= '\0';
      
      
      //Sumamos 4 caracteres a cada uno de los caracteres de la clave para incriptarlo
      for(int i=0; m.password[i] != '\0';i++){
      	m.password[i]=m.password[i]+5;
      }

}


void CorteDeControlArchivo(){
	
	
	ComandaHistorica ch;
	mozo m;
	
	FILE* a= fopen("auxiliar.dat","rb"); //abrimos el archivo en modo lectura
	FILE* b= fopen("mozos.dat","wb");    //abrimos el archivo en modo escritura, como no existe  se crea uno.
	
	if(a == NULL){
		
		cout<<"No se pudo abrir el archivo"<<endl;
		return;
	}
	if(b == NULL){
		
		cout<<"No se pudo crear el archivo"<<endl;
		return;
	}
	
	 
	int leido = fread(&ch,sizeof(ComandaHistorica),1,a);   
	int contador_id=1;
	
	while(leido==1){
		
		float cont_comision_mozo=0;   
		char campo_clave[50];
		strcpy(campo_clave, ch.nombreMozo); // elijimos ch.nombreMozo como campo clave  para realizar el corte de control
		
	    strcpy(m.nombre, ch.nombreMozo);       // guardamos el nombre del mozo
		m.id=contador_id;                      //guardamos su id
		contador_id++;                        //avanzamos uno  para el siguiente mozo
		
		//Incriptacion de Clave

		IncripitarClave(m);
		
		
		///
		
		while(leido==1 && strcmp(campo_clave, ch.nombreMozo) == 0){  //mientras ambos parametros  de strcmp sean iguales  y el archivo siga teniendo registros...
			
	        cont_comision_mozo=ch.comision+cont_comision_mozo; // Sumamos en cada vuelta donde el mozo siga siendo el mismo la comision de cada venta para obtener la comision total
	        
	        leido=fread(&ch,sizeof(ComandaHistorica),1,a);   
			
		}
		m.TotalComision=cont_comision_mozo; //guardamos la comision total
		
		fwrite(&m,sizeof(mozo),1,b);      //Guardamos los datos de ese mozo en el archivo y avanzamos una posicion en el archivo.
		
		
	}
	
	

	fclose(a);
	fclose(b);
	remove("auxiliar.dat");  //removemos el archivo auxiliar
	
	
}


//PARTE DE Planillas por dia y el stock  ----------------------------------------------------------------------------------------

long busquedabinaria(const char* nombre, int codigo, Producto& r){ //busqieda de la posicion del producto
	FILE* f = fopen(nombre, "rb");
    if (f == NULL) return -1;

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto); // Cantidad total de registros
    long pri = 0, ult = n - 1, pos = -1;

    while (pri <= ult && pos == -1) {
        long med = (pri + ult) / 2;
        fseek(f, med * sizeof(Producto), SEEK_SET);
        fread(&r, sizeof(Producto), 1, f);

        if (r.codigo == codigo) {
            pos = med;
        } else if (codigo > r.codigo) {
            pri = med + 1;
        } else {
            ult = med - 1;
        }
    }

    fclose(f);
    return pos;

}

void actualizarStock(){ // se actualiza el numero de stock del producto
	FILE* fCom = fopen("datos/comandas_historicas.dat", "rb");
	FILE* fInv = fopen("datos/inventario.dat", "rb+");
	if (fCom == NULL || fInv == NULL) {
		cout << "Niguno de los archivos se puso abrir"<< endl;
		if (fCom != NULL) fclose(fCom);
		if (fInv != NULL) fclose(fInv);
		return;
	}

	ComandaHistorica comanda;
	Producto prod;
	while (fread(&comanda, sizeof(ComandaHistorica), 1, fCom) == 1){ // bucle de lectura
		long pos = busquedabinaria( "datos/inventario.dat", comanda.codProd, prod); //llamamos a la funcion que creamos
			if (pos != -1){
			prod.stockActual -= comanda.cantidad;
			fseek(fInv, pos * sizeof(Producto), SEEK_SET); // descuenta del archivo inventario
			fwrite(&prod, sizeof(Producto), 1, fInv);
		}else{
			cout <<"No se encontro el prodcuto para descontar el stock" <<endl;
		}
		
	}

	fclose(fCom);
	fclose(fInv);

}

int obtenerIdMozo(const char* nombreBuscado) { //funcion axiliar para las planillas
    FILE* fMozos = fopen("mozos.dat", "rb"); //abre el archivo que hizo alex
    
    if (fMozos == NULL) {
        cout << "Error: No se pudo abrir el archivo mozos.dat" << endl;
        return -1;
    }

    mozo m;
    
    while (fread(&m, sizeof(mozo), 1, fMozos) == 1) { //Lee el registro 
        if (strcmp(m.nombre, nombreBuscado) == 0) { //compara los nombres del archivos historicas y mozos
            fclose(fMozos); // Cerramos el archivo apenas lo encontramos
            return m.id;    // Retornamos el id correspondiente
        }
    }

    fclose(fMozos);
    return -1;
}

void generarPlanillasPorDia() {
    // 1. Abrimos el archivo de ventas históricas
    FILE* fHist = fopen("datos/comandas_historicas.dat", "rb");
    
    if (fHist == NULL) {
        cout << "Error: No se pudo abrir comandas_historicas.dat" << endl;
        return;
    }

    ComandaHistorica ch;

	// LÍNEAS AGREGADAS: Arreglo para registrar las fechas encontradas y ordenarlas al final[cite: 1]
    char fechasEncontradas[50][11]; 
    int cantFechas = 0;

    // 2. Recorremos todas las comandas históricas
    while (fread(&ch, sizeof(ComandaHistorica), 1, fHist) == 1) {
        
        // 3. Usamos la función auxiliar para obtener el ID del mozo
        int idEncontrado = obtenerIdMozo(ch.nombreMozo);

        if (idEncontrado != -1) { // Si encontró al mozo en mozos.dat
            
            // 4. Armamos la nueva estructura traducida
            Comanda com;
            com.idMozo = idEncontrado;
            com.codigoProducto = ch.codProd;
            com.cantidad = ch.cantidad;
            com.comision = ch.comision;

            // 5. Armamos el nombre del archivo con la fecha del registro (ej: "comandas_03-06-2025.dat")
            char nombreArchivoDia[60];
            sprintf(nombreArchivoDia, "datos/comandas_%s.dat", ch.fecha);

			//Verificamos si es una fecha nueva para agregarla a la lista[cite: 1]
            bool yaEstaEnLista = false;
            for (int i = 0; i < cantFechas; i++) {
                if (strcmp(fechasEncontradas[i], ch.fecha) == 0) {
                    yaEstaEnLista = true;
                    break;
                }
            }
            if (!yaEstaEnLista && cantFechas < 50) {
                strcpy(fechasEncontradas[cantFechas], ch.fecha);
                cantFechas++;
            }

            // 6. Abrimos (o creamos) el archivo de esa fecha y agregamos la venta al final
            FILE* fDia = fopen(nombreArchivoDia, "ab");
            if (fDia != NULL) {
                fwrite(&com, sizeof(Comanda), 1, fDia);
                fclose(fDia);
            }
        }
    }

    fclose(fHist);

	// LÍNEAS AGREGADAS: ACÁ SE LLAMA A ORDENAR: Recorremos las fechas y ordenamos cada planilla[cite: 1]
    for (int i = 0; i < cantFechas; i++) {
        char nombreArchivo[60];
        sprintf(nombreArchivo, "datos/comandas_%s.dat", fechasEncontradas[i]); //en teoria se arregla el bug y se ordena 
        ordenarPlanillaDia(nombreArchivo); 
    }
	
}

void ordenarPlanillaDia(const char* nombreArchivo) {
    FILE* f = fopen(nombreArchivo, "rb+");
    if (f == NULL) return;

    fseek(f, 0, SEEK_END);
    int cantRegistros = ftell(f) / sizeof(Comanda);

    Comanda c1, c2;

    for (int i = 0; i < cantRegistros - 1; i++) {
        int posMinimo = i;
        
        // Buscamos el menor idMozo desde la posición i
        fseek(f, i * sizeof(Comanda), SEEK_SET);
        fread(&c1, sizeof(Comanda), 1, f);
        int minId = c1.idMozo;

        for (int j = i + 1; j < cantRegistros; j++) {
            fseek(f, j * sizeof(Comanda), SEEK_SET);
            fread(&c2, sizeof(Comanda), 1, f);
            
            if (c2.idMozo < minId) {
                minId = c2.idMozo;
                posMinimo = j;
            }
        }

        // Si encontramos un id menor en otra posición, intercambiamos los registros
        if (posMinimo != i) {
            // Leemos el registro en la posición i
            fseek(f, i * sizeof(Comanda), SEEK_SET);
            fread(&c1, sizeof(Comanda), 1, f);
            
            // Leemos el registro que tiene el menor valor (en posMinimo)
            fseek(f, posMinimo * sizeof(Comanda), SEEK_SET);
            fread(&c2, sizeof(Comanda), 1, f);

            // Escribimos el menor en la posición i
            fseek(f, i * sizeof(Comanda), SEEK_SET);
            fwrite(&c2, sizeof(Comanda), 1, f);
            
            // Escribimos el que estaba en i en la posición posMinimo
            fseek(f, posMinimo * sizeof(Comanda), SEEK_SET);
            fwrite(&c1, sizeof(Comanda), 1, f);
        }
    }

    fclose(f);
}
