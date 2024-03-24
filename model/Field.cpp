#include "Field.hpp"

Field::Field()
{}

Field::Field(const std::string name) : _name(name)
{}

Field::~Field()
{}

cv::Point Field::getCenterPosition()
{
    cv::Rect v = _mouse.getRect();
    return cv::Point(v.x + v.width / 2, v.y + v.height / 2);
}

cv::Rect Field::getRect()
{
    return _mouse.getRect();
}

int Field::getWidth()
{
    return _mouse.getRect().width;
}

int Field::getHeight()
{
    return _mouse.getRect().height;
}

void Field::setRect(const Area rect)
{
    _mouse.setRect(toCvRect(rect));
}

void Field::draw(cv::Mat frame)
{
    cv::Point v = _mouse.getPosition();
    cv::line(frame, cv::Point(v.x - 100, v.y), cv::Point(v.x + 100, v.y), cv::Scalar(255, 0, 0), 2);
    cv::line(frame, cv::Point(v.x, v.y - 100), cv::Point(v.x , v.y + 100), cv::Scalar(255, 0, 0), 2);

    cv::rectangle(frame, _mouse.getRect(), cv::Scalar(255, 0, 0), 2);
}

void Field::setWindow()
{
    std::cout << "Set Window ... " << std::endl;
    _mouse.setWindow("Game original");
}

bool Field::isSelected()
{
    return _mouse.isSelected();
}

std::string Field::getName()
{
    return _name;
}

cv::Rect Field::toCvRect(Area area){
    return cv::Rect2i(area.x, area.y, area.width, area.height);
}

void Field::setWindowName(std::string windowName){
    _mouse.setWindow(windowName);
}