#include "LineVectorWidget.h"
#include <cmath>


void LineVectorWidget::drawGraf(int type) {
    if (points.size() < 2) {
        return;
    }
    double dt = 0.01;

    for (const auto& point : points) {
        vW->drawCircle(point, QPoint(point.x() + 2, point.y()), Qt::red);
    }

     
    switch (typeL) {
    case 0: 
    {
        if (points.size() < 2 || tangents.size() != points.size()) {
            return;
        }

        for (int i = 0; i < points.size(); ++i) {
            QPointF vectorEnd = points[i] + tangents[i];
            vW->drawLine(points[i], QPoint(vectorEnd.x(),vectorEnd.y()), Qt::blue, type);
        }

        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF p0 = points[i];
            QPointF p1 = points[i + 1];
            QPointF t0 = tangents[i];
            QPointF t1 = tangents[i + 1];

            double step = 0.01;
            for (double t = 0; t < 1.0; t += step) {
                double h1 = 2 * t * t * t - 3 * t * t + 1;
                double h2 = -2 * t * t * t + 3 * t * t;
                double h3 = t * t * t - 2 * t * t + t;
                double h4 = t * t * t - t * t;

                qreal x = h1 * p0.x() + h2 * p1.x() + h3 * t0.x() + h4 * t1.x();
                qreal y = h1 * p0.y() + h2 * p1.y() + h3 * t0.y() + h4 * t1.y();

                double next_t = t + step;
                double next_h1 = 2 * next_t * next_t * next_t - 3 * next_t * next_t + 1;
                double next_h2 = -2 * next_t * next_t * next_t + 3 * next_t * next_t;
                double next_h3 = next_t * next_t * next_t - 2 * next_t * next_t + next_t;
                double next_h4 = next_t * next_t * next_t - next_t * next_t;

                qreal next_x = next_h1 * p0.x() + next_h2 * p1.x() + next_h3 * t0.x() + next_h4 * t1.x();
                qreal next_y = next_h1 * p0.y() + next_h2 * p1.y() + next_h3 * t0.y() + next_h4 * t1.y();

                vW->drawLine(QPoint(x, y), QPoint(next_x, next_y), color, type);
            }
        }

		break;
    }

    case 1:
        for (double t = 0; t <= 1.0; t += dt) {
            QPointF Q(0, 0);
            size_t n = points.size() - 1;

            for (size_t i = 0; i <= n; ++i) {
                double binomial = tgamma(n + 1) / (tgamma(i + 1) * tgamma(n - i + 1));
                double basis = binomial * pow(t, i) * pow(1 - t, n - i);
                Q += QPointF(points[i]) * basis;
            }

            if (t > 0) {
                vW->drawLine(temp_points[0], Q.toPoint(), color, type);
            }
            temp_points[0] = Q.toPoint();
        }
        break;

    case 2:
        if (points.size() < 4) {
            return;
        }

        for (size_t i = 3; i < points.size(); ++i) {
            QPointF Q0 = QPointF(points[i - 3]) / 6.0 + QPointF(points[i - 2]) * 2.0 / 3.0 + QPointF(points[i - 1]) / 6.0;

            for (double t = dt; t <= 1.0; t += dt) {
                double t2 = t * t;
                double t3 = t2 * t;

                double B0 = -t3 / 6 + t2 / 2 - t / 2 + 1.0 / 6;
                double B1 = t3 / 2 - t2 + 2.0 / 3;
                double B2 = -t3 / 2 + t2 / 2 + t / 2 + 1.0 / 6;
                double B3 = t3 / 6;

                QPointF Q1 = QPointF(points[i - 3]) * B0 + QPointF(points[i - 2]) * B1 + QPointF(points[i - 1]) * B2 + QPointF(points[i]) * B3;
                vW->drawLine(Q0.toPoint(), Q1.toPoint(), color, type);
                Q0 = Q1;
            }
        }
        break;
    default:
        break;
    }
}

void LineVectorWidget::generateTangents() {
    tangents.clear();
    tangents.resize(points.size());

    if (points.size() < 2) {
        return;
    }

    if (points.size() == 2) {
        qreal dx = static_cast<qreal>(points[1].x()) - static_cast<qreal>(points[0].x());
        qreal dy = static_cast<qreal>(points[1].y()) - static_cast<qreal>(points[0].y());

        qreal length = sqrt(dx * dx + dy * dy);
        qreal unit_dx = dx / length;
        qreal unit_dy = dy / length;

        tangents[0] = { -unit_dy * vecWidth, unit_dx * vecWidth };
        tangents[1] = { unit_dy * vecWidth, -unit_dx * vecWidth };
    }
    else {
        for (int i = 0; i < points.size(); ++i) {
            if (i == 0) {
                qreal dx = static_cast<qreal>(points[i + 1].x()) - static_cast<qreal>(points[i].x());
                qreal dy = static_cast<qreal>(points[i + 1].y()) - static_cast<qreal>(points[i].y());
                qreal length = sqrt(dx * dx + dy * dy);
                tangents[i] = { vecWidth * dx / length, vecWidth * dy / length };
            }
            else if (i == points.size() - 1) {
                qreal dx = static_cast<qreal>(points[i].x()) - static_cast<qreal>(points[i - 1].x());
                qreal dy = static_cast<qreal>(points[i].y()) - static_cast<qreal>(points[i - 1].y());
                qreal length = sqrt(dx * dx + dy * dy);
                tangents[i] = { vecWidth * dx / length, vecWidth * dy / length };
            }
            else {
                qreal dx = static_cast<qreal>(points[i + 1].x()) - static_cast<qreal>(points[i - 1].x());
                qreal dy = static_cast<qreal>(points[i + 1].y()) - static_cast<qreal>(points[i - 1].y());
                qreal length = sqrt(dx * dx + dy * dy);
                tangents[i] = { vecWidth * dx / length, vecWidth * dy / length };
            }
        }
    }
}
