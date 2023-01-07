// ZHL. Written by KMBR.
// 2016,2019,2021,2023 KMBR
// This code is licensed under a Attribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0) 
// https://creativecommons.org/licenses/by-nc-nd/4.0/
// Use ISO 8819-15 Encoding 

#include <string.h>
#include "./parser_defs.h"
#include "./parser.h"

// Flags del Juego
#include "./juego_flags.h"

// Librer�a gr�fica
#include "./libgfx/libgfx.h"
 
// Additional headers for DOS story files
#ifdef DOS
    #include <conio.h>
    #include <graph.h>
    #include <stdio.h>
    #include <string.h>
#endif 

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

// Compiler options for DOS games
#ifdef DOS
#endif 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// C�digo local
// Si necesitamos c�digo en p�ginas de memoria hay que compilarlas como .bin y a�adirlas aqu� como extern
// Se llaman desde el c�digo usando: setRAMpage (n) y llamando a la funci�n, al terminar hay que volver siempre a la p�gina 0

char respuestas();
char respuestas_post();
char proceso1();
char proceso1_post();
char proceso2();
char proceso2_post();

// ------------------------------------------
// Tablas externas de procesos y respuestas
// ------------------------------------------

// Variables que necesitamos del parser...
// Se pueden declarar m�s tablas de flags para el juego pero el parser
// s�lo trabaja con esta. 
extern unsigned char flags[255]; // Flags 60...250 Disponibles para el usuario
extern unsigned char playerInput[80];
extern unsigned char playerWord[25];
extern BYTE gWord_number; // Marker for current word, 1st word is 1
extern BYTE gChar_number; // Marker for current char, 1st char is 0

// Tabla de im�genes del juego
// Tabla de im�genes del juego
extern unsigned char *L01_img;
extern unsigned char *L02_img;
extern unsigned char *L03_img;
extern unsigned char *L04_img;
extern unsigned char *L05_img;
extern unsigned char *L06_img;
extern unsigned char *L07_img;
extern unsigned char *Europa_img;

// Include binary files for the adventure
#ifdef ZX
	#asm
	_L01_img:
	BINARY "./res/lPuente07.scr.rcs.zx7"
	_L02_img:
	BINARY "./res/lnode1.scr.rcs.zx7"
	_L03_img:
	BINARY "./res/lEsclusa.scr.rcs.zx7"
	_L04_img:
	BINARY "./res/lBodega01.scr.rcs.zx7"
	_L05_img:
	BINARY "./res/lExterior03.scr.rcs.zx7"
	_L06_img:
	BINARY "./res/lEntrada03.scr.rcs.zx7"
	_L07_img:
	BINARY "./res/lzonaA1.scr.rcs.zx7"
	_Europa_img:
	BINARY "./res/Europa.scr.rcs.zx7"
	#endasm

// id, page, memory pointer
// Terminated with 0
img_t imagenes_t [] = {
    { 1,0, L01_img},   
    { 2,0, L02_img},   
    { 3,0, L03_img},   
 	{ 4,0, L04_img},   
    { 5,0, L05_img},   
    { 6,0, L06_img},   
    { 7,0, L07_img},   
    { 8,0, L07_img},   
	{ 9,0, Europa_img},
    { 0,0,0}
    };
#endif 

#ifdef DOS 
	#ifdef TEXT 
	img_t imagenes_t [] = {
		{ 0,0,0}
		};
	#endif 
	#if defined CGA
	// id, page, memory pointer
	// It is convenient to use the same ID as the location but is not mandatory 
	// Terminated with 0
	img_t imagenes_t [] = {
		{ 1,0, "./resDOS/L01cga.pcx"},   
		{ 2,0, "./resDOS/L02cga.pcx"},   
		{ 3,0, "./resDOS/L03cga.pcx"},   
		{ 4,0, "./resDOS/L04cga.pcx"},   
		{ 5,0, "./resDOS/L05cga.pcx"},   
		{ 6,0, "./resDOS/L06cga.pcx"},   
		{ 7,0, "./resDOS/L07cga.pcx"},   
		{ 8,0, "./resDOS/L07cga.pcx"},   
		{ 9,0, "./resDOS/L09cga.pcx"},
		{ 0,0,0}
		};
	#endif 
	#if defined EGA
	// id, page, memory pointer
	// It is convenient to use the same ID as the location but is not mandatory 
	// Terminated with 0
	img_t imagenes_t [] = {
		{ 1,0, "./resDOS/L01ega.pcx"},   
		{ 2,0, "./resDOS/L02ega.pcx"},   
		{ 3,0, "./resDOS/L03ega.pcx"},   
		{ 4,0, "./resDOS/L04ega.pcx"},   
		{ 5,0, "./resDOS/L05ega.pcx"},   
		{ 6,0, "./resDOS/L06ega.pcx"},   
		{ 7,0, "./resDOS/L07ega.pcx"},   
		{ 8,0, "./resDOS/L07ega.pcx"},   
		{ 9,0, "./resDOS/L09ega.pcx"},
		{ 0,0,0}
		};
	#endif 
#endif 

// Tabla de regiones
// Regiones del juego...
//unsigned char region_exterior [] = { 4,6,7,0 };
//unsigned char region_entrada [] = { 13,10,17,14,9,0 };
//unsigned char region_superior [] = { 3,8,23,26,27,0 };
//unsigned char region_inferior [] = { 12,15,19,2,5,11,21,16,29,18,27,28,25,24,30,1,31,20,0 };

// Tabla de localidades de la aventura
// 250 to 255 reservadas
// �ltima localidad como ID 0 
// unsigned char *name;
//	unsigned char *descripcion;
//	unsigned char id;
//  unsigned char visitado
//	unsigned long int atributos; // 32bit

#ifdef SPANISH 
loc_t localidades_t [] =
{
	{"Puente de mando","Los instrumentos de navegaci�n iluminan el puente de mando. En el exterior, una tormenta de hielo se abate sobre la superficie de Europa. ",lPuente, FALSE, 0x00000000},
	{"Nodo central ","Los m�dulos de la nave est�n en penumbra. El nodo central conecta con el puente de mando, la esclusa al oeste y la bodega al sur. ",lNodo, FALSE, 0x00000000},
	{"Esclusa","Se escucha con intensidad la tormenta golpeando contra el fuselaje. Las sombras crean formas siniestras en los trajes de exploraci�n. ",lEsclusa, FALSE, 0x00000000},
	{"Bodega de carga","La zona de carga tiene espacio suficiente para atender peque�os env�os entre particulares. En la sombras hay varios paquetes que esperan su entrega. ", lBodega, FALSE, 0x00000000},
	{"Exterior","Me envuelve una tormenta de nieve y cristales de hielo que me impide ver en la distancia. Al oeste se distingue un edificio que podr�a ser la entrada del almac�n.",lExterior, FALSE, 0x00000000},
    {"Entrada al almac�n","Una mole de metal se encuentra enclavada en la superficie helada de Europa. ",lAlmacen, FALSE, 0x00000000},
    {"Zona A1","Las mon�tonas estanter�as repletas de contenedores blancos crean una atm�sfera impersonal. Al oeste un oscuro pasillo da acceso a una sala roja.",lZonaA1, FALSE, 0x00000000},
    {"Zona A2","El pasillo termina en una sala abovedada iluminada en rojo. Las mon�tonas estanter�as prosiguen su curso con m�s contenedores blancos perfectamente ordenados.",lZonaA2, FALSE, 0x00000000},
    {"","",0, FALSE, 0x00000000}
};
#endif

#ifdef ENGLISH
loc_t localidades_t [] =
{
	{"Bridge", "The bridge is dimly illuminated by the navigation panel. Outside, an ice storm is raging over the surface of Europe. ",lPuente, FALSE, 0x00000000},
	{"Central node ", "The ship's modules are in twilight. The central node connects to the command bridge, the airlock to the west and the cargo to the south. ",lNodo, FALSE, 0x00000000},
	{"Airlock", "The storm can be heard pounding intensely against the hull. The poor lighting traces sinister shapes in the exploration suits. ",lEsclusa, FALSE, 0x00000000},
	{"Cargo hold", "The cargo area has enough space to handle small shipments. In the shadows there are several packages awaiting delivery. ", lBodega, FALSE, 0x00000000},
	{"Outside", "I am engulfed by a storm of snow and ice crystals preventing me from seeing in the distance. To the west I can make out a building that could be the entrance to the warehouse.",lExterior, FALSE, 0x00000000},
    {"Warehouse entrance", "A mass of metal sits nestled on the frozen surface of Europe. ",lAlmacen, FALSE, 0x00000000},
    {"Zone A1", "The monotonous shelves filled with white containers create an impersonal atmosphere. To the west a dark corridor leads to a red room.",lZonaA1, FALSE, 0x00000000},
    {"Zone A2", "The corridor ends in a vaulted room lit in red. The monotonous shelves continue their course with more neatly arranged white containers.",lZonaA2, FALSE, 0x00000000},
	{"","",0, FALSE, 0x00000000}
};   
#endif 

// Localidades para contenedores

// Tabla de conexiones...
// Se usan los ID de localidad para idenfiticar las conexiones

cnx_t conexiones_t [] =
{
// LOC | AL_N | AL_S | AL_E | AL_O | AL_NE | AL_NO | AL_SE | AL_SO | ARRIB | ABAJO | ENTRAR | SALIR 
	{lPuente,{	0,		lNodo,		0,		0,		0,		0,		0,		0,		0,		lNodo,	0,	0}},
	{lNodo,{lPuente,		lBodega,		0,		lEsclusa,		0,		0,		0,		0,		lPuente,		0,0,0}},
	{lEsclusa,{0,		0,		lNodo,		0,		0,		0,		0,		0,		0,		0,0,0}},
	{lBodega,{lNodo,		0,		0,		0,		0,		0,		0,		0,		lNodo,		0,0,0}},
	{lExterior,{0,		0,		lEsclusa,		lEntrada,		0,		0,		0,		0,		0,		0,0,0}},
	{lEntrada,{0,		0,		lExterior,		0,		0,		0,		0,		0,		0,		0,0,0}},
	{lZonaA1,{0,		0,		lEntrada,		lZonaA2,		0,		0,		0,		0,		0,		0,0,lEntrada}},
	{lZonaA2,{0,		0,		lZonaA1,		0,		0,		0,		0,		0,		0,		0,0,0}},
	{0,{	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,0,0}}
	};


// Tabla de mensajes de la aventura
// 1 to 255
// Text, message_id (1 to 255)
// Messages can be printed directly in the code just calling the function writeText (unsigned char *string)
// More than one message table can be defined
#ifdef SPANISH 

