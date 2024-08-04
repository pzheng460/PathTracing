#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
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
#include "Hittable.h"

#include <vector>


class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = AABB(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    AABB bounding_box() const override { return bbox; }

    double pdf_value(const point3& origin, const Vec3& direction) const override {
        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
            sum += weight * object->pdf_value(origin, direction);

        return sum;
    }

    Vec3 random(const point3& origin) const override {
        auto int_size = int(objects.size());
        return objects[random_int(0, int_size-1)]->random(origin);
    }

  private:
    AABB bbox;
};


#endif
