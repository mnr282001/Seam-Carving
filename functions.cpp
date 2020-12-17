#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;

  // Create a one dimensional array on the heap of pointers to Pixels
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];

  bool fail = false;

  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];

    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }

  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }

  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
  /*seam = new length;
  seam = 0;*/
  int* seam= new int[length];
  for (int i = 0; i < length; ++i) {
    seam[i] = 0;
  }
  return seam;
}

void deleteSeam(int* seam) {
  delete [] seam;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
  ifstream ifs (filename);

  if (!ifs.is_open()) {
    cout << "Error: failed to open input file - " << filename << endl;
    return false;
  }

  char type[3];
  ifs >> type;
  if ((toupper(type[0]) != 'P') || (toupper(type[1]) != '3')) {
    if ((toupper(type[0]) != 'p') || (toupper(type[1]) != '3')) {
      cout << "Error: type is " << type << " instead of P3" << endl;
      return false;
    }
  }

  int w,h;
  ifs >> w >> h;
  if (ifs.fail()) {
    cout << "Error: read non-integer value" << endl;
    return false;
  }
  if (w != width) { // check that width matches what was passed into the function
    cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
    return false;
  }
  if (h != height) { // check that height matches what was passed into the function
    cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
    return false;
  }

  // get maximum value from preamble
  int colorMax = 0;
  ifs >> colorMax;
  if (ifs.fail()) {
    cout << "Error: read non-integer value" << endl;
    return false;
  }
  if (colorMax != 255) {
    cout << "Error: file is not using RGB color values." << endl;
    return false;
  }
  // get RGB pixel values
  while (!ifs.eof()) {
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        ifs >> image[col][row].r;
        if (ifs.fail()) {
          if(ifs.eof()) {
            cout << "Error: not enough color values" << endl;
          } else {
            cout << "Error: read non-integer value" << endl;
          }
          return false;
        }
        if (image[col][row].r < 0 || image[col][row].r > 255) {
          cout << "Error: invalid color value " << image[col][row].r << endl;
          return false;
        }
        ifs >> image[col][row].g;
        if (ifs.fail()) {
          if(ifs.eof()) {
            cout << "Error: not enough color values" << endl;
          } else {
            cout << "Error: read non-integer value" << endl;
          }
          return false;
        }
        if (image[col][row].g < 0 || image[col][row].g > 255) {
          cout << "Error: invalid color value " << image[col][row].g << endl;
          return false;
        }
        ifs >> image[col][row].b;
        if (ifs.fail()) {
          if(ifs.eof()) {
            cout << "Error: not enough color values" << endl;
          } else {
            cout << "Error: read non-integer value" << endl;
          }
          return false;
        }
        if (image[col][row].b < 0 || image[col][row].b > 255) {
          cout << "Error: invalid color value " << image[col][row].b << endl;
          return false;
        }
      }
    }
    int codeTester=0;
    ifs >> codeTester;
    if (!ifs.fail()) {
      cout << "Error: too many color values" << endl;
      return false;
    }
  }

  return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
  ofstream ofs (filename);

  // check if output stream opened successfully
  if (!ofs.is_open()) {
    cout << "Error: failed to open output file - " << filename << endl;
    return false;
  }

  // output preamble
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;

  // output pixels
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      ofs << image[col][row].r << " ";
      ofs << image[col][row].g << " ";
      ofs << image[col][row].b << " ";
    }
    ofs << endl;
  }
  return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
  //cout << "x: " << x << " y: " << y << " width: " << width << " height: " << height << endl;
  int energyOutput = 0; //energyX+energyY(what will be output)
  //int energyX = 0; //what the energy will be in the x gradient
  //int sumEnergyX = 0;
  //int energyY = 0; //what the energy will be in the y gradient
  //int sumEnergyY = 0;

  //energyX
  if(width <= 1) {
    energyOutput += 0;
  } else {
    if(x==0) {//(0,1)
      energyOutput += ((image[x+1][y].r - image[width-1][y].r) * (image[x+1][y].r -image[width-1][y].r));
      energyOutput += ((image[x+1][y].g - image[width-1][y].g) * (image[x+1][y].g -image[width-1][y].g));
      energyOutput += ((image[x+1][y].b - image[width-1][y].b) * (image[x+1][y].b -image[width-1][y].b));
    } else if (x == width -1) { //(2,1)
      energyOutput += ((image[x-1][y].r - image[0][y].r) * (image[x-1][y].r -image[0][y].r));
      energyOutput += ((image[x-1][y].g - image[0][y].g) * (image[x-1][y].g -image[0][y].g));
      energyOutput += ((image[x-1][y].b - image[0][y].b) * (image[x-1][y].b -image[0][y].b));
    } else { //(1,1)
      energyOutput += ((image[x+1][y].r - image[x-1][y].r) * (image[x+1][y].r -image[x-1][y].r));
      energyOutput += ((image[x+1][y].g - image[x-1][y].g) * (image[x+1][y].g -image[x-1][y].g));
      energyOutput += ((image[x+1][y].b - image[x-1][y].b) * (image[x+1][y].b -image[x-1][y].b));
    }
  }
  //energyY
  if(height <= 1) {
    energyOutput += 0;
  } else {
    if(y==0) {//(1,0)
      energyOutput += ((image[x][y+1].r - image[x][height-1].r) * (image[x][y+1].r - image[x][height-1].r));
      energyOutput += ((image[x][y+1].g - image[x][height-1].g) * (image[x][y+1].g - image[x][height-1].g));
      energyOutput += ((image[x][y+1].b - image[x][height-1].b) * (image[x][y+1].b - image[x][height-1].b));
    } else if (y == height -1) { //(1.2)
      energyOutput += ((image[x][y-1].r - image[x][0].r) * (image[x][y-1].r - image[x][0].r));
      energyOutput += ((image[x][y-1].g - image[x][0].g) * (image[x][y-1].g - image[x][0].g));
      energyOutput += ((image[x][y-1].b - image[x][0].b) * (image[x][y-1].b - image[x][0].b));
    } else { //(1,1)
      energyOutput += ((image[x][y+1].r - image[x][y-1].r) * (image[x][y+1].r - image[x][y-1].r));
      energyOutput += ((image[x][y+1].g - image[x][y-1].g) * (image[x][y+1].g - image[x][y-1].g));
      energyOutput += ((image[x][y+1].b - image[x][y-1].b) * (image[x][y+1].b - image[x][y-1].b));
    }
  }
  //energyOutput calculation
  //energyOutput = energyX + energyY;

  return energyOutput;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
  //create a diagram of the energy chart
  /*for(int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col){
      cout << energy(image, col,row,width,height) << " ";
    }
    cout << endl;
  }*/
  //set seam for the first row to the starting columns
  //seam[row] = column
  seam[0] = start_col;
  //initialize total energy to the energy for pixel(short_col,0)
  int totalEnergy = 0;
  /*int* energyArray = new int[height];
  for (int i = 0; i < height; ++i) {
    energyArray[i] = 0;
  }*/
  totalEnergy += energy(image, start_col, 0, width, height);
  //cout << "totalEnergy: " << totalEnergy << endl;
  //for loop that loops through each row
  int right;
  int forward;
  int left;
  for (int row = 1; row < height; ++row) {
    //cout << "Current energy: " << totalEnergy << endl;
    //calculate the energy of each possible next column
    right = 2147483647;
    forward = 2147483647;
    left = 2147483647;
    forward = energy(image, start_col, row, width, height);
    if (start_col == (width -1)) {
      left = 2147483647;
    } else {
      left = energy(image, start_col+1, row, width, height);
    }
    if(start_col == 0) {
      right = 2147483647;
    } else {
      right = energy(image, start_col-1, row, width, height);
    }

    //set the seam for current row to the current row to the column with the minimal energy
    //int checker = 0;
    if (forward == right && forward == left) {
      start_col = start_col;
      //energyArray[row] = forward;
    } else {
      if (right < forward && right < left) {
        //cout << "1isdb" << endl;
        --start_col;
        //energyArray[row] = right;
      } else if (forward < right && forward < left) {
        //cout << "2nvsdk" << endl;
        start_col = start_col;
        //energyArray[row] = forward;
      } else if (left < right && left < forward){
        //cout << "3jnvjj" << endl;
        ++start_col;
        //energyArray[row] = left;
      } else {
        if (forward < right && forward == left) {
          //cout << "4jhdsk" << endl;
          //++start_col;
          //energyArray[row] = forward;
        } else if (right < left && right == forward) {
          //cout << "5fjvns" << endl;
          start_col = start_col;
          //energyArray[row] = forward;
        } else if (right == left && left < forward) {
          //cout << "6jvnsn" << endl;
          ++start_col;
          //energyArray[row] = left;
        } else {
          cout << "error-error-error" << endl;
        }
      }
    }
    //energyArray[row] = energy(image, start_col, row, width, height);
    totalEnergy += energy(image, start_col, row, width, height);
    seam[row] = start_col;
  }
  /*for (int i = 0; i < height;++i) {
    totalEnergy += energyArray[i];
  }*/
  return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
  cout << image << endl;
  cout << start_row << endl;
  cout << width << endl;
  cout << height << endl;
  cout << seam << endl;
  return 0;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
  int* permSeam = createSeam(height);
  int permEnergy = loadVerticalSeam(image, 0, width, height, permSeam);
  for(int col = 1; col < width; ++col) {
    int* tempSeam = createSeam(height);
    int tempEnergy = loadVerticalSeam(image, col, width, height, tempSeam);
    if (tempEnergy < permEnergy) {
      for(int i =0; i < height; ++i) {
        permSeam[i] = tempSeam[i];
      }
      permEnergy = tempEnergy;
    }
    deleteSeam(tempSeam);
  }

  return permSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
  cout << image << endl;
  cout << width << endl;
  cout << height << endl;
  return nullptr;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
  for (int row = 0; row < height; ++row) {
    for (int col = verticalSeam[row]; col < width -1; ++col){
      image[col][row].r = image[col+1][row].r;
      image[col][row].g = image[col+1][row].g;
      image[col][row].b = image[col+1][row].b;
    }
  }
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
cout << image << endl;
cout << width << endl;
cout << height << endl;
cout << horizontalSeam << endl;
}