token_t mensajes_t [] =
{
	{"ZHL^ Entregamos su paquete en 24h. ^ (c) 2016, 2019, 2021 KMBR. Release 5.",1},
	{"No veo nada en particular.",2},
	{"ZHL by KMBR",3},
	{"Descienden al nodo central.",4},
	{"Ascienden al puente de mando.",5},
	{"El m�dulo para comandar la nave.",6},
	{"La v�a l�ctea en nuestro caso.",7},
	{"La consola de navegaci�n de la nave. Es un modelo bastante nuevo, el ordenador de abordo se encarga de la navegaci�n y todos los comandos se realizan por voz. En este momento la pantalla de la consola est� en negro.",8},
	{"-Funcionamos en modo de bajo consumo. La consola debe permanecer apagada -responde el ordenador.",9},
	{"Proviene de la consola de mandos. La consola est� en modo de bajo consumo. ",10},
	{"Nieve y hielo a 150 Celsius bajo cero.",11},
	{"El sistema t�rmico apenas puede evitar la formaci�n de cristales en el exterior del parabrisas.",12},
	{"Se forman en los gradientes t�rmicos del parabrisas de la nave.",13},
	{"La nave de reparto donde hago la ruta. Es un modelo industrial construido con m�dulos est�ndar. Hace su trabajo y no se aver�a demasiadas veces.",14},
	{"-Controla la temperatura en el interior y en los instrumentos para evitar su deterioro -informa el ordenador.",15},
	{"Es uno de los dos trajes de supervivencia necesario para realizar EVA. El traje es completamente autom�tico y tiene una autonom�a de ocho horas. ",16},
	{"Paquetes a entregar por el sistema solar.",17},
	{"Est� apagada.",18},
	{"No sobrevivir� en Europa sin el traje protector.",19},
	{"-Para evitar contaminaci�n deja el traje antes de pasar al nodo central -te recuerda el ordenador.",20},
	{"La esclusa sirve para igualar la presi�n entre el exterior y el interior de la nave. Se controla con el b�ton rojo para cerrar y el bot�n verde para abrir. ",21},
	{"Est� cerrada.",22},
	{"Est� abierta.",23},
	{"-La esclusa se abre con los controles manuales -informa el ordenador.",24}, 
	{"-La esclusa se cierra con los controles manuales -informa el ordenador. ",25},
	{"Pulsar para abrir la esclusa. ^ADVERTENCIA: ANTES DE ABRIR USAR EL TRAJE DE SUPERVIVENCIA EN ENTORNOS HOSTILES.",26},
	{"Pulsar para cerrar la esclusa.",27},
	{"-La esclusa ya est� abierta.",28},
	{"-Abriendo compuerta exterior y aislando el interior -dice el ordenador. La compuerta exterior se abre con un siseo mientras se igualan las presiones.",29},
	{"-Cerrando compuerta exterior- repite el ordenador",30},
	{"Una vez la compuerta exterior se ha cerrado, unos chorros de aire a presi�n surgen de las paredes para eliminar los contaminantes e igualar de nuevo la presi�n.",30},
	{"Un panel se abre y permite el acceso al interior de la nave.",31},
	{"-La esclusa ya est� cerrada -reporta el ordenador de abordo.",32},
	{"Apenas unos mil�metros de pl�stico y metal te separan del exterior.",33},
	{"Fr�o al tacto.",34},
	{"Como en una noche oscura salvo por el brillo esquivo de los trozos de hielo que est�n cayendo.",35},
	{"La nave modular est�ndar de ZHL. Pintada de amarillo y con el logo de la compa��a en grandes letras rojas.",36},
	{"ZHL 24h",37},
	{"Con dificultad distingo el perfil de un edificio de metal en la tormenta. ",38},
	{"En un lateral de la entrada brilla un teclado num�rico.",39},
	{"Por una abertura se desliza un ca��n l�ser que te apunta: TECLEE EL C�DIGO DE ACCESO. TIENE UNA OPORTUNIDAD ANTES DE SER DESINTEGRADO.",40},
	{"Un ca��n de aspecto peligroso sigue mis movimientos.",41},
	{"Todo son cajas blancas de tama�o similar. Las m�s grandes se encuentran en los estantes m�s bajos.",42},
	{"Alojan decenas de peque�os contenedores blancos.",43},
	{"Todo funcional. El espacio suficiente para mantener las condiciones necesarias para preservar materiales biol�gicos.",44},
	{"Una elevada b�veda forma el techo de esta sala.",45},
	{"Un paquete azul con un gran indicador de peligro biol�gico en el frontal. ",46},
	{"-Este es el paquete -susurra el ordenador en tu o�do.",47},
	{"-No puedo, las directivas de la empresa me lo impiden. ",48},
	{"Peligro biol�gico. Conservar a menos de 15 celsius bajo cero. No manipular sin protecci�n. No abrir. No golpear. Material fr�gil.",49},
	// Textos relativos al ordenador
	{"-Coordino todos los sistemas de la nave. Para hablar conmigo emplea: ordenador comando. Algunas sugerencias de comandos son: ordenador ayuda, ordenador nombre, ordenador misi�n... -te responde servicial el ordenador de navegaci�n.",50},
	{"-Oh, vaya como no pens� en leer eso -dice el ordenador con falsa admiraci�n.",51},
	{" ",52},
	{"No noto nada en particular.",53},
	{"Nada que destacar.",54},
	{"No es especialmente interesante.",55},
	{"No sucede nada inesperado.",56},
	{"El aullido del viento resuena en el fuselaje.",57},
	{"-Buen trabajo -aprueba la voz satisfecha del ordenador.  ",58},
	{"-Ahora es momento de poner rumbo a Marte. Baja a la bodega de carga mientras reinicio los sistemas de la nave -dice el ordenador con premura. ",59},
	{"Desciendo los escalones a la bodega y deposito el paquete junto a los dem�s pendientes de entrega. Al regresar descubro que la sala se ha cerrado.",60},
	{"-No es nada personal pero es momento de reciclar -dice la voz del ordenador- mientras te fallan las piernas y golpeas con la cabeza contra el suelo.",61}, 
	{"-Lo m�s duro es tener que reciclar sus piezas para la siguiente misi�n. Llegan a creerse humanos. En fin... -suspira el ordenador mientras recoge los restos de tu cuerpo con una robofregona.",62},
	{"-Central, aqu� Tod Connor -dice la voz- Volvemos a Marte con el paquete. Repito volvemos con el paquete. ",63},
	{"Hay dos botones: rojo y verde. Se utilizan para cerrar y abrir la esclusa al exterior. ",172},
	{"Los siguientes paquetes para entregar.",173},
	{"No es momento de jugar al Sokoban.",174},
	{"-Voolare... ooh oooh -te devuelve cruel el eco de la nave.",175},
	{"-Cantare, ooh oooh -intentas entonar ^ -Nel blu dipinto di blu...",176},
	{"S�lo escucho est�tica. La tormenta interfiere en las comunicaciones.",177},
	{"Estoy en la cara oculta, no veo J�piter.",178},
	{"Sat�lite helado e inh�spito.",179}, 
	{"La entrada al almac�n. ",180},
	{"Funciona con comandos de voz al ordenador: ordenador comando.",181}, 
	{"Es una construcci�n met�lica y rectangular que se interna bajo tierra.",182}, 
	{"(en el teclado)",183},
	{"La puerta del almac�n ya est� abierta.",184},
	{"El teclado se ilumina en rojo. CLAVE INCORRECTA.",185}, 
	{"El teclado se ilumina en verde. CLAVE CORRECTA. El ca��n l�ser se repliega en su compartimento.",186}, 
	{"El ca��n l�ser comienza a calentarse.",187},
	{"El ca��n l�ser se prepara a disparar. ",188},
	{"El ca��n l�ser dispara. ",189},
	{"Pero nada sucede. Una voluta de humo y algunas chispas surgen del ca��n.",190},
	{"Son contenedores de transporte.",191}, 
	{"(cogiendo antes el traje)",192},
	{"-Los sistemas ya est�n en modo de bajo consummo -te recuerda el ordenador.",193},
	{"-S�lo necesito un paquete azul. ",194},
	{"La superficie es lisa, s�lo se activa con el teclado.", 195},
	{"E U R O P A",196},
};
#endif 

#ifdef ENGLISH

token_t mensajes_t [] =
{
	{"ZHL^ We deliver your parcel within 24h. ^ (c) 2016, 2019, 2021 KMBR. Release 5.",1},
	{"I don't see anything in particular.",2},
	{"ZHL by KMBR",3},
	{"They descend to the central node.",4},
	{"They ascend to the command bridge.",5},
	{"The module to command the ship.",6},
	{"The Milky Way in our case.",7},
	{"The ship's navigation console. It is a fairly new model, the on-board computer is in charge of navigation and all commands are made by voice. At the moment the console screen is black.",8},
	{"'We are operating in low power mode. The console must remain off', replies the computer.",9},
	{"It comes from the control console. The console is in low power mode. ",10},
	{"Snow and ice at minus 150 Celsius.",11},
	{"The thermal system can hardly prevent the growth of crystals on the outside of the windscreen.",12},
	{"They form on the thermal gradients of the ship's windscreen",13},
	{"The delivery shed where I do the route. It is an industrial model built with standard modules. It does its job and doesn't break down too often.",14},
	{"'It controls the temperature inside and in the instruments to avoid their deterioration,' reports the computer",15},
	{"It is one of the two survival suits required to perform EVA. The suit is fully automatic and has an autonomy of eight hours. ",16},
	{"Packages to be delivered throughout the solar system.",17},
	{"It's off.",18},
	{"I won't survive in Europe without the protective suit.",19},
	{"'To avoid contamination, leave the suit behind before passing the central node,'  the computer reminds you.",20},
	{"The airlock is used to equalise the pressure between the outside and inside of the ship. It is controlled by the red button to close and the green button to open. ",21},
	{"It is closed.",22},
	{"It is open.",23},
	{"'The airlock is opened with the manual controls,' reports the computer",24},
	 {"'The airlock is opened with the manual controls,' reports the computer",24}, 
	{"'The airlock is closed with the manual controls,' computer reports. ",25},
	{"Press to open the airlock. WARNING: WEAR SURVIVAL SUIT IN HOSTILE ENVIRONMENT BEFORE OPENING.",26},
	{"Press to close the airlock.",27},
	{"The airlock is now open.",28},
	{"'Opening outer airlock and isolating the interior,' says the computer. The outer airlock opens with a hiss as the pressures equalise.",29},
	{"'Closing outer hatch,' repeats the computer",30},
	{"Once the outer hatch has closed, jets of pressurised air burst out of the walls to remove contaminants and equalise the pressure again.",30}, 
	{"A panel opens and allows access to the interior of the spacecraft.",31},
	{"'The airlock is now closed,' reports the on-board computer.",32},
	{"Just a few millimetres of plastic and metal isolate you from the outside.",33},
	{"Cold to the touch.",34},
	{"As in a dark night except for the elusive glow of falling ice chunks.",35},
	{"The standard ZHL modular ship. Painted yellow and with the company logo in big red letters.",36},
	{"ZHL 24h",37},
	{"With difficulty I can make out the outline of a metal building in the storm. ",38},
	{"On the side of the entrance shines a numeric keypad.",39},
	{"A laser cannon slides through an opening and points at you: TYPE IN THE ACCESS CODE. YOU HAVE ONE CHANCE BEFORE DESINTEGRATION.",40},
	{"A dangerous-looking cannon follows my movements.",41},
	{"All are white boxes of similar size. The largest are on the lowest shelves.",42},
	{"They house dozens of small white containers.",43},
	{"Bare minimum. Just enough space to maintain the conditions needed to preserve biological materials.",44},
	{"A high vault forms the ceiling of this room.",45},
	{"A blue package with a large biohazard indicator on the front. ",46},
	{"'This is the package,' the computer whispers in your ear.",47},
	{"I can't, the company's directives prevent me from doing so. ",48},
	{"Biohazard. Store below minus 15 Celsius. Do not handle without protection. Do not open. Do not handle unprotected. Fragile material.",49},
	// Computer-related texts
	{"'I manage all spacecraft's systems. To talk to me use: computer command. Some suggestions of commands are: computer help, computer name, computer mission..' the navigation computer responds helpfully to you.",50},
	{"'Oh, I didn't think I'd read that,' says the computer with false admiration.",51},
	{" ",52},
	{"I don't notice anything in particular.",53},
	{"Nothing to highlight.",54},
	{"It's not particularly interesting.",55},
	{"Nothing unexpected happens.",56},
	{"The howl of the wind echoes in the fuselage.",57},
	{"'Good job,' approves the satisfied voice of the computer.",58},
	{"'Now it's time to set course for Mars. Go down to the cargo hold while I reboot the ship's systems,' the computer says hastily.",59},
	{"I descend the steps to the cellar and deposit the package with the others awaiting delivery. When I return I discover that the room has closed.",60},
	{"'It's nothing personal but it's time to recycle,' says the computer voice, as your legs give out and you bang your head on the floor",61}, 
	{"'The hardest thing is having to recycle their parts for the next mission. They come to think of themselves as human. Anyway...' sighs the computer as it scoops up the remains of your body with a robo-scrubber.",62},
	{"'Central, this is Tod Connor,' says the voice, 'I'm returning to Mars with the package. I repeat I'm returning with the package.' ",63},
	{"There are two buttons: red and green. They are used to close and open the airlock to the outside. ",172},
	{"Next packages to be delivered.",173},
	{"This is no time to play Sokoban.",174},
	{"'Voolare... ooh oooh' you get a cruel echo back from the ship.",175}, 
	{"'Cantare, ooh oooh 'you try to intone ^ 'Nel blu dipinto di blu...'",176},
	{"I hear only static. The storm is interfering with communications.",177},
	{"I'm on the dark side, I can't see Jupiter",178},
	{"Icy, inhospitable satellite.",179}, 
	{"The entrance to the warehouse. ",180},
	{"It works with voice commands to the computer: computer command.",181}, 
	{"It is a rectangular metal construction that goes underground.",182}, 
	{"(on the keyboard)",183},
	{"The warehouse door is already open.",184},
	{"The keypad is illuminated red. INCORRECT KEY.",185}, 
	{"The keypad is illuminated green. CORRECT KEY. The laser cannon retracts into its compartment.",186}, 
	{"Laser cannon begins to heat up.",187},
	{"The laser cannon prepares to fire. ",188},
	{"Laser cannon fires. ",189},
	{"But nothing happens. A puff of smoke and a few sparks emerge from the cannon.",190},
	{"They're shipping containers.",191}, 
	{"(picking up the suit first)",192},
	{"-The systems are already in low power mode - the computer reminds you.",193},
	{"-I just need a blue packet. ",194},
	{"The surface is smooth, only activated with the keyboard.", 195},
	{"E U R O P A",196},
};
#endif 

