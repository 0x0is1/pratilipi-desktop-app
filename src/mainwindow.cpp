#include "mainwindow.h"
#include "ui_mainwindow.h"

static QString access_token;
static QJsonArray slug_container, plid_container, img_name_container;
static int page_no;
static QStringList languages = {
    "HINDI",
    "ENGLISH",
    "GUJARATI",
    "MARATHI",
    "TAMIL",
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    comic_type = new QComboBox(this);
    comics_name = new QComboBox(this);
    episode_name = new QComboBox(this);
    lang_select = new QComboBox(this);
    pic_container = new QLabel(this);
    page_no_container = new QLabel(this);
    film_window = new QPushButton(this);
    window = new mainwindow_video(this);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    film_window->setText("Watch pratilipi films");
    film_window->setFixedWidth(150);
    comic_type->setFixedWidth(150);
    comics_name->setFixedWidth(150);
    episode_name->setFixedWidth(150);
    ui->statusBar->addWidget(comic_type);
    ui->statusBar->addWidget(comics_name);
    ui->statusBar->addWidget(episode_name);
    ui->statusBar->addWidget(page_no_container);
    ui->actionPrevious->setDisabled(true);
    ui->actionPlay->setDisabled(true);
    ui->actionNext->setDisabled(true);
    pic_container->setScaledContents(true);
    pic_container->setText("Please select a comic to read");
    pic_container->setAlignment(Qt::AlignCenter);
    lang_select->addItems(languages);
    ui->mainToolBar->addWidget(spacer);
    ui->mainToolBar->addWidget(lang_select);
    ui->mainToolBar->addWidget(film_window);
    this->setCentralWidget(pic_container);
    connect(comic_type, SIGNAL(currentIndexChanged(int)), this, SLOT(comic_typeChanged(int)));
    connect(comics_name, SIGNAL(currentIndexChanged(int)), this, SLOT(comics_nameChanged(int)));
    connect(episode_name, SIGNAL(currentIndexChanged(int)), this, SLOT(episode_nameChanged(int)));
    connect(film_window, SIGNAL(clicked()), this, SLOT(open_film_window()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_accesstoken()
{
    QUrl url = QUrl("https://gamma.pratilipi.com/api/user/accesstoken");
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_accesstoken_onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    mgr->get(request);
}

void MainWindow::get_accesstoken_onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts), token;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject = document.object();
    access_token = jsonObject.value("accessToken").toString();
}

void MainWindow::get_homepage(QString lang, int limit=200)
{
    get_accesstoken();
    QUrl url = BASE_URL1 + "comics/init/v4.3/comic-Webhome?limit=" + QString::number(limit) + "&language="+lang+"&bucketId=10";
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_homepage_onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    request.setRawHeader("access-token", QByteArray::fromStdString(access_token.toStdString()));
    mgr->get(request);
}

void MainWindow::get_homepage_onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts), token;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, obj, attrib;
    jsonObject = document.object();
    QJsonValue data, indata;
    data = jsonObject.value("widgets");
    comic_type->clear();
    slug_container = QJsonArray();
    foreach(QJsonValue value, data.toArray())
    {
        QJsonArray subcontainer;
        obj = value.toObject();
        if(obj.value("type").toString() != "COMIC_LIST") continue;
        indata = obj.value("data");
        comic_type->addItem(indata.toObject().value("displayTitle").toString());
        foreach(QJsonValue inval, indata.toObject().value("list").toArray()){
            attrib = inval.toObject().value("attributes").toObject();
            subcontainer.append(attrib.value("title").toString());
            subcontainer.append(attrib.value("slug").toString());
        }
        slug_container.append(subcontainer);
    }
}

void MainWindow::get_episodes(QString slug, int limit=30)
{
    QUrl url = BASE_URL2 + "/api/series/v1.0/pratilipi?slug=" + slug + "&offset=0&limit=" + QString::number(limit);
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_episodes_onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    request.setRawHeader("access-token", QByteArray::fromStdString(access_token.toStdString()));
    mgr->get(request);
}

void MainWindow::get_episodes_onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, obj;
    jsonObject = document.object();
    QJsonValue data = jsonObject.value("pratilipiList");
    episode_name->clear();
    plid_container = QJsonArray();
    foreach(QJsonValue value, data.toArray()){
        obj = value.toObject();
        episode_name->addItem(obj.value("title").toString());
        plid_container.append(QString("%1").arg(obj.value("pratilipiId").toDouble(),0,'f',0));
    }
}

void MainWindow::get_episode(QString plid)
{
    QUrl url = BASE_URL2 + "/api/pratilipi/content?pratilipiId=" + plid + "&_apiVer=4";
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_episode_onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    request.setRawHeader("access-token", QByteArray::fromStdString(access_token.toStdString()));
    mgr->get(request);
}

void MainWindow::get_episode_onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, obj;
    jsonObject = document.object();
    QJsonValue data = jsonObject.value("content").toObject().value("chapters")
    .toArray()[0].toObject().value("pages").toArray()[0].toObject()
    .value("pagelets");
    img_name_container = QJsonArray();
    foreach(QJsonValue value, data.toArray()){
        obj = value.toObject().value("data").toObject();
        img_name_container.append(obj.value("name").toString());
    }
    page_no = 0;
}

void MainWindow::comic_typeChanged(int)
{
    ui->actionPlay->setDisabled(false);
    comics_name->clear();
    QJsonArray arr = slug_container[comic_type->currentIndex()].toArray();
    for(int i=0; i < arr.size(); i++){
        if(i%2==0){
            comics_name->addItem(arr[i].toString());
        }
    }
}

void MainWindow::comics_nameChanged(int)
{
    QJsonArray arr = slug_container[comic_type->currentIndex()].toArray();
    get_episodes(arr[(comics_name->currentIndex()*2)+1].toString());
}

void MainWindow::episode_nameChanged(int)
{
    QString plid = plid_container[episode_name->currentIndex()].toString();
    get_episode(plid);
    this->setWindowTitle(episode_name->currentText());
}

void MainWindow::open_film_window()
{
    window->show();
}

void MainWindow::get_image(int idx)
{
    QUrl url = "https://static-assets.pratilipi.com/pratilipi/content/image?pratilipiId=" + plid_container[episode_name->currentIndex()].toString() + "&name=" + img_name_container[idx].toString() + "&width=600";
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_image_onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    mgr->get(request);
}

void MainWindow::get_image_onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QImage auth_img;
    auth_img.loadFromData(bts);
    QPixmap image = QPixmap::fromImage(auth_img);
    pic_container->setPixmap(image.scaled((image.height()/pic_container->height())*image.width(), pic_container->height()));
    page_no_container->setText("Page " + QString::number(page_no+1) + "/" + QString::number(img_name_container.size()));
}

void MainWindow::on_actionPlay_triggered()
{
    ui->actionPrevious->setDisabled(false);
    ui->actionNext->setDisabled(false);
    pic_container->clear();
    get_image(page_no);
}

void MainWindow::on_actionRefresh_triggered()
{
    ui->actionPrevious->setDisabled(true);
    ui->actionPlay->setDisabled(true);
    ui->actionNext->setDisabled(true);
    get_homepage(lang_select->currentText());
}

void MainWindow::on_actionPrevious_triggered()
{
    if (page_no<=0) return;
    page_no = page_no - 1;
    pic_container->clear();
    get_image(page_no);
}

void MainWindow::on_actionNext_triggered()
{    
    if (page_no>=img_name_container.size()-1) return;
    page_no = page_no + 1;
    pic_container->clear();
    get_image(page_no);

}
