
#include "core/ultrasound_soundspeed_reconstruction/imgreader.h"
#include <iostream>
#include <fstream>
#include "eigen/Eigen/Sparse"
#include "eigen/Eigen/Dense"
#include <string>
#include <algorithm>

using namespace std;
using namespace Eigen;

string parser_filename(string filename){

    string ans = "";

    for(int i = filename.size() - 5; i >= 0; i --){

        if (filename[i] == '/') break;
        ans += filename[i];

    }

    reverse(ans.begin(), ans.end());
    //cout<<ans<<endl;
    return ans;
}

VectorXd pixel_matrix(const string file_name)
{

    int size = 0, pixels_address = 0, width = 0, height = 0;
    short int bits_per_pixel = 0;

    ifstream file (file_name, ios::in | ios::binary);

    file.seekg( 2, ios::beg);
    file.read ((char*)&size, sizeof(int));

    file.seekg( 10, ios::beg);
    file.read ((char*)&pixels_address, sizeof(int));

    file.seekg( 18, ios::beg);
    file.read ((char*)&width, sizeof(int));
    file.read ((char*)&height, sizeof(int));

    file.seekg( 28, ios::beg);
    file.read ((char*)&bits_per_pixel, sizeof(short int));

    file.seekg( pixels_address, ios::beg);

    cout << "Size: " << size << endl;
    cout << "pixels_adress: " << pixels_address << endl;
    cout << "bits per pixel: " << bits_per_pixel << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;

    Eigen::VectorXd X(400);
    int map[20][20];
    int pad = (3 * width) % 4;

    if( bits_per_pixel == 24 )
    {
        unsigned int bgr = 0;

        for( int y = 0; y < 20; y++ )
        {
            for( int x = 0; x < 20 ; x++ )
            {
                file.read ((char*)&bgr, 3);

                //cout << bgr << endl;

                if ( bgr == 0x000000 )
                    map[y][x] = 1;
                else
                    map[y][x] = 0;

                bgr = 0;
            }
            file.seekg(pad, ios::cur);
        }
    }
    else
    {
        cout << "Sorry, but your image needs to have 24 bits per pixel" << endl;
        system("PAUSE");
    }


    file.close();

    //ofstream outfile;

    //string file_pixel_matrix = "pixel_matrices/" + parser_filename(file_name) + ".txt";

    //outfile.open(file_pixel_matrix);

    for( int y = 19; y >= 0; y--)
    {
        for( int x = 0; x < 20 ; x++ )
        {

            //outfile<<map[y][x]<<" ";
            X((19-y)*20 + x) = map[y][x];
        }
        //outfile<<"\n";
    }
    //outfile.close();

    return X;
}

