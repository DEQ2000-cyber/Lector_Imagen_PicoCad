#include "LectorImagenPicoCad.h"

int main( void ){
	const char *NAME = "chunky_tank.txt";

	uint8_t ancho;
	uint8_t alto;
	uint8_t bpp;
	uint8_t *DT = (uint8_t*)LoadIMGPICOCAD( NAME, &ancho, &alto, &bpp, PICOCAD_BPP_RGB );
	uint32_t X,Y;
	for ( Y=0; Y<alto; ++Y )
	{
		for ( X=0; X<ancho; ++X )
		{
			printf("%c", DT[( Y * ancho + X )] );
		}
		printf("\n");
	}

	free(DT);

	return 0;
}


