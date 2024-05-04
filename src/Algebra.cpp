#include "Algebra.h"
namespace alg{

  bool Point::loadSettings(Json::Value& input){
    logger->trace("Point::loadSettings(input)");
    for(Json::Value::const_iterator itr = input.begin() ; itr != input.end() ; itr++){
      if(itr.key() == "x" && (*itr).isIntegral()){
        _x = (*itr).asInt();
      }else if(itr.key() == "y" && (*itr).isIntegral()){
        _y = (*itr).asInt();
      }else{
        cout << "Warning: Invalid key {" << itr.key() << "} in load Point" << endl;
      }
    }
    return true;
  }

  Json::Value Point::getJson(){
    logger->trace("Point::getJson()");
    Json::Value ret;
    ret["x"] = Json::Value(_x);
    ret["y"] = Json::Value(_y);
    return ret;
  }

  void PointCloud::set(Point p){
    logger->trace("PointCloud::set(p)");
    if(p.x() < 0 || p.y() < 0 || p.y() >= _yOverflow || p.x() >= _xOverflow)
      return;
    _points.insert(p.x() + p.y()*_xOverflow);
  }

  void PointCloud::erase(Point p){
    logger->trace("PointCloud::erase(p)");
    if(p.x() < 0 || p.y() < 0 || p.y() >= _yOverflow || p.x() >= _xOverflow)
      return;
    _points.erase(p.x() + p.y()*_xOverflow);
  }

  Point PointCloud::pickRandomPoint(){
    logger->trace("PointCloud::pickRandomPoint()");
    if(_points.size() == 0){
        cout << "Error: Cannot pick from empty pointCloud" << endl;
        return Point(0,0);
    }
    std::set<int>::iterator it = _points.begin();
    int rndNum = rand();
    if(RAND_MAX+1 < _points.size())
      rndNum += (RAND_MAX+1) * rand();
    advance(it, rndNum%_points.size());
    int ret = *it;
    _points.erase(it);
    return Point(ret%_xOverflow, ret/_xOverflow);
  }

  Point PointCloud::pickFirstPoint(){
    logger->trace("PointCloud::pickFirstPoint()");
    if(_points.size() == 0){
        cout << "Error: Cannot pick from empty pointCloud" << endl;
        return Point(0,0);
    }
    std::set<int>::iterator it = _points.begin();
    int ret = *it;
    _points.erase(it);
    return Point(ret%_xOverflow, ret/_xOverflow);
  }

  bool Line::loadSettings(Json::Value& input){
    logger->trace("Line::loadSettings(input)");
    for(Json::Value::iterator itr = input.begin() ; itr != input.end() ; itr++){
      if(itr.key() == "p1" && (*itr).isObject()){
        _p1.loadSettings((*itr));
      }else if(itr.key() == "p2" && (*itr).isObject()){
        _p2.loadSettings(*itr);
      }else{
        cout << "Warning: Invalid key {" << itr.key() << "} in load line" << endl;
      }
    }
    return true;
  }

  Json::Value Line::getJson(){
    logger->trace("Line::getJson()");
    Json::Value ret;
    ret["p1"] = _p1.getJson();
    ret["p2"] = _p2.getJson();
    return ret;
  }

  bool Rectangle::inside(Point p){
    // Projects Point p on _line, and checks that it is within the line
    // Then projects the Point p on _line rotated 90 degrees, and checks that it is not futher then width
    logger->trace("Rectangle::inside(p)");
    int lenSquared = _line.lenSquared();
    int len = sqrt(lenSquared);
    Point lineDir = _line.p2() - _line.p1();
    Point lineDirRotated = lineDir.rot90();
    Point pointDir = p - _line.p1();
    int projOnLine = pointDir* lineDir;
    int projOnLineRotated = pointDir*lineDirRotated / len;
    if(projOnLine < 0 || projOnLine > lenSquared)
      return false;
    if(projOnLineRotated < 0 || projOnLineRotated > _width)
      return false;
    return true;
  }

