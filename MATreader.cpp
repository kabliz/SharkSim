#include "MATreader.h"


void MATreader::checkErr(int chk, bool nonzeroerr)
{
	if((chk != 0 && nonzeroerr) || (chk < 0 && !nonzeroerr))
	{
		printf("error detected in MATLAB file reading\n");
		exit(-1);
	}
}

void MATreader::exitOnNull(char* chk)
{
	if(checkNull(chk))
	{
		printf("null detected in MATLAB file reading\n");
		exit(-1);
	}
}

bool MATreader::checkNull(char *chk)
{
	return (chk) == 0 ;
}

double MATreader::charToDouble(char data[9])
{
	double dataType;
	char redata[8];	
	if(lilEndian)
	{
		redata[0] = data[0];
		redata[1] = data[1];
		redata[2] = data[2];
		redata[3] = data[3];
		redata[4] = data[4];
		redata[5] = data[5];
		redata[6] = data[6];
		redata[7] = data[7];
	}
	else
	{
		redata[0] = data[7];
		redata[1] = data[6];
		redata[2] = data[5];
		redata[3] = data[4];
		redata[4] = data[3];
		redata[5] = data[2];
		redata[6] = data[1];
		redata[7] = data[0];
	}
	// get the 8 unsigned raw bytes, accumulate to a long and then
    	// convert the 64-bit pattern to a double.
   	/*long accum = 0;
	int i = 0;
	for ( int shiftBy=0; shiftBy<64; shiftBy+=8 )
	{
		// must cast to long or the shift would be done modulo 32
		accum |= ( (long)( data[i] & 0xff ) ) << shiftBy;
		i++;
	}*/


	dataType = *(reinterpret_cast<double*>( redata )); 
	return dataType;

}
int MATreader::charToInt(char data[5])
{
	int dataType;
	char redata[4];	
	redata[0] = data[0];
	redata[1] = data[1];
	redata[2] = data[2];
	redata[3] = data[3];
	dataType = *reinterpret_cast<unsigned*>( redata );

	if(!lilEndian)
	{
		dataType =	(dataType >> 24) |					// Move first byte to the end,
			((dataType << 8) & 0x00FF0000) |	// move 2nd byte to 3rd,
			((dataType >> 8) & 0x0000FF00) |	// move 3rd byte to 2nd,
			(dataType << 24);					// move last byte to start.
	}
	return dataType;
}

