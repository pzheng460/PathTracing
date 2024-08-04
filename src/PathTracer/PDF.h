#ifndef PDF_H
#define PDF_H
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

#include "utils.h"

#include "HittableList.h"
#include "ONB.h"


class PDF {
public:
    virtual ~PDF() {}

    virtual double value(const Vec3& direction) const = 0;
    virtual Vec3 generate() const = 0;
};


class SpherePDF : public PDF {
public:
    SpherePDF() {}

    double value(const Vec3& direction) const override {
        return 1/ (4 * pi);
    }

    Vec3 generate() const override {
        return random_unit_vector();
    }
};


class CosinePDF : public PDF {
public:
    CosinePDF(const Vec3& w) : uvw(w) {}

    double value(const Vec3& direction) const override {
        auto cosine_theta = dot(unit_vector(direction), uvw.w());
        return std::fmax(0, cosine_theta/pi);
    }

    Vec3 generate() const override {
        return uvw.transform(random_cosine_direction());
    }

private:
    ONB uvw;
};


class HittablePDF : public PDF {
public:
    HittablePDF(const Hittable& objects, const point3& origin)
      : objects(objects), origin(origin)
    {}

    double value(const Vec3& direction) const override {
        return objects.pdf_value(origin, direction);
    }

    Vec3 generate() const override {
        return objects.random(origin);
    }

  private:
    const Hittable& objects;
    point3 origin;
};


class MixturePDF : public PDF {
public:
    MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    double value(const Vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    Vec3 generate() const override {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

private:
    shared_ptr<PDF> p[2];
};


#endif
