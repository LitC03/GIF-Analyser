/*
Assignment 1 -  GIF Image Viewer
By Lito Paraskevi Chatzidavari
CID: 01711772
18-02-2021

This cpp file reads an non-animated GIF with no local color table nor extensions of any type.
I have used a Windows PC to write this code and it compiles without errors using the C++11 standard in MinGW.
Once run and after the user has entered the name of the GIF, the output is the same as the one specified in the assignment.

*/

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <complex>
#include <vector>

using namespace std;

struct sorted_bits
{
  int M;
  int cr;
  int sortf;
  int pixel;
};

struct cm_info
{
  int flag;
  int size;
};

struct im_des
{
  int ct_flag;
  int i_flag;
  int sort_flag;
  int reserved;
  int l_ct;
};

struct im_des_result
{
  int lct;
  int lct_size;
  int height;
  int width;
};

void read_header(fstream& pic);
cm_info read_log(fstream& pic);
int to_dec(vector<int>* bin);
int get_size(fstream& pic);
sorted_bits get_others(fstream& pic);
vector<vector<int>> read_gct(fstream& pic,int size);
void print_gct(vector<vector<int>>& ct);
im_des_result read_image_des(fstream& pic);
im_des get_rest_des(fstream& pic);
void read_tbid(fstream& pic, vector<vector<int>>& gct, int width, int height);
vector<vector<int>> create_code_t(int code_size);
vector<int> get_code_stream(vector<int>& bytes,int code_s);
vector<int> get_index_stream(vector<vector<int>>& code_t, vector<int>& code_stream, int width);
void color_channel(vector<int> index_stream, vector<vector<int>> gct, int width, int color);


int main()
{
  string file_to_open;
  int x, y, trash;
  cm_info info;
  im_des_result result;
  vector<vector<int>> gct;

  //title of program
  cout << "\nGIF IMAGE VIEWER\n";

  //ask user for name of GIF
  cout << "\nEnter file name: " << endl;

  //receive name of GIF
  cin >> file_to_open ;

  //open GIF using fstream
  fstream pic;
  pic.open(file_to_open, fstream::in | fstream::binary);

  //check if opening was succesful, if not display error message
  if (pic.fail())
    cout << "Input file opening failed" << endl;
  else
    {
      //read header and logical screen descriptor, extracting information from the latter
      read_header(pic);
      info = read_log(pic);

      //if the global color table flag is 1, read the global color table and print it
      if (info.flag == 1)
        {
          gct = read_gct(pic, info.size);
          cout << "Global Color Table:";
          print_gct(gct);
        }

      x = pic.get();

      //if a 33 is found (i.e. there's a graphics control extension, we will skip it)
      //note: there isn't any graphics control extension in the "squares.gif" file so this part of the code is not necessary for reading it
      if (x == 33)
        {
          do
          {
            for (int i=0; i < 8; i++)
            {
              if (i == 6)
              y = pic.get();
              if (i == 7)
              x = pic.get();
              else
              trash = pic.get();
            }
          }while((x != 44)||(y != 0)); //this "do-while" loop will go on until 44 (i.e. ',') is found
        }                  //note that the loop reads the block terminator and the number that comes after it.
                          // By doing this, we eliminate any posibilities of terminating the loop
                          // because of a random 44 inside the extension

        //if a 44 is found, read the image descriptor and return the lct flag, lct size and the dimensions of the image
        if (x == 44)
        result = read_image_des(pic);


        //if the local color table flag is 0, we don't need to read a local color table and so we will read the table based image data
        //using the global color table as the color table
        if (result.lct == 0)
          read_tbid(pic, gct, result.width, result.height);

        //read rest of file until 59 (i.e. the trailer) is found
        do
        {
          x = pic.get();
        }while(x != 59);

        if (x = 59)
            cout << "\n>> read trailer\n" << endl;

            cout << ">> program ended" << endl;

    }
  //close GIF
  pic.close();
  return 0;
}


