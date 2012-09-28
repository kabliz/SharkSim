#include "EXEreader.h"


void EXEreader::parseFile(const char* filename)
{
	//scan lines.
	readFile = fopen(filename, "r");
	if(readFile == NULL)
	{
		printf("csv file not found\n");
		exit(-1);
	}
	//spin past the first line, which only lists string names of columns
	char cc = fgetc(readFile);
	while(cc != '\n'){cc = fgetc(readFile);}
	
	while(!feof(readFile))
	{
		char time[3];
		char numb[32];
		int hour, min, msec, ttime;
		char *err;
		char dd = fgetc(readFile);
		//spin until first comma.i
		while(dd != ',')	
		{
			if(dd == EOF)
			{
				break;
			}
			dd = fgetc(readFile);
			//printf("%c", dd);
		}
		if(dd == EOF)
		{
			break;
		}

		//this value is Time, scan it in.
		int i = 0;
		while(dd != ':')
		{
			dd = fgetc(readFile);
			time[i] = dd;
			i++;
		}
		time[2] = '\0';
		hour = atoi(time);

		i = 0;
		while(dd != ':')
		{
			dd = fgetc(readFile);
			time[i] = dd;
			i++;
		}
		time[2] = '\0';
		min = atoi(time);
		
		i = 0;
		while(dd != ',')
		{
			dd = fgetc(readFile);
			time[i] = dd;
			i++;
		}
		time[2] = '\0';
		msec = atoi(time);
		/*err = fgets(time, 3, readFile);
		hour = atoi(time);
		fgetc(readFile); //colon
		
		err = fgets(time, 3, readFile);
		min = atoi(time);
		fgetc(readFile); //colon
		
		err = fgets(time, 3, readFile);
		msec = atoi(time);*/
		if(prevTime > msec){msec += 60;}  //TODO, calc minute into account
		ttime = msec - prevTime;
		
		//fgetc(readFile);

		//Latitude
		i = 0;
		while(!feof(readFile))
		{
			char cur = fgetc(readFile);
			if(cur == ',') {break;}
			numb[i] = cur;
			i++;
		}
		numb[i] = '\0';
		double Latitude = atof(numb);
			
		
		//Longitude
		i = 0;
		while(!feof(readFile))
		{
			char cur = fgetc(readFile);
			if(cur == ',') {break;}
			numb[i] = cur;
			i++;
		}
		numb[i] = '\0';
		double Longitude = atof(numb);
		

		//spin until end of line
		while(fgetc(readFile) != '\n'){}


		//Add to data structure
		
		if(!(Latitude >= -0.001 && Latitude <= 0.001 && Longitude >= -0.001 && Longitude <= 0.001)) //some data entries are blank
		{
			//printf("%f %f \n", Latitude, Longitude);
			latLong.push_back(Vector3f(Latitude, Longitude, 0));
			dts.push_back(ttime);
			prevTime = msec;
			if(Longitude < minlong){minlong = Longitude;}
			if(Longitude > maxlong){maxlong = Longitude;}
			if(Latitude < minlat){minlat = Latitude;}	
			if(Latitude > maxlat){maxlat = Latitude;}
		}
	}
	printf("%f %f \n", minlat, minlong);
	fclose(readFile);
}