void MATreader::parseComMatrix(Bytef* matrix, int fptr, int size)
{
	//parse the resulting matrix
	char dataScan[5];
	int dataType;
	int dataSize;
	int i = 0;
	bool isComplex;
	bool isGlobal;
	bool isLogical;
	int classType;
	
	//get array Flags, checking to make sure the right place is found.
	//fptr += 0;
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	dataScan[4] = 0;
	if(charToInt(dataScan) != 6){printf ("corrupt MAT file(1)\n"); exit(-1);}
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	if(charToInt(dataScan) != 8){printf ("corrupt MAT file(2)\n"); exit(-1);}
	
	//fptr += 2; //undefined space
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	int flagScan = charToInt(dataScan);
	char flag = ((char*)&flagScan)[2];
 	classType = ((char*)&flagScan)[3]; 
	/*printf("\t\tundefeind bytes %d %d\n", matrix[fptr++], matrix[fptr++]);
	char flag = matrix[fptr++];  //nobye swapping for these. single byte
	classType = matrix[fptr++];*/
	printf("\t\t %d %d \n", flag, classType);
	isComplex = flag & 8; 
	isGlobal = flag & 4;
	isLogical = flag & 2;
	//fptr += 4; //undefined space
	printf("\t\t complex %d, global %d, logical %d, class %d\n", isComplex, isGlobal, isLogical, classType);
	printf("\t\t undefined bytes: %d %d %d %d, dimensions: ", matrix[fptr++], matrix[fptr++], matrix[fptr++], matrix[fptr++] );

	//array dimensions	
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	if(charToInt(dataScan) != 5){printf ("corrupt MAT file(3)\n"); exit(-1);}
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	//if(charToInt(dataScan) != 8){printf ("corrupt MAT file(4)\n"); exit(-1);}
	int numDimension = charToInt(dataScan) / 4;	
	int dAr[numDimension];
	int dArc = 0;
	for(int m = 0; m < numDimension; m++)
	{
		dataScan[0] = matrix[fptr++];
		dataScan[1] = matrix[fptr++];
		dataScan[2] = matrix[fptr++];
		dataScan[3] = matrix[fptr++];
		dAr[m] = charToInt(dataScan);
		if (dAr[m] > dArc)
		{
			dArc = dAr[m];
		}
		printf("%d ", dAr[m]);
	}
	printf("\n");

	//array name
	//read uINt8, 8
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	int nameConfirmation = charToInt(dataScan);
	//if(nameConfirmation != 1){printf ("corrupt MAT file(5)\n");}
	dataScan[0] = matrix[fptr++];
	dataScan[1] = matrix[fptr++];
	dataScan[2] = matrix[fptr++];
	dataScan[3] = matrix[fptr++];
	int loops =0;
	loops = charToInt(dataScan);
	if(nameConfirmation == 1 || nameConfirmation == 2)
	{
		printf("\t\t");
		for(int k = 0; k < loops; k++)
		{
			printf("%c", matrix[fptr++]);
		}
		printf("\n");
		fptr += 8-loops; //default to moving two dimensions forward 
	}
	else
	{
		printf("namesize: %d, %d \n", nameConfirmation, loops);	
		//strange, errored names dont need to be moved forward
	}	
	
	

		
	//while(i < 4 && fptr < size)
	while(fptr < size)
	{
		dataScan[0] = matrix[fptr++];
		dataScan[1] = matrix[fptr++];
		dataScan[2] = matrix[fptr++];
		dataScan[3] = matrix[fptr++];
		dataType = charToInt(dataScan);	
		
		dataScan[0] = matrix[fptr++];
		dataScan[1] = matrix[fptr++];
		dataScan[2] = matrix[fptr++];
		dataScan[3] = matrix[fptr++];
		dataSize = charToInt(dataScan);
	
		printf("\t\t type: %d, size %d\n", dataType, dataSize);
		//fptr +=	dataSize;		
		int k = 0;
		//while(dataType == 9 && k < dataSize)
		while(k < dArc)
		{
			char doubleScan[9];
			doubleScan[8] = 0;
			doubleScan[0] = matrix[fptr++];	
			doubleScan[1] = matrix[fptr++];	
			doubleScan[2] = matrix[fptr++];	
			doubleScan[3] = matrix[fptr++];	
			doubleScan[4] = matrix[fptr++];	
			doubleScan[5] = matrix[fptr++];	
			doubleScan[6] = matrix[fptr++];	
			doubleScan[7] = matrix[fptr++];	
			k ++;
			double db = charToDouble(doubleScan);

			//printf("\t\t\t %f \n", db);
			pMatrices[numMatrix].push_back(db);
		}
		i++;
	}
}

