#ifndef PROTOAUTOPILOT_H
#define PROTOAUTOPILOT_H

#include <QSharedPointer>



class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
};

#endif // PROTOAUTOPILOT_H
