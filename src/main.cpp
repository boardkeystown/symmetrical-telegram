#define _USE_MATH_DEFINES
#include <cmath>
#include <raylib.h>
#include <string>
#include <iostream>
#include <random>
struct RandomRealRangeGen {
    bool alreadySet;
    std::mt19937 randomEngine;
    std::uniform_real_distribution<float> dist;
    RandomRealRangeGen(int seed, float min, float max) : 
        alreadySet(false)
    {
        this->randomEngine = std::mt19937(seed);
        this->dist = std::uniform_real_distribution<float>(min,max);
    }
    void setRange(float min, float max) {
        this->dist = std::uniform_real_distribution<float>(min,max);
    }
    float hit() {
        return this->dist(this->randomEngine);
    }
};
struct RandomRealIntGen {
    std::mt19937 randomEngine;
    std::uniform_int_distribution<int> dist;
    RandomRealIntGen(int seed, int min, int max) {
        this->randomEngine = std::mt19937(seed);
        this->dist = std::uniform_int_distribution<int>(min,max);
    }
    int hit() {
        return this->dist(this->randomEngine);
    }
};

struct PointBox {
    float width;
    float height;
    float orientation_angle;
    Vector2 mid_point;
    Vector2 point1;
    Vector2 point2;
    Vector2 point3;
    Vector2 point4;
    Vector2 point5;
    Vector2 point6;
    float radius;
    Color color;
    RandomRealIntGen rrIntGen;
    RandomRealRangeGen rrRealGen;
    RandomRealRangeGen rrRealGen2;
    PointBox() :
        width(0),
        height(0),
        orientation_angle(0),
        mid_point{0,0},
        point1{0,0},
        point2{0,0},
        point3{0,0},
        point4{0,0},
        point5{0,0},
        point6{0,0},
        radius(10),
        color(BLUE),
        rrIntGen(12345,0,1),
        rrRealGen(12345,0.f,1.f),
        rrRealGen2(12345,0.f,1.f)
    {}
    PointBox(
        float width,
        float height,
        float orientation_angle,
        Vector2 mid_point,
        float radius,
        Color color
    ) :
        width(width),
        height(height),
        orientation_angle(orientation_angle),
        mid_point{mid_point.x,mid_point.y},
        point1{mid_point.x,mid_point.y},
        point2{mid_point.x,mid_point.y},
        point3{mid_point.x,mid_point.y},
        point4{mid_point.x,mid_point.y},
        point5{mid_point.x,mid_point.y},
        point6{mid_point.x,mid_point.y},
        radius(radius),
        color(color),
        rrIntGen(12345,0,1),
        rrRealGen(12345,0.f,1.f),
        rrRealGen2(12345,0.f,1.f)
    {}
    ~PointBox() {}
    void buildBox() {
        auto deg2rad = [](float angle) {
            return angle * (static_cast<float>(M_PI) / 180.0f);
        };
        auto rad2deg = [](float rad) {
            return rad *  (180.0f / static_cast<float>(M_PI));
        };
        auto displaceAndRotatePoint = [deg2rad](
            const Vector2 &origin,
            Vector2 &point,
            float angle,
            float dx,
            float dy
        ) {
            // displace
            Vector2 displaced = { origin.x + dx, origin.y + dy };
            // rotate
            float rad = deg2rad(angle);
            point.x = std::cos(rad) * (displaced.x - origin.x) - 
                      std::sin(rad) * (displaced.y - origin.y) + origin.x;
            point.y = std::sin(rad) * (displaced.x - origin.x) + 
                      std::cos(rad) * (displaced.y - origin.y) + origin.y;
        };
        auto projectPoint = [deg2rad] (
            const Vector2 &origin,
            Vector2 &point,
            float angle,
            float distance
        ) {
            float rad = deg2rad(angle);
            point.x=origin.x;
            point.y=origin.y;
            point.x = point.x + std::cos(rad) * distance;
            point.y = point.y + std::sin(rad) * distance;
        };
        auto distance = [](
            const Vector2 &point1,
            const Vector2 &point2
        ) {
            double d = 0;
            d = std::sqrt(std::pow(point2.x-point1.x,2)+
                          std::pow(point2.y-point1.y,2));
            return static_cast<float>(d);
        };
        auto find_angle = [rad2deg,distance](
            const Vector2 &a,
            const Vector2 &b,
            const Vector2 &c 
        ) {
            double angle = 0;
            std::cout << "distance(c,b)" << distance(c,b)  <<"\n";
            std::cout << "distance(a,b)" << distance(a,b)  <<"\n";
            std::cout << "distance(c,b)/distance(a,b)" << distance(c,b)/distance(a,b)  <<"\n";
            angle = std::acos(distance(c,b)/distance(a,b));
            return rad2deg(static_cast<float>(angle));
        };

        float halfHeight = this->height / 2.0f;
        float halfWidth = this->width / 2.0f;
        // make box
        // top right (w/2,-h/2)
        displaceAndRotatePoint(
            mid_point, point1, orientation_angle, halfWidth, -halfHeight
        );
        // top left (-w/2,-h/2)
        displaceAndRotatePoint(
            mid_point, point2, orientation_angle, -halfWidth, -halfHeight
        );
        // bottom left (-w/2,h/2)
        displaceAndRotatePoint(
            mid_point, point3, orientation_angle, -halfWidth, halfHeight
        );
        // bottom right (w/2,h/2)
        displaceAndRotatePoint(
            mid_point, point4, orientation_angle, halfWidth, halfHeight
        );
        // start point
        projectPoint(mid_point,point5,orientation_angle-90,this->width);

        float a1 = find_angle(point3,point5,mid_point);
        float a2 = find_angle(point1,point5,mid_point);
        float offset = 0.f;
        std::cout << "a1=" << a1 << "\n";
        std::cout << "a2=" << a2 << "\n";

        if (!this->rrRealGen.alreadySet) {
            this->rrRealGen.alreadySet = false;
            this->rrRealGen.setRange(0,a1);
            this->rrRealGen2.setRange(0,a2);
        }
        if (this->rrIntGen.hit()) {
            offset = +(this->rrRealGen.hit());
        } else {
            offset = -(this->rrRealGen2.hit());
        }
        std::cout << "offset" << offset << "\n";
        // end point
        projectPoint(
            point5,point6,(orientation_angle+90)-offset,this->width+this->width
        );
    }
    void draw() {
        std::string angle_text("Angle: ");
        angle_text += std::to_string(this->orientation_angle);
        DrawText(
            angle_text.c_str(),
            25,
            25,
            32,
            RAYWHITE
        );
        DrawCircleV(this->point1,this->radius,BLUE);
        DrawCircleV(this->point2,this->radius,GREEN);
        DrawCircleV(this->point3,this->radius,YELLOW);
        DrawCircleV(this->point4,this->radius,PURPLE);
        DrawCircleV(this->point5,this->radius,WHITE);
        DrawCircleV(this->point6,this->radius,BLUE);
        DrawCircleV(this->mid_point,this->radius,RED);
    }
};