Bytef* MATreader::uncompressMatrix(unsigned int asize)
{
	uLongf aproxSize = asize*25;
	Bytef* matrix = (Bytef*) malloc(sizeof(char)*aproxSize);
	//read file into a buffer
	Bytef* buffer = (Bytef*) malloc(sizeof(char)*asize);
	//if(!lilEndian)
	{
		size_t res = fread((void*)buffer, asize, sizeof(Bytef), readfile);
	}


	int err = uncompress(matrix, &aproxSize, buffer, asize);
	if(err != Z_OK)
	{
		printf("MATLAB uncompression issue, ");
		if(err == Z_DATA_ERROR)
			printf("data not readable\n");
		else if(err == Z_BUF_ERROR)
			printf("buffer out of room\n");
		else if(err == Z_MEM_ERROR)
			printf("cannot allocate memory\n");
		//return buffer; //sneaky hax to free this
		aproxSize = 0;
	}
	else
	{
		printf("new size %d\n", (long) aproxSize);
	}
	free(buffer);

	//parse the resulting matrix
	int fptr = 0;
	char dataScan[5];
	int dataType;
	int dataSize;

	while(fptr < aproxSize)
	{
		dataScan[0] = matrix[fptr++];
		dataScan[1] = matrix[fptr++];
		dataScan[2] = matrix[fptr++];
		dataScan[3] = matrix[fptr++];
		dataScan[4] = 0;
		dataType = charToInt(dataScan);	
		
		dataScan[0] = matrix[fptr++];
		dataScan[1] = matrix[fptr++];
		dataScan[2] = matrix[fptr++];
		dataScan[3] = matrix[fptr++];
		dataSize = charToInt(dataScan);
	
		printf("\t type: %d, size %d\n", dataType, dataSize);
		pMatrices.push_back(vector<double>());
		parseComMatrix(matrix, fptr, dataSize);
		numMatrix++;
		fptr +=	dataSize;
	}
	
}


void MATreader::parseFile(const char* argc)
{
	readfile = fopen(argc, "r");
	if(readfile == NULL)
	{
		printf("MATLAB file not found\n");
		exit(-1);
	}

	//read in 116 byte header + 8 byte offset + 2 byte version. Only version 5 supported
	checkErr(fseek(readfile, 126, SEEK_SET), true);
	//next two bytes denote endianness. "IM" means that bytes are flipped.
	char endian[3];
	char * res = fgets(endian, 3, readfile);	
	if(res != NULL && endian[0] == 'I' && endian[1] == 'M')
		lilEndian = true;
	else if (res != NULL && endian[0] == 'M' && endian[1] == 'I')
		lilEndian = false;
	else
	{
		printf("Cannot determine endianness of MATLAB file (%s) \n", endian);
		exit(-1);
	}

	//Enter data entry loop. Read in four byte data type and four byte size indicator, then scan in the indicated bytes.
	while(!feof(readfile))
	{
		char data[5];
		int dataType;
		int dataSize;
		//scan data type
		res = fgets(data, 5, readfile);
		if(checkNull(res)) break;
		dataType = charToInt(data);	
		//size indicator
		res = fgets(data, 5, readfile);	
		if(checkNull(res)) break;
		dataSize = charToInt(data);	

		//evaluate type and do main loop
		Bytef* mat;
		switch(dataType)	
		{
			case(1):
				printf("8bit sign int\n");
				break;
			case(2):
				printf("8bit unsign int\n");
				break;
			
			case(3):
				printf("16bit sign int\n");
				break;

			case(4):
				printf("16bit unsign int\n");
				break;

			case(5):
				printf("32bit sign int\n");
				break;

			case(6):
				printf("32bit unsign int\n");
				break;
			case(7):
				printf("float\n");
				break;

			case(8):
			case(10):
			case(11):
				printf("reserved\n");
				break;

			case(9):
				printf("double\n");
				break;

			case(12):
				printf("64bit sign int\n");
				break;

			case(13):
				printf("64bit unsign int\n");
				break;

			case(14):
				printf("Matrix D:\n");
				break;

			case(15):
				printf("Compressed data:\n");
				mat = uncompressMatrix(dataSize);
				//free(mat);
				break;

			case(16):
				printf("UTF8\n");
				break;

			case(17):
				printf("UTF16\n");
				break;

			case(18):
				printf("UTF32\n");
				break;

			default:
				printf("invalid MATLAB type read- %d \n", dataType );
				//exit(-1);
				break;
		}


		//skipping, TODO
		//fseek(readfile, dataSize, SEEK_CUR);
		printf("...size %d....\n", dataSize);
	}
	fclose(readfile);
}