// Messages used by the parser. 
#ifdef SPANISH
token_t mensajesSistema_t [] =
{
	{"No puedo ver nada, est� muy oscuro.^",0},
	{"Tambi�n hay ",1},
	{"Escribe tus �rdenes aqu�",2},
	{"",3},
	{"",4},
	{"",5},
	{"�C�mo? Por favor prueba con otras palabras.^",6},
	{"No puedo ir en esa direcci�n.^",7},
	{"�Perd�n?^",8},
	{"Llevo ",9},
	{"(puesto)",10},
	{"ning�n objeto.",11},
	{"�Seguro? ",12},
	{"�Quieres abandonar la partida? ",13},
	{"Adi�s...",14},
	{"OK^",15},
	{"[...]",16},
	{"Has realizado ",17},
	{" turno",18},
	{"s",19},
	{".^ ",20},
	{"La puntuaci�n es de ",21},
	{" punto",22},
	{"No llevo puesto eso.^",23},
	{"No puedo, ya llevo eso puesto.^",24},
	{"Ya tengo ",25},
	{"No veo eso por aqu�.^",26},
	{"No puedo llevar m�s cosas.^",27},
	{"No tengo eso.^",28},
	{"Pero si ya llevo puesto",29},
	{"S",30},
	{"N",31},
	{"M�s...",32},
	{"> ",33},
	{"",34},
	{"El tiempo pasa...^",35},
	{"Ahora tengo ",36},
	{"Me pongo ",37},
	{"Me quito ",38},
	{"Dejo ",39},
	{"No puedo ponerme ",40},
	{"No puedo quitarme ",41},
	{"�Tengo demasiadas cosas en las manos!",42},
	{"Desgraciadamente pesa demasiado.",43},
	{"Meto",44},
	{"Ahora no est� en ",45},
	{",",46},
	{" y ",47},
	{".",48},
	{"No tengo ",49},
	{"No llevo puesto ",50},
	{"",51},
	{"Eso no est� en ",52},
	{"ning�n objeto",53},
	{"Fichero no encontrado.^",54},
	{"Fichero corrupto.^",55},
	{"Error de E/S. Fichero no grabado.^",56},
	{"Directorio lleno.^",57},
	{"Disco lleno.",58},
	{"Nombre de fichero no v�lido.^",59},
	{"Nombre del fichero: ",60},
	{"",61},
	{"�Perd�n? Por favor prueba con otras palabras.^",62},
	{"Aqu� ",SYSMESS_NPCLISTSTART},
	{"est� ",SYSMESS_NPCLISTCONTINUE},
	{"est�n",SYSMESS_NPCLISTCONTINUE_PLURAL},
    {"Dentro hay ",SYSMESS_INSIDE_YOUCANSEE},
    {"Encima hay ",SYSMESS_OVER_YOUCANSEE},
    {"",68},
	{"No es algo que pueda quitarme.^",69},
	{"Pongo ",SYSMESS_YOUPUTOBJECTON },
    {"No es algo que pueda cogerse.^",SYSMESS_YOUCANNOTTAKE},
	{"No parece que pueda moverse.^", SYSMESS_CANNOTMOVE},
	{"Llevo las manos vac�as.^", SYSMESS_CARRYNOTHING},
	{"Salidas visibles:",SYSMESS_EXITSLIST},
	{"",0}	
};
#endif 

#ifdef ENGLISH 
token_t mensajesSistema_t [] =
{
	{"It's too dark to see anything.^",0},
	{"You can see ",1},
	{"What do you want me to do now?",2},
	{"",3},
	{"",4},
	{"",5},
	{"Sorry? Please try other words.^",6},
	{"I can't go in that direction.^",7},
	{"Pardon me?^",8},
	{"I am carrying: ",9},
	{"(worn)",10},
	{"nothing at all.",11},
	{"Are you sure? ",12},
	{"Do you want to abandon? ",13},
	{"Goodbye...",14},
	{"OK^",15},
	{"[...]",16},
	{"You have typed ",17},
	{" turn",18},
	{"s",19},
	{".^ ",20},
	{"Your score is ",21},
	{" point",22},
	{"I'm not wearing that.^",23},
	{"I cannot, I'm already wearing that.^",24},
	{"I already have ",25},
	{"I cannot see that around.^",26},
	{"I cannot carry anymore.^",27},
	{"I don't have that.^",28},
	{"I'm already wearing ",29},
	{"Y",30},
	{"N",31},
	{"More...",32},
	{"> ",33},
	{"",34},
	{"Time passes ...^",35},
	{"I've taken ",36},
	{"I wear ",37},
	{"I remove ",38},
	{"I drop ",39},
	{"I cannot wear  ",40},
	{"I cannot remove ",41},
	{"My hands are full!",42},
	{"It's too heavy.",43},
	{"I put ",44},
	{"It is not in ",45},
	{",",46},
	{" and ",47},
	{".",48},
	{"I don't have ",49},
	{"I'm not wearing ",50},
	{"",51},
	{"That is not in ",52},
	{"nothing at all",53},
	{"File not found.^",54},
	{"Corrupted file.^",55},
	{"I/O error. File not saved.^",56},
	{"Directory full.^",57},
	{"Disc is full.",58},
	{"Invalid savegame name. Please check the name you entered is correct, and make sure you are trying to load the game from the same file you saved.^",59},
	{"Please enter savegame name. Remember to note down the name you choose, as it will be requested in order to restore the game status.",60},
	{"",61},
	{"Sorry? Please try other words.^",62},
	{"Here ",SYSMESS_NPCLISTSTART},
	{"I can see ",SYSMESS_NPCLISTCONTINUE},
	{"I can see ",SYSMESS_NPCLISTCONTINUE_PLURAL},
    {"Inside I see ",SYSMESS_INSIDE_YOUCANSEE},
    {"On top I see ",SYSMESS_OVER_YOUCANSEE},
    {"",68},
	{"It is not something I can remove.^",69},
	{"I put ",SYSMESS_YOUPUTOBJECTON },
    {"It is not something I can take.^",SYSMESS_YOUCANNOTTAKE},
	{"It cannot be moved.^", SYSMESS_CANNOTMOVE},
	{"My hands are empty.^", SYSMESS_CARRYNOTHING},
	{"Exits:",SYSMESS_EXITSLIST},
	{"",0}	
};

#endif 

// Tablas de vocabulario
// Nombre propios, sustantivos...
// �ltimo elemento debe ser 0
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


#ifdef SPANISH 
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
	{"todo",        nTodo},
	{"puerta", nPuerta},
	{"boton", nBoton},
	{"escal", nEscalera},
	{"pared", nPared},
	{"suelo", nSuelo},
	{"techo", nTecho},
	{"luz", nLuz},
	{"haz", nLuz},
	{"parab", 	nParabrisas},
	{"caja",	nCaja},
	{"paque",	nPaquete},
	{"conte",	nContenedor},
	{"cielo",	nCielo},
	{"nave",	nNave},
	{"fusel",	nNave},
	{"nodo",	nNodo},
	{"puent",	nPuente},
	{"torme",	nTormenta},
	{"europ",	nEuropa},
	{"luna",	nLuna},
	{"lunas",	nLuna},
	{"satel",	nSatelite},
	{"jupit",	nJupiter},
	{"jovia",	nJoviano},
	{"cara",	nCara},
	{"lado",	nLado},
    {"inter", 	nInterior},
	{"almac",	nAlmacen},
	{"mole",	nMole},
	{"edifi",	nEdificio},
	{"orden", 	nOrdenador},
//	{"compu", 	nOrdenador},
	{"ia", 		nOrdenador},
	{"dot", 	nOrdenador},
	{"tod",		nOrdenador},	
	{"navi", 	nOrdenador}, 
	{"gps", 	nOrdenador},  
	{"galileo", nOrdenador},
	{"tom", 	nOrdenador},    
	{"tomtom", 	nOrdenador}, 
	{"inter", 	nInterior},
	{"conso", 	nConsola},
	{"puent", 	nConsola},
	{"mando",	 nConsola},
	{"siste",	nSistema},
	{"esclu", 	nEsclusa},
	{"airlo", 	nEsclusa},
	{"compu",	nEsclusa},
	{"traje",	nTraje},
	{"bodega", nBodega},
	{"boveda", nBoveda},
	{"estant",	nEstanteria},
	{"camara",	nCamara},
	{"canon",	nCanon},
	{"32768",	n32768},
	{"tecla",	nTeclas},
	{"exter",	nExterior},
	{"fuera",	nFuera},
	{"afuer",	nFuera},
	{"etiqu",	nEtiqueta},
	{"indic",	nIndicador},
	{"panta", nPantalla},
	{"instr", nPantalla},
	{"contr", nControles},
	{"panel", nPantalla},
	{"sombr", nSombra},
	{"",0}
};

#endif 

#ifdef ENGLISH
token_t nombres_t [] =
{
    {"north",           nNorte},  //0
	{"n",               nNorte},
	{"south",             nSur},    //2
	{"s",               nSur},
	{"east",            nEste},   //4
	{"e",               nEste},
	{"west",           nOeste},  //6
	{"o",               nOeste},
	{"northeast",         nNoreste}, //8
	{"w",               nOeste},
	{"northwest",        nNorOeste}, //10
	{"ne",              nNoreste},
	{"southest",         nSurEste},	 //12
	{"se",              nSurEste},
	{"southwest",        nSurOeste},  // 14
    {"sw",           nSurOeste},
    {"up",          nArriba},   // 16
	{"nw",              nNorOeste},
	{"down",           nAbajo},   // 18
	{"enter",			nEntrar}, // 20
	{"exit",			nSalir}, // 22
	{"leave",			nSalir}, // 22
	{"up",	nArriba},
	{"down",  	nAbajo},
	{"desce",	nAbajo},
	{"i",               nInventario},
    {"inven",           nInventario},
	{"inv",         nInventario },
	{"score",       nPuntos},
	{"points",       nPuntos},
	{"turns",       nTurnos},    
	// Names < 20 can be used as verbs
	// Nombres para el Vocabulario del juego,
	{"all",        nTodo},
	{"door", nPuerta},
	{"button", nBoton},
	{"stair", nEscalera},
	{"wall", nPared},
	{"hull",nPared},
	{"floor", nSuelo},
	{"ceill", nTecho},
	{"light", nLuz},
	{"beam", nLuz},
	{"winds", 	nParabrisas},
	{"box",	nCaja},
	{"parcel",	nPaquete},
	{"packet",nPaquete},
	{"packag",nPaquete},
	{"conta",	nContenedor},
	{"sky",	nCielo},
	{"space", 	nNave},
	{"ship",	nNave},
	{"fusel",	nNave},
	{"node",	nNodo},
	{"bridg",	nPuente},
	{"storm",	nTormenta},
	{"europ",	nEuropa},
	{"moon",	nLuna},
	{"moons",	nLuna},
	{"satel",	nSatelite},
	{"jupit",	nJupiter},
	{"jovia",	nJoviano},
	{"face",	nCara},
	{"side",	nLado},
    {"inter", 	nInterior},
	{"wareh",	nAlmacen},
	{"mass",	nMole},
	{"build",	nEdificio},
	{"compu", 	nOrdenador},
	{"ia", 		nOrdenador},
	{"dot", 	nOrdenador},
	{"tod",		nOrdenador},	
	{"navi", 	nOrdenador}, 
	{"gps", 	nOrdenador},  
	{"galileo", nOrdenador},
	{"tom", 	nOrdenador},    
	{"tomtom", 	nOrdenador}, 
	{"inter", 	nInterior},
	{"conso", 	nConsola},
	{"bridge", 	nConsola},
	{"instr",	nSistema},
	{"airlo", 	nEsclusa},
	{"hatch",	nEsclusa},
	{"gate", 	nEsclusa},
	{"suit",	nTraje},
	{"cargo", nBodega},
	{"hold", nBoveda},
	{"shelv",	nEstanteria},
	{"camera",	nCamara},
	{"canno",	nCanon},
	{"32768",	n32768},
	{"key",	nTeclas},
	{"keypad", nTeclas},
	{"keybo", nTeclas},
	{"exter",	nExterior},
	{"outsi",	nFuera},
	{"label",	nEtiqueta},
	{"indic",	nIndicador},
	{"scree", nPantalla},
	{"instr", nPantalla},
	{"contr", nControles},
	{"panel", nPantalla},
	{"shado", nSombra},
	{"",0}
};

#endif 

// Verbos
// VOCABULARIO
// Verbos < 20 son iguales a nombres < 20
#ifdef SPANISH 

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
	{"ENSE�", 	36},
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
	{"VERTI",	86},  // T�rmino err�neo, pero ampliamente extendido
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
#endif 