void read_header(fstream& pic)
{
  char x;
  cout << "\n>> read header\n" << endl;
  cout << "Header:" << endl;

  cout << "signature:\t\t";

  //Print the first 3 bytes of the file (the signature)
  for (int i=0; i < 3; i++)
  {
     x=pic.get();
     cout << x;
  }

  cout << "\nversion:\t\t";

  //Print the next 3 bytes of the file (the version)
  for (int i=0; i < 3; i++)
  {
     x=pic.get();
     cout << x;
  }
  cout << endl;
}

cm_info read_log(fstream& pic)
{
  int width, height, back_i, ratio;
  sorted_bits others;
  cm_info result;

  cout << "\n>> read logical screen descriptor\n" << endl;
  cout << "Logical Screen Descriptor:" << endl;

  //get width and height of canvas and print it
  //note: we can use the same function for both width and height
  // because their information is of the same size and format

  width = get_size(pic);
  height = get_size(pic);

  cout << "width:\t\t\t" << width << endl;
  cout << "height:\t\t\t" << height << endl;

  //get information from the packed byte and print its contents
  others = get_others(pic);
  cout << "global ct flag:\t\t" << others.M << endl;
  cout << "color res:\t\t" << others.cr << endl;
  cout << "sort flag:\t\t" << others.sortf << endl;
  cout << "global ct size:\t\t" << others.pixel << endl;

  //since the background color index and the aspect ratio are all stored in 1 byte we can just use get() to find their integer value

  back_i = pic.get();
  cout << "background color i:\t" << back_i << endl;

  ratio = pic.get();
  cout << "aspect ratio:\t\t" << ratio << endl;


  result.flag = others.M;
  result.size = others.pixel;

  return result;
  //the function returns the global color table flag and its size which will be useful to read the next section of the file
}

int get_size(fstream& pic)
{
  /*This function takes two bytes, converts them into binary and returns the resultant decimal value */

  vector<int> bin;
  int x,y;
  int dec;

  //Get the two bytes (x and y) and find their binary value

  x=pic.get();
  bitset<8> bin_x(x);

  y=pic.get();
  bitset<8> bin_y(y);

  //Fill "bin" vector with the values of x and y
  //note: the least significant bit (LSB) is the first element of the vector
  // this is very uselful for conversion into decimal form.

  for (int i=0; i < 16 ; i++)
  {
    if (i < 8)
      bin.push_back(bin_x[i]);
    else
      bin.push_back(bin_y[i-8]);
  }

  //Convert the binary number into decimal form

  dec = to_dec(&bin);

  return dec;
}

int to_dec(vector<int>* bin)
{
  /* this function takes a vector containing an "inverted" binary number and converts it into a decimal number*/

  int sum = 0;
  int size = bin->size();

  for (int i=0; i < size; i++)
     sum = sum + (*bin)[i]*pow(2, i);
  return sum;
}

sorted_bits get_others(fstream& pic)
{
  int x;
  sorted_bits result;
  vector<int> pix_bin, cr_bin;
  int pix_dec, cr_dec;

  //get character in binary
  x=pic.get();
  bitset<8> bin_x(x);

  //bits number 3 and 7 are either 1 or 0 so we can put them in our struct
  //without converting them to decimal

  result.sortf = bin_x[3];
  result.M = bin_x[7];

  //get decimal value of the other 2 parameters and put them in our struct
  for (int i=0; i < 3 ; i++)
  {
    pix_bin.push_back(bin_x[i]);
    cr_bin.push_back(bin_x[i+4]);
  }

  pix_dec = to_dec(&pix_bin);
  cr_dec = to_dec(&cr_bin);

  result.pixel = pix_dec;
  result.cr = cr_dec;

  return result;
}

vector<vector<int>> read_gct(fstream& pic,int size)
 {
   int x,rows;
   vector<vector<int>> ct;
   vector<int> i_color;

   //the actual size of the global color table is 2^(the number we found in the logical screen descriptor + 1)
   size = pow(2,size+1);

   cout << "\n>> read global color table\n" << endl;

   //get index of color, then the three bytes coresponding to the RGB intensities and add them to the global color table
   for (int i=0; i < size; i++)
   {
     i_color.push_back(i);
     for (int k=0; k < 3; k++)
     {
       x = pic.get();
       i_color.push_back(x);
     }
     ct.push_back(i_color);
     i_color.clear();
   }

   return ct;
 }

