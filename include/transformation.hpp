#ifndef _TRANSFORMATION_HPP_
#define _TRANSFORMATION_HPP_

#include "vector2.hpp"

class Transformation {
    private:
        Coordinate position;
        double rotation;

    public:
        Transformation(const Coordinate thePosition = Coordinate(0, 0), const double theRotation = 0 )  { position = thePosition; rotation = theRotation; }

        Coordinate getPosition() const { return position; }
        double getRotation() const { return rotation;  }

        void setPosition(const Coordinate& theCoordinate) { position = theCoordinate; }
        void setPosition(const unsigned int theX, const unsigned int theY) { position = Coordinate(theX, theY); }
        void move(const Coordinate& theCoordinate) { position = position + theCoordinate; }
        void move(const unsigned int theX, const unsigned int theY) { position = position + Coordinate(theX, theY); }
        void setRotation(const double theRotation) { rotation = theRotation; }
        void rotate(const double theRotation) { rotation = rotation + theRotation; }

        Coordinate transformCoordinate(const Coordinate& toBeTransformed) const;
        Transformation operator* (const Transformation& theTransformation) const { return Transformation(position + theTransformation.getPosition(), rotation + theTransformation.rotation); } //combines two transformation
};

#endif // _TRANSFORMATION_HPP_
