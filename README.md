# TP Integrador — Buffet Albert Einstein

Trabajo Práctico grupal de **Algoritmos y Estructuras de Datos** (UTN FRBA) —
unidad de **archivos binarios en C/C++**.

> **Grupo:** _N°4_
> **Integrantes:**
> _Kevin Axel Saldaña Medrano — articulo13_
> _Lucas Facundo Tosco  — LucasTosco_
> _Alexander Ezequiel Catari Alarcon — acatarialarcon-ship-it_
> _Bautista Reina — breinautn_
> _Nahuel Benitez — NBenitez-UTN_

La consigna completa está en **`enunciado.pdf`**. Leela antes de arrancar: el
cliente (Alberto) cuenta su problema a su manera y ustedes tienen que descubrir
cómo resolverlo con lo que vimos de archivos.

## Estructura del repo

```
.
├── enunciado.pdf              # la consigna
├── datos/                     # archivos PROVISTOS por la cátedra (no se tocan)
│   ├── comandas_historicas.dat
│   ├── inventario.dat
│   └── dump_datos_de_prueba.txt   # los dos .dat en texto, para verificar la lectura
├── .gitignore
│
│   # --- estos los crean USTEDES (no vienen en el repo base) ---
├── normalizacion.cpp          # cada uno con su propio main; NO hay main.cpp
├── ventas.cpp
├── cierre.cpp
└── resumen.cpp
```

Los 4 programas los escriben ustedes desde cero: el repo base no trae código.
Créenlos en la raíz del repo (como en el diagrama del enunciado).

## Cómo trabajar

Los programas leen y escriben archivos `.dat` en la carpeta donde se ejecutan.
Los archivos de `datos/` son los **originales** y no se modifican: copiálos a la
carpeta donde vas a compilar y correr, y trabajá sobre esas copias.

```bash
cp datos/comandas_historicas.dat datos/inventario.dat .
```

Compilar y correr cada programa (cada uno tiene su propio `main`, no hay `main.cpp`):

```bash
g++ -O2 -o normalizacion normalizacion.cpp
g++ -O2 -o ventas        ventas.cpp
g++ -O2 -o cierre        cierre.cpp
g++ -O2 -o resumen       resumen.cpp
```

Orden de ejecución (cada programa deja los archivos que usa el siguiente):

```
./normalizacion     # historicas + inventario  ->  mozos.dat, comandas_dd-mm-aaaa.dat (varios), inventario actualizado
./ventas            # carga interactiva de la semana  ->  agrega a las planillas del día
./cierre            # junta los días de la semana      ->  comandas_semana_sX-mm.dat
./resumen           # imprime el resumen por mozo + total del buffet
```

## Verificar que leen bien el binario

`datos/dump_datos_de_prueba.txt` tiene el contenido de los dos archivos provistos
en texto legible. Si tu programa imprime otra cosa al leerlos, revisá los `struct`
(orden de campos / tamaños): tienen que dar `sizeof(ComandaHistorica)=76` y
`sizeof(Producto)=64`.

## Qué se entrega

- Este mismo repo (uno por grupo), con los **4 programas** `.cpp` en la raíz
  (cada uno con su propio `main`, no hay `main.cpp`).
- El **README** con el grupo, los integrantes (nombre + usuario de GitHub) y, si
  hace falta, cualquier aclaración de cómo correrlo.
- Los `.dat` de `datos/` son los **provistos por la cátedra** y ya vienen en el
  repo: no los borren ni agreguen otros. Cualquier `.dat` que ustedes copien a la
  raíz o generen al correr **no** se versiona (el `.gitignore` ya los deja afuera).
  La cátedra corre sus programas sobre su propio dataset.
- **Commits repartidos:** cada integrante tiene que tener commits propios a lo
  largo del trabajo. El historial es parte de lo que se mira para la defensa
  individual (no vale un único commit final ni que suba todo una sola persona).

### 🛠️ Decisiones de diseño y resolución de partes clave

#### Normalización (`normalizacion.cpp`)

* **Responsables:**
  * Alexander Catari Alarcón (`acatarialarcon-ship-it`): Calcular registros, archivo auxiliar, ordenar archivo,  creación e encriptación de claves y Corte de control para generar el archivo mozos.dat .
  * Kevin Axel Saldaña Medrano  (`[articulo13]`): Actualización de stock con búsqueda binaria, generación dinámica de planillas por fecha y ordenamiento insitu en disco por `idMozo`.

* **Resolución de partes clave:**
* **Actualización de stock:** Para poder lograr la actualizacion de stock, primero tuve que entender que el archivo de inventario (la migracion de datos) estaba ordenado por código pero tenian saltos de codigo. Asi que primero tuve que usar un el patron algoritmico de busqueda de archivo binario para poder y de ahi usar esta funcion para poder actualizar el stock.
* **Obtener id del mozo:** Se recorre el archivo `mozos.dat` comparando el nombre en texto del mozo hasta encontrar la coincidencia y retornar su `idMozo` correspondiente.