  void Rectangle::appendPointCloud(PointCloud& cloud){
    logger->trace("Rectangle::appendPointCloud(cloud)");
    Point lineDir = _line.p2() - _line.p1();
    Point lineDirRotated = lineDir.rot90();
    Point lineDirRotatedLenAdjusted = lineDirRotated * _width / lineDirRotated.len();
    std::vector<Point> Points;
    Points.push_back(_line.p2());
    Points.push_back(_line.p1() + lineDirRotatedLenAdjusted);
    Points.push_back(_line.p2() + lineDirRotatedLenAdjusted);
    int xMin = _line.p1().x();
    int xMax = _line.p1().x();
    int yMin = _line.p1().y();
    int yMax = _line.p1().y();
    for(Point& p: Points){
      if(p.x() < xMin)
        xMin = p.x();
      if(p.x() > xMax)
        xMax = p.x();
      if(p.y() < yMin)
        yMin = p.y();
      if(p.y() > yMax)
        yMax = p.y();
    }
    for(int y=yMin; y<=yMax; y++){
      for(int x=xMin; x<=xMax; x++){
        if(inside(Point(x, y))){
          if(_negative)
            cloud.erase(Point(x, y));
          else
            cloud.set(Point(x, y));
        }
      }
    }
  }

  bool Rectangle::loadSettings(Json::Value& input){
    logger->trace("Rectangle::loadSettings(input)");
    for(Json::Value::iterator itr = input.begin() ; itr != input.end() ; itr++){
      if(itr.key() == "line" && (*itr).isObject()){
        _line.loadSettings((*itr));
      }else if(itr.key() == "width" && (*itr).isIntegral()){
        _width = (*itr).asInt();
      }else if(itr.key() == "negative" && (*itr).isBool()){
        _negative = (*itr).asBool();
      }else if(itr.key() == "type" && (*itr).isString()){
      }else{
        cout << "Warning: Invalid key {" << itr.key() << "} in load rectangle" << endl;
      }
    }
    return true;
  }

  Json::Value Rectangle::getJson(){
    logger->trace("Rectangle::getJson()");
    Json::Value ret;
    ret["type"] = "rectangle";
    ret["line"] = _line.getJson();
    ret["width"] = Json::Value(_width);
    ret["negative"] = Json::Value(_negative);
    return ret;
  }

  void Circle::appendPointCloud(PointCloud& cloud){
    logger->trace("Circle::appendPointCloud(cloud)");
    int xMin = _center.x() - _radius;
    int xMax = _center.x() + _radius;
    int yMin = _center.y() - _radius;
    int yMax = _center.y() + _radius;
    for(int y=yMin; y<=yMax; y++){
      for(int x=xMin; x<=xMax; x++){
        if(inside(Point(x, y))){
          if(_negative)
            cloud.erase(Point(x, y));
          else
            cloud.set(Point(x, y));
        }
      }
    }
  }

  bool Circle::loadSettings(Json::Value& input){
    logger->trace("Circle::loadSettings(input)");
    for(Json::Value::iterator itr = input.begin() ; itr != input.end() ; itr++){
      if(itr.key() == "center" && (*itr).isObject()){
        _center.loadSettings((*itr));
      }else if(itr.key() == "radius" && (*itr).isIntegral()){
        _radius = (*itr).asInt();
      }else if(itr.key() == "negative" && (*itr).isBool()){
        _negative = (*itr).asBool();
      }else if(itr.key() == "type" && (*itr).isString()){
      }else{
        cout << "Warning: Invalid key {" << itr.key() << "} in load circle" << endl;
      }
    }
    return true;
  }

  bool Circle::inside(Point p){
    logger->trace("Circle::inside(p)");
    return _radius * _radius >= (p-_center).lenSquared();
  }

  Json::Value Circle::getJson(){
    logger->trace("Circle::getJson()");
    Json::Value ret;
    ret["type"] = "circle";
    ret["center"] = _center.getJson();
    ret["radius"] = Json::Value(_radius);
    ret["negative"] = Json::Value(_negative);
    return ret;
  }
}
