#include <iostream>
using std::cout ;
using std::cin ;
using std::endl ;
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

struct HEADER {
	unsigned int size ; /* File size in bytes */
	unsigned short int reserved1, reserved2 ;
	unsigned int offset ; /* Offset to image data, bytes */
} ;

struct INFOHEADER {
	unsigned int size ; /* Header size in bytes */
	int width, height ; /* Width and height of image */
	unsigned short int planes ; /* Number of colour planes */
	unsigned short int bits ; /* Bits per pixel */
	unsigned int compression ; /* Compression type */
	unsigned int imagesize ; /* Image size in bytes */
	int xresolution, yresolution ; /* Pixels per meter */
	unsigned int ncolours ; /* Number of colours */
	unsigned int importantcolours ; /* Important colours */
} ;

struct RGBQUAD {
	unsigned char rgbBlue ;
	unsigned char rgbGreen ;
	unsigned char rgbRed ;
} ;

int readImage (char [], int *, int *, char *) ; // read z-level from z-dimention file
int readBaseImage (RGBQUAD *, int *, int *, char *) ; // read image from base file
void randomDot (RGBQUAD *, int, int) ; // generate one-line random dot to output file
int randomColorValue () ; // generate a random value depend on setting
void baseImage (RGBQUAD *, RGBQUAD *, int, int, int) ; // copy one-line base image to output file
void shift (RGBQUAD *, char *, int, int, int, bool) ; // shift a dot depend on z-level
int outputImage (RGBQUAD *, int, int, char *) ; // output resulting image to output file

const int max_Pixels = 240000 ;
const int z_range = 18 ;
const int critical_distance = 144 ;
const int addition_height = 10 ;

