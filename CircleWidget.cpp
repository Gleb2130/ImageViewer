#include "CircleWidget.h"
#include <cmath>

void CircleWidget::drawGraf(int type)
{
	vW->drawCircle(points[0], points[1], color);
	if (fill) fillObj();
}

void CircleWidget::fillObj()
{
    if (points.size() < 2) return; 

    QPoint center = points[0];
	radius = sqrt(pow(points[1].x() - center.x(), 2) + pow(points[1].y() - center.y(), 2));

    if (!vW) return;

    QRgb fillColor = color.rgb();
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) { 
                vW->setPixel(center.x() + x, center.y() + y, color);
            }
        }
    }
}