#ifdef ENGLISH
token_t verbos_t [] =
{
    {"take",	vCoger},
    {"pick",	vCoger},
    {"drop",	vDejar},
    {"remove",	vQuitar},
    {"remove",	vSacar},
    {"remove",	vQuitar},
    {"l",		vMirar},
    {"look",	vMirar},
    {"r",		vMirar},
    {"redes",	vMirar},
    {"quit",	vFin},
    {"save",	vSave},
    {"salva",	vSave},
	{"load",	vLoad},
	{"ramsa",	vRamsave},
	{"ramlo",	vRamload},
	{"carga",	vLoad},
	{"x",       vExaminar},
	{"exami",	vExaminar},
	{"ex",		vExaminar},
	{"regis",	vExaminar},
	{"say", 	vDecir},
	{"talk",		vDecir},
	{"answer", 	vDecir},
	{"order",  	vDecir},
	{"throw",	vLanzar},
	{"push",	vEmpujar},
	{"press",	vEmpujar},	
	{"press",	vPulsar},
	{"turn",  	vGirar},
	{"rotat",	vGirar},
	{"listen", 	vEscuchar},
	{"hear",		vEscuchar},
	{"touch",	vTocar},
	{"pat",	vTocar},
	{"smell",	vOler},
	{"sniff",	vOler},
	{"wait",	vEsperar},
	{"z",		vEsperar},
	{"sing",	vCantar},
	{"jump", 	vSaltar},
	{"attac",	vAtacar},
	{"kill",	vAtacar},
	{"fight",	vAtacar},
	{"punch",	vAtacar},
	{"kick",	vAtacar},
	{"pee",	vAtacar},
	{"open",	vAbrir},
	{"close",	vCerrar},
	{"activ",	vEncender},
	{"deact",	vApagar},
	{"break",	vRomper},
	{"destroy",	vRomper},
	{"put",		vPoner},
	{"wear", vPoner},
	{"wear",	vVestir},
	{"put",	vVestir},
	{"give",		vDar},
	{"put",	vMeter},
	{"insert", 	vMeter},
	{"go",		vIr},
	{"walk",		vIr},
	{"run",	vIr},
	{"read",		vLeer},
	{"move",	vEmpujar},
	{"type", vTeclear},
	{"write", vEscribir},
	{"point", vPuntos},
	{"score", vPuntos},
	{"turns", vTurnos},
	{"use", vUsar},
	{"activ", vUsar},
    {"",0}
};
#endif 

// Tabla de adjetivos
#ifdef SPANISH
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
#endif 

#ifdef ENGLISH
token_t adjetivos_t [] =
{
	{"small", aPequeno},
	{"big", aGrande},
	{"old", aViejo},
	{"new", aNuevo},
	{"hard", aDuro},
	{"soft", aBlando},
	{"short", aCorto},
	{"long", aLargo},
	{"blue", aAzul},
	{"green", aVerde},
	{"black", aNegro},
	{"red", aRojo},
	{"yellow", aAmarillo},
	{"therm", aTermico},
	{"cold", aFrio},
	{"hot", aCaliente},
	{"",0}
};
#endif


/// Tabla de objetos
// No existe la limitaci�n de PAWS donde el objeto 1 siemmpre es la fuente de luz 
// La luz en ngpaws se calcula en funci�n del atributo de los objetos presentes en la localidad, puestos y llevados.
// Localidades de sistema: LOCATION_WORN,LOCATION_CARRIED, LOCATION_NONCREATED, LOCATION_HERE, CARRIED, HERE, NONCREATED, WORN

// Atributos con OR: aLight, aWear, aContainer, aNPC, aConcealed, aEdible, aDrinkable, aEnterable, aFemale, aLockable, aLocked, aMale, aNeuter, aOpenable, aOpen, aPluralName, aTransparent, aScenary, aSupporter, aSwitchable, aOn, aStatic, aExamined, aTaken, aDropped, aVisited, aTalked, aWore, aEaten, aPropio, aDeterminado
#ifdef SPANISH
obj_t objetos_t[]=
{
    // ID, LOC, NOMBRE, NOMBREID, ADJID, PESO, ATRIBUTOS
    {oCaja, lZonaA2,"paquete azul",     nPaquete, aAzul,   1, aMale_hex | aDeterminado_hex},  
    {oTraje, lEsclusa,"traje presurizado",     nTraje, EMPTY_WORD,   1, aWear_hex| aMale_hex | aDeterminado_hex},  
	{oEsclusa, lEsclusa,"compuerta de la esclusa", nEsclusa, EMPTY_WORD,   1, aStatic_hex | aFemale_hex | aDeterminado_hex},  
	{oPuerta, lEntrada,"puerta de metal",     nPuerta, EMPTY_WORD,   1, aStatic_hex | aFemale_hex},  
	{obotonrojo, lEsclusa,"bot�n rojo",     nBoton, aRojo,   1, aStatic_hex | aConcealed_hex | aMale_hex},  
	{obotonverde, lEsclusa,"bot�n verde",     nBoton, aVerde,   1, aStatic_hex | aConcealed_hex | aMale_hex},  
	{oCanon, NONCREATED,"ca��n de vigilancia",     nCanon, EMPTY_WORD,   1, aStatic_hex | aMale_hex},  
	{oTeclado, NONCREATED,"teclado",     nTeclado, EMPTY_WORD,   1, aStatic_hex | aMale_hex},  
    {0,0,"",                EMPTY_WORD,EMPTY_WORD,            0,0x00000000}
}; // Tabla de objetos de la aventura
#endif 

#ifdef ENGLISH 
obj_t objetos_t[]=
{
    // ID, LOC, NOMBRE, NOMBREID, ADJID, PESO, ATRIBUTOS
    {oCaja, lZonaA2,"blue packet",     nPaquete, aAzul,   1, aMale_hex | aDeterminado_hex},  
    {oTraje, lEsclusa,"presurized suit",     nTraje, EMPTY_WORD,   1, aWear_hex| aMale_hex | aDeterminado_hex},  
	{oEsclusa, lEsclusa,"airlock gate", nEsclusa, EMPTY_WORD,   1, aStatic_hex | aFemale_hex | aDeterminado_hex},  
	{oPuerta, lEntrada,"door metal",     nPuerta, EMPTY_WORD,   1, aStatic_hex | aFemale_hex},  
	{obotonrojo, lEsclusa,"red button",     nBoton, aRojo,   1, aStatic_hex | aConcealed_hex | aMale_hex},  
	{obotonverde, lEsclusa,"green button",     nBoton, aVerde,   1, aStatic_hex | aConcealed_hex | aMale_hex},  
	{oCanon, NONCREATED,"surveillance cannon",     nCanon, EMPTY_WORD,   1, aStatic_hex | aMale_hex},  
	{oTeclado, NONCREATED,"keypad",     nTeclado, EMPTY_WORD,   1, aStatic_hex | aMale_hex},  
    {0,0,"",                EMPTY_WORD,EMPTY_WORD,            0,0x00000000}
}; // Tabla de objetos de la aventura
#endif


// Para no usar las tablas de nombres y mensajes podemos a�adir una a medida
// para las conversaciones con PNJs. 

// Tabla de conversaci�n con el ordenador 
// El link entre la tabla de temas y los mensajes es el ID. 
#ifdef SPANISH
token_t ordenador_topics_t[] = 
{
	{"hola",1},
	{"adios",2},
	{"traba",3},
	{"yo",4},
	{"pilot",4},
	{"condu",4},
	{"trans",4},
	{"torme", 5},
	{"parab", 5},
	{"europ",7},
	{"luna",7},
	{"jovia",8},
	{"jupit",8},
	{"cara",9},
	{"lado",9},
	{"donde", 10},
	{"misio", 11},
	{"nombr",12},
	{"model",13},
	{"mega",14},
	{"megac",14},
	{"corpo",14},
	{"ciber",14},
	{"cyber",14},
	{"ac",17},
	{"avent",17},
	{"conve",17},
	{"if", 18},
	{"fi",18},
	{"ficci",18},
	{"inter",18},
	{"grafi",19},
	{"transi",20},
	{"isla", 21},
	{"uto", 22},
	{"csg",22},
	{"kmbr",23},
	{"kmbrkat",23},
	{"kno",24},
	{"dla",25},
	{"dela",25},
	{"franc",26},
	{"ingle",26},
	{"itali",26},
	{"alema",26},
	{"caste",26},
	{"tecno",27},
	{"comic",28},
	{"daniel",29},
	{"danixi", 29},
	{"obra",30},
	{"favor",31},
	{"jugar",32},
	{"canci", 33},
	{"oir", 33},
	{"tarar", 33},
	{"canta",33},
	{"guille",34},
	{"inter",35},
	{"viaja",36},
	{"entra",37},
	{"codig",38},
	{"centr",39},
	{"zeur", 40},
	{"zhl", 40},
	{"tierr",41},
	{"almac",42},
	{"tempe",43},
	{"conso",44},
	{"memoria", 45},
	{"relax", 46},
	{"jorna", 46},
	{"vacac", 46},
	{"marte", 47},
	{"venus", 48},
	{"satur", 48},
	{"pluto", 48},
	{"neptu", 48},
	{"mercu", 48},
	{"minas", 50},
	{"tened", 51},
	{"diabl", 51},
	{"hierr", 52},
	{"nique", 52},
	{"sindi", 53},
	{"human", 54},
	{"opera", 55},
	{"ocio", 56},
	{"paque", 57},
	{"sumin", 58},
	{"paquet", 59},
	{"recog", 60},
	{"nave", 61},
	{"estre", 62},
	{"sol", 63},
	{"solar", 64},
	{"sistm", 65},
	{"ayuda", 66},
	{"mensa", 67},
	{"coman", 67},
	{"encar", 67},
	{"entre", 67},
	{"clave", 70},
	{"contr", 70},
	{"passw", 70},
	{"codig", 70},
	{"radia", 71},
	{"esclu", 72},
	{"boton", 72},
	{"airlo", 72},
	{"comun", 68},
	{"eva", 73},
	{"traje", 74},
	{"super", 75},
	{"prote", 76},
	{"sigla", 77},
	{"mierd", 78},
	{"culo", 78},
	{"joder", 78},
	{"puta", 78},
	{"puton", 78},
	{"calla", 79},
	{"bodeg", 80},
	{"enfri", 81},
	{"frigo", 81},
	{"conge", 81},
	{"",0}
};
#endif 

#ifdef ENGLISH
token_t ordenador_topics_t[] = 
{
	{"hi",1},
	{"bye",2},
	{"job",3},
	{"me",4},
	{"I",4},
	{"pilot",4},
	{"drive",4},
	{"trans",4},
	{"storm", 5},
	{"winds", 5},
	{"europ",7},
	{"moon",7},
	{"moons",7},	
	{"jovia",8},
	{"jupit",8},
	{"face",9},
	{"side",9},
	{"where", 10},
	{"missi", 11},
	{"name",12},
	{"model",13},
	{"mega",14},
	{"megac",14},
	{"corpo",14},
	{"ciber",14},
	{"cyber",14},
	{"ac",17},
	{"avent",17},
	{"conve",17},
	{"if", 18},
	{"fi",18},
	{"ficci",18},
	{"inter",18},
	{"graph",19},
	{"transi",20},
	{"isla", 21},
	{"uto", 22},
	{"csg",22},
	{"kmbr",23},
	{"kmbrkat",23},
	{"kno",24},
	{"dla",25},
	{"dela",25},
	{"frenc",26},
	{"engli",26},
	{"itali",26},
	{"germa",26},
	{"spani",26},
	{"tecno",27},
	{"comic",28},
	{"daniel",29},
	{"danixi", 29},
	{"game",30},
	{"please",31},
	{"play",32},
	{"song", 33},
	{"listen", 33},
	{"hummi", 33},
	{"sing",33},
	{"guille",34},
	{"inter",35},
	{"trave",36},
	{"enter",37},
	{"code",38},
	{"centr",39},
	{"headq", 39},
	{"hq",39},
	{"zeur", 40},
	{"zhl", 40},
	{"earth",41},
	{"wareh",42},
	{"storm",43},
	{"conso",44},
	{"remem", 45},
	{"memory", 45},
	{"relax", 46},
	{"rest", 46},
	{"holyd", 46},
	{"mars", 47},
	{"venus", 48},
	{"satur", 48},
	{"pluto", 48},
	{"neptu", 48},
	{"mercu", 48},
	{"mines", 50},
	{"fork", 51},
	{"ddevil", 51},
	{"iron", 52},
	{"nickel", 52},
	{"sindi", 53},
	{"human", 54},
	{"opera", 55},
	{"leisu", 56},
	{"parcel", 57},
	{"sumin", 58},
	{"paquet", 59},
	{"pickup", 60},
	{"ship", 61},
	{"space",61},
	{"star", 62},
	{"sun", 63},
	{"solar", 64},
	{"systm", 65},
	{"help", 66},
	{"messa", 67},
	{"email", 67},
	{"item", 67},
	{"deliv", 67},
	{"order", 70},
	{"code", 70},
	{"pass", 70},
	{"passk", 70},
	{"key", 70},
	{"passw", 70},
	{"radia", 71},
	{"hatch", 72},
	{"button", 72},
	{"airlo", 72},
	{"commu", 68},
	{"eva", 73},
	{"suit", 74},
	{"super", 75},
	{"prote", 76},
	{"acron", 77},
	{"shit", 78},
	{"ass", 78},
	{"asssh", 78},
	{"bitch", 78},
	{"fuck", 78},
	{"shutup", 79},
	{"cargo", 80},
	{"froze", 81},
	{"fridge", 81},
	{"freez", 81},
	{"",0}
};
#endif 

