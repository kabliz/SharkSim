#include "Texture.h"


//Add all texture names here
void Texture::LoadAll() 
{
	//load textures
        LoadTexture("./Assets/underwater-light.bmp", 0);
        LoadTexture("./Assets/underwater.bmp", 1);
        LoadTexture("./Assets/rocks.bmp", 2);
}


//routines to load in a bmp files - must be 2^nx2^m and a 24bit bmp
GLvoid Texture::LoadTexture(string image, int texID) {
   char* image_file = new char[image.size() + 1];
   strcpy(image_file, image.c_str());

   TextureImage = (Image *) malloc(sizeof(Image));
   if (TextureImage == NULL) {
      printf("Error allocating space for image");
      exit(1);
   }
   //cout << "Trying to load " << image_file << " attempt #1" << endl;
   if (!ImageLoad(image_file, TextureImage)) {
      cout << "Failed to load " << image_file << endl;
      exit(1);
   }
   /*  2d texture, level of detail 0 (normal), 3 components (red, green, blue),            */
   /*  x size from image, y size from image,                                              */
   /*  border 0 (normal), rgb color data, unsigned byte data, data  */
   glBindTexture(GL_TEXTURE_2D, texID);
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
         TextureImage->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}

/* BMP file loader loads a 24-bit bmp file only */

/*
 * getint and getshort are help functions to load the bitmap byte by byte
 */
unsigned int Texture::getint(FILE *fp) {
   int c, c1, c2, c3;

   /*  get 4 bytes */
   c = getc(fp);
   c1 = getc(fp);
   c2 = getc(fp);
   c3 = getc(fp);

   return ((unsigned int) c) + (((unsigned int) c1) << 8) + (((unsigned int) c2) << 16) + (((unsigned int) c3) << 24);
}

unsigned int Texture::getshort(FILE *fp) {
   int c, c1;

   /* get 2 bytes*/
   c = getc(fp);
   c1 = getc(fp);

   return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int Texture::ImageLoad(char *filename, Image *image) {
   FILE *file;
   unsigned long size; /*  size of the image in bytes. */
   unsigned long i; /*  standard counter. */
   unsigned short int planes; /*  number of planes in image (must be 1)  */
   unsigned short int bpp; /*  number of bits per pixel (must be 24) */
   char temp; /*  used to convert bgr to rgb color. */

   /*  make sure the file is there. */
   if ((file = fopen(filename, "rb")) == NULL) {
      printf("File Not Found : %s\n", filename);
      return 0;
   }

   /*  seek through the bmp header, up to the width height: */
   fseek(file, 18, SEEK_CUR);

   /*  No 100% errorchecking anymore!!! */

   /*  read the width */
   image->sizeX = getint(file);

   /*  read the height */
   image->sizeY = getint(file);

   /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
   size = image->sizeX * image->sizeY * 3;

   /*  read the planes */
   planes = getshort(file);
   if (planes != 1) {
      printf("Planes from %s is not 1: %u\n", filename, planes);
      return 0;
   }

   /*  read the bpp */
   bpp = getshort(file);
   if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      return 0;
   }

   /*  seek past the rest of the bitmap header. */
   fseek(file, 24, SEEK_CUR);

   /*  read the data.  */
   image->data = (char *) malloc(size);
   if (image->data == NULL) {
      printf("Error allocating memory for color-corrected image data");
      return 0;
   }

   if ((i = fread(image->data, size, 1, file)) != 1) {
      printf("Error reading image data from %s.\n", filename);
      return 0;
   }

   for (i = 0; i < size; i += 3) { /*  reverse all of the colors. (bgr -> rgb) */
      temp = image->data[i];
      image->data[i] = image->data[i + 2];
      image->data[i + 2] = temp;
   }

   fclose(file); /* Close the file and release the filedes */

   /*  we're done. */
   return 1;
}
