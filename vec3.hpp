class Vec3 {
    public:
        float x;
        float y;
        float z;

    public:
        Vec3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        Vec3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        Vec3 operator*(const float & f) const
        {
            Vec3 ret;
            ret.x = f * this->x;
            ret.y = f * this->y;
            ret.z = f * this->z;
            return ret;
        }

        Vec3 operator-(const Vec3 & vect) const
        {
            Vec3 ret;
            ret.x = vect.x - this->x;
            ret.y = vect.y - this->y;
            ret.z = vect.z - this->z;
            return ret;
        }

        friend std::ostream& operator<<(std::ostream & out, const Vec3 & vect)
        {
            out << "Vector: (" << vect.x << ", " << vect.y << ", " << vect.z << ")";
            return out;
        }
};

// Type aliases for Vec3
using point3 = Vec3;   // 3D point
using color = Vec3;    // RGB color