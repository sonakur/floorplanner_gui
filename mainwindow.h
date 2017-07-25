#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <set>

#include "SlicingStructure.h"
#include "GraphicsArea.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createMenus();
    void createViews();

private slots:
    void openDesign();
    void saveDesign();
    void closeDesign();
    void runReduceDistance();
    void runNetMigration();
    void runNetContraction();
    void showHelp();
    void showAbout();
    void onContextMenuRequested(const QPoint& );

private:
    SlicingStructure* m_slicingStrucure;
    SlicingStructure* m_outputSlicingStructure;
    std::pair<std::vector<Module*>, std::set<Module*> > m_moduleInfo;
    GraphicsArea* m_inputView;
    GraphicsArea* m_outputView;
    QAction* m_netMigrationAction;
    QAction* m_reduceDistanceAction;
    QAction* m_netContraction;
    Point m_targetPoint;
};

#endif // MAINWINDOW_H
