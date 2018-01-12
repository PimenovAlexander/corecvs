#include "widgetBlockHarness.h"

#include <QGridLayout>
#include <QPushButton>

#include "reflectionWidget.h"

using namespace corecvs;

WidgetBlockHarness::WidgetBlockHarness(corecvs::DynamicObjectWrapper *blockReflection, NewStyleBlock *block, QWidget *parent) :
    QWidget(parent),
    blockReflection(blockReflection),
    block(block)
{
    inputs  = new ReflectionWidget(blockReflection->reflection, ReflectionWidget::TYPE_INPUT_PINS);
    params  = new ReflectionWidget(blockReflection->reflection, ReflectionWidget::TYPE_PARAMS, true);
    outputs = new ReflectionWidget(blockReflection->reflection, ReflectionWidget::TYPE_OUTPUT_PINS);

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(3);
    layout->setContentsMargins(3, 3, 3, 3);

    layout->addWidget(inputs , 0, 0, 1, 1);
    layout->addWidget(params , 0, 1, 1, 1);
    layout->addWidget(outputs, 0, 2, 1, 1);


    QPushButton *executeButton = new QPushButton(this);
    executeButton->setIcon(QIcon(":/new/prefix1/lightning.png"));
    executeButton->setText("Execute");
    layout->addWidget(executeButton, layout->rowCount(), 1, 1, 1);
    connect(executeButton, SIGNAL(released()), this, SLOT(executeCalled()));

    setLayout(layout);
}

void WidgetBlockHarness::executeCalled()
{
    SYNC_PRINT(("Executing...\n"));
    if (block == NULL) {
        SYNC_PRINT(("Block is null"));
        return;
    }

    inputs ->getParameters(blockReflection->rawObject);
    params ->getParameters(blockReflection->rawObject);
    int result = block->operator ()();
    outputs->setParameters(blockReflection->rawObject);
    SYNC_PRINT(("done %d\n", result));
}

WidgetBlockHarness::~WidgetBlockHarness()
{

}
