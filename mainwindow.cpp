#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/headers/GraphGraphicsView.h"
#include "basis/headers/GraphUtils.h"
#include "widgets/headers/MultiLineInputDialog.h"
#include "widgets/headers/MultiComboboxDialog.h"
#include <QtGui>
#include <QMessageBox>
#include <QTimer>
#include <dvninputdialog.h>
#include <widgets/headers/GraphOptionDialog.h>
#include "utils/qdebugstream.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        _ui(new Ui::MainWindow) {
    setWindowState(Qt::WindowMaximized);
    _ui->setupUi(this);
    _ui->statusBar->setStyleSheet("color: darkgrey");
    _ui->consoleText->setReadOnly(true);
    _ui->consoleText->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _ui->verticalSplitter->setStretchFactor(0, 1);
    this->_dataNeedSaving = false;
    this->setWindowTitle("Simple Graph Tool");
    auto *m = new QSignalMapper(this);
    auto *s1 = new QShortcut(QKeySequence("Alt+1"), this);
    auto *s2 = new QShortcut(QKeySequence("Alt+2"), this);
    auto *s3 = new QShortcut(QKeySequence("Alt+3"), this);
    connect(s1, SIGNAL(activated()), m, SLOT(map()));
    connect(s2, SIGNAL(activated()), m, SLOT(map()));
    connect(s3, SIGNAL(activated()), m, SLOT(map()));
    m->setMapping(s1, 0);
    m->setMapping(s2, 1);
    m->setMapping(s3, 2);
    connect(m, SIGNAL(mapped(int)), _ui->tabWidget, SLOT(setCurrentIndex(int)));
    QFont btnFont;
    btnFont.setPixelSize(32);
    _ui->createGraphButton->setFont(btnFont);
    _ui->openGraphButton->setFont(btnFont);

    this->_graph = new Graph(false, false);
    this->_scene = new GraphGraphicsScene(_graph);
    this->_view = new GraphGraphicsView();
    this->_adjMatrix = new AdjacencyMatrixTable(_graph);
    this->_incidenceMatrix = new IncidenceMatrixTable(_graph);
    this->_elementPropertiesTable = new ElementPropertiesTable(_graph);
    this->_graphPropertiesTable = new GraphPropertiesTable(_graph);

    connect(_adjMatrix, SIGNAL(graphChanged()), _scene, SLOT(reload()));
    connect(_adjMatrix, SIGNAL(graphChanged()), _incidenceMatrix, SLOT(reload()));
    connect(_scene, SIGNAL(graphChanged()), _adjMatrix, SLOT(reload()));
    connect(_scene, SIGNAL(graphChanged()), _incidenceMatrix, SLOT(reload()));
    connect(this, SIGNAL(graphChanged()), _scene, SLOT(reload()));
    connect(this, SIGNAL(graphChanged()), _adjMatrix, SLOT(reload()));
    connect(this, SIGNAL(graphChanged()), _incidenceMatrix, SLOT(reload()));
    connect(this, SIGNAL(graphChanged()), _view, SLOT(redraw()));
    connect(this, SIGNAL(graphChanged()), this, SLOT(onGraphChanged()));
    connect(_adjMatrix, SIGNAL(graphChanged()), this, SLOT(onGraphChanged()));
    connect(_scene, SIGNAL(graphChanged()), this, SLOT(onGraphChanged()));
    _scene->setInterval(20);

    connect(_view, SIGNAL(unSelected()), _elementPropertiesTable, SLOT(onUnSelected()));
    connect(this, SIGNAL(graphChanged()), _elementPropertiesTable, SLOT(onGraphChanged()));
    connect(this, SIGNAL(graphChanged()), _graphPropertiesTable, SLOT(onGraphChanged()));
    connect(_adjMatrix, SIGNAL(graphChanged()), _graphPropertiesTable, SLOT(onGraphChanged()));
    connect(_scene, SIGNAL(graphChanged()), _graphPropertiesTable, SLOT(onGraphChanged()));
    connect(_view, &GraphGraphicsView::nodeSelected, _elementPropertiesTable, &ElementPropertiesTable::onNodeSelected);
    connect(_view, &GraphGraphicsView::edgeSelected, _elementPropertiesTable, &ElementPropertiesTable::onEdgeSelected);
    connect(_adjMatrix, &AdjacencyMatrixTable::edgeSelected, _elementPropertiesTable,
            &ElementPropertiesTable::onEdgeSelected);
    connect(_incidenceMatrix, &IncidenceMatrixTable::edgeSelected, _elementPropertiesTable,
            &ElementPropertiesTable::onEdgeSelected);

    connect(this, SIGNAL(startDemoAlgorithm(std::list<std::list<std::string> >, GraphDemoFlag)), _scene,
            SLOT(demoAlgorithm(std::list<std::list<std::string> >, GraphDemoFlag)));
    connect(this, SIGNAL(startDemoAlgorithm(std::list<std::string>, GraphDemoFlag)), _scene,
            SLOT(demoAlgorithm(std::list<std::string>, GraphDemoFlag)));
    connect(this, SIGNAL(startDemoAlgorithm(std::list<std::pair<std::string, std::string> >, GraphDemoFlag)), _scene,
            SLOT(demoAlgorithm(std::list<std::pair<std::string, std::string> >, GraphDemoFlag)));

    connect(_view, &GraphGraphicsView::nodeAdded, this, [this](QPointF pos, bool auto_naming) {
        if (!auto_naming) {
            showNewNodeDialog(pos);
            return;
        }
        this->_graph->addNode(Node(_graph->nextNodeName(), pos));
        emit graphChanged();
    });
    connect(_view, &GraphGraphicsView::nodeRemoved, this, [this](const std::string &node_name) {
        if (this->_graph->removeNode(node_name))
                emit graphChanged();
    });
    connect(_view, &GraphGraphicsView::nodeIsolated, this, [this](const std::string &node_name) {
        if (this->_graph->isolateNode(node_name))
                emit graphChanged();
    });
    connect(_view, &GraphGraphicsView::edgeRemoved, this, [this](const std::string &uname, const std::string &vname) {
        if (_graph->removeEdge(uname, vname))
                emit graphChanged();
    });
    connect(_view, &GraphGraphicsView::edgeSet, this, [this](const std::string &uname, const std::string &vname) {
        bool ok{};
        int defaultValue = _graph->hasEdge(uname, vname) ? _graph->weight(uname, vname) : 1;
        int w = _graph->isWeighted() ?
                (QInputDialog::getInt(this, tr("Set weight for edge(")
                                            + QString::fromStdString(_graph->node(uname)->name()) + ", "
                                            + QString::fromStdString(_graph->node(vname)->name()) + tr(")"),
                                      "0 <= weight < " + QString::number(INT_MAX),
                                      defaultValue, 1, INT_MAX, 1, &ok))
                                     : 1;
        qDebug() << w;
        if ((ok || _graph->isUnweighted()) && this->_graph->setEdge(uname, vname, w))
                emit graphChanged();
    });
    connect(_view, &GraphGraphicsView::startAlgorithm, this,
            [this](const StartAlgoFlag &algo, const std::string &source_name) {
                QDebugStream qout(std::cout, _ui->consoleText);
                if (algo == StartAlgoFlag::BFS) {
                    this->_ui->consoleText->clear();
                    auto result = GraphUtils::BFSToDemo(this->_graph, source_name);
                    emit startDemoAlgorithm(result, GraphDemoFlag::EdgeAndNode);
                } else if (algo == StartAlgoFlag::DFS) {
                    this->_ui->consoleText->clear();
                    auto result = GraphUtils::DFSToDemo(this->_graph, source_name);
                    emit startDemoAlgorithm(result, GraphDemoFlag::EdgeAndNode);
                }
            });
    connect(_view, &GraphGraphicsView::nodeEdited, this, [this](const std::string &node_name) {
        bool ok;
        QRegExp re("[a-zA-Z0-9]{1,3}");
        auto new_name = QInputDialog::getText(this, "Изменить название вершины", "Имя: ", QLineEdit::Normal,
                                              QString::fromStdString(_graph->nextNodeName()), &ok);
        if (ok) {
            if (!re.exactMatch(new_name)) {
                QMessageBox::critical(this, "Ошибка", "Название вершины должно состоять только из английского алфавита или цифр\n"
                                                     +
                                                     tr("Длина имени не должно превышать больше 3 символов и не может быть меньше 1"));
                return;
            }
            if (this->_graph->hasNode(new_name.toStdString()))
                QMessageBox::critical(this, "Ошибка", "Это имя уже занято другой вершиной");
            else {
                this->_graph->setNodeName(node_name, new_name.toStdString());
                emit graphChanged();
            }
        }
    });

    _ui->adjMatLayout->addWidget(this->_adjMatrix, 0, Qt::AlignCenter);
    auto gLabel = new QLabel(this);
    gLabel->setText("Свойства графа");
    _ui->propertiesLayout->addWidget(gLabel);
    _ui->propertiesLayout->addWidget(this->_graphPropertiesTable, 0, Qt::AlignTop);
    auto eLabel = new QLabel(this);
    eLabel->setText("Свойство выбранного элемента");
    eLabel->setContentsMargins(0, 12, 0, 0);
    _ui->propertiesLayout->addWidget(eLabel);
    _ui->propertiesLayout->addWidget(this->_elementPropertiesTable, 0, Qt::AlignTop);
    this->_view->setScene(this->_scene);
    _ui->visualLayout->addWidget(this->_view);
    this->_view->show();
    setWorkspaceEnabled(false);
}

