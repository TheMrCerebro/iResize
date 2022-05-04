/*
 *
 * Copyright (c) 2022 TheMrCerebro
 *
 * iRESIZE - Zlib license.
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented;
 * you must not claim that you wrote the original software.
 * If you use this software in a product, an acknowledgment
 * in the product documentation would be appreciated but
 * is not required.
 *
 * 2. Altered source versions must be plainly marked as such,
 * and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any
 * source distribution.
 *
*/

#include <irrlicht.h>
#include <iostream>
#include <string>
#include <algorithm>

using namespace irr;
using namespace core;
using namespace video;
using namespace std;

int main()
{
    cout << "        ___   ______    _______  _______  ___   _______  _______ " << endl;
    cout << "       |   | |    _ |  |       ||       ||   | |       ||       |" << endl;
    cout << "       |   | |   | ||  |    ___||  _____||   | |____   ||    ___|" << endl;
    cout << "       |   | |   |_||_ |   |___ | |_____ |   |  ____|  ||   |___ " << endl;
    cout << "       |   | |    __  ||    ___||_____  ||   | | ______||    ___|" << endl;
    cout << "       |   | |   |  | ||   |___  _____| ||   | | |_____ |   |___ " << endl;
    cout << "       |___| |___|  |_||_______||_______||___| |_______||_______|" << endl;
    cout << " (o)=================================================================(o)" << endl;
    cout << "             - FAST AND EASY IMAGE RESIZER [by TheMrCerebro] -" << endl;
    cout << " (o)=================================================================(o)" << endl;
    cout << endl;
    cout << "     With this tool you can change the size of any image easily and" << endl;
    cout << "    quickly. Upon completion of the scaling process, a new file named" << endl;
    cout << "                'img_resize.bmp will be created." << endl;
    cout << endl;

    cout << " File: ";

    std::string path;
    getline(cin,path);

    // Elimina las comillas (") de la ruta del archivo
    path.erase(std::remove(path.begin(), path.end(), '\"'), path.end());

    cout << " Resize Width: ";
    u32 rWidth;
    cin >> rWidth;

    cout << " Resize Height: ";
    u32 rHeight;
    cin >> rHeight;

    cout << " Filter:" << endl;
    cout << "   (1) - Nearest" << endl;
    cout << "   (2) - Bilinear" << endl;
    u32 filter;
    cin >> filter;

    cout << endl;

    IrrlichtDevice *device = createDevice(EDT_OPENGL, core::dimension2du(1,1), 32);

    video::IVideoDriver* driver = device->getVideoDriver();

    //
    video::ITexture* img = driver->getTexture(path.c_str());

    u32 oWidth = img->getSize().Width, oHeight = img->getSize().Height;

    u32 *p = (u32*)img->lock();
    img->unlock();

    cout << " Converting... " << endl;

    video::IImage* imgw = driver->createImage(video::ECF_A8R8G8B8, core::dimension2du(rWidth,rHeight));

    u32 *p2 = (u32*)imgw->lock();

    if(filter==2)
    {
        // RESIZE BILINEAR FILTER
        // https://tech-algorithm.com/articles/bilinear-image-scaling/

        int a, b, c, d, x, y, index;
        float x_ratio = ((float)(oWidth-1))/rWidth;
        float y_ratio = ((float)(oHeight-1))/rHeight;
        float x_diff, y_diff, blue, red, green;
        int offset = 0;

        for(u32 i=0; i<rHeight; i++)
        {
            for(u32 j=0; j<rWidth; j++)
            {
                x = (int)(x_ratio * j);
                y = (int)(y_ratio * i);
                x_diff = (x_ratio * j) - x;
                y_diff = (y_ratio * i) - y;
                index = (y*oWidth+x);
                a = p[index];
                b = p[index+1];
                c = p[index+oWidth];
                d = p[index+oWidth+1];

                // blue element
                // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
                blue = (a & 0xff) * (1-x_diff) * (1-y_diff) + (b & 0xff) * (x_diff) * (1-y_diff) + (c & 0xff)*(y_diff)*(1-x_diff) + (d & 0xff) * (x_diff * y_diff);

                // green element
                // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
                green = ((a >> 8) & 0xff) * (1-x_diff) * (1-y_diff) + ((b >> 8) & 0xff) * (x_diff) * (1-y_diff) + ((c >> 8) & 0xff) * (y_diff) * (1-x_diff) + ((d >> 8) & 0xff) * (x_diff * y_diff);

                // red element
                // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
                red = ((a >> 16) & 0xff) * (1-x_diff) * (1-y_diff) + ((b >> 16) & 0xff) * (x_diff) * (1-y_diff) + ((c >> 16) & 0xff) * (y_diff) * (1-x_diff) + ((d >> 16) & 0xff) * (x_diff * y_diff);

                p2[offset++] = 0xff000000 | ((((int)red) << 16) & 0xff0000) | ((((int)green) << 8) & 0xff00) | ((int)blue) ;
            }
        }
    }
    else
    {
        // RESIZE NEAREST FILTER
        // https://tech-algorithm.com/articles/nearest-neighbor-image-scaling/

        u32 x_ratio = (u32)((oWidth << 16) / rWidth) + 1;
        u32 y_ratio = (u32)((oHeight << 16) / rHeight) + 1;

        for(u32 i=0; i<rHeight; i++)
        {
            for(u32 j=0; j<rWidth; j++)
            {
                u32 x2 = (j * x_ratio) >> 16;
                u32 y2 = (i * y_ratio) >> 16;
                p2[(i * rWidth) + j] = p[(y2 * oWidth) + x2];
            }
        }
    }

    imgw->unlock();

    cout << endl;

    if(driver->writeImageToFile(imgw,"img_resize.bmp"))
        cout << " Image saved" << endl;

    device->closeDevice();
    device->drop();

    cout << endl;
    system("pause");
    return 0;
}
