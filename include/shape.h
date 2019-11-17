#include <assert.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;

using std::pair;
using std::vector;
class shape {
protected:
  mutable vector<pair<int, int>> points;

public:
  std::pair<double, double> location{};
  std::pair<double, double> relative;
  double x() const { return location.first + relative.first; };
  double y() const { return location.second + relative.second; };
  pair<double, double> rel() { return relative; }
  double theta;

  // All of our shapes have to translate around the image
  // we use this function to translate them
  // @x @y
  // We compare to the original location and store a relative location
  // @theta
  // in the case of our shapes without a rotation cache we don't do any changes
  // of theta at all
  virtual void setLocation(double x, double y, double theta) {
    relative.first = x - location.first;
    relative.second = y - location.second;
    this->theta = (theta / 180.0) * M_PI;
  }

  virtual const vector<pair<int, int>> &getPoints() const { return points; };
  vector<pair<int, int>> consumePoints() { return std::move(points); }

  shape(double x, double y, double theta) {
    location = make_pair(static_cast<int>(x), static_cast<int>(y));
    this->theta = (theta / 180.0) * M_PI;
  }

  shape() {}
};

class Line : public shape {
private:
  double L;
  double N; // Number of sample points

public:
  std::pair<int, int> start;
  std::pair<int, int> end;

  Line(){};
  Line(double x, double y, double theta, double L) : shape(x, y, theta) {
    this->L = L;
    this->N = L * 5;
    auto new_start = make_pair(static_cast<int>(x), static_cast<int>(y));
    swap(start, new_start);
    auto new_end =
        make_pair(static_cast<int>(L * cos(this->theta) + start.first + 0.5),
                  static_cast<int>(L * sin(this->theta) + start.second + 0.5));
    swap(end, new_end);
  }

  const vector<pair<int, int>> &getPoints() const {
    if (points.empty() == true) {
      double x_increment = L * cos(theta) / N;
      double y_increment = L * sin(theta) / N;
      int x_plot, y_plot;
      for (int i = 0; i < N; i++) {
        x_plot = static_cast<int>(x_increment * i + x() + 0.5);
        y_plot = static_cast<int>(y_increment * i + y() + 0.5);
        points.emplace_back(x_plot, y_plot);
      }
    }
    return points;
  };
};

class Arc : shape {
public:
  double arc_angle;
  double radius;
  double N;
  Arc(){};
  Arc(int x, int y, double theta, double radius, double arc_angle)
      : shape(x, y, theta) {
    this->arc_angle = (arc_angle / 180) * M_PI;
    this->radius = radius;
    this->N = arc_angle * radius * 5;
  }

  void Print() const {
    cout << "Location()" << x() << "," << y() << "\n";
    cout << "Arc Angle(Radians):" << arc_angle << "\n";
    cout << "Radius:" << radius << "\n";
    cout << "Theta" << theta << "\n";
  }

  const vector<pair<int, int>> &getPoints() const {
    int x_plot, y_plot;
    double radian_increment = arc_angle / N;
    double radians = this->theta;
    for (int i = 0; i < N; i++) {
      x_plot = static_cast<int>(
          x() + radius * cos(radians + radian_increment * i) + 0.5);
      y_plot = static_cast<int>(
          y() + radius * sin(radians + radian_increment * i) + 0.5);
      points.emplace_back(x_plot, y_plot);
    }
    return points;
  };
};

class rhombus : public shape {
  double corner_angle;
  double width, height;
  Line A, B, C, D;

public:
  rhombus(int x, int y, double theta, int width, int height,
          double corner_angle)
      : shape(x, y, theta) {
    this->theta = theta;
    this->corner_angle = corner_angle;
    this->width = width;
    this->height = height;
    A = Line(x, y, theta, width);
    B = Line(A.end.first, A.end.second, this->theta + this->corner_angle,
             height);
    C = Line(B.end.first, B.end.second, theta + 180, width);
    D = Line(C.end.first, C.end.second, this->theta + this->corner_angle + 180,
             height);
  }

  const vector<pair<int, int>> &getPoints() const {
    if (points.empty()) {
      auto a = A.getPoints(), b = B.getPoints(), c = C.getPoints(),
           d = D.getPoints();
      points.assign(a.begin(), a.end());
      points.insert(points.end(), b.begin(), b.end());
      points.insert(points.end(), c.begin(), c.end());
      points.insert(points.end(), d.begin(), d.end());
    }
    assert(points.size() > 0);
    return points;
  };
};

class lens : public shape {
  Arc A;
  Arc B;
  int radius;
  double angle;
  int V;
  pair<double, double> v_unit;
  double separation;

public:
  lens(int x, int y, double theta, double radius, double angle)
      : shape(x, y, theta) {
    this->radius = radius;
    this->angle = (angle / 180.0) * M_PI;
    separation = 2 * radius * cos(this->angle / 2.0);
    v_unit.first = cos(theta + (this->angle / 2.0));
    v_unit.second = sin(theta + (this->angle / 2.0));
    A = Arc(static_cast<int>(this->x()), static_cast<int>(this->y()), theta,
            radius, angle);
    B = Arc(static_cast<int>(this->x()) + v_unit.first * separation,
            static_cast<int>(this->y()) + v_unit.second * separation,
            theta + 180, radius, angle);
  }

  const vector<pair<int, int>> &getPoints() {
    if (points.empty()) {
      auto A1 = A.getPoints();
      auto A2 = B.getPoints();
      points.assign(A1.begin(), A1.end());
      points.insert(points.end(), A2.begin(), A2.end());
    }
    return points;
  };
};

class capsule : public shape {
  Arc A, B;
  Line C, D;
  int width;
  int height;

public:
  capsule(double x, double y, double theta, int width, int height)
      : shape(x, y, theta) {
    this->width = width;
    this->height = height;
    C = Line(x, y, this->theta, width);
    D = Line(x + width * cos(this->theta) - height * sin(this->theta),
             y + width * sin(this->theta) + height * cos(this->theta),
             theta + 180, width);
    A = Arc(x + width * cos(this->theta) - height / 2 * sin(this->theta),
            y + width * sin(this->theta) + height / 2 * cos(this->theta),
            theta - 90, height / 2.0, 180);
    B = Arc(x - height / 2 * sin(this->theta),
            y + height / 2 * cos(this->theta), theta + 90, height / 2.0, 180);
  }

  const vector<pair<int, int>> &getPoints() {
    if (points.empty()) {
      auto a = A.getPoints(), b = B.getPoints(), c = C.getPoints(),
           d = D.getPoints();
      points.assign(a.begin(), a.end());
      points.insert(points.end(), b.begin(), b.end());
      points.insert(points.end(), c.begin(), c.end());
      points.insert(points.end(), d.begin(), d.end());
    }
    return points;
  };
};

template <class Shape, int N> class Rotation : shape {
  int R = 360 / N;
  vector<Shape> rotations;
  Shape &current;
  template <typename... Types> Rotation() {}
  template <class... Ts>
  Rotation(int x, int y, double theta, Ts... args) : shape(x, y, theta) {
    for (int i = 0; i < 360; i = i + R) {
      rotations.emplace_back(x, y, theta + i, args...);
    }
  }
  void setLocation(double x, double y, double theta) override {
    shape::setLocation(x, y, theta);
    current = rotations[static_cast<int>(theta) / N];
    this->theta = (theta / 180.0) * M_PI;
  }
  virtual const vector<pair<int, int>> &getPoints() { current.getPoints(); };
};
