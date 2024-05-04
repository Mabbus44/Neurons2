#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "common.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <set>
#include <vector>
#include <json/json.h>

namespace alg{
  class Point
  {
    public:
      Point(){}
      Point(int x, int y) {_x = x; _y = y;}
      virtual ~Point(){}
      Point operator+(const Point& other){return Point(_x + other._x,_y + other._y);}
      Point operator-(const Point& other){return Point(_x - other._x,_y - other._y);}
      Point operator*(const int& factor){return Point(_x * factor,_y * factor);}
      Point operator/(const int& divisor){return Point(_x / divisor,_y / divisor);}
      Point rot90() {return Point(-_y, _x);}
      bool loadSettings(Json::Value& input);
      int operator*(const Point& other){return other._x * _x + other._y * _y;}                    //Dot product
      int lenSquared() {return _x*_x + _y*_y;}
      int len() {return sqrt(lenSquared());}
      int x() {return _x;}
      int y() {return _y;}
      void x(int x) {_x = x;}
      void y(int y) {_y = y;}
      Json::Value getJson();
    protected:

    private:
      int _x;
      int _y;
  };

  class PointCloud
  {
    public:
      PointCloud(){}
      virtual ~PointCloud(){}
      void set(Point p);
      void erase(Point p);
      int size() {return _points.size();}
      void clear() {_points.clear();}
      Point pickRandomPoint();
      Point pickFirstPoint();
      void xOverflow(int o) {if(o<POINT_CLOUD_SIDE_OVERFLOW) _xOverflow = o;}
      void yOverflow(int o) {if(o<POINT_CLOUD_SIDE_OVERFLOW) _yOverflow = o;}
      int xOverflow() {return _xOverflow;}
      int yOverflow() {return _yOverflow;}
    protected:

    private:
      std::set<int> _points;
      int _xOverflow = POINT_CLOUD_SIDE_OVERFLOW;
      int _yOverflow = POINT_CLOUD_SIDE_OVERFLOW;
  };

  class Line
  {
    public:
      Line(){}
      Line(Point p1, Point p2) {_p1 = p1; _p2 = p2;}
      virtual ~Line(){}
      bool loadSettings(Json::Value& input);
      int lenSquared() {return (_p2-_p1).lenSquared();}
      int len() {return sqrt(lenSquared());}
      Point p1() {return _p1;}
      Point p2() {return _p2;}
      void p1(Point p1) {_p1 = p1;}
      void p2(Point p2) {_p2 = p2;}
      Json::Value getJson();

    protected:

    private:
      Point _p1;
      Point _p2;
  };

  class Shape2D{
    public:
      Shape2D(){}
      virtual ~Shape2D(){}
      virtual void appendPointCloud(PointCloud& cloud)=0;
      virtual bool loadSettings(Json::Value& input)=0;
      virtual bool inside(Point p)=0;
      virtual Json::Value getJson() = 0;
      bool negative() {return _negative;}
      void negative(int n) {_negative = n;}
    protected:
      bool _negative = false;         // Removes this shape from point clouds, instead of adding it
  };

  class Rectangle: public Shape2D{
    public:
      Rectangle(){}
      Rectangle(Line line, int width){_line = line; _width = width;}
      virtual ~Rectangle(){}
      void appendPointCloud(PointCloud& cloud);
      bool loadSettings(Json::Value& input);
      bool inside(Point p);
      Json::Value getJson();

    protected:

    private:
      Line _line;
      int _width;
  };

  class Circle: public Shape2D{
    public:
      Circle(){}
      virtual ~Circle(){}
      void appendPointCloud(PointCloud& cloud);
      bool loadSettings(Json::Value& input);
      bool inside(Point p);
      Json::Value getJson();

    protected:

    private:
      Point _center;
      int _radius;
  };
}
#endif // ALGEBRA_H
