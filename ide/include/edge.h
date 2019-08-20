#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>

class Node;
class NodeItem;

/**
 * @brief The Edge;
 */
class Edge
{
public:
    explicit Edge(Node *sourceNode, Node *destNode);

    Node *sourceNode() const;
    Node *destNode() const;

protected:
    Node *source, *dest;
};

/**
 * @brief The EdgeItem;
 */
class EdgeItemPrivate;
class EdgeItem : public Edge, public QGraphicsItem
{
    //Q_OBJECT

    QScopedPointer<EdgeItemPrivate> d_ptr;
    Q_DECLARE_PRIVATE(EdgeItem)
public:
    explicit EdgeItem(NodeItem *sourceNode, NodeItem *destNode);

    Node *sourceNode() const;
    Node *destNode() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};


/**
 * @brief The FakeEdgeItem;
 */
class FakeEdgeItem : public QGraphicsItem
{
    QPointF sourcePoint;
    QPointF destPoint;
public:
    explicit FakeEdgeItem(QPointF sourcePoint);

    void setDest(QPointF destPoint);
    void adjust();

    enum { Type = UserType + 4 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
#endif // EDGE_H