// Topic and mensaje linked by ID 
// N:1 relationship
#ifdef SPANISH 
token_t mensajes_ordenador_t[]= 
{
	{"-Hola, soy el ordenador de navegaci�n -responde una voz met�lica.",1},
	{"-Hasta pronto.",2},
	{"-Soy el sistema de navegaci�n de la nave. ",3},
	{"-Eres Tod Connor, el piloto de la nave. ",4},
	{"-La tormenta es moderada. No deber�a ser un problema para acceder al almac�n usando el traje de protecci�n.",5},
	{"-Estamos en el sat�lite Joviano. -responde el ordenador.",7},
	{"-Europa es un sat�lite de Jupiter. En la cara iluminada por J�piter el espect�culo es soberbio. Por desgracia nuestro encargo es en la cara oculta. ",8},
	{"-Europa tiene una cara siempre orientada hacia J�piter.",9},
	{"-Est� en un almac�n, en el exterior. Tendr�s que salir con esta tormenta. ",10},
	{"-Tienes que recoger un paquete en Europa y entregarlo en Marte antes de 24h -responde.",11},
	{"-Mi nombre es DOT. Si lo prefieres, puedes usar este nombre al hablar conmigo.  ",12},
	{"-Soy un modelo Cyberdine 1997.  Mi programador original fue el Dr. Guillermo Han de la MegaCorp(tm). Me ense�� a cantar una canci�n, �quieres o�rla?",13},
	{"-La empresa responsable de que t� y yo estemos conversando en Europa.",14},
	{"-Me encantan las aventuras conversacionales.",17},
	{"-Desconozco ese t�rmino, �te refieres a las conversacionales?",18},
	{"-No est�n mal, pero no creo que puedas jugar a una mientras conduces.",19},
	{"-No est� mal, pero es un poco corta para mi gusto. Prefiero 'la noche m�s larga', que dura unas 12h de juego de tiempo real.",20},
	{"-Pero bueno, �si esa nunca la terminaron!. Recuerdo que uno de los autores, un tal UTO le hizo una visita al programador que termin� tan mal que destruyeron todo un bloque de oficinas. Pero esa es otra historia.",21},
	{"-Realmente no tengo m�s datos sobre esta persona. Sin duda era una mente maestra para escapar a mis registros.",22},
	{"-Un misterio, se rumorea que era un arenque rojo mutante. ",23},
	{"-Un celebrado autor de c�mic que vivi� m�s de doscientos a�os. Ten�a un callo en el dedo tan descomunal que finalmente muri� aplastado por �l. Es un cl�sico, deber�as leer toda su obra varias veces.",24},
	{"-Un escriba del siglo XV, gracias a �l tenemos disponibles maravillas como 'Cuando com� queso negro' en franc�s.",25},
	{"-Lenguas muertas, hoy en d�a todo el universo habla tecnollano.",26},
	{"-La lengua franca de nuestros d�as.",27},
	{"-Arte secuencial muy popular a finales del siglo XX. ",28},
	{"-El responsable de la mega corporation.",29},
	{ "-Hablemos de ello cuando termines tu misi�n.",30},
	{"-Es dif�cil elegir, las aventuras de texto se volvieron realmente populares a ra�z de la proliferaci�n de sistemas de navegaci�n en autom�viles, barcos y naves espaciales. ",31},
	{ "-Te buscar� una realmente interesante una vez termines la misi�n.",32},
	{"-Daisy... -Comienza a entonar sin demasiada fortuna.",33},
	{"-El ingeniero responsable de la interfaz humana de los sistemas de navegaci�n de la Megacorp(tm)",34},
	{"-Lo que usas para comunicarte conmigo.",35},
	{"-Primero hay que recoger el paquete, luego pondr� rumbo a Marte. -te recuerda el ordenador.",36},
	{"-La entrada est� hay fuera. Es posible que necesites alg�n c�digo de acceso para entrar.",37},
	{"-No tengo ning�n dato en la orden de la central acerca del c�digo.",38},
	{"-La central de ZHL est� en la Luna.",39},
	{"-Es la empresa de reparto de paquetes en 24h que nos paga el sueldo y las piezas para seguir recorriendo el Universo.",40},
	{"-La Tierra fue devastada despu�s de varios apocalipsis. De momento va tirando.",41 },
	{"-Hemos aterrizado cerca de la entrada. Debes salir al exterior, entrar en el almac�n y volver con el paquete para que puedas terminar la misi�n. ",42},
	{"-En torno a 150�C bajo cero en el exterior. Te recomiendo que lleves el traje de superviviencia.",43},
	{"-La consola de mando muestra el estado de la nave y los controles manuales. Actualmente est� desactivada para ahorrar energ�a. ",44},
	{"-Es normal que no recuerdes mucho al comienzo de una misi�n. Pero no te preocupes de eso ahora, despu�s de unas jornadas de relax todo volver� a la normalidad.",45},
	{ "-Despu�s de este trabajo podr�s pasar unos d�as de relax en Marte.",46},
	{"-Ya sabes, el planeta rojo. Bueno, anaranjado desde que comenz� la terraformaci�n.",47},
	{"-Es mejor no salirse del tema de la misi�n. -responde",48},
	{"-Despu�s de la recogida debemos entregar el paquete en el tenedor del diablo. Una de las mayores minas de Hierro y N�quel del Sistema Solar.",50},
	{"-Tambi�n se le conoce como el tenedor del diablo. Es una mina gobernada por el sindicato unificado de miner�a de Marte. ",51},
	{"-Es un metal abundante pero muy preciado. Lo dif�cil es su transporte, pues es muy denso y cuesta bastante dinero extraerlo de la atm�sfera en la Tierra. ",52},
	{"-Gobiernan con mano dura los precios de venta y los acuerdos de suministros. ",53},
	{"-Son tiempos dif�ciles. La humanidad tuvo que abandonar la Tierra debido a varias guerras nucleares y diseminarse por el sistema solar. ",54},
	{"-Ya sabes suministros, paqueter�a, ocio...",55},
	{"-Aqu� nada de ocio, hay que terminar el trabajo.",56},
	{"-Nuestro trabajo en ZHL es entregar los paquetes en 24h.",57},
	{"-Se han abierto varias minas en Marte.",58},
	{"-Seg�n la descripci�n del mensaje se trata de un paquete de 27cm x 29cm x 30cm que pesa 10Kg. ",59},
	{"-Hemos aterrizado en la entrada del almac�n. Sugiero que salgas ah� fuera, llames a la puerta y recojas el paquete.",60},
	{"-Volamos en una nave de reparto de tipo Tesla. Es completamente el�ctrica salvo el motor principal.",61},
	{"-La m�s cercana es Sol.",62},
	{"-Es la estrella principal de este sistema. ",63},
	{ "-Es el sistema al que pertenece la Tierra. ",64},
	{"-Todos los sistemas en orden. Operamos en modo de bajo consumo.",65},
	{"-Para eso estoy aqu�, para ayudarte. Algunos comandos que puedes usar son: misi�n, J�piter, nombre, modelo, mensaje...",66},
	{"Tema: Env�o urgente al Tenedor de Marte. Contenido: Recogida en almac�n en coordenadas en Europa. Usar clave: 32768. Importante: Mantener a temperatura bajo cero. ",67},
	{ "-Quiz� haya algo en el mensaje del encargo.",70},
	{"-No es un problema con el traje de supervivencia. Sin �l la radiaci�n es tan elevada que no vivir�as m�s de un d�a. ",71},
	{"-La esclusa se opera manualmente. El bot�n verde cierra la esclusa y el rojo la abre.  Aseg�rate de llevar el traje de supervivencia puesto. ",72},
	{"-Debido a la fuerte radiaci�n nuestras comunicaciones s�lo funcionan en la nave. No estar� contigo ah� fuera. ",68},
	{ "-Son las siglas de actividad extra-vehicular. Es cuando sales en misi�n fuera de la nave. ",73},
	{"-Lo encontrar�s en la esclusa listo para su uso. Recuerda no salir de la nave sin llevarlo puesto.",74},
	{ "-Esa acepci�n no forma parte de mi base de datos. -responde el ordenador con elegancia. ",78},
	{"-Ya sabes, para acortar palabras demasiado largas. ",77},
	{"-Sin problemas, ya no tarareo mas. -responde un poco dolido.",79},
	{"-Donde almacenamos los paquetes para su distribuci�n.",80},
	{ "-Tenemos un frigor�fico dedicado a este tipo de paquetes. -responde despu�s de un breve silencio",81},
	{0,0}
};
#endif 

#ifdef ENGLISH
token_t mensajes_ordenador_t[]= 
{
	{"'Hello, this is the navigation computer,' replies a metallic voice.",1},
	{"'See you soon'",2},
	{"'I am the ship's navigation system' ",3},
	{"'You are Tod Connor, the spacecraft's driver' ",4},
	{"'The storm is moderate. It shouldn't be a problem to access the warehouse wearing the protective suit'",5},
	{"'We are on the Jovian satellite,' replies the computer.",7},
	{"'Europe is a satellite of Jupiter. On the face illuminated by Jupiter the spectacle is great. Unfortunately our assignment is on the hidden side' ",8},
	{"'Europe has a side always oriented towards Jupiter'",9},
	{"'It's in a warehouse, outside. You'll have to go out in this storm' ",10},
	{"'You have to pick up a package on Europe and deliver it to Mars within 24h,' it replies.",11},
	{"'My name is DOT. If you prefer, you can use this name when talking to me'  ",12},
	{"'I am a 1997 Cyberdine model.  My original programmer was Dr. Guillermo Han of MegaCorp(tm). He taught me to sing a song, do you want to hear it?'",13},
	{"'The company responsible for you and me having this conversation in Europe'",14},
	{"'I love commandline adventures'",17},
	{"'I don't know that term, you mean the command line ones?'",18},
	{"'Not bad, but I don't think you can play one while driving'",19}, 
	{"'Not bad, but it's a little short for my taste. I prefer 'the longest night', which lasts about 12h of real time gameplay'",20},
	{"'Well, they never finished that one. I remember that one of the authors, a certain UTO, paid a visit to the programmer that ended so badly that they destroyed a whole block of offices. But that's another story'",21},
	{"'I really don't have any more data on this person. No doubt he was a mastermind to escape my records'",22},
	{"'A mystery, rumor has it he was a mutant red herring' ",23},
	{"'A celebrated comic author who lived to be over two hundred years old. He had a callus on his finger so gigantic that he was eventually crushed to death by it. He is a cult classic, you should read all his work several times'",24},
	{"'A fifteenth century scribe, thanks to him we have such wonders as 'When I ate black cheese' available in French'",25},
	{"'Dead languages, nowadays the whole universe speaks Technolingual.",26}, 
	{"'The lingua franca of our days'",27},
	{"'Sequential art, very popular at the end of the 20th century'",28},
	{"'The man responsible for the mega corporation'",29},
	{"'Let's talk about it when you finish your mission'",30},
	{"'It's hard to choose, voice adventures became really popular in the wake of the proliferation of navigation systems in cars, ships and spacecrafts' ",31},
	{"'I'll find you a really interesting one once you finish the mission'",32},
	{"'Daisy...,' the computer starts singing without much fortune.",33},
	{"'The engineer responsible for the human interface of the Megacorp(tm)'s navigation systems'",34}, 
	{"'What you use to communicate with me'",35},
	{"'First we have to pick up the package, then I'll set course for Mars, ' the computer reminds you.",36},
	{"'The entrance is outside. You may need some access code to get in'",37},
	{"'I don't have any data in the order from the central station about the code'",38},
	{"'ZHL's headquarters is on Earth's Moon'",39},
	{"'It's the 24h package delivery company that pays our salary and the parts to continue touring the Solar system'",40},
	{"'Earth was devastated after several apocalypses. Now is doing just fine.'",41 },
	{"'We have landed near the entrance. You must go outside, enter the warehouse and return with the package so you can finish the mission' ",42},
	{"'About 150�C below zero outside. I recommend you wear the survival suit'",43},
	{"'The command console displays ship status and manual controls. It is currently disabled to save power'",44},
	{"'It's normal that you don't remember much at the beginning of a mission. But don't worry about that now, after a few days of relaxation everything will be back to normal'",45}, 
	{"'After this work you will be able to spend a few days relaxing on Mars'",46},
	{"'You know, the red planet. Well, orange since terraforming began'",47},
	{"'It's better not to get off the subject of the mission. '",48},
	{"'After the pickup we must deliver the package to the devil's fork. One of the largest Iron and Nickel mines in the Solar System'",50},
	{"'It is also known as the devil's fork. It is a mine governed by the unified mining syndicate of Mars' ",51},
	{"'It is an abundant but very precious metal. What is difficult is its transportation, for it is very dense and costs quite a lot of money to extract it from Earth' ",52},
	{"'They govern with iron fist the selling prices and the supply agreements' ",53},
	{"'These are hard times. Mankind had to leave the Earth due to polution and spread throughout the solar system' ",54},
	{"'You know supplies, parcels, leisure...'",55},
	{"'No fun is allowed, we have to finish the job'",56},
	{"'Our job at ZHL is to deliver parcels within 24h'",57},
	{"'Several mines have been excavated on Mars'",58},
	{"'According to the message description it is a 27cm x 29cm x 30cm package weighing 10Kg'",59},
	{"'We have landed at the warehouse entrance. I suggest you go out there, open the door and pick up the package'",60},
	{"'We're flying in a Tesla-type delivery ship. It's all electric except for the main engine'",61},
	{"'The nearest one is the Sun'.",62},
	{"'It is the main star of this system' ",63},
	{"'It is the system to which the Earth belongs' ",64},
	{"'All systems in order. We operate in low power mode'",65},
	{"'That's why I'm here, to help you. Some commands you can use are: mission, Jupiter, name, model, message...'",66},
	{"'Subject: Urgent shipment to Mars Fork. Content: Pick up in warehouse at coordinates in Europe. Use key: 32768. Important: Keep at subzero temperature.' ",67},
	{"'Maybe there's something in the order message'",70},
	{"'It's not a problem with the survival suit. Without it the radiation is so high you wouldn't live more than a day' ",71},
	{"'The airlock is operated manually. The green button closes the airlock and the red button opens it.  Make sure you have your survival suit on' ",72},
	{"'Due to the strong radiation our communications only work on the ship. I won't be with you out there' ",68},
	{"'That stands for extra-vehicular activity. It's when you go on a mission outside the ship' ",73},
	{"'You'll find it in the airlock ready for use. Remember not to leave the ship without wearing it'",74},
	{"'That meaning is not part of my database,' responds the computer gracefully. ",78},
	{"'You know, to shorten words that are too long' ",77},
	{"'No problem, I don't hum anymore,' responds a little hurt.",79},
	{"'Where we store the packages for distribution'",80},
	{"'We have a refrigerator dedicated to this type of packages,' answers after a brief silence.",81},
	{0,0}
};
#endif 


