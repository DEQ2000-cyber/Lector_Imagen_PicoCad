#ifndef __LECTORIMAGENPICOCAD_H__
#define __LECTORIMAGENPICOCAD_H__
/* CODE DISEÑADO POR: DEQ : GitHub: DEQ2000-ciber */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PICOCAD_STANDARD_WIDTH     0x80
#define PICOCAD_STANDARD_HEIGHT    0x78

#define PICOCAD_STANDARD_BPP       0x4
#define PICOCAD_BPP_RGB            0x18
#define PICOCAD_BPP_RGBA           0x20

#define PICOCAD_STANDARD_SIZE      0x3c00
#define PICOCAD_SIZE_RGB           0xb400
#define PICOCAD_SIZE_RGBA          0xf000

#ifdef __cplusplus
extern "C" {
#endif


/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/

/* LECTOR DE IMAGEN PICOCAD. */
/* PARAMETROS: */
/* TIPO *X = (TIPO*)NOMBRE, &ANCHO, &ALTO, &BPP, MODO_BPP. */

/* DEVUELVE: BPP = { 4, 24, 32 } */
/* DEVUELVE: BPP = { PICOCAD_STANDARD_BPP, PICOCAD_BPP_RGB, PICOCAD_BPP_RGBA } */

/* INGRESO: BPPMOD = { 0, 4, 24, 32 } */
/* INGRESO: BPPMOD = { PICOCAD_STANDARD_BPP, PICOCAD_STANDARD_BPP, PICOCAD_BPP_RGB, PICOCAD_BPP_RGBA } */

static void *LoadIMGPICOCAD( const char FileName[], uint8_t *width, uint8_t *height, uint8_t *bpp, uint8_t bpp_mod );

/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/
/*###############################################################################################################*/



/* PALETA DE 16 COLORES DE 4 BPP TRADUCIDO A 32 BPP. */
const uint32_t paleta_color_pico8[16] = {
	0xff000000,0xff1d2b53,0xff7e2553,0xff008751,0xffab5236,0xff5f574f,0xffc2c395,0xfffff1e8,
	0xffff004d,0xffffa300,0xffffec27,0xff00e436,0xff29adff,0xff83769c,0xffff77a8,0xffffccaa
};

/* TRADUCE EL FORMATO 4 BPP AL COLOR DESEADO EN EL ARRAY DE COLORES 32 BPP. */
static const uint8_t FormatPicoColor8( const char *valor ){
	return ( *valor>0x39? (*valor-0x57) : (*valor-0x30) );
}

/* LECTOR DE IMAGEN PICOCAD. */
static void *LoadIMGPICOCAD( const char FileName[], uint8_t *width, uint8_t *height, uint8_t *bpp, uint8_t bpp_mod ){
	/* CREAMOS EL DATA. */
	register uint8_t *DT = NULL;

	/* CARGAMOS MEMORIA AL DATA. */
	switch( bpp_mod ){
		case 0: case 4: {
			/* SI BPPMOD ES 0 NO ADMITE MODIFICAIONES Y DA LA IMAGEN CON LOS BYTES REALES. */
			DT = (uint8_t*)malloc( PICOCAD_STANDARD_SIZE );
			/* CARGAMOS TODO CON 255 AL DATA. */
			memset( DT, 255, PICOCAD_STANDARD_SIZE );
			break;
		}
		case 24:{
			/* SI BPPMOD ES 24 ADMITE MODIFICACIONES DE 24 BPP DE COLOR. */
			DT = (uint8_t*)malloc( PICOCAD_SIZE_RGB );
			/* CARGAMOS TODO CON 255 AL DATA. */
			memset( DT, 255, PICOCAD_SIZE_RGB );
			break;
		}
		case 32:{
			/* SI BPPMOD ES 32 ADMITE MODIFICACIONES DE 32 BPP DE COLOR. */
			DT = (uint8_t*)malloc( PICOCAD_SIZE_RGBA );
			/* CARGAMOS TODO CON 255 AL DATA. */
			memset( DT, 255, PICOCAD_SIZE_RGBA );
			break;
		}

	}

	/* SETEAMOS ANCHO. */
	*width = PICOCAD_STANDARD_WIDTH;

	/* SETEAMOS ALTO. */
	*height = PICOCAD_STANDARD_HEIGHT;

	/* SETEAMO BPP. */
	*bpp = ( bpp_mod == 0 ? PICOCAD_STANDARD_BPP : bpp_mod );

	/* ABRIMOS FICHERO. */
	FILE *f = fopen( FileName, "r" );

	/* VERIFICAMOS SÍ ABRIO CORRECTAMENTE. */
	if (!f)
	{
		// #error "ERROR OPEN FILE..."
		/* RETORNAMOS DATA. */
		return DT;
	}

	/* BUFFER PARA LECTURA PRINCIPAL. */
	char D[0x8];

	/* LEEMOS PARA EL BUFFER. */
	fread( D, 0x7,1, f );

	/* COMPARAMOS SI EXISTE LA PALABRA picocad. */
	if ( strstr(D,"picocad") == 0 )
	{
		/* CERRAMOS EL FICHERO. */
		fclose(f);
		/* RETORNAMOS DATA. */
		return DT;
	}

	/* POSICIONAMOS DEL FIN, HACIA ARRIBA. */
	fseek( f, 0xffffc30d, SEEK_END );

	/* SETEAMOS 0 EN POSICION 1. */
	uint32_t PTR = D[1] = 0;


	/* EMPESAMOS A LEER LA IMAGEN. */
	do{
		/* SI CAPTURAMOS % UNA VEZ ACTIVAMOS LA LECTURA DEL DATA. */
		if (D[0]=='%')
			/* LEEMOS DOS VECES. NO ES IMPORTANTE. */
			D[0] = fgetc( f ),
			D[0] = fgetc( f ),
			/* SETEAMOS 1 EN POSICION 1. */
			D[1] = 1;

		/* ESCRIBIENDO EN DATA LOS CANALES. */
		if (D[1]==1 && D[0]!=0xa){

			/* CARGANDO EL COLOR EN EL DATA TEMPORAL. */
			register uint32_t CLR = paleta_color_pico8[FormatPicoColor8( &D[0] )];

			/* VERIFICAMOS QUE TIPO DE CANALES SE A ELEJIDO. */
			switch( bpp_mod ){
				/* SI ES 0 O 4 DARA EL BUFFER NORMAL. */
				case 0:
				case 4:{
					/* CARGANDO EL COLOR EN EL DATA. */
					DT[PTR] = FormatPicoColor8( &D[0] );
					PTR++;
					break;
				}

				case 24:{
					/* SEPARAMOS LOS CANALES. */
					DT[PTR  ] = ((CLR)>>(0));
					DT[PTR+1] = ((CLR)>>(8));
					DT[PTR+2] = ((CLR)>>(16));
					PTR+=3;
					break;
				}

				case 32:{
					/* SEPARAMOS LOS CANALES. */
					DT[PTR  ] = ((CLR)>>(0));
					DT[PTR+1] = ((CLR)>>(8));
					DT[PTR+2] = ((CLR)>>(16));
					DT[PTR+3] = ((CLR)>>(24));
					PTR+=4;
					break;
				}

			}

		}
		/* VERIFICANDO LA DATA LEIDA Y SI EL ARCHIVO LLEGA A SU FINAL. */
	}while( D[0]=fgetc( f ), !feof( f ) );

	/* CERRAMOS EL FICHERO. */
	fclose(f);

	/* RETORNAMOS DATA. */
	return DT;
}

#ifdef __cplusplus
	}
#endif

#endif