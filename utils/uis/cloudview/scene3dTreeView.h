#ifndef SCENE3DTREEVIEW_H
#define SCENE3DTREEVIEW_H

#include <QTreeView>

class Scene3DTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit Scene3DTreeView(QWidget *parent = 0);

signals:
    void toggleInitiated( void );

public slots:

protected:
    virtual void keyPressEvent(QKeyEvent * e) override;

};

#endif // SCENE3DTREEVIEW_H
