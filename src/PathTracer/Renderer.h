#ifndef PATHTRACING_RENDERER_H
#define PATHTRACING_RENDERER_H

#include <iostream>
#include <fstream>
#include <thread>
#include "Camera.h"

class Renderer {
public:
    std::mutex clog_mutex;  // 用于保护 std::clog 的互斥锁
    std::atomic<int> scanlines_progress; // 用于跟踪扫描行数

    void render_thread(int start, int end, const Hittable& world, const Hittable& lights, Camera& cam, std::vector<color>& frame_buffer);
    void render(const Hittable& world, const Hittable& lights, Camera& cam);

    color path_tracing(const Ray& r, int depth, const Hittable& world, const Hittable& lights, Camera& cam) const;
};


#endif //PATHTRACING_RENDERER_H
