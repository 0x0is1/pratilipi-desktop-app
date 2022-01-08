#include "mainwindow_video.h"
#include "ui_mainwindow_video.h"

static QJsonArray playlist_id_container, video_id_container;

mainwindow_video::mainwindow_video(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwindow_video)
{
    ui->setupUi(this);
    playlist_dropdown = new QComboBox(this);
    video_dropdown = new QComboBox(this);
    player = new QMediaPlayer(this);
    vw = new QVideoWidget(this);
    playlist_dropdown->setFixedWidth(150);
    video_dropdown->setFixedWidth(150);
    player->setVideoOutput(vw);
    this->setCentralWidget(vw);
    ui->statusbar->addWidget(playlist_dropdown);
    ui->statusbar->addWidget(video_dropdown);
    ui->actionPlay->setDisabled(true);
    ui->actionNext->setDisabled(true);
    ui->actionPrevious->setDisabled(true);
    ui->actionPause->setDisabled(true);
    connect(playlist_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(list_playlist(int)));
    connect(video_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(list_videos(int)));
}

mainwindow_video::~mainwindow_video()
{
    system("pkill youtube-dl");
    delete player;
    delete vw;
    delete ui;
}

void mainwindow_video::list_playlist(int)
{
    QString playlist_uri = "https://www.youtube.com/playlist?list=";
    std::string cmd = ("youtube-dl -J --flat-playlist " + playlist_uri.toStdString() + playlist_id_container[playlist_dropdown->currentIndex()].toString().toStdString() + " > video_container.json");
    system(cmd.c_str());
    QFile file;
    file.setFileName("video_container.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument documents = QJsonDocument::fromJson(data.toUtf8());
    QJsonValue jdata = documents.object().value("entries");
    QJsonObject obj;
    video_id_container = QJsonArray();
    video_dropdown->clear();
    foreach(QJsonValue val, jdata.toArray()){
        obj = val.toObject();
        video_id_container.append(obj.value("id").toString());
        video_dropdown->addItem(obj.value("title").toString());
    }
    system("rm video_container.json");
}

void mainwindow_video::list_videos(int)
{
    QString video_uri = "https://www.youtube.com/watch?v=";
    system(("youtube-dl -f 18 -g " + video_uri.toStdString() + video_id_container[video_dropdown->currentIndex()].toString().toStdString() + " > link_container.txt").c_str());
    QFile file;
    file.setFileName("link_container.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString link = file.readAll();
    file.close();
    player->setMedia(QUrl(link.toUtf8()));
    this->setWindowTitle(video_dropdown->currentText());
    ui->actionPlay->setDisabled(false);
    ui->actionPause->setDisabled(true);
    system("rm link_container.txt");
}

void mainwindow_video::on_actionPrevious_triggered()
{
    if(video_dropdown->currentIndex() <= 0){
        video_dropdown->setCurrentIndex(0);
        return;
    }
    video_dropdown->setCurrentIndex(video_dropdown->currentIndex()-1);
}

void mainwindow_video::on_actionRefresh_triggered()
{
    QString playlist_uri = "https://www.youtube.com/c/PratilipiFilms2020/playlists";
    system(("youtube-dl -J --flat-playlist " + playlist_uri.toStdString() + " > playlist_container.json").c_str());
    QFile file;
    file.setFileName("playlist_container.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument documents = QJsonDocument::fromJson(data.toUtf8());
    QJsonValue jdata = documents.object().value("entries");
    QJsonObject obj;
    playlist_id_container = QJsonArray();
    playlist_dropdown->clear();
    foreach(QJsonValue val, jdata.toArray()){
        obj = val.toObject();
        playlist_id_container.append(obj.value("id").toString());
        playlist_dropdown->addItem(obj.value("title").toString());
    }
    ui->actionPlay->setDisabled(false);
    ui->actionNext->setDisabled(false);
    ui->actionPrevious->setDisabled(false);
    ui->actionPause->setDisabled(true);
    system("rm playlist_container.json");
}

void mainwindow_video::on_actionPlay_triggered()
{
    player->play();
    ui->actionPlay->setDisabled(true);
    ui->actionPause->setDisabled(false);
}

void mainwindow_video::on_actionNext_triggered()
{
    if(video_dropdown->currentIndex() >= video_dropdown->count()+1){
        video_dropdown->setCurrentIndex(video_dropdown->count()+1);
        return;
    }
    video_dropdown->setCurrentIndex(video_dropdown->currentIndex()+1);
}

void mainwindow_video::on_actionPause_triggered()
{
    player->pause();
    ui->actionPause->setDisabled(true);
    ui->actionPlay->setDisabled(false);
}
