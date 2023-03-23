void processInput(GLFWwindow *window);

const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;"
"void main()\n"
"{\n"
" FragColor = ourColor;\n"
"}\0";

typedef struct color {
    float r;
    float g;
    float b;
} color_t;

/* Vector for colors, points in space and vectors*/
class Vector3 {
    private:
        float x;
        float y;
        float z;

    public:
        Vector3 operator*(const float &) const;
        Vector3 operator-(const Vector3 &) const;
        friend std::ostream& operator<<(std::ostream &, const Vector3 &);
};

/* [TODO] define other utility classes like for instance Vector4
or Ray, which represents a ray through its origin and direction*/

class Image {
    private:

    public:
        bool write(const std::string &) const;

};