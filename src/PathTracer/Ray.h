#ifndef RAY_H
#define RAY_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "Vec3.h"


class Ray {
public:
    Ray() {}

    Ray(const point3& origin, const Vec3& direction, double time)
      : orig(origin), dir(direction), tm(time) {}

    Ray(const point3& origin, const Vec3& direction)
      : Ray(origin, direction, 0) {}

    const point3& origin() const  { return orig; }
    const Vec3& direction() const { return dir; }

    double time() const { return tm; }

    point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig;
    Vec3 dir;
    double tm;
};


#endif
