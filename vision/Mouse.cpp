#include "Mouse.hpp"

Mouse::Mouse() : _isSelected(false), _isDrag(false)
{
}

Mouse::~Mouse()
{
       
}

bool Mouse::isSelected()
{
    return _isSelected;
}

cv::Point Mouse::getPosition()
{
    return cv::Point(_pos.x, _pos.y);
}

cv::Rect Mouse::getRect()
{
    return _rectangle;
}

void Mouse::setRect(const cv::Rect rect)
{
    _rectangle = rect;
}

void Mouse::setWindow(const std::string nameFrame)
{
    cv::setMouseCallback(nameFrame, Mouse::mouseCallback, this);
}

void Mouse::mouseHandler(int& event, int& x, int& y, int& flags)
{
    _pos.x = x;
    _pos.y = y;

    if(event == cv::EVENT_LBUTTONDOWN && !_isDrag)
    {
        _isDrag = true;
        _point1 = cv::Point(x, y);
        _point2 = cv::Point(x, y);
    }

    else if(event == cv::EVENT_LBUTTONDOWN && _isDrag)
    {
        _isDrag = false;
        _isSelected = true;
        _point2 = cv::Point(x, y);
        _rectangle = cv::Rect(_point1, _point2);
    }

    if(event == cv::EVENT_MOUSEMOVE && _isDrag)
    {
        _point2 = cv::Point(x, y);
        _rectangle = cv::Rect(_point1, _point2);
    }
}

void Mouse::mouseCallback(int event, int x, int y, int flags, void *ptr)
{
    Mouse* mouse = static_cast<Mouse*>(ptr);
    mouse->mouseHandler(event, x, y, flags);
}