#ifndef SPHERE_H
#define SPHERE_H
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

#include "Hittable.h"
#include "ONB.h"


class Sphere : public Hittable {
  public:
    // Stationary Sphere
    Sphere(const point3& center, double radius, shared_ptr<Material> mat)
      : center1(center), radius(std::fmax(0,radius)), mat(mat), is_moving(false)
    {
        auto rvec = Vec3(radius, radius, radius);
        bbox = AABB(center1 - rvec, center1 + rvec);
    }

    // Moving Sphere
    Sphere(const point3& center1, const point3& center2, double radius,
           shared_ptr<Material> mat)
      : center1(center1), radius(std::fmax(0,radius)), mat(mat), is_moving(true)
    {
        auto rvec = Vec3(radius, radius, radius);
        AABB box1(center1 - rvec, center1 + rvec);
        AABB box2(center2 - rvec, center2 + rvec);
        bbox = AABB(box1, box2);

        center_vec = center2 - center1;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        point3 center = is_moving ? sphere_center(r.time()) : center1;
        Vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    AABB bounding_box() const override { return bbox; }

    double pdf_value(const point3& origin, const Vec3& direction) const override {
        // This method only works for stationary spheres.

        HitRecord rec;
        if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec))
            return 0;

        auto cos_theta_max = std::sqrt(1 - radius*radius/(center1 - origin).length_squared());
        auto solid_angle = 2*pi*(1-cos_theta_max);

        return  1 / solid_angle;
    }

    Vec3 random(const point3& origin) const override {
        Vec3 direction = center1 - origin;
        auto distance_squared = direction.length_squared();
        ONB uvw(direction);
        return uvw.transform(random_to_sphere(radius, distance_squared));
    }

  private:
    point3 center1;
    double radius;
    shared_ptr<Material> mat;
    bool is_moving;
    Vec3 center_vec;
    AABB bbox;

    point3 sphere_center(double time) const {
        // Linearly interpolate from center1 to center2 according to time, where t=0 yields
        // center1, and t=1 yields center2.
        return center1 + time*center_vec;
    }

    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }

    static Vec3 random_to_sphere(double radius, double distance_squared) {
        auto r1 = random_double();
        auto r2 = random_double();
        auto z = 1 + r2*(std::sqrt(1-radius*radius/distance_squared) - 1);

        auto phi = 2*pi*r1;
        auto x = std::cos(phi) * std::sqrt(1-z*z);
        auto y = std::sin(phi) * std::sqrt(1-z*z);

        return Vec3(x, y, z);
    }
};


#endif