int main () {
	char filename1[30] ; // z-dimension file name
	char filename2[30] ; // base file name
	char filename3[30] ; // output file name
	char imageArray_3D[max_Pixels] ; // array of z-dimension file
	RGBQUAD baseImageArray[critical_distance * 150] ; // array of base file
	RGBQUAD imageArray[max_Pixels] ; // array of output file
	int origin_width ; // width of z-dimension file
	int output_width ; // width of output file ( = origin_width + critical_distance )
	int origin_height ; // height of z-dimension file
	int output_height ; // height of output file ( = origin_height + addition_height )
	int base_width ; // width of base
	int base_height ; // height of base
	int distance ;
	bool randomMode = true ;
	int rf_point ;
	int i, j, rx, ry ;
	int random_option = 1 ;
	
	srand(time(0)) ;

	cout << "Enter the name of z-dimension file (*.bmp) : " ;
	cin >> filename1 ;

	if (readImage(imageArray_3D, &origin_width, &origin_height, filename1) != 0)
		return 1 ;
	output_width = origin_width + critical_distance ;
	output_height = origin_height + addition_height ;

	cout << "Enter the name of base file (*.bmp), \nor enter \"rand\" to use random-dotted mode : " ;
	cin >> filename2 ;

	if (strcmp(filename2, "rand") != 0) {
		if (readBaseImage(baseImageArray, &base_width, &base_height, filename2) != 0)
			return 1 ;
		randomMode = false ;
	}
	else {
		cout << "Random-dotted option : \n( 1. color  2. black-white  3. red  4. green  5. blue ) " ;
		cin >> random_option ;
	}

	cout << "Please wait a minute..." << endl ;

	rf_point = origin_width / 2 ;
	for (i = 0 ; i < origin_height ; i++) {
		if (randomMode)
			randomDot(imageArray + i * output_width + rf_point, critical_distance, random_option) ;
		else
			baseImage(imageArray + i * output_width + rf_point, baseImageArray, critical_distance, base_width, i % base_height) ;
		for (j = 0 ; rf_point + j <= origin_width ; j += critical_distance) {
			if (origin_width - (rf_point + j) + 1 < critical_distance)
				distance = origin_width - (rf_point + j) + 1 ;
			else
				distance = critical_distance ;
			if (randomMode)
				randomDot(imageArray + i * output_width + critical_distance + rf_point + j, distance, random_option) ;
			else
				baseImage(imageArray + i * output_width + critical_distance + rf_point + j, baseImageArray, distance, base_width, i % base_height) ;
			shift(imageArray + i * output_width + critical_distance + rf_point + j,
				    imageArray_3D + i * origin_width + rf_point + j, origin_width, origin_height, distance, true) ;
		}
		for (j = -1 ; rf_point + j >= 0 ; j -= critical_distance) {
			if ((rf_point + j) + 1 < critical_distance)
				distance = (rf_point + j) + 1 ;
			else
				distance = critical_distance ;
			if (randomMode)
				randomDot(imageArray + i * output_width + rf_point + j - distance + 1, distance, random_option) ;
			else
				baseImage(imageArray + i * output_width + rf_point + j - distance + 1, baseImageArray, distance, base_width, i % base_height) ;
			shift(imageArray + i * output_width + rf_point + j,
				    imageArray_3D + i * origin_width + rf_point + j, origin_width, origin_height, distance, false) ;
		}
	}

	for (ry = 0 ; ry < addition_height ; ry++)
		for (rx = 0 ; rx < output_width ; rx++) {

//			if ((rx - output_width / 2 - critical_distance / 2) *
//				  (rx - output_width / 2 - critical_distance / 2) +
//					(ry - 2) * (ry - 4) <= 4 ||
//					(rx - output_width / 2 + critical_distance / 2) *
//				  (rx - output_width / 2 + critical_distance / 2) +
//					(ry - 2) * (ry - 4) <= 4) {

			if ((rx % critical_distance - (output_width / 2 - critical_distance / 2) % critical_distance) *
				  (rx % critical_distance - (output_width / 2 - critical_distance / 2) % critical_distance) +
					(ry - addition_height + 4) * (ry - addition_height + 4) <= 5) {
				imageArray[output_width * (origin_height + ry) + rx].rgbBlue = 0 ;
				imageArray[output_width * (origin_height + ry) + rx].rgbGreen = 0 ;
				imageArray[output_width * (origin_height + ry) + rx].rgbRed = 0 ;
			}
			else {
				imageArray[output_width * (origin_height + ry) + rx].rgbBlue = 255 ;
				imageArray[output_width * (origin_height + ry) + rx].rgbGreen = 255 ;
				imageArray[output_width * (origin_height + ry) + rx].rgbRed = 255 ;
			}
		}

	for (ry = output_height - 1 ; ry >= 0 ; ry--)
		for (rx = 0 ; rx < output_width ; rx++) {
			imageArray[output_width * (ry + addition_height) + rx].rgbBlue = imageArray[output_width * ry + rx].rgbBlue ;
			imageArray[output_width * (ry + addition_height) + rx].rgbGreen = imageArray[output_width * ry + rx].rgbGreen ;
			imageArray[output_width * (ry + addition_height) + rx].rgbRed = imageArray[output_width * ry + rx].rgbRed ;
		}

		
	for (ry = 0 ; ry < addition_height ; ry++)
		for (rx = 0 ; rx < output_width ; rx++) {
			if ((rx % critical_distance - (output_width / 2 - critical_distance / 2) % critical_distance) *
				  (rx % critical_distance - (output_width / 2 - critical_distance / 2) % critical_distance) +
					(ry - 4) * (ry - 4) <= 5) {
				imageArray[output_width * ry + rx].rgbBlue = 0 ;
				imageArray[output_width * ry + rx].rgbGreen = 0 ;
				imageArray[output_width * ry + rx].rgbRed = 0 ;
			}
			else {
				imageArray[output_width * ry + rx].rgbBlue = 255 ;
				imageArray[output_width * ry + rx].rgbGreen = 255 ;
				imageArray[output_width * ry + rx].rgbRed = 255 ;
			}
		}

	cout << "Enter the name of output file (*.bmp) : " ;
	cin >> filename3 ;

	output_height += addition_height ;

	if (outputImage(imageArray, output_width, output_height, filename3) == 0)
		cout << "File successfully build up !!" << endl ;

	return 0 ;
}

