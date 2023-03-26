#include <fstream>
#include "vec3.hpp"

class Image {
    private:
        int image_width;
        int image_height;
        color* pixels;

    public:
        Image()
        {
            this->image_width = 0;
            this->image_height = 0;
        }

        Image(const int w, const int h)
        {
            this->image_width = w;
            this->image_height = h;
            this->pixels = new color[w * h];

            for (int i = 0; i < w * h; i++){
                this->pixels[i] = color(0.0f,0.0f,0.0f);
            }
        }

        /* Save the image (series of pixels whose color is represented by the Vec3) on disk */
        bool write(const std::string & filename) const
        {
            std::ofstream imageFile;
            imageFile.open(filename);

            imageFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
            for (int i = 0; i < image_width * image_height; i++){
                imageFile << static_cast<int>(pixels[i].x) << ' '
                 << static_cast<int>(pixels[i].y) << ' ' 
                 << static_cast<int>(pixels[i].z) << std::endl;
            }
           
            imageFile.close();
            return true;
        }
};