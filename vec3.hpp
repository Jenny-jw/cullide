class vec3 {
    private:
        float x;
        float y;
        float z;

    public:
        vec3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        vec3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        vec3 operator*(const float & f) const
        {
            vec3 ret;
            ret.x = f * this->x;
            ret.y = f * this->y;
            ret.z = f * this->z;
            return ret;
        }

        vec3 operator-(const vec3 & vect) const
        {
            vec3 ret;
            ret.x = vect.x - this->x;
            ret.y = vect.y - this->y;
            ret.z = vect.z - this->z;
            return ret;
        }

        friend std::ostream& operator<<(std::ostream & out, const vec3 & vect)
        {
            out << "Vector: (" << vect.x << ", " << vect.y << ", " << vect.z << ")";
            return out;
        }
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color