void MainWindow::resetGraph(Graph *graph) {
    delete this->_graph;
    this->_graph = graph;
    this->_scene->setGraph(_graph);
    this->_adjMatrix->setGraph(_graph);
    this->_incidenceMatrix->setGraph(_graph);
    this->_elementPropertiesTable->setGraph(_graph);
    this->_graphPropertiesTable->setGraph(_graph);
}

void MainWindow::initWorkspace(const QString &filename, bool new_file) {

    try {
        if (!new_file) {
            if (this->_dataNeedSaving) {
                QMessageBox::StandardButton reply = QMessageBox::question(this, "Сохранить граф?",
                                                                          "Ваши изминения пропадут если вы их не сохрнаите!!",
                                                                          QMessageBox::No | QMessageBox::Yes |
                                                                          QMessageBox::Cancel);
                if (reply == QMessageBox::Cancel)
                    return;
            }
            resetGraph(new Graph(Graph::readFromFile(filename.toStdString())));
            this->_dataNeedSaving = false;
        } else {
            bool weighted, directed, ok;
            int node_num;
            GraphOptionDialog::initGraph(this, weighted, directed, node_num, ok);
            if (!ok) return;
            this->_dataNeedSaving = true;
            resetGraph(new Graph(node_num, directed, weighted));
        }
        emit graphChanged();
    }
    catch (...) {
        setWorkspaceEnabled(false);
        QMessageBox::critical(this, "Ошибки",
                              "Что-то пошло не так",
                              QMessageBox::Cancel);
        if (_workingFilename != "") setWorkspaceEnabled(true);
        return;
    }
    this->_workingFilename = filename;
    int index = filename.toStdString().find_last_of("/\\");
    std::string input_trace_filename = filename.toStdString().substr(index + 1);
    setWindowTitle(QString::fromStdString(input_trace_filename) + " - Simple Graph Tool");
    setWorkspaceEnabled(true);
}

