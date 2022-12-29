// ISO 8859-15
#include <string.h>
#include "parser_defs.h"
#include "parser.h"

// Flags del Juego
#include "juego_flags.h"

// Librería gráfica
#include "./libgfx/libgfx.h"
 

// Compiler options for ZX Spectrum
#ifdef ZX
	/* Allocate space for the stack */
	#pragma output STACKPTR=24200
	#pragma -reqpag=0
	#pragma -no-expandz88
	#pragma output CRT_ENABLE_CLOSE = 0    // do not close open files on exit (at this time this has no effect)
	#pragma output CLIB_EXIT_STACK_SIZE = 0   // get rid of the exit stack (no functions can be registered with atexit() )
	#pragma output CLIB_MALLOC_HEAP_SIZE = 0  // malloc's heap will not exist
	#pragma output CLIB_STDIO_HEAP_SIZE = 0  // stdio's heap will not exist (you will not be able to open files)
	#pragma output CLIB_FOPEN_MAX = -1  // don't even create the open files list
	#pragma output CRT_ENABLE_RESTART = 1
#endif 

// Compiler options for C64
#ifdef C64
#endif 

// Compiler options for CPC 
#ifdef CPC 
#endif 

// Compiler options for DOS
#ifdef DOS
#endif 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Código local
// Si necesitamos código en páginas de memoria hay que compilarlas como .bin y añadirlas aquí como extern
// Se llaman desde el código usando: setRAMpage (n) y llamando a la función, al terminar hay que volver siempre a la página 0

char respuestas();
char respuestas_post();
char proceso1();
char proceso1_post();
char proceso2();
char proceso2_post();

// ------------------------------------------
// Tablas externas de procesos y respuestas
// ------------------------------------------

extern char respuestas_desierto ();

// Variables que necesitamos del parser...
// Se pueden declarar más tablas de flags para el juego pero el parser
// sólo trabaja con esta. 
extern unsigned char flags[255]; // Flags 60...250 Disponibles para el usuario
extern unsigned char playerInput[80];
extern unsigned char playerWord[25];
extern BYTE gWord_number; // Marker for current word, 1st word is 1
extern BYTE gChar_number; // Marker for current char, 1st char is 0

// Tabla de imágenes del juego
extern unsigned char L01_img[];

// Include binary files for the adventure
#ifdef ZX
	#asm
	#endasm
#endif 

// id, page, memory pointer
// Terminated with 0
img_t imagenes_t [] = {
    { 0,0,0}
    };


// Tabla de regiones
// Regiones del juego...
unsigned char region_exterior [] = { 4,6,7,0 };
unsigned char region_entrada [] = { 13,10,17,14,9,0 };
unsigned char region_superior [] = { 3,8,23,26,27,0 };
unsigned char region_inferior [] = { 12,15,19,2,5,11,21,16,29,18,27,28,25,24,30,1,31,20,0 };

// Tabla de localidades de la aventura
// 250 to 255 reservadas
// última localidad como ID 0 

