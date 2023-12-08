#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <vector>
#include "Board.h"
#include "CircleButton.h"
#include "BridgeLine.h"

class BoardWindow
{
public:
	BoardWindow(QGraphicsScene* scene, int width, int height,const Board& b);



private:
	std::vector<CircleButton*> m_points;
	std::vector<BridgeLine*> m_lines;
};

#endif //BOARDWINDOW_H