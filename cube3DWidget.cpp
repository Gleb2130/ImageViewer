#include "cube3DWidget.h"

void cube3DWidget::calculateCube() {
    vertices.clear();

    int x = origin.x();
    int y = origin.y();

    vertices = {
        {x, y},                              
        {x + sideLength, y},                   
        {x + sideLength, y - sideLength},      
        {x, y - sideLength},                   

        {x, y},                                 
        {x + sideLength, y},                  
        {x + sideLength, y - sideLength},      
        {x, y - sideLength}                    
    };
}


void cube3DWidget::drawGraf(int type) {
    calculateCube();

    vW->drawLine(vertices[0], vertices[1], Qt::black, type);
    vW->drawLine(vertices[1], vertices[2], Qt::black, type);
    vW->drawLine(vertices[2], vertices[3], Qt::black, type);
    vW->drawLine(vertices[3], vertices[0], Qt::black, type);

    vW->drawLine(vertices[4], vertices[5], Qt::blue, type);
    vW->drawLine(vertices[5], vertices[6], Qt::blue, type);
    vW->drawLine(vertices[6], vertices[7], Qt::blue, type);
    vW->drawLine(vertices[7], vertices[4], Qt::blue, type);

    for (int i = 0; i < 4; ++i) {
        vW->drawLine(vertices[i], vertices[i + 4], color, type);
    }
}

void cube3DWidget::saveCubeToVTK(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);


    out << "# vtk DataFile Version 3.0\n";
    out << "3D Cube Example\n";
    out << "ASCII\n";
    out << "DATASET POLYDATA\n";


    out << "POINTS 8 float\n";
    for (const auto& vertex : vertices) {
        out << vertex.x() << " " << vertex.y() << " 0\n"; 
    }

    out << "LINES 12 36\n";
    for (int i = 0; i < 4; ++i) {
        out << "2 " << i << " " << (i + 1) % 4 << "\n";       
        out << "2 " << i + 4 << " " << (i + 1) % 4 + 4 << "\n"; 
        out << "2 " << i << " " << i + 4 << "\n";              
    }

    file.close();
}
