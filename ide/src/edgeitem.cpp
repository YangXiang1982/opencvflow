#include "edge.h"
#include "node.h"

#include <QPainter>
#include <QLineF>

class EdgeItemPrivate {
    QPointF sourcePoint;
    QPointF destPoint;

    friend class EdgeItem;
};

EdgeItem::EdgeItem(NodeItem *sourceNode, NodeItem *destNode) :
    Edge(sourceNode, destNode),
    d_ptr(new EdgeItemPrivate)
{
    setAcceptedMouseButtons(0);
    
    adjust();
}

void EdgeItem::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(static_cast<NodeItem*>(source), 193, 55),
                mapFromItem(static_cast<NodeItem*>(dest), -15, 55));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        d_func()->sourcePoint = line.p1() + edgeOffset;
        d_func()->destPoint = line.p2() - edgeOffset;
    } else {
        d_func()->sourcePoint = d_func()->destPoint = line.p1();
    }
}

QRectF EdgeItem::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = penWidth; //(penWidth + arrowSize) / 2.0;

    return QRectF(d_func()->sourcePoint, QSizeF(d_func()->destPoint.x() - d_func()->sourcePoint.x(),
                                      d_func()->destPoint.y() - d_func()->sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(d_func()->sourcePoint, d_func()->destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;
    // Draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // Draw the arrows
    double angle = std::atan2(-line.dy(), line.dx());

    int arrowSize = 8;
//    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + M_PI / 3) * arrowSize,
//                                                  cos(angle + M_PI / 3) * arrowSize);
//    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
//                                                  cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPointF destArrowP1 = d_func()->destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = d_func()->destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->setBrush(Qt::black);
    //painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}





FakeEdgeItem::FakeEdgeItem(QPointF sourcePoint)
{
    this->sourcePoint = sourcePoint;

    setAcceptedMouseButtons(0);
}

void FakeEdgeItem::setDest(QPointF destPoint)
{
    this->destPoint = destPoint;
}

void FakeEdgeItem::adjust()
{
    QLineF line(sourcePoint, destPoint);
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

QRectF FakeEdgeItem::boundingRect() const
{
    qreal penWidth = 1;
    qreal extra = penWidth; //(penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void FakeEdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!destPoint.x() && !destPoint.y())
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;
    // Draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // Draw the arrows
    double angle = std::atan2(-line.dy(), line.dx());

    int arrowSize = 8;
//    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + M_PI / 3) * arrowSize,
//                                                  cos(angle + M_PI / 3) * arrowSize);
//    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
//                                                  cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->setBrush(Qt::black);
    //painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
