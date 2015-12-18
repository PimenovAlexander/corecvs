#include <QKeyEvent>
#include "scene3dTreeView.h"


Scene3DTreeView::Scene3DTreeView(QWidget *parent) : QTreeView(parent)
{

}

void Scene3DTreeView::keyPressEvent(QKeyEvent *e)
{
    QAbstractItemView::keyPressEvent(e);
    if (e->key() == Qt::Key_Space)
    {
        emit toggleInitiated();

        /* I will put this code here but probably it should be in CloudViewDialog */


    }
}

