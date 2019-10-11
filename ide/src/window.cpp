#include "window.h"
#include "items.h"
#include "component.h"
#include "plugin.h"

#include <chrono>
#include <dlfcn.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMenuBar>
#include <QToolBar>
#include <QThread>
#include <QDockWidget>

using namespace ocvflow;

class ocvflow::MainWindowPrivate {
    QMap<ToolBarNames, QToolBar*> toolbars;
    QMap<QString, Component *> components;

    QDockWidget *propertiesDock;

    QThread *runner;
    bool runing{false};

    friend class MainWindow;
};

MainWindow::MainWindow(QWidget *parent) :
        d_ptr(new MainWindowPrivate)
{

    setCentralWidget(new CentralWidget);

    makeToolbar();
    makeActions();
    makeDocks();

    loadPlugins();
}

MainWindow::~MainWindow()
{

}

MainWindow *MainWindow::inst = nullptr;
MainWindow *MainWindow::instance()
{
    if (!inst) inst = new MainWindow();
    return inst;
}

CentralWidget *MainWindow::centralWidget() const
{
    return static_cast<CentralWidget *>(QMainWindow::centralWidget());
}

QToolBar *MainWindow::toolbar(const ToolBarNames &name)
{
    return d_func()->toolbars[name];
}

Component *MainWindow::component(const std::string &name)
{
    if (d_func()->components.contains(QString::fromStdString(name)))
        return d_func()->components[QString::fromStdString(name)];
    return nullptr;
}

void MainWindow::addNode(NodeItem *node)
{
    centralWidget()->scene()->addItem(node);
}

void MainWindow::connectNode(NodeItem *source, NodeItem *dest)
{
    centralWidget()->scene()->addItem(new EdgeItem(source, dest));
}

void MainWindow::nodeClicked(NodeItem *node)
{
    auto last = d_func()->propertiesDock->widget();
    d_func()->propertiesDock->setWidget(nullptr);

    if (last) {
        last->hide();
        last->deleteLater();
    }

    QWidget* widget = node->createPropertiesWidget(d_func()->propertiesDock);
    if (widget) {
        d_func()->propertiesDock->setWidget(widget);
    }
}

void MainWindow::makeToolbar()
{
    setMenuBar(new QMenuBar);

    d_func()->toolbars.insert(FilesTB, new QToolBar);
    d_func()->toolbars.insert(SourcesTB, new QToolBar);
    d_func()->toolbars.insert(ProcessorsTB, new QToolBar);
    d_func()->toolbars.insert(ConnectorsTB, new QToolBar);
    d_func()->toolbars.insert(BuildTB, new QToolBar);
    d_func()->toolbars.insert(WindowTB, new QToolBar);

    for (auto && tb: d_func()->toolbars.values())
    {
        tb->hide();
        tb->setMovable(false);

        this->addToolBar(tb);
    }

    auto act = new QAction("Files", this);
    act->setData(FilesTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);

    act = new QAction("Sources", this);
    act->setData(SourcesTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);
    act->hover();

    act = new QAction("Processors", this);
    act->setData(ProcessorsTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);

    act = new QAction("Connectors", this);
    act->setData(ConnectorsTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);

    act = new QAction("Build", this);
    act->setData(BuildTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);

    act = new QAction("Window", this);
    act->setData(WindowTB);
    connect(act, &QAction::hovered, this, &MainWindow::showToolBar);
    menuBar()->addAction(act);
}

void MainWindow::showToolBar()
{
    sender()->setProperty("menuselected", true);

    for (auto && tb: d_func()->toolbars.values())
        tb->hide();

    QVariant data = qobject_cast< QAction* >(sender())->data();
    if (data.isValid())
        d_func()->toolbars.value( static_cast<ToolBarNames>(data.toUInt()) )->show();
}

void MainWindow::makeActions()
{
    auto tb = toolbar(BuildTB);

    auto act = new QAction("Run");
    connect(act, &QAction::triggered, this, &MainWindow::run);
    tb->addAction(act);

    act = new QAction("Stop");
    connect(act, &QAction::triggered, this, &MainWindow::stopRun);
    tb->addAction(act);
}