// generate one-line random dot to output file
void randomDot (RGBQUAD *row, int width, int option) {
	int temp ;
	for (int i = 0 ; i < width ; i++) {
		switch (option) {
			case 1 :
				(row + i)->rgbBlue = randomColorValue() ;
				(row + i)->rgbGreen = randomColorValue() ;
				(row + i)->rgbRed = randomColorValue() ;
				break ;
			case 2 :
				temp = randomColorValue() ;
				(row + i)->rgbBlue = temp ;
				(row + i)->rgbGreen = temp ;
				(row + i)->rgbRed = temp ;
				break ;
			case 3 :
				temp = randomColorValue() ;
				(row + i)->rgbBlue = temp ;
				(row + i)->rgbGreen = temp ;
				(row + i)->rgbRed = 255 ;
				break ;
			case 4 :
				temp = randomColorValue() ;
				(row + i)->rgbBlue = temp ;
				(row + i)->rgbGreen = 255 ;
				(row + i)->rgbRed = temp ;
				break ;
			case 5 :
				temp = randomColorValue() ;
				(row + i)->rgbBlue = 255 ;
				(row + i)->rgbGreen = temp ;
				(row + i)->rgbRed = temp ;
				break ;
		}
	}
}

// generate a random value depend on setting
int randomColorValue () {
	int colorValue ;
	switch (rand() % 5) {
		case 0 :
		case 1 :
			colorValue = 0 ; break ;
		case 2 :
		case 3 :
		case 4 :
			colorValue = 255 ; break ;
	}
	return colorValue ;
}

// copy one-line base image to output file
void baseImage (RGBQUAD *row, RGBQUAD *base, int width, int base_width, int h) {
	for (int i = 0 ; i < width ; i++) {
		(row + i)->rgbBlue = (base + h * base_width + i % base_width)->rgbBlue ;
		(row + i)->rgbGreen = (base + h * base_width + i % base_width)->rgbGreen ;
		(row + i)->rgbRed = (base + h * base_width + i % base_width)->rgbRed ;
	}
}

// shift a dot depend on z-level
void shift (RGBQUAD *output_start, char *input_start, int width, int height, int op_width, bool direction) {
	int source ;
	int target ;
	int i, im, j, jm ;
	int sign ;
	if (direction)
		sign = 1 ;
	else
		sign = -1 ;
	for (i = 0 ; i < op_width ; i++) {
		im = i * sign ;
		source = 0 ;
		for (j = 0 ; j < op_width ; j++) {
			jm = j * sign ;
			if (im == jm + (*(input_start + jm) / 2 - z_range / 4) * sign)
				source = jm ;
		}
		target = 0 ;
		for (j = 0 ; j < op_width ; j++) {
			jm = j * sign ;
			if (im == jm - ((*(input_start + jm) + 1) / 2 - z_range / 4) * sign)
				target = jm ;
		}
		if (source != 0 && target != 0)
			*(output_start + target) = *(output_start - critical_distance * sign + source) ;
	}
}

