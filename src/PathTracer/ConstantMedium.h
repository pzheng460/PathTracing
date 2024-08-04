#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H
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

#include "Hittable.h"
#include "Material.h"
#include "Texture.h"


class ConstantMedium : public Hittable {
public:
    ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<Isotropic>(tex))
    {}

    ConstantMedium(shared_ptr<Hittable> boundary, double density, const color& albedo)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<Isotropic>(albedo))
    {}

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord rec1, rec2;

        if (!boundary->hit(r, Interval::universe, rec1))
            return false;

        if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
            return false;

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * std::log(random_double());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = Vec3(1, 0, 0);  // arbitrary
        rec.front_face = true;     // also arbitrary
        rec.mat = phase_function;

        return true;
    }

    AABB bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_function;
};


#endif
