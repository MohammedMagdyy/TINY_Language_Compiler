#include "TreeDraw.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QDebug>

TreeDraw::TreeDraw(QWidget *parent) : QWidget(parent)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setGeometry(10, 10, 800, 600); // Adjust as needed
    font = QFont("Arial", 12);
    redPen = QPen(Qt::red); // Use a red pen for drawing lines
}

void TreeDraw::drawSyntaxTree(std::shared_ptr<TreeNode> root)
{
    std::map<int, int> currentForVLevel;
    drawNode(root, 50, 50, currentForVLevel);
    view->show();
}

void TreeDraw::drawNode(std::shared_ptr<TreeNode> node, int x, int y, std::map<int, int> &currentForVLevel)
{
    if (!node)
        return;

    QString nodeText = QString::fromStdString(node->name);
    if (!node->value.empty())
    {
        nodeText += " (" + QString::fromStdString(node->value) + ")";
    }

    QGraphicsItem *nodeItem = nullptr;
    if (node->name == "if" || node->name == "repeat" || node->name == "assign" ||
        node->name == "read" || node->name == "write")
    {
        nodeItem = scene->addRect(x, y, nodeWidth, nodeHeight, QPen(Qt::black), QBrush(Qt::white));
    }
    else
    {
        nodeItem = scene->addEllipse(x, y, nodeWidth, nodeHeight, QPen(Qt::black), QBrush(Qt::white));
    }

    QGraphicsTextItem *textItem = scene->addText(nodeText);
    qreal xOffsetText = x + (nodeWidth - textItem->boundingRect().width()) / 2;
    qreal yOffsetText = y + (nodeHeight - textItem->boundingRect().height()) / 2 - 10;
    textItem->setPos(xOffsetText, yOffsetText);
    textItem->setDefaultTextColor(Qt::black);

    if (!node->value.empty())
    {
        QGraphicsTextItem *valueItem = scene->addText("(" + QString::fromStdString(node->value) + ")");
        valueItem->setFont(font);
        valueItem->setDefaultTextColor(Qt::black);
        valueItem->setPos(x + (nodeWidth - valueItem->boundingRect().width()) / 2, y + 20);
    }

    int currentX = x;
    int currentY = y + yOffset;

    size_t numChildren = node->children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        auto child = node->children[i];
        if (!child)
            continue;

        int childX = x;
        if (numChildren == 2)
        {
            childX = (i == 0) ? x - xOffset : x + xOffset;
        }
        else if (numChildren == 3)
        {
            if (i == 0)
                childX = x - xOffset;
            else if (i == 2)
                childX = x + xOffset;
        }

        scene->addLine(x + nodeWidth / 2, y + nodeHeight, childX + nodeWidth / 2, currentY, redPen);
        drawNode(child, childX, currentY, currentForVLevel);
    }

    if (!node->siblings.empty())
    {
        currentX = x + (numChildren + 1) * (2 * xOffset);
        for (const auto &sibling : node->siblings)
        {
            if (sibling)
            {
                scene->addLine(x + nodeWidth, y + nodeHeight / 2, currentX + nodeWidth / 2, y + nodeHeight / 2, redPen);
                drawNode(sibling, currentX, y, currentForVLevel);
                currentX = currentX + (2 * xOffset);
            }
        }
    }
}

/*#include "TreeDraw.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QDebug>

TreeDraw::TreeDraw(QWidget *parent) : QWidget(parent) {
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setGeometry(10, 10, 800, 600); // Adjust as needed
    font = QFont("Arial", 12);
    redPen = QPen(Qt::red); // Use a red pen for drawing lines
}

bool TreeDraw::isPositionOccupied(int x, int y) {
    return drawnPositions.find({x, y}) != drawnPositions.end();
}

std::pair<int, int> TreeDraw::findNonOverlappingPosition(int x, int y) {
    int newY = y;
    while (isPositionOccupied(x, newY)) {
        newY += yOffset;
    }
    drawnPositions.insert({x, newY});
    return {x, newY};
}

void TreeDraw::drawSyntaxTree(std::shared_ptr<TreeNode> root) {
    drawnPositions.clear(); // Clear drawn positions
    std::map<int, int> currentForVLevel;
    drawNode(root, 50, 50, currentForVLevel);
    view->show();
}

void TreeDraw::drawNode(std::shared_ptr<TreeNode> node, int x, int y, std::map<int, int> &currentForVLevel) {
    if (!node) return;

    auto [newX, newY] = findNonOverlappingPosition(x, y);

    QString nodeText = QString::fromStdString(node->name);
    if (!node->value.empty()) {
        nodeText += " (" + QString::fromStdString(node->value) + ")";
    }

    QGraphicsItem* nodeItem = nullptr;
    if (node->name == "if" || node->name == "repeat" || node->name == "assign" ||
        node->name == "read" || node->name == "write") {
        nodeItem = scene->addRect(newX, newY, nodeWidth, nodeHeight, QPen(Qt::black), QBrush(Qt::white));
    } else {
        nodeItem = scene->addEllipse(newX, newY, nodeWidth, nodeHeight, QPen(Qt::black), QBrush(Qt::white));
    }

    QGraphicsTextItem *textItem = scene->addText(nodeText);
    qreal xOffsetText = newX + (nodeWidth - textItem->boundingRect().width()) / 2;
    qreal yOffsetText = newY + (nodeHeight - textItem->boundingRect().height()) / 2 - 10;
    textItem->setPos(xOffsetText, yOffsetText);
    textItem->setDefaultTextColor(Qt::black);

    if (!node->value.empty()) {
        QGraphicsTextItem *valueItem = scene->addText("(" + QString::fromStdString(node->value) + ")");
        valueItem->setFont(font);
        valueItem->setDefaultTextColor(Qt::black);
        valueItem->setPos(newX + (nodeWidth - valueItem->boundingRect().width()) / 2, newY + 20);
    }

    int currentX = newX;
    int currentY = newY + yOffset;

    size_t numChildren = node->children.size();
    for (size_t i = 0; i < numChildren; ++i) {
        auto child = node->children[i];
        if (!child) continue;

        int childX = newX;
        if (numChildren == 2) {
            childX = (i == 0) ? newX - xOffset : newX + xOffset;
        } else if (numChildren == 3) {
            if (i == 0) childX = newX - xOffset;
            else if (i == 2) childX = newX + xOffset;
        }

        scene->addLine(newX + nodeWidth / 2, newY + nodeHeight, childX + nodeWidth / 2, currentY, redPen);
        drawNode(child, childX, currentY, currentForVLevel);
    }

    if (!node->siblings.empty()) {
        currentX = newX + (numChildren + 1) * xOffset;
        for (const auto &sibling : node->siblings) {
            if (sibling) {
                scene->addLine(newX + nodeWidth, newY + nodeHeight / 2, currentX + nodeWidth / 2, newY + nodeHeight / 2, redPen);
                drawNode(sibling, currentX, newY, currentForVLevel);
                currentX = currentX + (3* xOffset) ;

            }
        }
    }
}*/
