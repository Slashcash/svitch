#ifndef _VECTOR2_HPP_
#define _VECTOR2_HPP_

template<class T>
class Vector2 {
    public:
        T x;            //the x value of this vector
        T y;            //the y value of this vector

        Vector2() { x = 0; y = 0; }
        Vector2(const T theX, const T theY) { x = theX; y = theY; }

        Vector2 operator+(const Vector2& theVector) const { return Vector2(x+theVector.x, y+theVector.y); }
        Vector2 operator-(const Vector2& theVector) const { return Vector2(x-theVector.x, y-theVector.y); }
        Vector2 operator*(const Vector2& theVector) const { return Vector2(x*theVector.x, y*theVector.y); }
        bool operator==(const Vector2& theVector) const { return ((x == theVector.x) && (y == theVector.y)); }
};

typedef Vector2<unsigned int> Size; //the size on a 2d plane is a vector with two ints
typedef Vector2<int> Coordinate; //and coordinates too


#endif // _VECTOR2_HPP_
