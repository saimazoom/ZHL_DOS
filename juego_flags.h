/*
ISO-8859-15

 ZMiniIF para ordenadores de 8bit
 Basado en PAWS y NGPAWS-Beta 9 (Uto/Carlos Sanchez) http://www.ngpaws.com
 (c) 2016. Written by KMBR.
 v1.0

*/

// Game flags available 64 to 255

// #define f.... xx
#define fherramientas 60 		// Se incrementa al buscar herrramientas
#define fCobraAqui	  61 		// Controla si la cobra est� en la localidad. 1=S�, 0=No
#define fvasija_rota  62 		// 0=Entera, 1=Rota
#define fcontadorMomia 63 		// Contador de turnos de la momia. Inicialmente=0
#define fCofreAbierto 64 		// 0=Cerrado, 1=Abierto
#define	fCuerdaColgada 65 		// 0=No colgada, 1=Colgada
#define fSepulcroDestapado 66 	// 0=Tapado, 1=Destapado
#define fSepulcroConAnillo 67 	// 0=Sin Anillo, 1=Con Anillo;
#define fAntorchaConAceite 68 	// 0=Sin Aceite, 1=Con Aceite
#define fAntorchaEncendida 69 	// 0=Apagada, 1=Encendida
#define fLamparaEncendida 70 	// 0=Apagada, 1=Encendida
#define fLinternaEncendida 71 	// 0=Apagada, 1=Encendida
#define fLinternaVacia	72 		// 0=Llena, 1=Vac�a
#define fBarraDesencajada 73 	// 0=Encajada, 1=Desencajada
#define fTurnosLinterna	  74
#define fLocRoca		  75    // Inicialmente l_LargoCorredor
#define fCuerdaAtada	76      // 0: No Atada, 1:Atada
#define fEspadaNoClavada	77		// 0: Clavada, 1: No Clavada
#define fCerrojoEstaAbierto	78	// 0: Cerrado, 1: Abierto
#define fTurnosOscuridad    79  // Como m�ximo 3 turnos en la oscuridad...
#define fTutorial   80          // 0: Juego Normal, 1: Tutorial
#define fLuminaria  81          // 0: Sin Luminaria, 1: Antorcha encendida en Luminaria
#define fPrevLocation 82        // La usamos para el puzzle de la pasarela, contiene la localidad anterior
#define fOfrenda    83          // 0: Sin ofrenda, 1: Ofrenda realizada
#define fTemp 84
#define fTemp2 85
#define fcontadorTrampa 86
#define ftrampabloqueada 87     // 0: Sin Bloquear, 1: Trampa bloqueada
#define fcuerdaLosa     88      // 0: Sin atar, 1: Atada
#define fcuerdaPolea    89      // 0: Sin atar, 1: Atada


// Game locations 1 to 250 are available
#define l_AccesoALaTrampa 	1
#define l_AltarDeOfrendas 	2
#define l_Antesala			3
#define l_Desierto			4
#define l_DiminutaSala		5
#define l_Entrada			6
#define l_Escaleras			7
#define l_Estancia			8
#define l_EstrechoPasadizo	9
#define l_FinalDelCorredor	10
#define l_GranPasaje		11
#define l_GranSala			12
#define l_LargoCorredor		13
#define l_PasadizoBajo		14
#define l_PasajeDeLosGrabados	15
#define l_Pasarela			16
#define l_PasilloProfundo	17
#define l_PozoDeLasAlmas	18
#define l_Rampa				19
#define l_SalaAlargada		20
#define l_SalaDeLaLuminaria	21
#define l_SalaDeLaMomia		22
#define l_SalaDeLaVidaEterna	23
#define l_SalaDeLosVivos	24
#define l_SalaDelAltarDePiedra	25
#define l_SalaDelCofre		26
#define l_SalaDelDiosRey	27
#define l_SalaDelEspacio	28
#define l_SalaDelSepulcro	29
#define l_SalaFuneraria		30
#define l_Trampa			31
#define l_AnexoAutores		33
#define l_eyes              34
#define l_lord              35

