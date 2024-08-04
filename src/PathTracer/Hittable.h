#ifndef HITTABLE_H
#define HITTABLE_H
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

#include "AABB.h"


class Material;


class HitRecord {
  public:
    point3 p; // The position of intersection.
    Vec3 normal; // The normal vector at the intersection.
    shared_ptr<Material> mat; // The material of the object that was hit.
    double t; // The time of the hit.
    double u; // The u texture coordinate.
    double v; // The v texture coordinate.
    bool front_face; // Whether the ray hit the front or back face of the object.

    void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


class Hittable {
  public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;

    virtual AABB bounding_box() const = 0;

    virtual double pdf_value(const point3& origin, const Vec3& direction) const {
        return 0.0;
    }

    virtual Vec3 random(const point3& origin) const {
        return Vec3(1, 0, 0);
    }
};


class translate : public Hittable {
  public:
    translate(shared_ptr<Hittable> object, const Vec3& offset)
      : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        // Move the ray backwards by the offset
        Ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    AABB bounding_box() const override { return bbox; }

  private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    AABB bbox;
};


class rotate_y : public Hittable {
  public:
    rotate_y(shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx =  cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where)
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.p;
        p[0] =  cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] =  cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    AABB bounding_box() const override { return bbox; }

  private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    AABB bbox;
};


#endif