// ----------------------------------------------------------------
// Tabla de respuestas
// ----------------------------------------------------------------
// Las tablas de respuestas se pueden agrupar en distintos c�digos fuente de 16KB, que luego pueden ser alojados en p�ginas independientes de 16Kb. Este m�todo es compatible
// con todos los ordenadores de 8bit y con PC.
char respuestas()
{
 BYTE aux;
 //setRAMPage(0);
 //if (respuestas_pagina0()==FALSE)
  //  {
   // setRAMPage(1);
   // if (respuestas_pagina1()==TRUE) return TRUE;
   // }
   // else return TRUE;

//; S�lo podemos hablar con el ordenador en la nave
//; ORDENADOR, palabra_clave

// ordenador encender consola -> encender ordenador consola -> encender consola
if (fnombre1==nOrdenador) {
	// Llamar al procesado de la tabla por tema...
	// ordenador encender consola -> encender ordenador consola -> encender consola
	// ordenador palabra_clave
	
	// Fuera del alcance...
	if (CNDatgt (lBodega)) { ACCmessage (177); DONE; }
	
	// Comandos al ordenador
	if (fverbo==vAbrir && fnombre2==nEsclusa) { ACCmessage (24); DONE;}
	if (fverbo==vCerrar && fnombre2==nEsclusa) { ACCmessage (25); DONE;}
	
	if (fverbo==vApagar) { ACCmessage (193); DONE; }
	if (fverbo==vEncender && (fnombre1==nPantalla || fnombre2==nConsola)) { ACCmessage (9); DONE; }
	// Preguntas al ordenador
	// Parsea el input del jugador...
	// From the beginning...
    gChar_number = 0;
    gWord_number = 0;
	while (ACCNextWord())
	{
		//writeText (playerWord);
		// S�lo admite una palabra clave
		if (buscador (ordenador_topics_t, playerWord, &flags[fTemp])!=EMPTY_WORD)
		{
			// El ID del topic es el mismo que el mensaje
			ACCwriteln (mensajes_ordenador_t[get_table_pos(mensajes_ordenador_t, flags[fTemp])].word);
			//ACCmessage (flags[fTemp])
			DONE;
		}			
	}
	//if (aux=buscador_tema(ordenador_t, nombres_t[fnombre2]))
	//{
	//	writeText (ordenador_t[aux].respuesta);
	//	DONE;
	//}	

	// Si no encaja con ning�n tema...
	ACCmessage (50);
	DONE;
	}

//-------------------------------------------------------------
// Cosas que se pueden hacer con los objetos...

if (fverbo== vExaminar) 
	{
		if (fnombre1== nContenedor  && CNDpresent (oCaja)) 
		{
			ACCmessage (46);
			DONE;
		}
		if (fnombre1==nIndicador && CNDpresent(oCaja)) 
		{
			ACCmessage (49);
			DONE;
		}
		if (fnombre1==nTraje && CNDpresent(oTraje)) { ACCmessage (16); DONE; }
	}

if (fverbo==vPoner && fnombre1==nTraje && CNDpresent(oTraje))
	{
		if (CNDnotcarr(oTraje) && CNDnotworn(oTraje))
		{
			ACCmessage (192);
			ACCget (oTraje);
		}
	}

// Quitar el traje...
if (fverbo==vQuitar && fnombre1==nTraje && CNDworn(oTraje))
{
	if (CNDatgt(lBodega)) { ACCmessage(19); DONE; }
}

if (fverbo==vAbrir && fnombre1== nContenedor) 
	{
		ACCmessage(48);
		DONE;
	}

// ---------------------------------------------------------------
// Descripciones com�nes para la nave
if (fverbo==vExaminar)  {
	if (fnombre1==nNave)	
	{
		if (CNDatlt (lExterior)) { ACCmessage (14); return TRUE; }
		if (CNDatlt (lAlmacen)) { ACCmessage(36); return TRUE;}
		// Si estamos dentro del almac�n no vemos la nave...
	}

	if (fnombre1==nPared || fnombre1==nSuelo) 
	{
		if (CNDatlt(lExterior)) { ACCmessage (33); DONE; }
			else { ACCmessage (179); DONE; }
	}

	if (fnombre1==nTecho || fnombre1==nCielo) {
		if (CNDatlt(lExterior)) { ACCmessage (33); DONE;}
			else { ACCmessage(178); DONE; }
	}

	if (fnombre1==nEuropa) { ACCmessage (179); DONE; }
	if (fnombre1==nTormenta) { ACCmessage (11); DONE; }
}

if (fverbo==vSaltar) { ACCmessage(56); DONE; }
if (fverbo==vEscuchar) 
{
	if (CNDatlt (lExterior)) { ACCmessage (57); DONE; }
	ACCmessage(56);
	DONE;
}

if (fverbo==vCantar) 
{
	if (CNDatlt(lExterior)) { ACCmessage (175); DONE;}
		else { ACCmessage (176); DONE; }
}

// ---------------------------------------------------------------
// Cosas que hacer en las localidades...
// --------------------------------------------------
// Puente de mando
// --------------------------------------------------

if (flocalidad==lPuente)
	{
		if (fverbo==vTeclear) { ACCmessage (181); DONE; }
		// Atrezzo 
		if (fverbo==vExaminar) 
		{
			if (fnombre1==nSistema && fadjetivo1==aTermico ) 
			{
				ACCmessage(15); DONE;
			}

			if (fnombre1==nParabrisas)
			{
				ACCmessage (12); DONE;
			}

			if (fnombre1==nCristales) { ACCmessage (13); DONE; }
			if (fnombre1==nEscaleras) { ACCmessage (4); DONE; }
			if (fnombre1==nPantalla) { ACCmessage(18); DONE; }
			if (fnombre1==nInterior || fnombre1==nConsola || fnombre1==nControles) 
			{
				ACCmessage (8); DONE;
			}	
			if (fnombre1==nLuz) { ACCmessage (10); DONE; }		
		}

		if (fverbo==vEncender && (fnombre1==nPantalla || fnombre1==nConsola)) { ACCmessage (9); DONE; }
		if (fverbo==vIr && fnombre1==nNodo) { ACCgoto(lNodo); DONE;	}
	}
// --------------------------------------------------
// Nodo central 
// --------------------------------------------------

if (flocalidad == lNodo) 	
	{
		if (fverbo==vExaminar) 
		{
			#ifdef SPANISH
			if (fnombre1==nBodega) { ACCwriteln ("Al sur."); DONE;}
			if (fnombre1==nEsclusa) { ACCwriteln ("Al oeste."); DONE;}
			#endif 

			#ifdef ENGLISH
			if (fnombre1==nBodega) { ACCwriteln ("At south."); DONE;}
			if (fnombre1==nEsclusa) { ACCwriteln ("At west."); DONE;}			
			#endif
			
			if (fnombre1==nEscaleras || fnombre1==nPuente) 
			{
				ACCmessage(5);
				DONE;
			}
			#ifdef SPANISH
			if (fnombre1==nLuz) { ACCwriteln ("Proviene del puente de mando."); DONE; }
			#endif 
			#ifdef ENGLISH
				if (fnombre1==nLuz) { ACCwriteln ("It comes from the bridge."); DONE; }
			#endif

			#ifdef SPANISH 
			//if (fnombre1==nNodo) { ACCwriteln ("El m�dulo de interconexi�n de la nave. "); DONE; }
			#endif
			#ifdef ENGLISH 
			if (fnombre1==nNodo) { ACCwriteln ("The central interconnection module of the spacecraft. "); DONE; }
			#endif
			
		}
		if (fverbo==vIr) 
		{			
			if (fnombre1==nPuente) 
				{
				ACCgoto (lPuente);
				DONE;
				}
			if (fnombre1==nEsclusa)
			{
				ACCgoto (lEsclusa);
				DONE;				
			}
			if (fnombre1==nBodega)
			{
				ACCgoto(lBodega);
				DONE;
			}
		}

	// Escena de casi-final...
	if (CNDcarried(oCaja) && flags[fCasifin]==0)
		{
			ACCmessage (58);
			flags[fCasifin]=1;
			ACCanykey();
			ACCmessage (59);
			DONE;
		}

	}

// --------------------------------------------------
// Esclusa 
// --------------------------------------------------
if (flocalidad == lEsclusa) 
	{
	// Descripciones 
	if (fverbo==vExaminar)
	{
		#ifdef SPANISH 
		if (fnombre1==nSombra) { writeText ("Resultado de la pobre iluminaci�n en la nave. Nada de lo que preocuparse.^ "); DONE; }
		#endif 

		#ifdef ENGLISH 
		if (fnombre1==nSombra) { writeText ("Due to poor lighting in the ship. Nothing to worry about.^ "); DONE; }
		#endif 

	}
	// Sin�nimos...
	if (fverbo==vIr && fnombre1==nExterior) { fverbo = vSalir; }
	if (fverbo==nOeste) { fverbo = vSalir; }
	if (fverbo==nEste) { fverbo = vIr; fnombre1=nNodo; }
	// Puzzle de la esclusa y ponerse el traje
	// No podemos quitarnos el traje con la esclusa abierta...
	if (fverbo==vQuitar && fnombre1==nTraje  && CNDcarried(oTraje))
		{
			if (CNDonotzero(oEsclusa, aOpen)) { ACCmessage (19); DONE; }
		}

	if (fverbo==vSalir)
		{
		// Salir, compuerta cerrada...
		if (CNDozero (oEsclusa, aOpen)) { ACCmes(24); ACCmessage(22); DONE; }
		// Salir, Pero no lleva el traje
		if (CNDnotworn (oTraje)) { ACCmessage (19); DONE;}
		// Salir con �xito 
		if (CNDonotzero(oEsclusa, aOpen) && CNDworn(oTraje)) { ACCgoto (lExterior); DONE; }
		}
	
	// Regresa al nodo pero lleva puesto el traje...
	if (fverbo==vIr && fnombre1==nNodo)
		{
			if (CNDworn(oTraje) || CNDcarried(oTraje)) { ACCmessage (20); DONE; }
				else { ACCgoto (lNodo); DONE; }
		}

	// Puzzle de abrir la compuerta
	if (fnombre1==nPuerta || fnombre1==nCompuerta) fnombre1=nEsclusa;
	if (fverbo==vAbrir && fnombre1==nEsclusa ) { ACCmessage(24); DONE; }
	if (fverbo==vCerrar && fnombre1==nEsclusa) { ACCmessage(25); DONE; }
	if (fverbo==vExaminar)
		{			
			if (fnombre1==nEsclusa || fnombre1==nCompu || fnombre1==nControles)
			{
				ACCmes(21);
				// Aparecen listados...
				ACCoclear (obotonrojo,aConcealed);
				ACCoclear (obotonverde,aConcealed);
				if (CNDonotzero(oEsclusa, aOpen)) { ACCmessage(23); DONE; }
					else { ACCmessage(22); DONE; }
			}
			if (fnombre1==nBoton) 
			{
				if (fadjetivo1==aVerde) { ACCmessage (26); DONE; }
				if (fadjetivo1==aRojo) { ACCmessage(27); DONE; }	
				ACCmessage(172); 
				DONE; 
			} 
		}
	// Usar los botones...

	if (fverbo==vPulsar)
	{
		if (fnombre1==nBoton)
		{
			// Cerrar esclusa 
			if (fadjetivo1==aRojo)
			{
				if (CNDozero(oEsclusa,aOpen)) { ACCmessage(32); DONE; }
				else { ACCoclear (oEsclusa, aOpen); ACCmessage(30); DONE; }
			}
			// Abrir esclusa...
			if (fadjetivo1==aVerde)
			{
				if (CNDonotzero(oEsclusa,aOpen)) { ACCmessage(28); DONE; }
				else { 
					// Pero no tiene puesto el traje...
					if (CNDnotworn(oTraje)) { ACCmessage(19); DONE; }
					// Abre la compuerta
					ACCoset (oEsclusa, aOpen); ACCmessage(29); DONE; }
			}
		}	
	}
		
	}

// --------------------------------------------------
// Bodega 
// --------------------------------------------------

if (flocalidad==lBodega)
	{
	// Fin del juego
	if (CNDcarried(oCaja))
		{
			ACCmessage (60);
			ACCanykey();
			ACCmessage (61);
			ACCanykey();
			ACCmessage (62);
			ACCanykey();
			ACCmessage (63);
			ACCend();
			DONE;
		}
	if (fverbo==vExaminar)
		{
		if (fnombre1==nPaquetes) { ACCmessage (173); DONE; }
		}

	if ( (fverbo==vCoger || fverbo==vEmpujar) && fnombre1==nPaquetes)
		{
			ACCmessage (174);
			DONE;
		}
	}
// --------------------------------------------------
// Localidad exterior 
// --------------------------------------------------
if (fverbo==vExaminar)
{
	if (flocalidad==lExterior || flocalidad==lAlmacen)
	{
		if (fnombre1==nCielo) { ACCmessage (7); DONE; }
		if (fnombre1==nJupiter) { ACCmessage (178); DONE; }
		if (fnombre1==nCristales) { ACCmessage (11); DONE; }	
	}
}


if (flocalidad==lExterior)
	{
		if (fverbo==vIr)
		{
			if (fnombre1==nNave) 
			{
				ACCgoto (lEsclusa);
				DONE;
			}	
			if (fnombre1==nAlmacen || fnombre1==nMole || fnombre1==nEdificio)
			{
				ACCgoto (lAlmacen);
				DONE;
			}
		}

		if (fverbo==vExaminar)
		{
			if (fnombre1==nNave) 
			{
				ACCmessage (36);
				DONE;	
			}
			if (fnombre1==nMole) 
			{
				ACCmessage (38);
				DONE;
			}
		}
	}

// --------------------------------------------------
// Entrada al almac�n
// --------------------------------------------------
if (flocalidad==lEntrada)
{		
	if (fverbo==vIr) 
	{
		if (fnombre1==nNave) { ACCgoto (lExterior); DONE; }	
	}
	
	
	if (fverbo==nEste) { fverbo=vIr; fnombre1=nNave; }
	if (fverbo==vIr && fnombre1==nNave) { ACCgoto(lExterior); DONE; }
	if (fverbo==nOeste || fnombre1==nOeste) { fverbo=vEntrar; }
	// Entrar al almac�n...
	if (fverbo==vEntrar) 
	{
		if (CNDozero(oPuerta, aOpen)) { ACCmessage(22); DONE; }
		ACCgoto(lZonaA1);
		DONE;
	}

	if (fverbo==vExaminar)
	{
		if (fnombre1==nEdificio) { ACCmessage(182); DONE; }	

		if (fnombre1==nPuerta) 
		{
			ACCmes(180);
			if (!CNDisat(oTeclado,lEntrada))
			{
				ACCmes (39);
				ACCplace (oTeclado,lEntrada);		
			}	
			if (CNDonotzero(oPuerta, aOpen)) { ACCmessage(23); DONE;}
				else { ACCmessage(22); DONE; }
			DONE;
		}

		if (fnombre1==nCanon && CNDpresent(oCanon)) { ACCmessage(41); DONE; }
		if (fnombre1==nTeclado && CNDpresent(oTeclado)) 
		{
			if (CNDabsent(oCanon) && CNDozero (oPuerta, aOpen)) 
			{
				ACCplace(oCanon, lEntrada);
				ACCmessage(40);
				DONE;
			}
			#ifdef SPANISH 
			if (CNDonotzero(oPuerta, aOpen)) writeText ("El teclado no responde. La puerta ya est� abierta.^");
			#endif
			#ifdef ENGLISH 
			if (CNDonotzero(oPuerta, aOpen)) writeText ("The keypad does not work. The warehouse's door is already open.^");
			#endif			
			 else ACCmessage(39);
			DONE;
		}
	}

	if ( (fverbo==vAbrir||fverbo==vCerrar) && fnombre1==nPuerta)
	{
		ACCmessage (195);
		if (!CNDisat(oTeclado,lEntrada))
		{
			ACCmes (39);
			ACCplace (oTeclado,lEntrada);			
		}	
		DONE;
	}
	
	
	if (fverbo==vUsar && fnombre1==nTeclado) 
		{ 
			#ifdef SPANISH
			ACCwriteln ("Debo teclear la clave correcta en el teclado."); 
			#endif 
			#ifdef ENGLISH
			ACCwriteln ("I have to type the correct code on the keyboard."); 
			#endif
			DONE; 
		}

	if (fverbo==vTeclear) fverbo=vEscribir;
	if (fverbo==vEscribir)
	{
		if (fnombre2==EMPTY_WORD && CNDpresent(oTeclado)) { fnombre2=nTeclado; ACCmes(183); }
		if (fnombre2==nTeclado && CNDpresent(oTeclado))
		{
			if (CNDabsent(oCanon) && CNDozero (oPuerta, aOpen)) 
			{
				ACCplace(oCanon, lEntrada);
				ACCmessage(40);
				DONE;
			}

			// Si ya est� abierta...
			if (CNDpresent(oTeclado) && CNDonotzero(oPuerta, aOpen))
			{
				ACCmessage(184);
				DONE;
			}
			if (fnombre1==n32768 && CNDpresent(oTeclado))
			{
				// No est� abierta...
				if (CNDozero(oPuerta, aOpen))
				{
					ACCmessage(186);
					ACCoset (oPuerta, aOpen);
					ACCplace (oCanon, LOCATION_NOTCREATED);
					ACCsetexit (lEntrada, nOeste, lZonaA1);
					ACCsetexit (lEntrada, nEntrar, lZonaA1);
					ACCanykey();
					ACCdesc();
					DONE;
				}	
			}

			if (CNDpresent(oTeclado) && CNDpresent(oCanon)) 
			{
				ACCmes (185);
				flags[fCanon]++;
				if (flags[fCanon]<3)
				{
					ACCmessage (186+flags[fCanon]);
					DONE;
				} 
				if (flags[fCanon]==3)
				{
					ACCmessage (189);
					ACCanykey();
					ACCmessage (190);
					flags[fCanon]=0;
					DONE;
				} 
				
				// Ha excedido el n�mero de intentos
				ACCmessage(41);			
				DONE;	
			}
		}
	}
}
// --------------------------------------------------
// Zona A1
// --------------------------------------------------

if (flocalidad==lZonaA1)
{
	if (fverbo==vExaminar) 
	{
		if (fnombre1==nEstanterias) { ACCmessage (43); DONE;}			
		if (fnombre1==nTecho || fnombre1==nSuelo || fnombre1==nParedes || fnombre1==nPasillo) 
		{
			ACCmessage (44); DONE;
		}
		if (fnombre1==nContenedores) 
		{
			ACCmessage (191);
			DONE;
		}
	}
}

// --------------------------------------------------
// Zona A2
// --------------------------------------------------
if (fverbo==vCoger) 
{
	if (fnombre1==nPaquete)
	{
		if (fadjetivo1==aAzul) 
		{
			ACCautog(); DONE; 
		} 
				
		if (flocalidad==lZonaA2 || flocalidad==lZonaA1) { ACCmessage (194);DONE;}
		
	}
}
if (flocalidad==lZonaA2)
{
	 
	if (fverbo==vExaminar)
	{
		if (fnombre1==nBoveda) 
		{
			ACCmessage (45);
			DONE;
		}	
		if (fnombre1==nPasillo) 
		{
			ACCmessage (44);
			DONE;
		}
		if (fnombre1==nEstanterias)
		{
			ACCmessage (44);
			DONE;
		}
		if (fnombre1==nContenedores)
		{
			ACCmessage (191);
			DONE;
		}
	}

}
 NOTDONE;

// ================= LIBRER�A BASE FINAL=======================================

}

