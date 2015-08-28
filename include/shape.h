#include <math.h>
#include <vector>
#include <iostream>
#include <utility>
#include <tuple>
using namespace std;
//TODO Remove isvalid and draw
//TODO add function to return the points of the object
//TODO change compound objects to generate the original objects at a given angle then translate
//

using std::vector;
using std::pair;
class shape {
public:
    std::pair<double,double> location{};
    double x(){return location.first;};
    double y(){return location.second;};
    double theta; // Location
    virtual void setLocation(double x, double y, double theta) {
        auto temp=make_pair(x,y);
        swap(location,temp);
        this->theta = theta;
    }

    virtual vector <pair<int, int>> getPoints() {return vector<pair<int,int>>{};};

    shape(double x, double y, double theta) {
        auto temp = make_pair(x,y);
        swap(location,temp);
        this->theta = (theta / 180.0) * M_PI;
    }

    shape() { }
};

class Line : public shape {
    double L;
    double N;// Number of sample points
public:
    std::pair<int, int> start;
    std::pair<int, int> end;

    Line() { };

    Line(double x, double y, double theta, double L) : shape(x, y, theta) {
        this->L = L;
        this->N = L * 5;
        auto new_start=make_pair(static_cast<int>(x),static_cast<int>(y));
        swap(start,new_start);
        auto new_end=make_pair(static_cast<int>(L * cos(this->theta) +start.first + 0.5),
                static_cast<int>(L * sin(this->theta) + start.second+ 0.5));
        swap(end,new_end);
    }

    void Set(double x, double y, double theta, int L) {

        auto temp=make_pair(x,y);
        swap(location,temp);
        this->theta = (theta / 180.0) * M_PI;
        this->L = L;
        this->N = L * 5;


        auto new_start=make_pair(static_cast<int>(x),static_cast<int>(y));
        swap(start,new_start);
        auto new_end=make_pair(static_cast<int>(L * cos(this->theta) +start.first + 0.5),
                               static_cast<int>(L * sin(this->theta) + start.second+ 0.5));
        swap(end,new_end);
    }

    virtual vector <pair<int, int>> getPoints() {
        vector <pair<int, int>> points;
        double x_increment = L * cos(theta) / N;
        double y_increment = L * sin(theta) / N;
        int x_plot,y_plot;
        for (int i = 0; i < N; i++) {
            x_plot = static_cast<int>(x_increment * i + x() + 0.5);
            y_plot = static_cast<int>(x_increment * i + y() + 0.5);
            points.emplace_back(x_plot, y_plot);
        }
        return points;
    };
};


class Arc : shape {
    double arc_angle;
    double radius;
    double N;
public:
    Arc() { };

    Arc(int x, int y, double theta, double radius, double arc_angle) : shape(x, y, theta) {
        this->arc_angle = arc_angle;
        this->radius = radius;
        this->N = arc_angle * radius * 5;
    }

    void Set(int x, int y, double theta, double radius, double arc_angle) {
        auto temp=make_pair(static_cast<double>(x),static_cast<double>(y));
        swap(location,temp);
        this->theta = (theta / 180.0) * M_PI;
        this->radius = radius;
        this->arc_angle = (arc_angle / 180.0) * M_PI;
        this->N = arc_angle * radius * 5;
    }

    virtual vector <pair<int, int>> getPoints() {
        int x_plot,y_plot;
        vector <pair<int, int>> points;
        double radian_increment = arc_angle/N;
        double radians = this->theta;
    for(int i=0; i<N;i++){
        x_plot = static_cast<int>(x()+radius*cos(radians+radian_increment*i) +0.5);
        y_plot = static_cast<int>(y()+radius*sin(radians+radian_increment*i)+0.5);
        points.emplace_back(x_plot, y_plot);
    }
        return points;
    };

};

class rhombus : public shape {
    double corner_angle;
    double width, height;
    Line a, b, c, d;

public:
    rhombus() { }

    rhombus(int x, int y, double theta, int width, int height, double corner_angle) : shape(x, y, theta) {
        this->theta = theta;
        this->corner_angle = corner_angle;
        this->width = width;
        this->height = height;
        a.Set(x, y, theta, width);
        b.Set(a.end.first, a.end.second, this->theta + this->corner_angle, height);
        c.Set(b.end.first, b.end.second, theta + 180, width);
        d.Set(c.end.first, c.end.second, this->theta + this->corner_angle + 180, height);

    }

    void Set(int x, int y, double theta, int width, int height, double corner_angle) {

        this->theta = theta;
        this->corner_angle = corner_angle;
        this->width = width;
        this->height = height;
        a.Set(x, y, theta, width);
        b.Set(a.end.first, a.end.second, this->theta + this->corner_angle, height);
        c.Set(b.end.first, b.end.second, theta + 180, width);
        d.Set(c.end.first, c.end.second, this->theta + this->corner_angle + 180, height);

    }

    void setParams(double width, double height, double corner_angle) {
        this->corner_angle = corner_angle;
        this->width = width;
        this->height = height;

    }

    void Init() {
        a.Set(x(), y(), theta, width);
        b.Set(a.end.first, a.end.second, this->theta + this->corner_angle, height);
        c.Set(b.end.first, b.end.second, theta + 180, width);
        d.Set(c.end.first, c.end.second, this->theta + this->corner_angle + 180, height);

    }