1. **Generación:** Se lee `comandas_historicas.dat` de forma secuencial y se van guardando las comandas en sus respectivos archivos diarios (`comandas_DD-MM-AAAA.dat`) abriéndolos en modo `"ab"`.
2. **Ordenamiento:** Una vez creado el archivo de cada día, se aplica el algoritmo de Selección para ordenar el archivo binario por `idMozo` utilizando `fseek` y `fwrite` para reubicar los registros.


* **Calcular Registros:** Usamos ftell al final del archivo y sizeof en los archivos para obtener la cantidad de registros del archivo que recibe.
* **Archivo Auxiliar**: Cree un archivo auxiliar  para evitar alteras los datos de comandas_historicas.dat, el mismo se  eliminara mas adelante después de cumplir  su papel.
* **Ordenar Archivo**: Use el método de selección  para  ordenar de menor  a mayor  los registros del archivo  usando la variable "nombreMozo" como campo clave.
* **Creación y Encriptación claves**: Como la encriptación debe ser "automática" decide utilizar los primeros 5 caracteres de la variable nombreMozo de comandas_historicas.dat (evitando los espacios).Como la clave no se debe guardar tal cual en el archivo mozos.dat entonces le sumaremos 5 a cada carácter de la clave.
* **Corte de Control**: Usaremos este patrón algorítmico para obtener  la comisión total de cada mozo y a su vez  aprovecharemos el primer  bucle While del corte de control  para escribir los datos en mozo.dat (id, nombre, password).


### Ventas ('ventas.cpp')

* **Responsables:**
  * Nahuel Ezequiel Benitez

* **Resolución de partes clave:**

  * **Carga de datos:** Al arrancar el programa, se leen `mozos.dat` e `inventario.dat` completos y se guardan en arrays en memoria. Después el programa trabaja con esos arrays hasta que hay algo que guardar.
  * **Fecha y planilla del día:** La fecha se pide una sola vez y se valida. Con ella se arma el nombre `comandas_DD-MM-AAAA.dat`.
  * **Login y claves:** Se pide el ID y la clave tanto al principio del programa como al momento de registrar cada venta.
  * **Registro de la venta:** Si el mozo, el producto y la cantidad son válidos y hay stock, se calcula la comisión y se agrega la comanda a la planilla del día que crea el archivo o reescribe en caso de ya existir. Si algo falla, se descarta solo esa venta y se vuelve al menú.
  * **Actualización de stock:** Se descuenta en `inventario.dat`, sin reescribirlo entero.
  * **Ordenamiento:** Al salir del menú se envían todas las comandas del día a un array, se ordenan por `idMozo` y genera el archivo `comandas_DD-MM-AAAA.dat` ordenado.

### Cierre ('cierre.cpp')

* **Responsables:**
  * Bautista Horacio Reina

* **Explicacion del codigo:**
  * **Busqueda de archivos:** El principio del mes, decidimos que arranque con una busqueda de archivos que entran mediante fechas, por eso te pide la fecha del dia que quieras buscar para que abra el archivo. Claramente verifica la existencia de esa fecha.
  * **Lectura de archivo:** Analizamos los mozos, y anotamos a los que aparecen por primera vez, luego a los que se repiten los anotamos detras de si mismo en la lista.
  * **Planilla de mozos:** Una vez analizado el archivo crea una planilla de archivos, creando una en la terminal para verificar que se pueda ver de manera sencilla lo que se anoto.
  * **Creacion de Archivos:** Se crea un archivo que copia la planilla mostrada en la terminal, luego de la creacion te pide tanto la semana como el mes, para darle el nombre al archivo.


### Resumen ('resumen.cpp')

* **Responsables:**
  * Lucas Tosco

* **Busqueda de archivos:** Al iniciar, el programa solicita el número de semana y el mes para buscar y abrir la planilla semanal correspondiente (`comandas_semana_sX-mm.dat`). Además, verifica mediante un ciclo que el archivo exista antes de avanzar con la lectura.
* **Lectura de archivo y Corte de Control:** Como el archivo semanal ya se encuentra ordenado por número de mozo, se recorre directamente leyendo cada comanda con `fread`. Mediante un corte de control por `idMozo`, se agrupan las ventas del mismo mozo para ir acumulando la cantidad de productos vendidos y su comisión.
* **Resumen por mozo:** Cada vez que cambia el mozo leído, se imprime en la terminal el ID del mozo junto con el total de productos que vendió en la semana y la plata de comisión que le corresponde cobrar.
* **Total general del buffet:** Al mismo tiempo que se calculan los datos de cada mozo, se suman a una variable para mostrar al final de la pantalla el total de productos que vendió el buffet en toda la semana y el monto total de comisiones.