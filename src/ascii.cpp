#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <stdint.h>


#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION


#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"


enum ImageType {
  PNG, JPG, BMP
};

struct Image {
  uint8_t *data = NULL;
  size_t size = 0;
  int width;
  int height;
  int channels;

 
  Image(const char* filename){
    if (read(filename)){
      printf("Read %s\n", filename);
	  //1rd breakpoint
      size = width*height*channels;
    } else {
      printf("Failed to read %s\n", filename);
    }
  }
  
  Image(int width, int height, int channels) : width(width), height(height), channels(channels){
    size = width*height*channels;
    data =  new unsigned char[size];
  }
  
  Image(const Image& image) : Image(image.width, image.height, image.channels){
    memcpy(data, image.data, size);
  }

  ~Image(){
	//2th breakpoint
    stbi_image_free(data);
  }

  
  bool read(const char* filename){
    data = stbi_load(filename, &width, &height, &channels, 0);
    return data != NULL;
  }


  bool write(const char* filename){
    ImageType type = getFileType(filename);
    int success;
    switch(type){
    case PNG:
      success = stbi_write_png(filename, width, height, channels, data, width*channels);
      break;
    case BMP:
      success = stbi_write_bmp(filename, width, height, channels, data);
      break;
    case JPG:
      success = stbi_write_jpg(filename, width, height, channels, data, 100);
      break;
    }
    
    return success!=0;
  }

  ImageType getFileType(const char* filename){
    const char* ext = strrchr(filename, '.');
    if(ext != nullptr){
      if(strcmp(ext, ".png") == 0){
	return PNG;
      }else if(strcmp(ext, ".jpg") == 0){
	return JPG;
      } else if(strcmp(ext, ".bmp") == 0){
	return BMP;
      }
    }
    return PNG;
  }

  Image& resize(double width_scale, double height_scale){
    int new_width = (this->width) * width_scale;
    int new_height = (this->height) * height_scale;
    //3th breakpoint
    uint8_t *resizedPixels = (uint8_t *) malloc(new_width*new_height*channels);
    //4th breakpoint
    stbir_resize_uint8(data, this->width, this->height, 0, resizedPixels, new_width, new_height, 0, channels);
    //5th breakpoint
    stbi_image_free(data);
    //6th breakpoint
    data = resizedPixels;
    //7th breakpoint
    this->width = new_width;
    this->height = new_height;
    size = new_width*new_height*channels;
    //8th breakoint
    return *this;
  }
  
  Image& grayscale_lum(){
    if(channels < 3){
      printf("Image &p has less than 3 channels, it is assumed to be arleade grayscaled");
    } else {
      for(int i = 0; i < size; i+=channels){
	int gray = (0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2])/3;
	memset(data+i, gray, 3);
      }
    }
    return *this;
  }

  std::string convert_to_gray_scale(){
    std::string str = "";
    if(channels < 3){
      printf("Image &p has less than 3 channels, it is assumed to be arleady grayscaled");
    } else {
      for(int i=0, j=0; i < size-(channels-1); i+=channels, j++){
	int gray = 0.2126*data[i] + 0.7152*data[i+1] + 0.722*data[i+2];
	

	std::string gray_ramp = " .:!/r(l1X4H9W8$@";
	int gray_length = gray_ramp.size();
	
	char character = gray_ramp[ceil((gray_length-1) * gray/255)];

	if(j!=0 && (j)%width==0) {
	  str+='\n';
	}
	    
	str+=character;
      }
    }
    return str;
  }
};

int main(int argc, char* argv[]){
  if(argc != 4){
    printf("ERROR");
    return -1;
  }

  const int MAXIMUM_WIDTH=80;
  const int MAXIMUM_HEIGHT=50;
  
  Image img(argv[1]);
  //"9th breakpoint
  if(img.width > MAXIMUM_WIDTH || img.height > MAXIMUM_HEIGHT){
    double width_scale = 1;
    double height_scale = 1;
    
    if(img.width > MAXIMUM_WIDTH){
      width_scale = static_cast<double>(MAXIMUM_HEIGHT) / (double) img.height;
    }
    if(img.height > MAXIMUM_HEIGHT){
      height_scale = static_cast<double>(MAXIMUM_WIDTH) / (double) img.width;
    }
    
    std::cout << "Width scale to height scale = " << width_scale << ":" << height_scale << std::endl;
    img.resize(width_scale, height_scale);
  }
  img.grayscale_lum();
  //10th breakpoint
  img.write(argv[2]);
  std::string test = img.convert_to_gray_scale();
  std::ofstream ofs(argv[3]);
  ofs << test;
  return 0;
}