loc_t localidades_t [] =
{
	// L1
    {"ACCESO A LA TRAMPA","Esta sala tiene algo raro, como si faltase algo importante. No hay ninguna salida en la pared norte, las otras están oradadas de pequeños agujeros. ", 1, FALSE, 0x00000000 },
	{"ALTAR DE OFRENDAS","Debo estar en otro de los altares de ofrendas. Sobre una columna reposa un recipiente.",2, FALSE, 0x00000000},
	{"ANTESALA","Una fisura en la pared deja pasar la luz. El suelo cubierto de arena muestra las huellas dejadas por los trabajadores. ",3, FALSE, 0x00000000},
	{"DESIERTO","El sol comienza su lento trepar por el azul. El suelo pedregroso del desierto se arrastra hasta la entrada del Valle de los Reyes. ",4, FALSE, 0x00000000},
    {"SALA DIMINUTA","Un estrecho recinto permite avanzar de este a oeste. La altura del techo es la normal, pero en este espacio apenas cabe una persona.", 5, FALSE, 0x00000000},
	{"ENTRADA","Al final de una explanada se encuentra la pirámide descubierta por el profesor Petrie. ", 6, FALSE, 0x00000000},
	{"ESCALERA","La entrada a la pirámide se asemeja a la de otras tumbas del valle. Desperdigadas por el suelo, veo un montón de herramientas que parecen haber sido abandonadas a toda prisa.",  7, FALSE, 0x00000000},
	{"ESTANCIA","Dos vasijas policromadas flanquean una de las puertas de la estancia, situada al sur, mientras que la otra salida se sitúa al oeste", 8},
	{"PASADIZO ESTRECHO", "", 9 , FALSE, 0x00000000},
	{"FINAL DEL CORREDOR", "El corredor gira al oeste ante un tramo ascendente de escaleras. Los muros son lisos y carentes de ornamentos.",10, FALSE, 0x00000000},
	{"PASAJE","Esta cámara es la central de la tumba. De aquí parten pasillos hacia otras salas en todas las direcciones.  ^",11, FALSE, 0x00000000},
	{"DISTRIBUIDOR", "La altura del techo me obliga a caminar ligeramente agachada. Todo sigue siendo muy austero. El pasillo comunica con otras salas al norte y al oeste. ",12, FALSE, 0x00000000},
	{"CORREDOR LARGO","Dejando atrás la claridad del valle, el corredor se adentra en la tumba.", 13, FALSE, 0x00000000},
	{"PASADIZO BAJO", "Aquí el pasillo se estrecha y se retuerce al norte. Hay varias herramientas rotas tiradas por el suelo. ",	14, FALSE, 0x00000000},
	{"PASAJE DE LOS GRABADOS","El corredor comunica el distribuidor con una sala al oeste. A diferencia de otros pasillos en este veo algunos grabados.", 15, FALSE, 0x00000000},
	{"PASARELA","Es increíble lo que estoy viendo. Un puente colgante sobre una sima comunica al este con otra región de la galería.", 16, FALSE, 0x00000000},
	{"PASILLO PROFUNDO","El pasillo desciende a una galería subterránea. ",17, FALSE, 0x00000000},
	// {"POZO DE ALMAS", "Este lugar resulta realmente triste y desolado, con altas y estrechas columnas, muchas de ellas rotas. Algunos pictogramas decoran las paredes. Puedes volver a subir por la cuerda.",	18},
	{"RAMPA", "Otra rampa me interna aun más en el corazón de la tumba. ", 19, FALSE, 0x00000000},
	{"SALA ALARGADA","Un pasillo estrecho y de techo elevado desciende aun más en la galería. ", 20, FALSE, 0x00000000},
	{"SALA DE LA LUMINARIA","En una de las paredes hay un hueco que probablemente se usaba para iluminar la estancia. ", 21, FALSE, 0x00000000},
	{"SALA DE LA MOMIA", "Los trabajadores encontraron una momia y apoyaron el sarcófago en la pared. Me pregunto por qué mostrarían tan poco cuidado. ", 22, FALSE, 0x00000000},
	{"SALA DE LA VIDA ETERNA", "En esta sala hay varios objetos usados durante el enterramiento. Veo varias estatuas de animales, adornos y las ruedas de un carro real. ",		23, FALSE, 0x00000000},
	{"SALA DE LOS VIVOS","La figura de Anubis me sugiere lo que Carter menciona como la antesala de la muerte. Un recinto que se ha encontrado en las grandes pirámides de Giza. Si esto es así, ¿Cuales son las auténticas dimensiones de esta tumba?",24, FALSE, 0x00000000},
	{"ALTAR DE PIEDRA", "Esta sala está parcialmente sepultada por un derrumbe. De entre los escombros surge un altar de piedra. ",	 25, FALSE, 0x00000000},
	{"SALA DE COFRE","Recipientes con aceite sagrado iluminan un cofre con muchos grabados. Al oeste un gran arco permite el acceso a una sala aparentemente importante. Al norte está la salida.", 26, FALSE, 0x00000000},
	/*
	{"SALA DEL DIOS REY","Esta sala parece completamente dedicada a la vida del rey Aceps I. En la imagen, parece querer ponerse un anillo, pesando sobre su cabeza una espada con una copa. La sala tiene pasajes al norte y al sur.",				   27},
	{"SALA DEL ESPACIO","Gracias a un boquete en la pared, se aprecia la sala hacia el sur. Es como si se hubiera hecho aposta, un ventanal para que la imagen del rey, al norte, pudiese contemplar el altar.",		  	   28},
	*/
	{"SALA DEL SEPULCRO","Finalmente aquí se halla el sepulcro del Rey. Un pesado féretro de piedra adornado con numerosos pictogramas. Ubicados en las esquinas hay varios focos de luz y una polea. ", 29, FALSE, 0x00000000},
	/*
	{"SALA FUNERARIA","En las paredes se aprecian los grabados de un papiro. Solo hay un pasaje al norte.", 30},
	{"TRAMPA", "Alrededor de la salida aparecen grabados de todo tipo. Una abertura de forma oblonga rompe la uniformidad de una de las paredes al sur. Puedes volver al norte.",		  		   							   31},
	*/
	{"MESA","",34, FALSE, 0x00000000}, // Como superficie-contenedor
    {"","",0, FALSE, 0x00000000}
};

// Tabla de conexiones...
// Se usan los ID de localidad para idenfiticar las conexiones