// ----------------------------------------------------------
// Tabla de respuestas-post
// Se llama despu�s de ejecutar el proceso de respuestas
// ----------------------------------------------------------

char respuestas_post()
{
 //setRAMPage(0);
 // respuestas_post_pagina0();

// ------------------- LIBRER�A BASE -----------------------------------
 // Comandos de direcci�n...
 // writeText ("Respuestas Post: %u %u^", flags[fverb], flags[fnoun1]);
 // Movimiento entre localidades...
    BYTE i;
    BYTE loc_temp;

	if (fverbo==vFin) { ACCquit(); DONE; }

	if (fverbo==vQuitar) {  ACCautor(); DONE; }
	if (fverbo==vPoner) { ACCautow(); DONE; }
	
	// En este punto el examinar no ha sido cubierto por las respuestas
    if (fverbo==vExaminar)
    {
        if (findMatchingObject(get_loc_pos(loc_here()))!=EMPTY_OBJECT)
		#ifdef SPANISH
            writeText ("Deber�a coger eso antes.^");
		#endif 
		#ifdef ENGLISH 
		    writeText ("I have to take that first.^");
		#endif
        else
		#ifdef SPANISH
			writeText ("No veo eso por aqu�.^");
		#endif
		#ifdef ENGLISH 
			writeText ("I do not see that around here.^");
		#endif
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

    // Si es un nombre/verbo de conexi�n...
	
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
	
    // Comandos t�picos...
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
		#ifdef SPANISH
		ACCwriteln ("No necesitas cargar o salvar en este viaje -dice el ordenador");
		#endif 
		#ifdef ENGLISH 
		ACCwriteln ("'You don't need to load or save on this trip,' says the computer.");
		#endif 
		DONE;
	}

	if (fverbo==vPuntos) { ACCscore(); DONE; }
	if (fverbo==vTurnos) { ACCturns(); DONE; }
	if (fverbo==vEsperar) { writeSysMessage (35); DONE; }
	if (fverbo==vTocar || fverbo==vOler) { ACCmessage (56); DONE; }
	// Si ninguna acci�n es v�lida...
    ACCsysmess(SYSMESS_IDONTUNDERSTAND);
    newLine();
}

char proceso1() // Antes de la descripci�n de la localidad...
{
	
	// Muestra la pantalla..
	#ifdef ZX
		// Oculta el dibujado
		defineTextWindow (1,1,30,10); 
		clearTextWindow(INK_BLACK | PAPER_BLACK , FALSE);
		ACCpicture(flags[flocation]);
	#endif 

	if (CNDat(lZonaA2)) 
	{
		#ifdef ZX 
		clearTextWindow(INK_BLACK | PAPER_RED | BRIGHT, FALSE);
		defineTextWindow (15,4,3,3); 
		clearTextWindow(INK_BLACK | PAPER_RED | BRIGHT, TRUE);
		#endif 
	}
	#ifdef ZX
		defineTextWindow (0,11,32,14); 
		clearTextWindow(INK_WHITE | PAPER_BLUE, TRUE);
	#endif 

	// Imprime el nombre de la localidad 
	#ifdef DOS
		// En DOS y en modo TEXTO es m�s c�modo para el jugador mantener la pantalla de texto
		#ifdef TEXT
			//defineTextWindow (0,0,80,25);
			//clearTextWindow(INK_WHITE | PAPER_BLUE, TRUE);
			ACCink (INK_BRIGHT_WHITE);
			ACCwriteln (localidades_t[get_loc_pos (flocalidad)].name); 
			ACCink (INK_WHITE);
		#endif 
		#if defined CGA || defined EGA 
			// Gr�fico de localidad
			// Paleta 
			if (flocalidad<5)// Interior de la nave
			{
				ACCpalette (0);
			}
			// Exterior
			if (flocalidad==5 || flocalidad==5)
			{
				ACCpalette (1);
			}
			// Interior del almac�n 
			if (flocalidad==7)
			{
				ACCpalette (1);
			}
			if (flocalidad==8)
			{
				ACCpalette (0);
			}
			clearTextWindow(INK_CYAN | PAPER_BLACK, TRUE);
			ACCpicture (flocalidad);			
			defineTextWindow (0,11,40,14); 
			
		#endif 
	#endif 
	// C�lculo de luz
	// En ZHL todas las localidades tienen luz
	flags[fdark]=0; // No est� oscuro

}