    virtual vector <pair<int, int>> getPoints() {return shape::getPoints();};
    /*virtual void drawOutline(unsigned char *image, int width, int height) {
        Init();
        a.drawOutline(image, width, height);
        b.drawOutline(image, width, height);
        c.drawOutline(image, width, height);
        d.drawOutline(image, width, height);

    }

    virtual bool isValidLocation(unsigned char *image, int width, int height) {
        Init();
        return a.isValidLocation(image, width, height) &&
               b.isValidLocation(image, width, height) &&
               c.isValidLocation(image, width, height) &&
               d.isValidLocation(image, width, height);
    }*/
};

class lens : public shape {
    Arc A;
    Arc B;
    int radius;
    double angle;
    int V;
    pair<double,double> v_unit;
    double separation;
public:
    lens() { }

    lens(int x, int y, double theta, double radius, double angle) : shape(x, y, theta) {
        this->radius = radius;
        this->theta = theta;
        this->angle = angle;
        separation = 2 * radius * cos(angle / 2);
        v_unit.first = cos(theta + angle / 2.0);
        v_unit.second = sin(theta + angle / 2.0);
        A.Set(static_cast<int>(this->x()), static_cast<int>(this->y()), theta, angle, radius);
        B.Set(static_cast<int>(this->x())+ v_unit.first * separation,
              static_cast<int>(this->y()) + v_unit.second * separation,
              theta + 180, angle, radius);
    }

    void Set(int x, int y, double theta, double radius, double angle) {

        this->radius = radius;
        this->theta = theta;
        this->angle = angle;


    }

    void Init() {
        separation = 2 * radius * cos(((angle / 180.0) * M_PI) / 2.0);
        v_unit.first = cos((theta / 180.0) * M_PI + ((angle / 180.0) * M_PI) / 2.0);
        v_unit.second = sin((theta / 180.0) * M_PI + ((angle / 180.0) * M_PI) / 2.0);
        A.Set(x(), y(), theta, radius, angle);
        B.Set(x() + v_unit.first * separation, y() + v_unit.second * separation, theta + 180, radius, angle);


    }

    void setParams(double radius, double arc_angle) {
        this->radius = radius;
        this->angle = arc_angle;


    }
    virtual vector <pair<int, int>> getPoints() {return shape::getPoints();};

    /*void drawOutline(unsigned char *image, int width, int height) {
        Init();
        A.drawOutline(image, width, height);
        B.drawOutline(image, width, height);
    }

    bool isValidLocation(unsigned char *image, int width, int height) {
        Init();
        return (A.isValidLocation(image, width, height) && B.isValidLocation(image, width, height));

    }*/
};

class capsule : public shape {
    Arc A, B;
    Line C, D;
    int width;
    int height;
public:
    capsule() { };
    capsule(double x, double y, double theta, int width, int height) {
        auto temp=make_pair(x,y);
        swap(location,temp);
        this->theta = theta;
        this->width = width;
        this->height = height;
        C.Set(x, y, (theta / 180.0) * M_PI, width);
        D.Set(
                x + width * cos(((theta / 180.0) * M_PI)) - height * sin(((theta / 180.0) * M_PI)),
                y + width * sin((theta / 180.0) * M_PI) + height * cos(((theta / 180.0) * M_PI)),
                theta + 180, width);
        A.Set(
                x + width * cos((theta / 180.0) * M_PI) - height / 2 * sin((theta / 180.0) * M_PI),
                y + width * sin((theta / 180.0) * M_PI) + height / 2 * cos((theta / 180.0) * M_PI),
                theta - 90, height / 2.0, 180);
        B.Set(
                x - height / 2 * sin((theta / 180.0) * M_PI),
                y + height / 2 * cos((theta / 180.0) * M_PI),
                theta + 90, height / 2.0, 180);

    }

    void Init() {
        C.Set(x(), y(), theta, width);
        D.Set(
                x() + width * cos(((theta / 180.0) * M_PI)) - height * sin(((theta / 180.0) * M_PI)),
                y() + width * sin((theta / 180.0) * M_PI) + height * cos(((theta / 180.0) * M_PI)),
                theta + 180, width);

        A.Set(
                x() + width * cos((theta / 180.0) * M_PI) - height / 2 * sin((theta / 180.0) * M_PI),
                y() + width * sin((theta / 180.0) * M_PI) + height / 2 * cos((theta / 180.0) * M_PI),
                theta - 90, height / 2.0, 180);
        B.Set(
                x() - height / 2 * sin((theta / 180.0) * M_PI),
                y() + height / 2 * cos((theta / 180.0) * M_PI),
                theta + 90, height / 2.0, 180);


    }

    void setParams(double width, double height) {
        this->width = width;
        this->height = height;
    }
    virtual vector <pair<int, int>> getPoints() {return shape::getPoints();};

    /*void drawOutline(unsigned char * image,int width,int height){
        Init();
    A.drawOutline(image,width,height);
    B.drawOutline(image,width,height);
    C.drawOutline(image,width,height);
    D.drawOutline(image,width,height);


    }
    bool isValidLocation(unsigned char * image,int width,int height){
        Init();
return (A.isValidLocation(image,width,height)&&
        B.isValidLocation(image,width,height)&&
        C.isValidLocation(image,width,height)&&
        D.isValidLocation(image,width,height));

    }*/
};


