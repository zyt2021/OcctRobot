#ifndef DHINDICATER_H
#define DHINDICATER_H

#include <QVector>
//#include <gp_Trsf.hxx>

#include "occ/AIS_Coordinate.h"

struct DHARG
{
    double theta;
    double d;
    double alpha;
    double a;
};

class DHIndicater
{
public:
    DHIndicater();

    //! compute the coordinates of DH with the arguments of
    //! it. The calculation formula is
    //!
    //!        |cosθi -sinθi•cosαi  sinθi•sinαi ai•cosθi|
    //! |Ai| = |sinθi  cosθi•cosαi -cosθi•sinαi ai•sinθi|
    //!        |  0       sinαi        cosαi       di   |
    //!        |  0          0           0          1   |
    //!
    //! |X|       |0|
    //! |Y| = ∏Ai•|0|
    //! |Z|       |0|
    //! |1|       |1|
    //!
    //! the gp_Trsf class has hide the row (0,0,0,1), but
    //! use it in default when calculating, and the
    //! gp_Trsf::TranslationPart is the coordinate value
    void ComputeFK();

    QVector<Handle(AIS_Coordinate)> GetIndicaters() const {
        return myDHCoord;
    }

private:
    QVector<DHARG> myDHData;
    QVector<Handle(AIS_Coordinate)> myDHCoord;
};

#endif // DHINDICATER_H
