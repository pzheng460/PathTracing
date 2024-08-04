#ifndef TEXTURE_H
#define TEXTURE_H
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

#include "Perlin.h"
#include "rtw_stb_image.h"


class Texture {
  public:
    virtual ~Texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};


class SolidColor : public Texture {
  public:
    SolidColor(const color& albedo) : albedo(albedo) {}

    SolidColor(double red, double green, double blue) : SolidColor(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }

  private:
    color albedo;
};


class CheckerTexture : public Texture {
  public:
    CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

    CheckerTexture(double scale, const color& c1, const color& c2)
      : CheckerTexture(scale, make_shared<SolidColor>(c1), make_shared<SolidColor>(c2)) {}

    color value(double u, double v, const point3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

  private:
    double inv_scale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};


class ImageTexture : public Texture {
  public:
    ImageTexture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    rtw_image image;
};


class NoiseTexture : public Texture {
  public:
    NoiseTexture(double scale) : scale(scale) {}

    color value(double u, double v, const point3& p) const override {
        return color(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

  private:
    Perlin noise;
    double scale;
};


#endif
