#include "MyGraphicsScene.h"
/*#include "items/TextContent.h"
#include "items/TextConfig.h"*/
/*#include "RenderOpts.h"*/
#include <QAbstractTextDocumentLayout>
#include <QFile>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QImageReader>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
// #include <QNetworkAccessManager>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTimer>
#include <QUrl>

#define COLORPICKER_W 200
#define COLORPICKER_H 150

MyGraphicsScene::MyGraphicsScene(QObject * parent)
    : QGraphicsScene(parent)
{
}

MyGraphicsScene::~MyGraphicsScene()
{
}