void print_gct(vector<vector<int>>& ct)
{
  /*This functions prints a vector containing vectors and adds "#" before each row*/
  int rows = ct[0].size();

  for (int i=0; i < ct.size(); i++)
  {
    for (int j=0; j < rows; j++)
    {
      if (j == 0)
        cout << "\n#";

      cout << ct[i][j] << "\t";
    }
  }
  cout << endl;
}

im_des_result read_image_des(fstream& pic)
{
  /*this function gets all the information from the image descriptor, displays it on the console and
  returns the local color table flag, the local color table size and the image dimensions*/

  int left, top, width, height;
  im_des rest;
  im_des_result result;
  cout << "\n>> read image descriptor\n" << endl;
  cout << "Image Descriptor:" << endl;

  left = get_size(pic);
  top = get_size(pic);
  width = get_size(pic);
  height = get_size(pic);

  cout << "left position:\t\t" << left << endl;
  cout << "top position:\t\t" << top << endl;
  cout << "image width:\t\t" << width << endl;
  cout << "image height:\t\t" << height << endl;

  rest = get_rest_des(pic);
  cout << "local ct flag:\t\t" << rest.ct_flag << endl;
  cout << "interlace flag:\t\t" << rest.i_flag << endl;
  cout << "sort flag:\t\t" << rest.sort_flag << endl;
  cout << "reserved:\t\t" << rest.reserved << endl;
  cout << "local ct size:\t\t" << rest.l_ct << endl;

  result.lct = rest.ct_flag;
  result.lct_size = rest.l_ct;
  result.width = width;
  result.height = height;

  return result;
}

im_des get_rest_des(fstream& pic)
{
  /*this function takes the packed byte of the image descriptor as an integer, converts it
  into binary form and returns a struct with all the information contained in it*/
  int x;
  im_des result;
  vector<int> res_bin, size_bin;
  int res_dec, size_dec;

  x=pic.get();
  bitset<8> bin_x(x);

  result.ct_flag = bin_x[7];
  result.i_flag = bin_x[6];
  result.sort_flag = bin_x[5];

  for (int i=0; i < 2 ; i++)
  {
    res_bin.push_back(bin_x[i+3]);
  }
  for (int i=0; i < 3 ; i++)
  {
    size_bin.push_back(bin_x[i]);
  }

  res_dec = to_dec(&res_bin);
  size_dec = to_dec(&size_bin);

  result.reserved = res_dec;
  result.l_ct = size_dec;
  return result;
}

void read_tbid(fstream& pic, vector<vector<int>>& gct, int width, int height)
{
  int x,y;
  int block_size;
  vector<vector<int>> code_t;
  vector<int> index_stream;
  vector<int> bytes, code_stream;

  cout << "\n>> read table based image data\n" << endl;
  cout << "Table Based Image Data:\n" << endl;

  //get LZW minimum code size and display it on console
  x = pic.get();

  cout << "LZW Minimum Code Size:\t" << x << endl;

  //print color table (in this case it's the global color table)
  cout << "\nColor Table:";
  print_gct(gct);

  //create the initialized code table where the two last elements have the values -1 and -2. Then, print the table
  code_t = create_code_t(gct.size());
  cout << "\nInitial Code Table:";
  print_gct(code_t);

  //get block size
  block_size = pic.get();
  cout << "\nBlock Bytes:" << endl;
  cout << "block size:  " << block_size << endl;

  //get the bytes and save them in vector called "bytes". While we are adding
  //the elements in the vector, we are also displaying them on the console
  for (int i=0; i < block_size; i++)
  {
    y = pic.get();
    bytes.push_back(y);
    cout << "byte #" << i << ":\t" << bytes[i] << endl;
  }

  //get code stream and display it on the console
  code_stream = get_code_stream(bytes, x);
  cout << "\nCode Stream:" << endl;
  for (int i=0; i < code_stream.size(); i++)
  {
    cout << "code #" << i << ":\t" << code_stream[i] << endl;
  }

  //get index stream and display its contents (this is done in 1 funcion)
  index_stream = get_index_stream(code_t, code_stream, width);

  //print RGB values for the image
  color_channel(index_stream, gct, width, 1);
  color_channel(index_stream, gct, width, 2);
  color_channel(index_stream, gct, width, 3);
}

