#include "Renderer.h"

void Renderer::render_thread(int start, int end, const Hittable& world, const Hittable& lights, Camera& cam, std::vector<color>& frame_buffer) {
    for (int j = start; j < end; j++) {
        {
            std::lock_guard<std::mutex> guard(clog_mutex);
            std::clog << "\rProgress: " << 100 * ++scanlines_progress / cam.image_height << '%' << std::flush;
        }
        for (int i = 0; i < cam.image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s_j = 0; s_j < cam.sqrt_spp; s_j++) {
                for (int s_i = 0; s_i < cam.sqrt_spp; s_i++) {
                    Ray r = cam.get_ray(i, j, s_i, s_j);
                    pixel_color += path_tracing(r, cam.max_depth, world, lights, cam);
                }
            }
            frame_buffer[j * cam.image_width + i] = cam.pixel_samples_scale * pixel_color;
        }
    }
}

void Renderer::render(const Hittable& world, const Hittable& lights, Camera& cam) {
    cam.initialize();

    std::cout << "Image width: " << cam.image_width << std::endl;
    std::cout << "Image height: " << cam.image_height << std::endl;

    std::vector<color> frame_buffer(cam.image_width * cam.image_height);

    scanlines_progress = 0;

    int num_threads = std::thread::hardware_concurrency();

    std::cout << "Number of threads: " << num_threads << std::endl;

    std::vector<std::thread> threads;
    int scanlines_per_thread = cam.image_height / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int start = t * scanlines_per_thread;
        int end = (t == num_threads - 1) ? cam.image_height : (t + 1) * scanlines_per_thread;
        threads.emplace_back(&Renderer::render_thread, this, start, end, std::ref(world), std::ref(lights), std::ref(cam), std::ref(frame_buffer));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::ofstream image_file("image.ppm");

    if (!image_file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    image_file << "P3\n" << cam.image_width << ' ' << cam.image_height << "\n255\n";
    for (const auto& pixel : frame_buffer) {
        write_color(image_file, pixel);
    }

    image_file.close();

    std::clog << "\rDone.                 \n";
}

color Renderer::path_tracing(const Ray& r, int depth, const Hittable& world, const Hittable& lights, Camera& cam) const {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    HitRecord rec;

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, Interval(0.001, infinity), rec))
        return cam.background;

    ScatterRecord srec; // Scatter record for the material
    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

    // If the material does not scatter, return the emitted color.
    if (!rec.mat->scatter(r, rec, srec))
        return color_from_emission;

    // If the material is a light, return the emitted color.
    if (srec.skip_pdf) {
        return srec.attenuation * path_tracing(srec.skip_pdf_ray, depth - 1, world, lights, cam);
    }

    auto light_ptr = make_shared<HittablePDF>(lights, rec.p);
    MixturePDF p(light_ptr, srec.pdf_ptr);

    Ray scattered = Ray(rec.p, p.generate(), r.time());
    auto pdf_value = p.value(scattered.direction());

    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

    color sample_color = path_tracing(scattered, depth - 1, world, lights, cam);
    color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_value;

    return color_from_emission + color_from_scatter;
}