// read z-level from z-dimention file
int readImage (char image_3D[], int *width, int *height, char *filename) {
	char magic[2] ;
	HEADER header ;
	INFOHEADER infoheader ;
	RGBQUAD rgbquad ;
	char space[1] ;
	FILE *bmpf ;
	int space_width = 0 ;

	if ((bmpf = fopen(filename, "r")) == NULL) {
		cout << "Fail when opening file !!" << endl ;
		return 1 ;
	}

	fread(magic, 1, 2, bmpf) ;
	fread(&header, sizeof(HEADER), 1, bmpf) ;
	fread(&infoheader, sizeof(INFOHEADER), 1, bmpf) ;
	*width = infoheader.width ;
	*height = infoheader.height ;
	if ((*width + critical_distance) * (*height + addition_height) > max_Pixels) {
		cout << "The original image is too large !!" << endl ;
		return 1 ;
	}

	while ((space_width + *width * 3) % 4 != 0)
		space_width++ ;

	for (int i = 0 ; i < *height ; i++) {
		for (int j = 0 ; j < *width ; j++) {
			fread(&rgbquad, sizeof(RGBQUAD), 1, bmpf) ;
			image_3D[i * *width + j] = (rgbquad.rgbBlue + 
				rgbquad.rgbGreen + rgbquad.rgbRed) / (765 / z_range) ;
		}
		for (int k = 1 ; k <= space_width ; k++)
			fread(space, 1, 1, bmpf) ;
	}

	fclose(bmpf) ;
	return 0 ;
}

// read image from base file
int readBaseImage (RGBQUAD *image, int *width, int *height, char *filename) {
	char magic[2] ;
	HEADER header ;
	INFOHEADER infoheader ;
	char space[1] ;
	FILE *bmpf ;
	int space_width = 0 ;

	if ((bmpf = fopen(filename, "r")) == NULL) {
		cout << "Fail when opening file !!" << endl ;
		return 1 ;
	}

	fread(magic, 1, 2, bmpf) ;
	fread(&header, sizeof(HEADER), 1, bmpf) ;
	fread(&infoheader, sizeof(INFOHEADER), 1, bmpf) ;
	*width = infoheader.width ;
	*height = infoheader.height ;
	if (*width * *height > critical_distance * critical_distance) {
		cout << "The base image is too large !!" << endl ;
		return 1 ;
	}

	while ((space_width + *width * 3) % 4 != 0)
		space_width++ ;

	for (int i = 0 ; i < *height ; i++) {
		for (int j = 0 ; j < *width ; j++)
			fread(image + i * *width + j, sizeof(RGBQUAD), 1, bmpf) ;
		for (int k = 1 ; k <= space_width ; k++)
			fread(space, 1, 1, bmpf) ;
	}

	fclose(bmpf) ;
	return 0 ;
}

// output resulting image to output file
int outputImage (RGBQUAD *image, int width, int height, char *filename) {
	char magic[2] = {'B', 'M'} ;
	HEADER header ;
	INFOHEADER infoheader ;
	char space[1] = {'\0'} ;
	FILE *bmpf ;
	int space_width = 0 ;

	if ((bmpf = fopen(filename, "w")) == NULL) {
		cout << "Fail when opening file !!" << endl ;
		return 1 ;
	}

	while ((space_width + width * 3) % 4 != 0)
		space_width++ ;

	header.size = 54 + (space_width + width * 3) * height ;
	header.reserved1 = 0 ;
	header.reserved2 = 0 ;
	header.offset = 54 ;
	infoheader.size = 40 ;
	infoheader.width = width ;
	infoheader.height = height ;
	infoheader.planes = 1 ;
	infoheader.bits = 24 ;
	infoheader.compression = 0 ;
	infoheader.imagesize = (space_width + width * 3) * height ;
	infoheader.xresolution = 0 ;
	infoheader.yresolution = 0 ;
	infoheader.ncolours = 0 ;
	infoheader.importantcolours = 0 ;

	fwrite(magic, 1, 2, bmpf) ;
	fwrite(&header, sizeof(HEADER), 1, bmpf) ;
	fwrite(&infoheader, sizeof(INFOHEADER), 1, bmpf) ;

	for (int i = 0 ; i < height ; i++) {
		for (int j = 0 ; j < width ; j++)
			fwrite((image + i * width + j), sizeof(RGBQUAD), 1, bmpf) ;
		for (int k = 1 ; k <= space_width ; k++)
			fwrite(space, 1, 1, bmpf) ;
	}

	fclose(bmpf) ;
	return 0 ;
}