// For containers and supporters the location ID number shall match the object ID number. 
#define l_mesa 34	// La mesa no es un algo que lleve el jugador pero es un objeto de tipo contenedor.

// Definitions for objects, 1 to 255 available
//; Objetos
#define o_linterna 1
#define o_aceite 2
#define o_barra 3
#define o_copa 4
#define o_cuerda 5
#define o_espada 6
#define o_antorcha 7
#define o_pedernal 8
#define o_sandalia 9
#define o_tabla 10
#define o_roca 11
#define o_carta 12
#define o_brujula 13
#define o_trigo   14
#define o_maiz 15
#define o_llave    16
#define o_anillo	17
#define o_mesa      34

// -------------------------------------------------------------------------------------------
// ----------------------------------- VOCABULARIO ------------------------------------------- 
// -------------------------------------------------------------------------------------------
// Definiciones de vocabulario
// Nombres de 1 a 20 reservados
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
#define nAfuera 12
#define nInventario 14
#define nPuntos 15
#define nTurnos 16
#define nTodo 20
 

// Nombres < 20 se pueden usar como verbos
// Nombres por debajo de 50 como nombre propio, no admite lo,las...
// ------------------------------------------------
// Nombres de 21 a 255 disponibles
// ------------------------------------------------

#define n_arena    21
#define n_piramide  22
#define n_cuerpo    23
#define	n_trampa    24
#define n_pinchos   25
#define n_acceso    26
#define n_altar     27
#define n_vasija    28
#define n_llama     30
#define n_fuego     31
#define n_aceite    32
#define n_serpiente 33
#define n_arana     34
#define n_pared     35
#define n_muro      36
#define n_brecha    37
#define n_luz       38
#define n_abertura  39
#define n_sol       41
#define n_sepulcro  43
#define n_cafax     45
#define n_aceps     46
#define n_sala      47
#define n_techo     48
#define n_suelo     49
#define n_entrada   50
#define n_columna   51
#define n_escalera  52
#define n_nubes     53
#define n_cielo     54
#define n_pasillo   55
#define n_arte      56
#define n_ojo       58
#define n_oscuridad 59
#define n_soporte   60
#define n_puente    61
#define n_sima      62
#define n_rampa     63
#define n_pendiente 64
#define n_objeto    65
#define n_hueco     66
#define n_agujero   66
#define n_ranura    67
#define n_luminaria 68
#define n_brasa     69
#define n_carbon    70
#define n_momia     71
#define n_ataud     72
#define n_mesa      73
#define n_polvo     74
#define n_animales  75
#define n_semilla   76
#define n_maiz      77
#define n_anubis    78
#define n_vivos     79
#define n_farao     80
#define n_vida      81
#define n_antes     82
#define n_muerte    83
#define n_charco    84
#define n_cofre     85
#define n_cerradura 86
#define n_copa      87
#define n_anillo    88
#define n_espada    89
#define n_cabeza    90
#define n_piedra    91
#define n_mano      92
#define n_hueso     93
#define n_rey       94
#define n_pozo      95
#define n_herramientas 96
#define n_linterna  97
#define n_barra     98
#define n_cuerda    99
#define n_antorcha  100
#define n_pedernal 101
#define n_sandalia 102
#define n_carta     103
#define n_brujula   104
#define n_tabla     105
#define n_roca      106
#define n_trigo		107
#define n_llave		108
#define n_tapa		109
#define n_poste		110
#define n_papiro	111
#define n_esquina   112
#define n_aguja     n_pinchos
#define n_valle     113
#define n_losa      114
#define n_dedo      115
#define n_polea     116



// Verbos que funcionan como nombres < 20

