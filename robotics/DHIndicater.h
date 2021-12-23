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
    static DHIndicater* Instance();
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
    static void ComputeFK();

    static void SetDHData(const QVector<DHARG>& data);

    static QVector<Handle(AIS_Coordinate)> GetIndicaters() {
        return myDHCoord;
    }

private:
    DHIndicater();
    class CGabor
    {
    public:
        ~CGabor(){
            if (DHIndicater::ptrSelf){
                delete DHIndicater::ptrSelf;
                DHIndicater::ptrSelf = nullptr;
            }
        }
    };
    static DHIndicater* ptrSelf;
    static QVector<DHARG> myDHData;
    static QVector<Handle(AIS_Coordinate)> myDHCoord;
};

#endif // DHINDICATER_H
