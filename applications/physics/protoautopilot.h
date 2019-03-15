#ifndef PROTOAUTOPILOT_H
#define PROTOAUTOPILOT_H

#include <QSharedPointer>
#include "copterInputs.h"


class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
    CopterInputs output;
    bool active=false;
};

#endif // PROTOAUTOPILOT_H