vector<vector<int>> create_code_t(int code_size)
{
  vector<vector<int>> code_t;
  vector<int> rows;

  for (int i = 0; i < code_size+2; i++)
  {
    //enumarate each code
    rows.push_back(i);

    //make the last code be -2
    if (i == code_size+1)
    rows.push_back(-2);

    //make the second-to-last code be -1
    else if (i == code_size)
    rows.push_back(-1);

    else
    rows.push_back(i);

    code_t.push_back(rows);

    rows.clear();
  }
  return code_t;
}

vector<int> get_code_stream(vector<int>& bytes, int code_s)
{
  /*This function takes the bytes found before, converts them to binary and creates a vector ("super") made of all the bits of these bytes.
  The function then reads these bits in groups of the number contained in "first_code_s".
  Every time a number is read, it is added to a vector called "code_stream"*/

  // count2 counts how many numbers have been read in groups of X bits and is set to 0 when 2^(X-1) numbers have been read
  int count2;

  //count keeps track of how many bits have been read
  int count;
  int first_code_s, read_dec;
  vector<int> super;
  vector<int> code_stream, read_bin;

  first_code_s = pow(2, code_s)-1;

  //create the vector containing all the bits that are going to be read
  for (int i=0; i < bytes.size(); i++)
  {
    bitset<8> bin_x(bytes[i]);
    for (int j=0; j < 8 ; j++)
    {
        super.push_back(bin_x[j]);
    }
  }

  //We know that the first number is going to be read by using 2^(2-1)= 3 bits,
  //so we get its value and add it to the code stream
  for (int i=0; i < first_code_s; i++)
   {
     read_bin.push_back(super[i]);
   }
   read_dec = to_dec(&read_bin);
   code_stream.push_back(read_dec);
   read_bin.clear();

   //3 bits have been read, and 1 element has been read with 3 bits
   count = 3;
   count2 = 1;

   //this loop will go on until all the bits in the vector "super" have been read
   do
   {
     //Read "first_code_s" bits (this value will be 3, 4, 5...) and convert them to decimal
     for (int i=0; i < first_code_s; i++)
    {
      read_bin.push_back(super[i+count]);
    }
    read_dec = to_dec(&read_bin);
    read_bin.clear();

    //add the decimal number to the code stream
    code_stream.push_back(read_dec);

    //keep track of how many bits have been read
    count = count + first_code_s;

    //keep track of how many numbers have been read with X bits (X being the number of bits used to read a number)
    count2 = count2 + 1;

    //If 2^(X-1) numbers have been read in groups of X bits, we have to increase X by 1.
    //We also need to reset count2
    if (count2 == pow(2, first_code_s-1))
       {
         first_code_s = first_code_s + 1;
         count2 = 0;
       }
 }while(count < super.size());

  code_stream.pop_back();
  return code_stream;
}