// Verbos
// Some notes about vocabulary:
//    Words with same number but different type are not considered synomyms.
//    Nouns under number 20 are considered "convertible", what means they can be converted to verbs if a verb is missing (so "north" does the same than "go north").
//    Verbs undernumber 14 are considered "direction verbs", so if you have not added a specific response for a player order like "north" then ngPAWS will try to go in that direction. That doesn't happen with verbs 14 and above.
//    You can leave gaps when adding vocabulary, that is, is not required that vocabulary numbers are consecutive.

// Names and verbs uses different tables. Verbs below 20 are reserved for directions
// Same definitions as for names below 20, just for convenience
// Declarations are in the table for names and are used as convertibles
#define vNorte  1
#define vSur 	2
#define vEste	3
#define vOeste  4
#define vNoreste 5
#define vNorOeste 6
#define vSurEste 7
#define vSurOeste 8
#define vSubir 0
#define vArriba 9
#define vAbajo 10
#define vBajar 10
#define vEntrar 11
#define vEntra 11
#define vAdentro 11
#define vDentro 11
#define vSalir 12
#define vSal 12 
#define vAfuera 12

// Verbs of normal use
#define vCoger 20
#define vDejar 21
#define vSacar 22
#define vQuitar 23
#define vMeter 74
#define vMirar 24
#define vFin 25
#define vMirar 24
#define vSave 26
#define vLoad  27
#define vRamsave 28
#define vRamload 29

#define vExaminar   30      
#define vDecir  31      
#define vMandar   31      
#define vPreguntar 31      
#define vHablar  31      
#define vPulsa   32      
#define vPulsar   32      
#define vEmpujar   32      
#define vApretar   32      
#define vPresionar   32      
#define vAbrir   33      
#define vAbre    33      
#define vCierr   34      
#define vCerrar  34      
#define vTirar   35      
#define vTeclear   36
#define vTeclea 36      
#define vEscribe   37
#define vEscribir 37      
#define vEnciede   38      
#define vEncender   38      
#define vApagar   39      
#define vIntroduce 40    
#define vIr      41      
#define vVe      41      
#define vVes     41      
#define vVolver  41      
#define vRegresa 41      
#define vCamina  41      
#define vOler    42      
#define vHuele   42      
#define vTocar   43      
#define vPalpar  43      
#define vToca    43      
#define vEscuchar 44     
#define vOir      44     
#define vOye      44     
#define vSentarse 45     
#define vSientate 45     
#define vSaltar   46     
#define vMea      47     
#define vCaga     47     
#define vMear     47     
#define vCagar    47     
#define vChupar   48     
#define vLamer    48     
#define vCantar   49     
#define vPaper    50     
#define vInk      51     
#define vTest     52     
#define vT        52
#define vLanzar   53
#define vGirar    54
#define vEsperar    55
#define vLeer 56
#define vAtacar 57
#define vPoner 58
#define vRomper 59 
#define vVestir 60
#define vDar 61
#define vPuntos 62
#define vTurnos 63
#define vUsar 64 

// Adverbios
#define adRapidamente 2
#define adLentamente 3
#define adSilenciosamente 4
#define adRuidosamente 5
#define adCuidadosamente 6
#define adTranquilamente 7

#define pA 2
#define pDe 3
#define pDentro 4
#define pDesde 5
#define pHacia 6
#define pTras 7
#define pBajo 8
#define pCon 9
#define pPara 10
#define pPor 11
#define pExcepto 12
#define pFuera 13
#define pDelante 14
#define pEntre 15
#define pEncima 16

// Adjetivos

#define aPequeno 2
#define aGrande  3
#define aViejo   4
#define aVieja   4
#define aNuevo   5
#define aNueva   5
#define aDuro   6
#define aDura    6
#define aBlando  7
#define aCorto   8
#define aCorta   8
#define aLargo   9
#define aLarga   9
#define aAzul    10    
#define aBlanco  11    
#define aRojo    12    
#define aVerde   13    
#define aNegro   14    
#define aTermico 15    
#define aFrio    16    
#define aCaliente 16   
#define aCalor   16    
#define aAmarillo 17   
 
// Funciones adicionales del juego