MainWindow::~MainWindow() {
    delete _ui;
    delete _graph;
    delete _scene;
    delete _view;
    delete _elementPropertiesTable;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_dataNeedSaving) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Сохранить граф?",
                                                                  "Ваши изминения пропадут если вы их не сохрнаите!!",
                                                                  QMessageBox::No | QMessageBox::Yes |
                                                                  QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
            Graph::writeToFile(_workingFilename.toStdString(), *_graph);
        else if (reply == QMessageBox::Cancel)
            event->ignore();
    }
}

QString MainWindow::showOpenFileDialog() {
    return QFileDialog::getOpenFileName(
            this,
            tr("Открыть файл"),
            QDir::currentPath(),
            tr("Файлы графа (*.gph)"),
            nullptr,
            QFileDialog::DontUseNativeDialog);
}

QString MainWindow::showSaveFileDialog() {
    QString newFilename;
    for (int i = 1; true; ++i) {
        std::ifstream is(QDir::currentPath().toStdString()
                         + "/graph" + std::to_string(i) + ".gph");
        if (!is.good()) {
            newFilename = QString::fromStdString(QDir::currentPath().toStdString()
                                                 + "/graph" + std::to_string(i) + ".gph");
            break;
        }
    }
    return QFileDialog::getSaveFileName(this, tr("Новый граф"),
                                        newFilename,
                                        tr("Граф файлы (*.gph)"),
                                        nullptr,
                                        QFileDialog::DontUseNativeDialog);
}

void MainWindow::showNewNodeDialog(QPointF pos) {
    bool ok;
    QRegExp re("[a-zA-Z0-9]{1,3}");
    QString newNodeName = QInputDialog::getText(this, "Добавить вершину", "Имя: ", QLineEdit::Normal,
                                                QString::fromStdString(_graph->nextNodeName()), &ok);
    if (ok) {
        if (!re.exactMatch(newNodeName)) {
            QMessageBox::critical(this, "Ошибка", "Название вершины должно состоять только из английского алфавита или цифр\n"
                                                 +
                                                 tr("Длина имени не должно превышать больше 3 символов и не может быть меньше 1"));
            return;
        }
        Node newNode(newNodeName.toStdString(), pos);
        bool succeeded = _graph->addNode(newNode);
        if (!succeeded)
            QMessageBox::critical(this, "Ошибка", "Это имя уже используется для другой вершины");
        else
                emit graphChanged();
    }
}

