#ifndef _TRANSFORMATION_HPP_
#define _TRANSFORMATION_HPP_

#include "vector2.hpp"

class Transformation {
    private:
        Coordinate position;
        double rotation;
        Vector2<float> scaling;

    public:
        Transformation(const Coordinate thePosition = Coordinate(0, 0), const double theRotation = 0, const Vector2<float> theScaling = Vector2<float>(1, 1) )  { position = thePosition; rotation = theRotation; scaling = theScaling; }

        Coordinate getPosition() const { return position; }
        double getRotation() const { return rotation;  }
        Vector2<float> getScaling() const { return scaling; }

        void setPosition(const Coordinate& theCoordinate) { position = theCoordinate; }
        void setPosition(const unsigned int theX, const unsigned int theY) { position = Coordinate(theX, theY); }
        void move(const Coordinate& theCoordinate) { position = position + theCoordinate; }
        void move(const unsigned int theX, const unsigned int theY) { position = position + Coordinate(theX, theY); }

        void setScale(const Vector2<float> theScaling) { scaling = theScaling; }
        void setScale(const float theX, const float theY) { scaling = Vector2<float>(theX, theY); }
        void scale(const Vector2<float> theScaling) { scaling.x = scaling.x * theScaling.x; scaling.y = scaling.y * theScaling.y; }
        void scale(const float theX, const float theY) { scaling.x = scaling.x * theX; scaling.y = scaling.y * theY; }

        void setRotation(const double theRotation) { rotation = theRotation; }
        void rotate(const double theRotation) { rotation = rotation + theRotation; }

        Coordinate transformCoordinate(const Coordinate& toBeTransformed) const;
        Transformation operator*(const Transformation& theTransformation) const { return Transformation(position + theTransformation.getPosition(), rotation + theTransformation.rotation, scaling * theTransformation.getScaling()); } //combines two transformation
};

#endif // _TRANSFORMATION_HPP_
