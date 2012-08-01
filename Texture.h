#ifndef TEXTURE_H
#define TEXTURE_H

#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#ifdef __unix__
#include <GL/freeglut.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;


typedef struct Image {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Image;

typedef struct RGB {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} RGB;


class Texture
{
  public:
	Texture(){}
	~Texture(){}
  	Image *TextureImage; 
  	RGB myimage[64][64]; 
  	RGB* pixel;
  	void LoadAll();
  	GLvoid LoadTexture(string image_file, int texID);

  private:

	unsigned int getint(FILE *fp);
  	unsigned int getshort(FILE *fp);
  	int ImageLoad(char *filename, Image *image);
};
#endif