void MainWindow::setWorkspaceEnabled(bool ready) {
    if (ready) _ui->entryWidget->setVisible(false);
    _ui->workingWidget->setVisible(ready);
    _ui->menuGraph->setEnabled(ready);
    _ui->menuAlgorithms->setEnabled(ready);
    for (auto action: _ui->menuFile->actions())
        if (!action->menu() && !action->isSeparator()
            && action->text().contains("Сохранить"))
            action->setEnabled(ready);
    if (ready)
        _view->scale(1, 1);
}

void MainWindow::onGraphChanged() {
    this->_dataNeedSaving = true;
    _ui->statusBar->clearMessage();
    _ui->consoleText->clear();
}

void MainWindow::on_createGraphButton_clicked() {
    QString filename = showSaveFileDialog();
    if (!filename.isNull())
        initWorkspace(filename, true);
}

void MainWindow::on_openGraphButton_clicked() {
    QString filename = showOpenFileDialog();
    if (!filename.isNull())
        initWorkspace(filename);
}

void MainWindow::on_actionSave_triggered() {
    if (_dataNeedSaving) {
        this->_dataNeedSaving = false;
        Graph::writeToFile(_workingFilename.toStdString(), *_graph);
        _ui->statusBar->showMessage("Успешно сохранено");
        QTimer::singleShot(2000, this, [this]() {
            this->_ui->statusBar->clearMessage();
        });
    }
}

void MainWindow::on_actionSave_As_triggered() {
    QString filename = showSaveFileDialog();
    if (!filename.isNull())
        Graph::writeToFile(_workingFilename.toStdString(), *_graph);
}

void MainWindow::on_actionNew_Graph_triggered() {
    QString filename = showSaveFileDialog();
    if (!filename.isNull())
        initWorkspace(filename, true);
}

void MainWindow::on_actionOpen_Graph_triggered() {
    QString filename = showOpenFileDialog();
    if (!filename.isNull() && filename != _workingFilename)
        initWorkspace(filename);
}

void MainWindow::on_actionCredits_triggered() {
    QMessageBox::about(this, "Авторы", "Главный разработчик: Hao Phan Phu - KHTN2018 - UIT, \nИдейный последователи и пере"
                                       "водичики: Байлов Е.В. Алексенков А.Е. \nКрылов Л.К. "
                                       "Нургалиева А.И. Калекина М.А. Ключникова Д.Д. ");
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit(0);
}

void MainWindow::on_actionAddNode_triggered() {
    showNewNodeDialog();
}

void MainWindow::on_actionAddEdge_triggered() {
    bool ok{};
    QList<QString> labelText;
    labelText.push_back("Первая вершина: ");
    labelText.push_back("Вторая вершина: ");
    labelText.push_back("Weight: ");
    QList<QString> list = MultiLineInputDialog::getStrings(this, "Добавить новое ребрно", labelText, &ok);
    QRegExp re("\\d*");
    if (ok && !list.empty() && re.exactMatch(list[2])) {
        bool succeeded = _graph->setEdge(list[0].toStdString(), list[1].toStdString(), list[2].toInt());
        if (succeeded)
                emit graphChanged();
        else
            QMessageBox::critical(this, "Ошибка", "Нельзя создать такое ребро!");
    }
}

void MainWindow::on_actionEditEdge_triggered() {
    bool ok{};
    QList<QString> labelText;
    labelText.push_back("Первая вершина: ");
    labelText.push_back("Вторая вершина: ");
    labelText.push_back("Weight: ");
    QList<QString> list = MultiLineInputDialog::getStrings(this, "Изменить ребро", labelText, &ok);
    QRegExp re("\\d*");
    if (ok && !list.empty() && re.exactMatch(list[2])) {
        if (_graph->hasEdge(list[0].toStdString(), list[1].toStdString())) {
            bool succeeded = _graph->setEdge(list[0].toStdString(), list[1].toStdString(), list[2].toInt());
            if (succeeded) {
                emit graphChanged();
                return;
            }
        }
        QMessageBox::critical(this, "Ошибка", "Здесь нет такого ребра!");
    }
}

