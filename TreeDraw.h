#ifndef TREEDRAW_H
#define TREEDRAW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <map>
#include <set>
#include "TreeNode.h"

class TreeDraw : public QWidget
{
    Q_OBJECT
public:
    explicit TreeDraw(QWidget *parent = nullptr);
    void drawSyntaxTree(std::shared_ptr<TreeNode> root);

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    void drawNode(std::shared_ptr<TreeNode> node, int x, int y, std::map<int, int> &currentForVLevel);
    int nodeHeight = 50;
    int nodeWidth = 100;
    int xOffset = 150;
    int yOffset = 100;
    QFont font;
    QPen redPen;
    std::set<std::pair<int, int>> drawnPositions; // To keep track of drawn positions

    bool isPositionOccupied(int x, int y);
    std::pair<int, int> findNonOverlappingPosition(int x, int y);
};

#endif // TREEDRAW_H