void MainWindow::makeDocks()
{
    //propertiesDock
    d_func()->propertiesDock = new QDockWidget(this->tr("Propriedades"), this);
    d_func()->propertiesDock->setObjectName("propertiesDock");
    d_func()->propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    this->addDockWidget(Qt::RightDockWidgetArea, d_func()->propertiesDock);

    toolbar(WindowTB)->addAction(d_func()->propertiesDock->toggleViewAction());
}

void MainWindow::loadPlugins()
{
    try{
        std::vector<Component*> comps;

        QStringList typeFiles;

        //set plugin path
        QString path = qApp->applicationDirPath();
        path = path.left(path.lastIndexOf("/")) + "/plugins";

        //load libs on plugins dir
        QDir pluginsDir(path);
        for (QString fileName: pluginsDir.entryList(typeFiles, QDir::Files))
        {
            try{
                //Carrega a biblioteca
                void * handle = dlopen((path + "/" + fileName).toStdString().c_str(), RTLD_LAZY);
                if (handle) {
                    //Procura pela função loadPlugin
                    ocvflow::PluginInterface* (*loadPlugin)() = (ocvflow::PluginInterface* (*)()) dlsym(handle, "loadPlugin");

                    //Verifica se foi encontrada a funcao
                    if (loadPlugin) {
                        //carrega a classe plugin
                        ocvflow::PluginInterface* plugin = loadPlugin();
                        if (plugin) {
                            auto compsLoaded = plugin->components();
                            comps.insert(comps.end(), compsLoaded.begin(), compsLoaded.end());
                        }
                    }
                }
            } catch (...) {}
        }

        for (auto && comp: comps)
        {
            d_func()->components.insert(QString::fromStdString(comp->name()), comp);

            auto tb = toolbar( static_cast<ToolBarNames>(comp->actionToolBar()) );
            if (tb) {
                auto action = comp->createAction();
                if (action)
                    tb->addAction(action);

                auto widget = comp->createWidget();
                if (widget)
                    tb->addWidget(widget);
                if (!action && !widget) {
                    tb->addAction(new QAction(QString::fromStdString(comp->name())));
                }
            }
        }
    }
    catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        assert(0);
    }
}

void itemsAdd(QList< NodeItem* >& ordered, QGraphicsItem* item) {
    if (NodeItem::Type != item->type())
        return;

    auto nodeItem = static_cast<NodeItem*>(item);

    if (!ordered.contains(nodeItem))
        for (auto edge: nodeItem->edges()) {
            auto edgeItem = static_cast<EdgeItem*>(edge);

            if (edgeItem->sourceNode() != nodeItem)
                itemsAdd(ordered, edgeItem->sourceNode());
        }

    if (!ordered.contains(nodeItem))
        ordered.append(nodeItem);
}

void MainWindow::run()
{
    if (d_func()->runing) return;
    MainWindow::stopRun();
    d_func()->runing = true;

    d_func()->runner = QThread::create([this] {
        std::clock_t last = std::clock();

        //Enfileira ordem de processamento.
        QList<NodeItem *> items;
        for (auto && item: this->centralWidget()->items()) {
            itemsAdd(items, item);
        }

        for (auto item: items) {
            item->start();
            item->setData(ErrorData, QVariant());
        }
        forever {
            //Executa todos os processos
            for (auto item: items)
            {
                //Todo: add semapharo
                item->acquire();
                try {
                    item->proccess();
                } catch (cv::Exception& ex) {
                    item->setData(ErrorData, QString::fromStdString(ex.msg));
                } catch (...) {}

                item->release();
            }
            //Atualiza a tela
            if (float( std::clock () - last ) > 42) {
                last = std::clock();
                for (auto && item: items) {
                    item->setData(LastUpdateCall, float(last));
                    item->update();
                }
                QThread::msleep(10 + items.size());
            }
            if (!d_func()->runing)
                break;
        }
        for (auto item: items) {
            item->stop();
        }
    });

    d_func()->runner->start();
    d_func()->runner->setPriority(QThread::LowestPriority);
}

void MainWindow::stopRun()
{
    if (d_func()->runing) {
        d_func()->runing = false;
        if (d_func()->runner->wait(1000)) {
            d_func()->runner->terminate();
            d_func()->runner->wait();
        }
        d_func()->runner->deleteLater();
    }
}