void MainWindow::on_actionDelNode_triggered() {
    bool ok;
    QString nameToDel = QInputDialog::getText(this, "Удалить вершину", "Имя вершины: ", QLineEdit::Normal, QString(), &ok);
    if (ok) {
        bool succeeded = _graph->removeNode(nameToDel.toStdString());
        if (!succeeded)
            QMessageBox::critical(this, "Ошибка", "Несуществует такой вершины!");
        else
                emit graphChanged();
    }
}

void MainWindow::on_actionDelEdge_triggered() {
    bool ok{};
    QList<QString> labelText;
    labelText.push_back("Веришна 1: ");
    labelText.push_back("Вершина 2: ");
    QList<QString> list = MultiLineInputDialog::getStrings(this, "Удалить ребро", labelText, &ok);
    if (ok && !list.empty()) {
        bool succeeded = _graph->removeEdge(list[0].toStdString(), list[1].toStdString());
        if (succeeded)
                emit graphChanged();
        else
            QMessageBox::critical(this, "Ошибка", "Между этими вершинами нет ребра!");
    }
}

void MainWindow::on_BFSbtn_clicked() {
    _ui->consoleText->clear();
    bool ok{};
    QStringList items;
    for (auto node: _graph->nodeList())
        items.append(QString::fromStdString(node->name()));
    auto source_str = QInputDialog::getItem(this, "Начальная вершина:", "Имя", items, 0, false, &ok);
    if (ok) {
        if (source_str.isNull())
            return;
        auto source = _graph->node(source_str.toStdString());
        if (_graph->hasNode(source)) {
            QDebugStream qout(std::cout, _ui->consoleText);
            auto result = GraphUtils::BFSToDemo(_graph, source->name());
            emit startDemoAlgorithm(result, GraphDemoFlag::EdgeAndNode);
        } else {
            QMessageBox::critical(this, "Ошибка", tr("Вершина не названа ") + source_str);
        }
    }
}

void MainWindow::on_DFSbtn_clicked() {
    _ui->consoleText->clear();
    bool ok{};
    QStringList items;
    for (auto node: _graph->nodeList())
        items.append(QString::fromStdString(node->name()));
    auto source_str = QInputDialog::getItem(this, "Начальная вершина:", "Имя", items, 0, false, &ok);
    if (ok) {
        if (source_str.isNull())
            return;
        auto source = _graph->node(source_str.toStdString());
        if (_graph->hasNode(source)) {
            QDebugStream qout(std::cout, _ui->consoleText);
            auto result = GraphUtils::DFSToDemo(_graph, source->name());
            emit startDemoAlgorithm(result, GraphDemoFlag::EdgeAndNode);
        } else
            QMessageBox::critical(this, "Ошибка", tr("Веришна не названа ") + source_str);
    }
}

void MainWindow::on_EulerBtn_clicked() {

    _ui->consoleText->clear();
    QDebugStream qout(std::cout, _ui->consoleText);
    auto result = GraphUtils::displayAllEulerianCircuits(_graph);
    emit startDemoAlgorithm(result, GraphDemoFlag::EdgeAndNode);
}



void MainWindow::on_actionBFS_triggered() {
    on_BFSbtn_clicked();
}

void MainWindow::on_actionDFS_triggered() {
    on_DFSbtn_clicked();
}


void MainWindow::on_actionEuler_Cycle_triggered() {
    on_EulerBtn_clicked();
}


void MainWindow::on_tabWidget_currentChanged(int index) {
    _elementPropertiesTable->onUnSelected();
}


//void MainWindow::on_isFull_clicked()
//{
//    _ui->consoleText->clear();
//    _ui->consoleText->append(GraphUtils::isFull(this->_graph));
//}

//void MainWindow::on_sum_clicked()
//{
//    _ui->consoleText->clear();
//    _ui->consoleText->append(GraphUtils::SummaryOfAllNodesDegrees(this->_graph));
//}

void MainWindow::on_dVN_clicked()
{
    std::unordered_map<std::string, bool> visited;
    std::list<std::string> vertexes;
    int radix{};
    QStringList items;
    for (auto node: _graph->nodeList())
        items.append(QString::fromStdString(node->name()));
    auto source = DVNINPUTDIALOG::initDVN(this,items,radix);
    GraphUtils::DFSRadix(_graph, source, visited, vertexes, radix);
    QString list;
    QString space = " ";
    std::for_each(vertexes.begin(), vertexes.end(), [&list, &space](auto &el){
        list.append(QString::fromStdString(el));
        list.append(space);
    });
    _ui->consoleText->clear();
    _ui->consoleText->append("Нач. вершина (" + QString::fromStdString(source) + "):");
    _ui->consoleText->append(list);
}