vector<int> get_index_stream(vector<vector<int>>& code_t, vector<int>& code_stream, int width)
{
  int x, y, a;
  int z;
  int  eoi = code_t[code_t.size()-1][0];
  int cc = code_t[code_t.size()-2][0];
  int count;
  int x_pos;
  vector<int> index_stream;
  vector<int> row;

  //check that the first number of the code stream is "clear code"
  if (code_stream[0] == cc)
    {
      for (int i=1; i <= code_stream.size(); i++)
      {
        //add the second number of the code stream in the index stream
        if (i == 1)
          index_stream.push_back(code_t[code_stream[i]][0]);

        //for the rest of codes we enter the loop
        else
          {
            //let "x" be the code read before and "y" the code we are currently reading
            x = code_stream[i-1];
            y = code_stream[i];

            //if end of information code is found, finalise the loop
            if (y==eoi)
              break;

            // find the position of x in the code table
            z = -1;
            do{
              z++;
              x_pos = z;
            }while(x != code_t[z][0]);

            //try to find y in the code table
            for (int j=0; j < code_t.size(); j++)
               {
                // if y is in the code table
                if (y == code_t[j][0])
                {
                  //output the contents of y in the index stream
                  for (int k=0; k < code_t[j].size()-1; k++)
                    {
                    index_stream.push_back(code_t[j][k+1]);
                    }

                  //Let "a" be the first index of the contents of y
                  a = code_t[j][1];

                  //create a new code in the code table

                  //create the first element of the new row of the code table
                  row.push_back(code_t.size());

                  //add the contents of y + "a" into the new row
                  for (int k=0; k < code_t[x_pos].size()-1; k++)
                    {
                    row.push_back(code_t[x_pos][k+1]);
                    }
                  row.push_back(a);

                  //put the new row in the code table
                  code_t.push_back(row);

                  //clear the vector used as a row for future use
                  row.clear();

                  //exit the for loop where we were looking if "y" was in the code table
                  break;
                }
                // if we have gotten to the end of the loop and see that "y" is not the last code of the code table
                //it means that "y" is not in the code table
                else
                {
                  if (j ==code_t.size()-1)
                  {
                    if (y != code_t[j][0])
                    {
                      //Let "a" be the first index of the contents of x
                      a = code_t[x_pos][1];

                      //create the first element of the new row of the code table
                      row.push_back(code_t.size());

                      //output the contents of "x" in the index stream
                      //the contents of "x" will also be added to our row vector
                      for (int k=0; k < code_t[x_pos].size()-1; k++)
                        {
                        index_stream.push_back(code_t[x_pos][k+1]);
                        row.push_back(code_t[x_pos][k+1]);
                        }

                      //output "a" to the index stream
                      index_stream.push_back(a);

                      //add "a" to our row vector
                      row.push_back(a);

                      //put the new row in the code table and clear the row vector for future use
                      code_t.push_back(row);
                      row.clear();

                      //exit "for" loop
                      break;
                    }
                  }
                }

          }
        }
      }
    }
  //If the first element of the code stream is not clear code, print error message
  else
    cout << "There is a problem with the code_stream" << endl;

    //Display the contents of the index stream
    cout << "\nIndex Stream:" << endl;
    cout << "i_stream size: " << index_stream.size() << endl;

    // the indexes are displayed in lines of ten (since ten is the width of the image)
    count = 0;
    for (int j=0; j < index_stream.size(); j++)
      {
        count++;
        cout << index_stream[j];
        if (count==width)
        {
          count=0;
          cout << "\n";
        }
      }

  return index_stream;
}

void color_channel(vector<int> index_stream, vector<vector<int>> gct, int width, int color)
{
  /*Depending on the number of the variable "color", the intesities of red, green or blue will be dispayed for each index.
  red -> color == 1
  green -> color == 2
  blue -> color == 3 */

  vector<int> image;
  int count;

  //create a vector with all the intesities of red/green/blue for each index
  for (int i=0; i < index_stream.size(); i++)
  {
    image.push_back(gct[index_stream[i]][color]);
  }

  if (color == 1)
  cout <<"\nImage Data - Red Channel" << endl;
  else if (color == 2)
  cout <<"\nImage Data - Green Channel" << endl;
  else if (color == 3)
  cout <<"\nImage Data - Blue Channel" << endl;
  else
  cout << "\nError, invalid color number" << endl;

  //Display the intensities as they would appear in the image
  count = 0;

  for (int j=0; j < image.size(); j++)
    {
      count++;
      cout << image[j] << "\t";
      if (count==width)
      {
        count=0;
        cout << "\n";
      }
    }
}