char proceso1_post() // Despu�s de la descripci�n
{
	if (CNDat (lPuente) && !localidades_t[0].visited)
	#ifdef SPANISH
		ACCwriteln ("El ordenador de navegaci�n tararea una canci�n.");
	#endif
	#ifdef ENGLISH 
		ACCwriteln ("The navigation computer hums a tune.");
	#endif 

 //setRAMPage(0);
 // Usar proceso en otras p�ginas require compilar c�digo por separado
 //proceso1_post_pagina0();
}

char proceso2() // Despu�s de cada turno, haya tenido o no �xito la entrada en la tabla de respuestas
{
	NOTDONE;
}

// ------------------------------------------------------------
// Bucle principal, men� del juego
// ------------------------------------------------------------

void main (void)
{
	// Inicializa variables
	BYTE salir=0,n,i;

	#ifdef DOS
		#ifdef TEXT 
			TextMode ();
			clearScreen (INK_WHITE | PAPER_BLUE);
		#endif 

		#ifdef CGA
			HighResMode ();
			ACCpalette (1); // 1-> 0-Black, 1- Cyan, 2-Magenta 3 White
			clearScreen (INK_WHITE | PAPER_BLACK);
		#endif

		#ifdef EGA 
			HighResMode ();
			clearScreen (INK_WHITE | PAPER_BLACK);
		#endif 	
	#endif

	#ifdef ZX 
		clearScreen (INK_BLACK | PAPER_BLACK);
	#endif 

	InitParser ();                // Inicializa el parser y la pantalla
	flags[fobjects_carried_count] = 0;
	// Inicializa objetos
	
	ACCplace(oCaja, lZonaA2);
	ACCplace(oTraje, lEsclusa);
	
	ACCplace (oEsclusa, lEsclusa);
	ACCoclear (oEsclusa, aOpen);
	
	ACCplace (oPuerta, lEntrada);
	ACCoclear (oPuerta, aOpen);

	ACCplace (obotonrojo, lEsclusa);
	ACCoset (obotonrojo, aConcealed);
	ACCplace (obotonverde,lEsclusa);
	ACCplace (obotonverde, aConcealed);

	ACCplace (oCanon, NONCREATED);
	ACCplace (oTeclado, NONCREATED);

	#ifdef ZX 
 		defineTextWindow (0,0,40,25); // Full Text screen in the menu  
	#endif 

	
	#ifdef DOS 
		#ifdef TEXT 
			defineTextWindow (0,0,80,25); // Full Text screen 
			ACCbox (27,7,21,4,INK_BLUE|PAPER_BRIGHT_WHITE,"Ad Astra per Aspera");
			getch();

		#endif

		#if defined EGA || defined CGA 
			n = _registerfonts( "1.fon" );
			_setfont( "n0" );
		#endif 

		#ifdef EGA 
			defineTextWindow (0,0,40,25); // Graphics + Text 
			ACCbox (11,7,21,4,INK_WHITE|PAPER_BLACK,"Ad Astra per Aspera");
			getch();
		#endif 

		#ifdef CGA 
			defineTextWindow (0,0,40,25); // Graphics + Text 
			ACCbox (11,7,21,4,INK_WHITE|PAPER_BLACK,"Ad Astra per Aspera");
			getch();

		#endif 
	#endif 

	 // Men� de juego
     #ifdef ZX 
	 	clearTextWindow(INK_GREEN | PAPER_BLACK  | BRIGHT, TRUE);
	 #endif 

	 #ifdef DOS 
	 	#ifdef TEXT
	 		clearTextWindow(INK_WHITE | PAPER_BLUE , TRUE);		
	 	#endif

		#if defined CGA || defined EGA 
			clearTextWindow(INK_WHITE | PAPER_BLACK , TRUE);		
	 	#endif

	 #endif 
	
	#if defined ZX || defined CGA || defined EGA 
		ACCpicture(9); // Muestra el gr�fico superior del men� 
	#endif 

	#ifdef ZX 
	 	gotoxy (13,12);
    #endif 

	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (35,7);
		#endif 
		#if defined CGA || defined EGA 
			gotoxy (13,12);
		#endif 
	#endif

	 writeText (" Z H L ");
     
	 #ifdef ZX 
	 	gotoxy (12,14);
	 #endif 
	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (34,9);
		#endif 
		#if defined EGA || defined CGA
			gotoxy (12,14);
		#endif
	#endif

	 #ifdef SPANISH
    	 writeText ("1 Jugar");
	 #endif 
	 #ifdef ENGLISH
	 	writeText ("1 Start");
	 #endif
    
	 #ifdef ZX 
	 	gotoxy (12,15);
	 #endif 
	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (34,11);
		#endif 
		#if defined EGA || defined CGA
			gotoxy (12,15);
		#endif
	#endif

	 #ifdef SPANISH
     	writeText ("2 Instrucciones");
	 #endif 
	 #ifdef ENGLISH 
	 	writeText ("2 How to play");
	 #endif
     
	 #ifdef ZX 
	 	gotoxy (12,16);
	 #endif 

	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (34,13);
		#endif 
		#if defined EGA || defined CGA
			gotoxy (12,16);
		#endif
	#endif

	 #ifdef SPANISH
     writeText ("3 Cr�ditos");
	 #endif 
	 #ifdef ENGLISH 
	 writeText ("3 Credits");
	 #endif

	 #ifdef ZX 
	 	gotoxy (12,17);
	 #endif 

	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (34,15);
		#endif 
		#if defined EGA || defined CGA
			gotoxy (12,17);
		#endif
	#endif


	 #ifdef SPANISH
     writeText ("0 Salir");
	 #endif 
	 #ifdef ENGLISH 
	 writeText ("0 Exit");
	 #endif

	 #ifdef ZX 
     	gotoxy (9,20);
     #endif 

	#ifdef DOS 
		#ifdef TEXT 
			gotoxy (28,23);
		#endif 
		#if defined EGA || defined CGA
			gotoxy (9,20);
		#endif
	#endif

	writeText ("(C) 2019-2021,2023 KMBR ");
	
	while (!salir) 
	{
	switch (getKey())
		{
			//case 'j': // Cargar Partida
			//break;
			case '2': // Instrucciones 
				#ifdef ZX 
					clearTextWindow(INK_YELLOW | PAPER_BLACK, TRUE);
				#endif 
				#ifdef DOS 
					#ifdef TEXT 
						clearScreen(INK_WHITE | PAPER_BLUE);
					#endif 
					#if defined EGA || defined CGA 
						clearScreen(INK_WHITE | PAPER_BLACK);
					#endif 
	
				#endif 
				gotoxy(1,1);
				writeTextCenter ("Instrucciones");
				writeTextCenter ("=================");
				writeText (" Tienes ante ti un juego de texto basado en comandos, o tradicionalmente conocido como aventura conversacional. ");
				writeTextln ("Estos juegos ofrecen una gran libertad al jugador en la exploraci�n del mundo pues no se basan en un n�mero reducido de iconos o verbos en pantalla sino en las �rdenes que teclees.");
				ACCanykey();

				writeTextln ("^ Las �rdenes se basan en un formato sencillo de verbo+nombre. A continuaci�n se listan algunos de los comandos m�s com�nes y sus abreviaturas:");
				writeTextln ("^  Desplazamiento: Norte (N), Sur (S), Este (E), Oeste (O), Arriba, Abajo, Entrar, Salir.");
				writeTextln ("^  De interacci�n con el entorno: Examinar (X), Empujar, Tirar, Sacar _ en _, Meter _ en _, Dejar, Coger, Dar _ a _, Saltar, Escuchar, Inventario (i). ");
				writeTextln ("^  De juego: Mirar(m), Salvar partida (save), Cargar  partida (load), Fin.");

				writeTextln ("^ La interacci�n con el mundo del juego no se limita a estos comandos, tienes que experimentar con nuevas ideas."); 

				ACCanykey();

				writeText ("^ En esta aventura eres un repartidor profesional. ZHL es una empresa multiplanetaria que trabaja a lo largo y ancho del sistema Solar. ");

				writeTextln ("Cada d�a mueve cientos de paquetes y mercanc�as entre Marte, la Luna y la Tierra. ^");

			 	writeTextln (" El juego comienza en Europa, la luna helada de J�piter. En las proximidades de un almac�n central de ZHL donde debes acudir para recoger un paquete. ");

				writeTextln ("^ La nave de ZHL incorpora un ordenador de navegaci�n con el que puedes entablar conversaci�n usando: ordenador nombre, ordenador ayuda, etc. El ordenador, que responde por DOT, te informar� acerca de los detalles de tu misi�n y te dar� el apoyo necesario. DOT guarda tambi�n gran cantidad de informaci�n acerca del mundo actual y su historia, no dudes en indagar con tus preguntas. ");

				writeTextln ("^ Puedes desplazarte por el mapa con los comandos b�sicos: Norte, Sur, Este, Oeste, etc. Recuerda examinar todo lo que te llame la atenci�n usando el verbo Examinar o x. "); 

				writeTextln ("^ Mucha suerte con la aventura, disfr�tala. "); 

				writeTextCenter ("(C) 2019-2021, 2023 Gui�n, programaci�n e ilustraciones por KMBR");
				ACCanykey();

				main();
				break;
			case '0': // Exit 
				salir = 1;
				#ifdef DOS 
					#if defined CGA || defined EGA
						_unregisterfonts ();
					#endif 
					// Return to Shell 
					_setvideomode( _DEFAULTMODE );
					printf ("ZHL (C) 2019-2021, 2023 Created by KMBR");
				#endif 

				// To the void...
				#ifdef ZX
					#asm 
						jp 0
					#endasm 
				#endif 
				exit(0);
				break;
			case '3': // Cr�ditos
				#ifdef ZX 
					clearTextWindow(INK_YELLOW | PAPER_BLACK, TRUE);
				#endif 
				#ifdef DOS 
					#ifdef TEXT 
						clearScreen(INK_WHITE | PAPER_BLUE);
					#endif 
					#if defined EGA || defined CGA 
						clearScreen(INK_WHITE | PAPER_BLACK);
					#endif 
				#endif 
				gotoxy(0,13);
				writeTextln ("Z H L");
				#ifdef SPANISH
				writeTextln ("Creada por KMBR");
				writeTextln ("Release 5 ^");
				writeTextln ("Agradecimientos a aa@zdk.org y Carlos S�nchez (UTO)");
				writeTextln ("Compresi�n de gr�ficos con ZX7 por Einar Saukas");
				#endif 
				#ifdef ENGLISH
				writeTextln ("Written by KMBR");
				writeTextln ("Release 5 ^");
				writeTextln ("Acknowledgments to aa@zdk.org and Carlos Sanchez (UTO)");
				writeTextln ("ZX7 graphic compression by Einar Saukas");
				#endif
				writeTextln ("Reynolds font by DamienG");
				waitForAnyKey();
				main();
				break;
			//case 't': // Jugar en modo Tutorial...
			//     flags[fTutorial]=1;
			case '1': // Jugar...
				#ifdef ZX 
			 		defineTextWindow (0,11,32,14); // Pantalla reducida en 128Kb, Gr�ficos + Texto
					clearScreen(INK_YELLOW | PAPER_BLACK);
				#endif

				#ifdef DOS 
					#ifdef TEXT 
						clearScreen(INK_WHITE | PAPER_BLUE);
					#endif 
					#if defined EGA || defined CGA 
				 		defineTextWindow (0,11,40,14); // Pantalla reducida en 128Kb, Gr�ficos + Texto
						clearScreen(INK_WHITE | PAPER_BLACK);
					#endif 
				#endif 
								
				flags[LOCATION_MAX] = 8; // N�mero m�s alto de localidad
				ACCability(10,20); // 10 objetos, 20 piedras
				ACCgoto(lPuente); // Localidad inicial, en el puente de mando
				ParserLoop (); // Pone en marcha el bucle principal
				// Parser gets out of the parserloop when the player ENDS the game 	
				// The player wants to abandon the game
				main ();				
		}
	}

	
}

// ------------------------------------------------------------
// Funciones propias del juego
// ------------------------------------------------------------
