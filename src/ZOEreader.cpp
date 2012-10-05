#include "ZOEreader.h"


void ZOEreader::parseFile(const char* argc)
{
	readFile = fopen(argc, "r");
	if(readFile == NULL)
	{
		printf("Zoe txt file not found\n");
		exit(-1);
	}




	while(!feof(readFile))
	{
		char time[3];
		char numb[32];
		int hour, min, msec, ttime = 0;
		char *err;
		char dd = fgetc(readFile);
		
		//spin whitespace away 
		/*while(dd == ' ' || dd == ',' || dd == '\r' || dd == '\n' || dd == '\t')
		{
			if(dd == EOF)
			{
				break;
			}
			dd = fgetc(readFile);
		}*/
		if(dd == EOF)
		{
			break;
		}

		//Latitude
		int i = 0;
		while(!feof(readFile))
		{
			char cur = fgetc(readFile);
			if(cur == ',' || cur == ' ') {break;}
			numb[i] = cur;
			i++;
		}
		numb[i] = '\0';
		double Latitude = atof(numb);

		//Altitude
		i = 0;
		while(!feof(readFile))
		{
			char cur = fgetc(readFile);
			if(cur == ',' || cur == ' ') {break;}
			numb[i] = cur;
			i++;
		}
		numb[i] = '\0';
		double Altitude = atof(numb);


		//Longitude
		i = 0;
		while(!feof(readFile))
		{
			char cur = fgetc(readFile);
			if(cur == ',' || cur == ' ') {break;}
			numb[i] = cur;
			i++;
		}
		numb[i] = '\0';
		double Longitude = atof(numb);

		//spin until end of line
		while(fgetc(readFile) != '\n'){}

		//Add to data structure
		latLong.push_back(Vector3f(Latitude, Altitude, Longitude));
		Vector3f(Latitude, Altitude, Longitude).Print();
		dts.push_back(1.0);
		if(Longitude < minlong){minlong = Longitude;}
		if(Longitude > maxlong){maxlong = Longitude;}
		if(Latitude < minlat){minlat = Latitude;}
		if(Latitude > maxlat){maxlat = Latitude;}
		if(Altitude > maxalt){maxalt = Altitude;}
		if(Altitude < minalt){minalt = Altitude;}
	}
	fclose(readFile);

}
