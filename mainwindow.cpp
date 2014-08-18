#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "InputOutputManager.h"

#include <QSplitter>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

#include <cassert>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_slicingStrucure(0)
    , m_outputSlicingStructure(0)
    , m_inputView(new GraphicsArea())
    , m_outputView(new GraphicsArea())
    , m_netMigrationAction(0)
    , m_reduceDistanceAction(0)
    , m_targetPoint(QPoint(0, 0))
{
    setWindowTitle("Floorplanner");
    resize(1000, 700);
    setMinimumSize(800, 600);
    createMenus();
    createViews();

    m_inputView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_inputView, SIGNAL(customContextMenuRequested(const QPoint& )), this, SLOT(onContextMenuRequested(const QPoint& )));
}

MainWindow::~MainWindow()
{

}

void MainWindow::onContextMenuRequested(const QPoint& pos)
{
    if (!m_netMigrationAction->isEnabled()) {
        return;
    }
    QString itemText = "Mark As Target Point";
    QMenu contextMenu;
    contextMenu.addAction(new QAction(itemText, m_inputView));
    QPoint globalPos = m_inputView->mapToGlobal(pos);
    QAction* selectedItem = contextMenu.exec(globalPos);
    if (selectedItem) {
        assert(selectedItem->text() == itemText);
        m_targetPoint = pos;
    }
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = new QMenu(tr("&File"), 0);
    QMenu* runMenu = new QMenu(tr("&Run"), 0);
    QMenu* helpMenu = new QMenu(tr("&Help"), 0);

    // file menu items
    QAction* openAction = new QAction(tr("&Open Design"), this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openDesign()));
    fileMenu->addAction(openAction);

    QAction* saveAction = new QAction(tr("&Save Design"), this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveDesign()));
    fileMenu->addAction(saveAction);

    QAction* closeAction = new QAction(tr("&Close Design"), this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeDesign()));
    fileMenu->addAction(closeAction);

    // run menu items
    m_reduceDistanceAction = new QAction(tr("&Reduce Distance"), this);
    connect(m_reduceDistanceAction, SIGNAL(triggered()), this, SLOT(runReduceDistance()));
    runMenu->addAction(m_reduceDistanceAction);
    m_reduceDistanceAction->setEnabled(false);

    m_netMigrationAction = new QAction(tr("&Net Migration"), this);
    connect(m_netMigrationAction, SIGNAL(triggered()), this, SLOT(runNetMigration()));
    runMenu->addAction(m_netMigrationAction);
    m_netMigrationAction->setEnabled(false);

    // help menu items
    QAction* helpAction = new QAction(tr("Help"), this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showHelp()));
    helpMenu->addAction(helpAction);

    QAction* aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    helpMenu->addAction(aboutAction);

    this->menuBar()->addMenu(fileMenu);
    this->menuBar()->addMenu(runMenu);
    this->menuBar()->addMenu(helpMenu);
}

void MainWindow::createViews()
{
    QSplitter* horizSplitter = new QSplitter(Qt::Horizontal);
    horizSplitter->addWidget(m_inputView);
    horizSplitter->addWidget(m_outputView);
    horizSplitter->setStretchFactor(0, 1);
    horizSplitter->setStretchFactor(1, 1);
    this->setCentralWidget(horizSplitter);
}


void MainWindow::openDesign()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."));
    if (fileName != "") {
        std::pair<std::vector<Module*>, std::set<Module*> > moduleInfo = readBlocks(fileName.toStdString());
        m_moduleInfo = moduleInfo;
        m_slicingStrucure = new SlicingStructure(moduleInfo.first);
        m_inputView->setFloorplan(m_slicingStrucure->floorplan());
        m_inputView->setSelectedItems(moduleInfo.second);
        m_inputView->draw();

        if (moduleInfo.second.size() > 0) {
            m_netMigrationAction->setEnabled(true);
            if (moduleInfo.second.size() == 2) {
                m_reduceDistanceAction->setEnabled(true);
            } else {
                m_reduceDistanceAction->setEnabled(false);
            }
        } else {
            m_netMigrationAction->setEnabled(false);
            m_reduceDistanceAction->setEnabled(false);
        }
    }
}

void MainWindow::saveDesign()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File..."));
    if (fileName != "") {
        writeFloorplan(fileName.toStdString(), m_outputSlicingStructure->floorplan(), m_moduleInfo.second);
    }
}

void MainWindow::closeDesign()
{
    delete m_outputSlicingStructure;
    m_outputSlicingStructure = 0;

    delete m_slicingStrucure;
    m_slicingStrucure = 0;

    m_moduleInfo.first.clear();
    m_moduleInfo.second.clear();

    m_inputView->reset();
    m_outputView->reset();
}

void MainWindow::runReduceDistance()
{
    assert(!m_moduleInfo.first.empty() && m_moduleInfo.second.size() == 2);
    if (m_outputSlicingStructure == 0) {
        m_outputSlicingStructure = new SlicingStructure(m_moduleInfo.first);
        m_outputView->setFloorplan(m_outputSlicingStructure->floorplan());
        m_outputView->setSelectedItems(m_moduleInfo.second);
    }
    std::set<Module*>::iterator it = m_moduleInfo.second.begin();
    Module* module1 = *it;
    Module* module2 = *(++it);
    m_outputSlicingStructure->reduceDistnace(module1, module2);
    m_outputView->draw();
}

void MainWindow::runNetMigration()
{
    assert(!m_moduleInfo.first.empty() && !m_moduleInfo.second.empty());
    if (m_outputSlicingStructure == 0) {
        m_outputSlicingStructure = new SlicingStructure(m_moduleInfo.first);
        m_outputView->setFloorplan(m_outputSlicingStructure->floorplan());
        m_outputView->setSelectedItems(m_moduleInfo.second);
    }
    m_outputSlicingStructure->applyNetMigration(m_moduleInfo.second, m_inputView->recalculatePoint(m_targetPoint));
    m_outputView->draw();
}

void MainWindow::showHelp()
{

}

void MainWindow::showAbout()
{
    QMessageBox::information(this, "About Floorplanner", "Created by Sona Kurazyan\nCopyright 2014 Sona Kurazyan. All rights reserved.");
}