cnx_t conexiones_t [] =
{
// LOC | AL_N | AL_S | AL_E | AL_O | AL_NE | AL_NO | AL_SE | AL_SO | ARRIB | ABAJO | ENTRAR | SALIR 
	{1,{	0,	20,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}},
	{2,{	0,	19,		5,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Altar ofrendas
	{3,{	0,	23,		8,		0,		0,		0,		0,		0,		0,		10,		0,		0}},// Antesala
	{4,{	0,	6,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Desierto
	{5,{	0,	0,		11,		2,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala Diminuta
	{6,{	4,	7,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Entrada
	{7,{	6,	13,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Escaleras Cafax
	{8,{	0,	26,		0,		3,		0,		0,		0,		0,		0,		0,		0,		0}}, // Estancia
	{9,{	0,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Pasadizo Estrecho, DESCARTADA
	{10,{	13,	0,		0,		14,		0,		0,		0,		0,		3,		0,		0,		0}}, // Final del Corredor
	{11,{	20,	12,		21,		5,		0,		0,		0,		0,		0,		0,		0,		0}}, // Gran Pasaje
	{12,{	11,	0,		0,		15,		0,		0,		0,		0,		17,		0,		0,		0}}, // Gran Sala, Distr
	{13,{	7,	10,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Largo Corredor
	{14,{	17,	0,		10,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Pasadizo Bajo
	{15,{	0,	0,		12,		19,		0,		0,		0,		0,		0,		0,		0,		0}}, // Pasaje de Grabados
	{16,{	0,	0,		29,		21,		0,		0,		0,		0,		0,		0,		0,		0}}, // Pasarela
	{17,{	0,	14,		0,		0,		0,		0,		0,		0,		0,	   12,		0,		0}}, // Pasillo Profundo
//	{18,{	0,	0,		0,		0,		0,		0,		0,		0,		29,		0}}, // Pozo de Almas
	{19,{	2,	0,		15,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Rampa
	{20,{	1,	11,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala Alargada
	{21,{	0,	24,		16,		11,		0,		0,		0,		0,		0,		0,		0,		0}}, // Luminaria
	{22,{	0,	0,		0,		26,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala de Momia
	{23,{	3,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala Vida Eterna
	{24,{	0,	21,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala de Vivos
	{25,{	0,	29,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Altar de Piedra
	{26,{	8,	0,		22,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Cofre
	{27,{	0,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Dios Rey, DESCARTADA
	{28,{	0,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala del Espacio DESCARTADA
	{29,{	0,	0,		0,		16,		0,		0,		0,		0,		0,      0,		0,		0}},// Sala del Sepulcro
	{30,{	0,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Sala funeraria, DESCARTADA
	{31,{	1,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0}}, // Trampa, DESCARTADA
	{33,{   0,  0,      0,      0,      0,      0,      0,      0,      0,      0,		0,		0}}, // Créditos orig
	{34,{   0,  0,      0,      0,      0,      0,      0,      0,      0,      0,		0,		0}} // Mesa
	};


// Tabla de mensajes de la aventura
// 1 to 255
// Text, message_id (1 to 255)
// Messages can be printed directly in the code just calling the function writeText (unsigned char *string)
// More than one message table can be defined
token_t mensajes_t [] =
{
	{"",0},
};

// Messages used by the parser. 
token_t mensajesSistema_t [] =
{
	{"No puedo ver nada, está muy oscuro.^",0},
	{"También hay ",1},
	{"Escribe tus órdenes aquí",2},
	{"",3},
	{"",4},
	{"",5},
	{"¿Cómo? Por favor prueba con otras palabras.^",6},
	{"No puedo ir en esa dirección.^",7},
	{"¿Perdón?^",8},
	{"Llevo ",9},
	{"(puesto)",10},
	{"ningún objeto.",11},
	{"¿Seguro? ",12},
	{"¿Juegas de nuevo? ",13},
	{"Adiós...",14},
	{"OK^",15},
	{"[...]",16},
	{"Has realizado ",17},
	{" turno",18},
	{"s",19},
	{".^ ",20},
	{"La puntuación es de ",21},
	{" punto",22},
	{"No llevo puesto eso.^",23},
	{"No puedo, ya llevo eso puesto.^",24},
	{"Ya tengo ",25},
	{"No veo eso por aquí.^",26},
	{"No puedo llevar más cosas.^",27},
	{"No tengo eso.^",28},
	{"Pero si ya llevo puesto",29},
	{"S",30},
	{"N",31},
	{"Más...",32},
	{"> ",33},
	{"",34},
	{"El tiempo pasa...^",35},
	{"Ahora tengo ",36},
	{"Me pongo ",37},
	{"Me quito ",38},
	{"Dejo ",39},
	{"No puedo ponerme ",40},
	{"No puedo quitarme ",41},
	{"¡Tengo demasiadas cosas en las manos!",42},
	{"Desgraciadamente pesa demasiado.",43},
	{"Meto",44},
	{"Ahora no está en ",45},
	{",",46},
	{" y ",47},
	{".",48},
	{"No tengo ",49},
	{"No llevo puesto ",50},
	{"",51},
	{"Eso no está en ",52},
	{"ningún objeto",53},
	{"Fichero no encontrado.^",54},
	{"Fichero corrupto.^",55},
	{"Error de E/S. Fichero no grabado.^",56},
	{"Directorio lleno.^",57},
	{"Disco lleno.",58},
	{"Nombre de fichero no válido.^",59},
	{"Nombre del fichero: ",60},
	{"",61},
	{"¿Perdón? Por favor prueba con otras palabras.^",62},
	{"Aquí ",SYSMESS_NPCLISTSTART},
	{"está ",SYSMESS_NPCLISTCONTINUE},
	{"están",SYSMESS_NPCLISTCONTINUE_PLURAL},
    {"Dentro hay ",SYSMESS_INSIDE_YOUCANSEE},
    {"Encima hay ",SYSMESS_OVER_YOUCANSEE},
    {"",68},
	{"No es algo que pueda quitarme.^",69},
	{"Pongo ",SYSMESS_YOUPUTOBJECTON },
    {"No es algo que pueda cogerse.^",SYSMESS_YOUCANNOTTAKE},
	{"No parece que pueda moverse.^", SYSMESS_CANNOTMOVE},
	{"Llevo las manos vacías.^", SYSMESS_CARRYNOTHING},
	{"Salidas visibles:",SYSMESS_EXITSLIST},
	{"",0}	
};

// Tablas de vocabulario
// Nombre propios, sustantivos...
// último elemento debe ser 0
#define nNorte  1
#define nSur 	2
#define nEste	3
#define nOeste  4
#define nNoreste 5
#define nNorOeste 6
#define nSurEste 7
#define nSurOeste 8
#define nArriba 9
#define nSubir 9
#define nAbajo 10
#define nBajar 10
#define nEntrar 11
#define nEntra 11
#define nAdentro 11
#define nDentro 11
#define nSalir 12
#define nSal 12 


token_t nombres_t [] =
{
    {"norte",           nNorte},  //0
	{"n",               nNorte},
	{"sur",             nSur},    //2
	{"s",               nSur},
	{"este",            nEste},   //4
	{"e",               nEste},
	{"oeste",           nOeste},  //6
	{"o",               nOeste},
	{"noreste",         nNoreste}, //8
	{"w",               nOeste},
	{"noroeste",        nNorOeste}, //10
	{"ne",              nNoreste},
	{"sureste",         nSurEste},	 //12
	{"se",              nSurEste},
	{"suroeste",        nSurOeste},  // 14
    {"suroe",           nSurOeste},
    {"arriba",          nArriba},   // 16
	{"up",              nNorOeste},
	{"abajo",           nAbajo},   // 18
	{"no",              nNorOeste},
	{"entrar",			nEntrar}, // 20
	{"entra",			nEntrar},
	{"salir",			nSalir}, // 22
	{"sal",				nSalir},
	{"ascie",	nArriba},
	{"subet",	nArriba},
	{"sube",   	nArriba},
	{"bajar",  	nAbajo},
	{"baja",    nAbajo},
	{"desce",	nAbajo},
	{"desci",  	nAbajo},
	{"bajat",  	nAbajo},
	{"entra",	nEntrar},
	{"enter",	nEntrar},
	{"intro",	nEntrar},
	{"adent",   nEntrar},
	{"dentr",   nEntrar},
	{"inter",	nEntrar},
	{"salir",	nSalir},
	{"sal",		nSalir},
	{"exit",	nSalir},
	{"fuera",   nSalir},
	{"afuer",   nSalir},
    {"i",               nInventario},
    {"inven",           nInventario},
	{"inv",         nInventario },
	{"punto",       nPuntos},
	{"puntu",       nPuntos},
	{"conta",       nPuntos},
	{"turno",       nTurnos},    
	// Names < 20 can be used as verbs
	// Nombres para el Vocabulario del juego,
	{"arena",   n_arena},
	{"suelo",   n_arena},
	{"expla",   n_arena},
	{"desie",   n_arena},
	{"piram",   n_piramide},
	{"pies",    n_cuerpo},
	{"pie",     n_cuerpo},
    {"cabez",  n_cuerpo},
	{"cejas",   n_cuerpo},
	{"brazo",   n_cuerpo},
	{"piern",   n_cuerpo},
	{"zapat",   n_cuerpo},
	{"suela",   n_cuerpo},
	{"tramp",   n_trampa},
	{"pinch",   n_pinchos},
	{"cuchi",   n_pinchos},
	{"aguja",   n_pinchos},
	{"acces",   n_acceso},
	{"altar",   n_altar},
	{"vasij",   n_vasija},
	{"recip",   n_vasija},
	{"urna",    n_vasija},
	{"urnas",    n_vasija},
	{"llama",   n_llama},
	{"fuego",   n_fuego},
	{"aceit",   n_aceite},
	{"serpi",   n_serpiente},
	{"cobra",   n_serpiente},
	{"bicha",   n_serpiente},
	{"arana",   n_arana},
	{"pared",   n_pared},
	{"muro",    n_pared},
	{"muros",   n_pared},
	{"esqui",   n_esquina},
	{"brech",   n_brecha},
	{"luz",     n_luz},
	{"abert",   n_abertura},
	{"apert",   n_abertura},
	{"sol",     n_sol},
	{"cielo",   n_sol},
	{"feret",   n_sepulcro},
	{"sepul",   n_sepulcro},
	{"tumba",   n_sepulcro},
	{"templ",   n_piramide},
	{"monum",   n_piramide},
	{"cafax",   n_cafax},
	{"aceps",   n_aceps},
	{"recin",   n_sala},
	{"sala",    n_sala},
	{"salas",   n_sala},
	{"techo",   n_techo},
	{"suelo",   n_suelo},
    {"entra",   n_entrada},
    {"puert",   n_entrada},
	{"pilar",   n_columna},
	{"colum",   n_columna},
	{"escal",   n_escalera},
	{"nubes",   n_nubes},
	{"nube",    n_nubes},
	{"pasad",   n_pasillo},
	{"corre",   n_pasillo},
	{"pasil",   n_pasillo},
	{"detal",   n_arte},
	{"jerog",   n_arte},
	{"arte",    n_arte},
	{"muest",   n_arte},
	{"picto",   n_arte},
	{"pintu",   n_arte},
	{"image",   n_arte},
	{"graba",   n_arte},
	{"autor",   n_arte},
	{"simbo",   n_arte},
	{"grand",   n_arte},
	{"majes",   n_arte},
	{"ojos",    n_ojo},
	{"ojo",     n_ojo},
	{"oscur",   n_oscuridad},
	{"banca",   n_soporte},
	{"sopor",   n_soporte},
	{"puent",   n_puente},
	{"pasar",   n_puente},
	{"colga",   n_puente},
	{"sima",    n_sima},
	{"preci",   n_sima},
	{"vacio",   n_sima},
	{"caida",   n_sima},
	{"rampa",   n_rampa},
	{"pendi",   n_pendiente},
	{"objet",   n_objeto},
    {"hueco",   n_hueco},
    {"aguje",   n_hueco},
    {"orifi",   n_hueco},
    {"ranur",   n_ranura},
	{"lumin",   n_luminaria},
	{"brasa",   n_brasa},
	{"carbo",   n_carbon},
	{"momia",   n_momia},
	{"fiamb",   n_momia},
	{"ataud",   n_ataud},
	{"tumba",   n_ataud},
	{"sarco",   n_ataud},
	{"mesa",    n_mesa},
	{"polvo",   n_polvo},
	{"anima",   n_animales},
	{"semil",   n_semilla},
	{"maiz",    n_maiz},
	{"anubi",   n_anubis},
	{"vivos",   n_vivos},
	{"farao",   n_farao},
	{"vida",    n_vida},
	{"antes",   n_antes},
	{"muert",   n_muerte},
	{"charc",   n_charco},
	{"agua",    n_charco},
	{"cofre",   n_cofre},
	{"cerra",   n_cerradura},
	{"ranur",   n_cerradura},
	{"cerro",   n_cerradura},
	{"copa",    n_copa},
	{"anill",   n_anillo},
	{"espad",   n_espada},
	{"cabez",   n_cabeza},
	{"piedr",   n_piedra},
	{"mano",    n_mano},
	{"manos",   n_mano},
	{"dedo",    n_dedo},
	{"falan",   n_dedo},
	{"dedos",   n_dedo},
	{"hueso",   n_hueso},
	{"rey",     n_rey},
    {"farao",   n_rey},
    {"pozo",    n_pozo},
    {"herra",   n_herramientas},
    {"linte",   n_linterna},
    {"barra",   n_barra},
    {"cuerd",   n_cuerda},
    {"soga",    n_cuerda},
    {"cabo",    n_cuerda},
    {"antor",   n_antorcha},
    {"peder",   n_pedernal},
    {"sanda",   n_sandalia},
    {"carta",   n_carta},
    {"teleg",   n_carta},
    {"nota",    n_carta},
    {"papel",   n_carta},
    {"bruju",   n_brujula},
    {"roca",    n_roca},
    {"bloqu",   n_roca},
    {"tabla",   n_tabla},
    {"mader",   n_tabla},
	{"trigo",	n_trigo},
	{"llave",	n_llave},
	{"tapa",	n_tapa},
	{"losa",    n_losa},
	{"poste",	n_poste},
	{"papir",	n_papiro},
	{"carto",   n_papiro},
	{"valle",   n_valle},
	{"monta",   n_valle},
	{"expla",   n_suelo},
	{"polea",   n_polea},
	{"rueda",   n_polea},
	{"mecan",   n_polea},
	{"",0}
};

// Verbos
// VOCABULARIO
// Verbos < 20 son iguales a nombres < 20
token_t verbos_t [] =
{
    {"coger",	vCoger},
    {"coge",	vCoger},
    {"tomar",	vCoger},
    {"toma",	vCoger},
    {"take",	vCoger},
    {"dejar",	vDejar},
    {"deja",	vDejar},
    {"solta",	vDejar},
    {"suelt",	vDejar},
    {"drop",	vDejar},
    {"quita",	vQuitar},
    {"sacat",	vQuitar},
    {"sacar",	vSacar},
    {"desvi",	vQuitar},
    {"desve",	vQuitar},
//    {"SENTA",	23},
//    {"SENTA",	23},
//    {"SENTA",	23},
//    {"SIENT",	23},
    {"l",		vMirar},
    {"look",	vMirar},
    {"m",		vMirar},
    {"mirar",	vMirar},
    {"mira",	24},
    {"quit",	vFin},
    {"fin",		vFin},
    {"save",	vSave},
    {"graba",	vSave},
    {"salva",	vSave},
	{"guard",	vSave},
	{"load",	vLoad},
	{"carga",	vLoad},
	{"ramsa",	vRamsave},
	{"graba",	vSave},
	{"ramlo",	vRamload},
	{"carga",	vLoad},
	{"x",       vExaminar},
	{"exami",	vExaminar},
	{"ex",		vExaminar},
	{"inspe",	vExaminar},
	{"obser",	vExaminar},
	{"decir", 	vDecir},
	{"di",		vDecir},
	{"habla", 	vDecir},
	{"respo",  	vDecir},
	{"manda",	vDecir},
	{"lanza",	vLanzar},
	{"tira", 	vLanzar},
	{"tirar",	vLanzar},
	{"arroj",	vLanzar},
	{"empuj",	vEmpujar},
	{"apret",	vEmpujar},
	{"pulsa",	vPulsar},
	/*
	{"AYUDA",	34},
	{"HELP",	34},
	*/
	{"girar",  	vGirar},
	{"gira",    vGirar},
	{"rota",	vGirar},
	{"rotar",	vGirar},
	/*
	{"VOLTE",	35},
	{"MOSTR",	36},
	{"MUEST",	36},
	{"ENSEÑ", 	36},
	*/
	{"escuc", 	vEscuchar},
	{"oir",		vEscuchar},
	{"oye",		vEscuchar},
	/*
	{"COMER",	38},
	{"COME",	38},
	{"COMET",	38},
	{"TRAGA",	38},
	{"INGIE",	38},
	{"INGER",	38},
	{"MASTI",	38},
	{"BEBER",	39},
	{"BEBE",	39},
	{"BEBET",	39},
	{"BEBER",	39},
	*/
	{"tocar",	vTocar},
	{"toca",	vTocar},
	{"acari",	vTocar},
	{"palpa",	vTocar},
	/*
	{"SALID",	41},
	{"EXITS",   41},
	{"X",		41},
	*/
	{"oler",	vOler},
	{"huele",	vOler},
	{"olfat",	vOler},
	{"husme", 	vOler},
	{"esper",	vEsperar},
	{"z",		vEsperar},
	{"canta",	vCantar},
	{"salta", 	vSaltar},
	{"brinc",	vSaltar},
	{"ataca",	vAtacar},
	{"agred",	vAtacar},
	{"matar",	vAtacar},
	{"mata",	vAtacar},
	{"asesi",	vAtacar},
	{"estra",	vAtacar},
	{"patea",	vAtacar},
	{"pisot",	vAtacar},
	{"tortu",	vAtacar},
	{"noque",	vAtacar},
	{"lucha",	vAtacar},
/*
	{"ORINA",	47},
	{"MEAR",	47},
	{"MEA",		47},
	{"MEATE",	47},
	{"MEARS",	47},
	{"MEART",	47},
	{"DEFEC",	47},
	{"CAGAR",	47},
	{"ERUCT",	47},
	{"VOMIT",	47},
	{"ESCUP",	48},
	{"agita",	49},
	{"menea",	49},
	{"sacud",	49},
	{"remov",	49},
	{"remue",	49},
	{"balan",	49},
	{"COLUM",	50},
	{"EXCAV",	51},
	{"CAVAR",	51},
	{"CAVA",	51},
	{"DESEN",	51},
	{"ENTER",	51},
	{"ENTIE",	51},
	{"CORTA",	52},
	{"RASGA",	52},
	{"LEVAN",	53},
	{"atar",	54},
	{"ata",		54},
	{"enlaz",	54},
	{"anuda",	54},
	{"LLENA",	55},
	{"RELLE",	55},
	{"NADAR",	56},
	{"NADA",	56},
	{"TREPA",	57},
	{"ESCAL",	57},
	{"RETOR",	58},
	{"RETUE",	58},
	{"TORCE",	58},
	{"TUERC",	58},
	{"REZAR",	59},
	{"REZA",	59},
	{"ORAR",	59},
	{"ORA",		59},
	{"PENSA",	60},
	{"PIENS",	60},
	{"DORMI",	61},
	{"DUERM",	61},
	{"DORMI",	61},
	{"DESCA",	61},
	{"RONCA",	61},
	{"ECHAT",	61},
	{"ECHAR",	61},
	{"BESAR",	62},
	{"BESA",	62},
	{"CHUPA",	63},
	{"LAMER",	63},
	{"LAME",	63},
	{"PROBA",	63},
	{"PRUEB",	63},
	{"PALAD",	63},
	{"SABOR",	63},
	*/
	{"abri",	vAbrir},
	{"abre",	vAbrir},
    {"abrir",   vAbrir},
	{"cerrar",	vCerrar},
	{"cierra",	vCerrar},
//	{"quema",	66},
	{"encen",	vEncender},
	{"encie",	vEncender},
	{"incen",	vEncender},
	{"prend",	vEncender},
	{"apaga",	vApagar},
	{"extin",	vApagar},
	{"sofoc",	vApagar},
//	{"trans",	68},
	{"rompe",	vRomper},
	{"parti",	vRomper},
	{"parte",	vRomper},
	{"quebr",	vRomper},
	{"quieb",	vRomper},
	{"destr",	vRomper},
//	{"versi",	70},
	{"poner",	vPoner},
	{"pon",		vPoner},
	{"ponte",	vPoner},
	{"ponse",	vPoner},
	{"pone",	vPoner},
	{"viste",	vVestir},
	{"vesti",	vVestir},
	{"golpe",	vAtacar},
	{"dar",		vDar},
	{"da",		vDar},
	{"dase",	vDar},
	{"darse",	vDar},
	{"darte",	vDar},
	{"dale",	vDar},
	{"darle",	vDar},
	{"ofrec",	vDar},
//	{"regal",	73},
	{"meter",	vMeter},
	{"mete", 	vMeter},
	{"intro",	vMeter},
	{"inser",	vMeter},
	{"echa",	vMeter},
	{"echar",	vMeter},
/*
	{"LLAMA",	76},
	{"GRITA",	77},
	{"CHILL",	77},
	{"REGIS",	78},
	{"REBUS",	78},
	{"ARRAN",	79},
	{"ESTIR",	79},
	{"USAR",	80},
	{"USA",		80},
	{"UTILI",	80},
	{"MATAT",	81},
	{"MATAR",	81},
	{"SUICI",	81},
	{"XYZZY",	82},
	{"ARRAS",	83},
	{"REGIS",	84},
	{"MIRAT",	85},
	{"MIRAR",	85},
	{"EXAMI",	85},
	{"VACIA",	86},
	{"VERTE",	86},
	{"VERTI",	86},  // Término erróneo, pero ampliamente extendido
	{"VIERT",	86},
	{"DESPI",	87},
	{"DESPE",	87},
	{"ESPAB",	87},
	{"LAVAT",	88},
	{"LAVAR",	88},
	{"LAVAR",	88},
	{"LIMPI",	88},
	{"INSUL",	89},
	{"INCRE",	89},
	*/
	{"ir",		vIr},
	{"ve",		vIr},
	{"vete",	vIr},
	{"irte",	vIr},
	{"irse",	vIr},
	{"camin",	vIr},
	{"anda",	vIr},
	{"andar",	vIr},
	{"corre",	vIr},
	{"huir",	vIr},
	{"huye",	vIr},
	{"dirig",	vIr},
	/*
	{"ESCON",	91},
	{"OCULT",	91},
	{"ESCON",	91},
	{"OCULTA",	91},
	{"ESCON",	92},
	{"OCULT",	92},
	{"LLORA",	93},
	{"LLORI",	93},
	{"SOLLO",	93},
	{"RECOR",	94},
	{"ACORD",	94},
	{"ACUER",	94},
	{"SOPLA",	95},
	{"DOBLA",	96},
	{"DESDO",	97},
	{"desat",	98},
	{"UNIR",	99},
	{"UNE",		99},
	{"JUNTA",	99},
	{"ACERC",	99},
	{"ARRIM",	99},
	{"APROX",	99},
	{"REUNE",	99},
	{"REUNI",	99},
	{"SEPAR",	100},
	{"LIMPI",	101},
	{"FROTA",	101},
	{"LAVAR",	101},
	{"LAVA",	101},
	{"PULE",	101},
	{"PULIR",	101},
	{"FREGA",	101},
	{"FRIEG",	101},
	{"RASCA",	102},
	{"RASPA",	102},
	{"CONEC",	103},
	{"ACTIV",	103},
	{"DESCO",	104},
	{"DESAC",	104},
	{"ABRAZ",	105},
	{"COMPRA",	106},	{"ADQUI",	106},
	{"CONSU",	107},
	{"PREGU",	108},
	*/
	{"lee",		vLeer},
	{"leer",	vLeer},
	{"leers",	vLeer},
	{"leert",	vLeer},
	{"leete",	vLeer},
	{"mover",	vEmpujar},
	{"mueve",	vEmpujar},
	{"despl",	vEmpujar},
/*
	{"APRET",	111},
	{"APRIE",	111},
	{"ESTRU",	111},
	{"BAILA",	112},
	{"DANZA",	112},
	{"SALUD",	113},
	{"LEVAN",	114},
	{"ESPAN",	115},
	{"SECA",	116},
	{"SECAR",	116},
	{"COLOC",	117},
	{"MACHA",	118},
	{"asust", 120},
	{"moja",121},
	{"mojar",121},
	{"empap",121},
	{"impre",121},
*/
	{"tecle", vTeclear},
	{"escri", vEscribir},
	{"punto", vPuntos},
	{"score", vPuntos},
	{"turno", vTurnos},
	{"usa", vUsar},
	{"usar", vUsar},
	{"activ", vUsar},
    {"",0}
};

// Tabla de adjetivos
token_t adjetivos_t [] =
{
	{"peque", aPequeno},
	{"grand", aGrande},
	{"viejo", aViejo},
	{"vieja", aVieja},
	{"nuevo", aNuevo},
	{"nueva", aNueva},
	{"duro", aDuro},
	{"dura", aDura},
	{"bland", aBlando},
	{"corto", aCorto},
	{"corta", aCorta},
	{"largo", aLargo},
	{"larga", aLarga},
	{"azul", aAzul},
	{"verde", aVerde},
	{"negro", aNegro},
	{"rojo", aRojo},
	{"verde", aVerde},
	{"amari", aAmarillo},
	{"termi", aTermico},
	{"frio", aFrio},
	{"calie", aCaliente},
	{"",0}
};


// Tabla de objetos
// No existe la limitación de PAWS donde el objeto 1 siemmpre es la fuente de luz 
// La luz en ngpaws se calcula en función del atributo de los objetos presentes en la localidad, puestos y llevados.
// Localidades de sistema: LOCATION_WORN,LOCATION_CARRIED, LOCATION_NONCREATED, LOCATION_HERE, CARRIED, HERE, NONCREATED, WORN

// Atributos con OR: aLight, aWear, aContainer, aNPC, aConcealed, aEdible, aDrinkable, aEnterable, aFemale, aLockable, aLocked, aMale, aNeuter, aOpenable, aOpen, aPluralName, aTransparent, aScenary, aSupporter, aSwitchable, aOn, aStatic, aExamined, aTaken, aDropped, aVisited, aTalked, aWore, aEaten, aPropio, aDeterminado

obj_t objetos_t[]=
{
    // ID, LOC, NOMBRE, NOMBREID, ADJID, PESO, ATRIBUTOS
    {1, LOCATION_NONCREATED,"linterna",     n_linterna,EMPTY_WORD,   1,0x0000 | aLight | aSwitchable | aFemale  },
    {2, 2,"aceite",         n_aceite,EMPTY_WORD,     2,0x0000 | aMale  },                             // Altar de Ofrendas
    {3, 14,"barra de metal",n_barra,EMPTY_WORD,      1,0x0000 | aFemale  }, // Pasadizo Bajo
    {4, LOCATION_NONCREATED,"copa",         n_copa,EMPTY_WORD,       1,0x0000 | aFemale  }, // Dentro del Cofre
    {5, 19,"cuerda",        n_cuerda,EMPTY_WORD,     1,0x0000 | aFemale  }, // Rampa
    {6, LOCATION_NONCREATED,"espada",       n_espada,EMPTY_WORD,     1,0x0000 | aLight | aSwitchable | aFemale  }, // Dentro del Altar de piedra,
    {7, 15, "antorcha",     n_antorcha,EMPTY_WORD,   1,0x0000 | aLight | aSwitchable | aFemale  }, // Pasaje de los Grabados
    {8, LOCATION_NONCREATED,"pedernal",     n_pedernal,EMPTY_WORD,   1,0x0000 | aMale },
    {9, 30, "sandalia",     n_sandalia,EMPTY_WORD,   1,0x0000 | aFemale  }, // Sala funeraria
    {10, 5,  "tabla",       n_tabla,EMPTY_WORD,      18,0x0000 | aFemale  }, // Sala diminuta
    {11, l_LargoCorredor, "bloque",n_roca,EMPTY_WORD,  255,0x0000 | aMale  }, // Corredor Largo
    {12, 1,"papel",       n_carta,EMPTY_WORD,      1,0x0000 | aMale}, // En la trampa...
    {13, 254,"br'jula",     n_brujula,EMPTY_WORD,    1,0x0000 | aFemale | aDeterminado }, // Llevado
	{14, LOCATION_NONCREATED,"semillas de trigo",       n_trigo,EMPTY_WORD,      1,0x0000 | aFemale | aPluralName}, // No llevado
	{15, l_SalaDeLaVidaEterna,"semillas de maíz",    n_maiz,EMPTY_WORD,    1,0x0000 | aFemale | aPluralName}, // No llevado
	{16, LOCATION_NONCREATED,"llave",    n_llave,EMPTY_WORD,    1,0x0000 | aFemale }, // No llevado, llave de plata
	{17, LOCATION_WORN,"mi anillo",      n_anillo,EMPTY_WORD,     1,0x0000 | aPropio | aWear}, // Llevado, el regalo de Carter
	{34, l_SalaDeLaMomia,"mesa", n_mesa, EMPTY_WORD, 255, 0x0000 | aFemale | aSupporter},
    {0,0,"",                EMPTY_WORD,EMPTY_WORD,            0,0x0000}
}; // Tabla de objetos de la aventura

// ----------------------------------------------------------------
// Tabla de respuestas
// ----------------------------------------------------------------
// Las tablas de respuestas se pueden agrupar en distintos códigos fuente de 16KB, que luego pueden ser alojados en páginas independientes de 16Kb. Este método es compatible
// con todos los ordenadores de 8bit y con PC.
char respuestas()
{
	// Tablas de respuesta por zonas
	// Aquí separamos las tablas de respuestas por funciones, esto no permite utilizar paginación en los modelos
	// con más memoria. 

	// ---------------------------------------------
	// Desierto
	// ---------------------------------------------

	if (flocalidad==l_Desierto || flocalidad==l_Entrada || flocalidad==l_Escaleras)
		{
			respuestas_desierto ();
		}

	// -----------------------------------------------
	// Primera Planta
	// -----------------------------------------------
	
	// -----------------------------------------------
	// Segunda Planta
	// -----------------------------------------------
	
	// -----------------------------------------------
	// Subterráneo
	// -----------------------------------------------



 NOTDONE;
// ================= LIBRERíA BASE FINAL=======================================
}

// ----------------------------------------------------------
// Tabla de respuestas-post
// Se llama después de ejecutar el proceso de respuestas
// ----------------------------------------------------------

char respuestas_post()
{
 //setRAMPage(0);
 // respuestas_post_pagina0();

// ------------------- LIBRERÍA BASE -----------------------------------
 // Comandos de dirección...
 // writeText ("Respuestas Post: %u %u^", flags[fverb], flags[fnoun1]);
 // Movimiento entre localidades...
    BYTE i;
    BYTE loc_temp;

	if (fverbo==vFin) { ACCend(); DONE; }

	if (fverbo==vQuitar) {  ACCautor(); DONE; }
	if (fverbo==vPoner) { ACCautow(); DONE; }
	
	// En este punto el examinar no ha sido cubierto por las respuestas
    if (fverbo==vExaminar)
    {
        if (findMatchingObject(get_loc_pos(loc_here()))!=EMPTY_OBJECT)
            writeText ("Deberías coger eso antes.^");
        else
			writeText ("No ves eso por aquí.^");
		DONE;
    }

    if(fverbo==nInventario)
    {
        ACCinven();
        ACCnewline();
        DONE;
    }

    if (fverbo==vCoger)  { ACCautog(); DONE; }

    if (fverbo==vDejar) { ACCautod(); DONE; }

    if (fverbo==vMeter) { DONE; }

    if (fverbo==vSacar) { DONE; }

    // Si es un nombre/verbo de conexión...
	
    if (flags[fverb]>0 && flags[fverb]<NUM_CONNECTION_VERBS)
        {
            i=0;
            loc_temp = get_loc_pos (flags[flocation]); // La posicion en el array no tiene porque coincidir con su id
			i = conexiones_t[loc_temp].con[flags[fverb]-1];
	        if (i>0)
            {
                ACCgoto(i);
                DONE;
            }
            else {
                ACCsysmess(SYSMESS_WRONGDIRECTION);
                NOTDONE;
            }

        }
	
    // Comandos típicos...
    if (fverbo==vMirar)
    {
        ACCgoto( flags[flocation]);
        DONE;
    }
	if (fverbo==vEmpujar || fverbo==vTirar)
	{
		ACCsysmess (SYSMESS_CANNOTMOVE);
		DONE;
	}

	if (fverbo==vSave || fverbo==vRamsave || fverbo==vLoad || fverbo==vRamload)
	{
		//ACCsave();
		ACCwriteln ("No necesitas cargar o salvar en este viaje -dice el ordenador");
		DONE;
	}

	if (fverbo==vPuntos) { ACCscore(); DONE; }
	if (fverbo==vTurnos) { ACCturns(); DONE; }
	if (fverbo==vEsperar) { writeSysMessage (35); DONE; }
	if (fverbo==vTocar || fverbo==vOler) { ACCmessage (56); DONE; }
	// Si ninguna acción es válida...
    ACCsysmess(SYSMESS_IDONTUNDERSTAND);
    newLine();
	NOTDONE;
}

char proceso1() // Antes de la descripción de la localidad...
{
	// Cálculo de luz
	flags[flight]=1; // No está oscuro
	NOTDONE;
}

char proceso1_post() // Después de la descripción
{
	NOTDONE;
}

char proceso2() // Después de cada turno, haya tenido o no éxito la entrada en la tabla de respuestas
{
	NOTDONE;
}

// ------------------------------------------------------------
// Bucle principal, menú del juego
// ------------------------------------------------------------

void main (void)
{
	// Inicializar variables
	initParser ();                // Inicializa el parser y la pantalla
	

// Añadir menú de juego
	defineTextWindow (0,0,32,24); // Pantalla reducida en 128Kb, Gráficos + Texto
	flags[LOCATION_MAX] = 34; // Número más alto de localidad
	ACCability(10,20); // 10 objetos, 20 piedras
	
	clearTextWindow(INK_YELLOW | PAPER_BLACK, TRUE);
	ACCgoto(l_Desierto); // Localidad inicial, en el puente de mando
	ParserLoop (); // Pone en marcha el bucle principal
	// Parser gets out of the parserloop when the player ENDS the game 	
	
	// -------------------------------------------------
	// The player wants to abandon the game
	writeSysMessage(SYSMESS_PLAYAGAIN);
	flags[fTemp] = getKey();
	// Restart
	if (flags[fTemp]=='y' || flags[fTemp]=='s' || flags[fTemp]=='S' || flags[fTemp]=='Y')
	{
		main();
	}
	
	// To the void...
#ifdef ZX
	#asm 
		jp 0
	#endasm 
#endif
}

// ------------------------------------------------------------
// Funciones propias del juego
// ------------------------------------------------------------
