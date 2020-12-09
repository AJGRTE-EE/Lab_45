#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <widgets/headers/GraphPropertiesTable.h>
#include <widgets/headers/IncidenceMatrixTable.h>

#include "basis/headers/Graph.h"
#include "graphics/headers/GraphGraphicsScene.h"
#include "widgets/headers/AdjacencyMatrixTable.h"
#include "graphics/headers/GraphGraphicsView.h"
#include "widgets/headers/ElementPropertiesTable.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:

    void onGraphChanged();

    void on_openGraphButton_clicked();

    void on_createGraphButton_clicked();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_Graph_triggered();

    void on_actionOpen_Graph_triggered();

    void on_actionCredits_triggered();

    static void on_actionExit_triggered();

    void on_actionAddNode_triggered();

    void on_actionAddEdge_triggered();

    void on_actionEditEdge_triggered();

    void on_actionDelNode_triggered();

    void on_actionDelEdge_triggered();

    void on_BFSbtn_clicked();

    void on_DFSbtn_clicked();

    void on_EulerBtn_clicked();

    void on_actionBFS_triggered();

    void on_actionDFS_triggered();

    void on_actionEuler_Cycle_triggered();

    void on_tabWidget_currentChanged(int index);

//    void on_isFull_clicked();

//    void on_sum_clicked();

    void on_dVN_clicked();

signals:

    void graphChanged();

    void startDemoAlgorithm(std::list<std::pair<std::string, std::string>> listOfPair, GraphDemoFlag flag);

    void startDemoAlgorithm(std::list<std::string> listOfNum, GraphDemoFlag flag);

    void startDemoAlgorithm(std::list<std::list<std::string>> listOfList, GraphDemoFlag flag);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QString _workingFilename;
    Ui::MainWindow *_ui;
    GraphGraphicsScene *_scene;
    GraphGraphicsView *_view;
    AdjacencyMatrixTable *_adjMatrix;
    IncidenceMatrixTable *_incidenceMatrix;
    ElementPropertiesTable *_elementPropertiesTable;
    GraphPropertiesTable *_graphPropertiesTable;
    GraphType::Graph *_graph;
    bool _dataNeedSaving;

    QString showOpenFileDialog();

    QString showSaveFileDialog();

    void showNewNodeDialog(QPointF pos = QPointF(0, 0));

    void setWorkspaceEnabled(bool ready);

    void initWorkspace(const QString &filename, bool new_file = false);

    void resetGraph(GraphType::Graph *graph);
};

#endif // MAINWINDOW_H