struct Timer {
    float duration;
    float timeElapsed;
    Timer() : 
        duration(0.f),
        timeElapsed(0.f)
    {}
    Timer(
        float duration
    ) : 
        duration(duration),
        timeElapsed(0.f)
    {}
    void updateTime(float time) {
        this->timeElapsed += time;
    }
    bool ready() {
        bool ready = false;
        if (this->timeElapsed >= this->duration) {
            this->timeElapsed = 0.f;
            ready = true;
        }
        return ready;
    }
};

class Game {
private:
//
public:
Game() {}
~Game() { CloseWindow();}
void run() {
    InitWindow(
        800,
        500,
        "GAME"
    );
    SetTargetFPS(60);
    Timer timer(0.1f);
    PointBox pb(100,10,90,Vector2{800/2,500/2},5,RED);
    pb.buildBox();
    while (!WindowShouldClose()) {
        // input
        // update
        timer.updateTime(GetFrameTime());
        if (timer.ready()) {
            pb.orientation_angle = std::fmod<float>(
                pb.orientation_angle+1.f,
                360.f
            );
            pb.buildBox();
        }
        // render
        BeginDrawing();
        ClearBackground(BLACK);
        pb.draw();
        EndDrawing();
    }
}
};

int main() {
    Game game;
    game.run();
    return 0;
}