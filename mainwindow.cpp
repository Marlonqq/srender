#include "mainwindow.h"

static inline QString GenerateStyleSheet(QColor& color)
{
    return "background-color: rgb("
        + QString::number(static_cast<int>(color.red())) + ','
        + QString::number(static_cast<int>(color.green())) + ','
        + QString::number(static_cast<int>(color.blue())) + ");";
}


mainwindow::mainwindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::mainwindowClass)
{
    ui->setupUi(this);

    connect(ui->RenderWidget, &SRenderWidget::SendModelData, this,
        [this](int FaceCount, int VertCount)
        {
            ui->FaceCountLabel->setText(QString::number(FaceCount));
            ui->VertCountLabel->setText(QString::number(VertCount));
        }
    );

    SetLightColor(AMBITIENT, QColor(102, 102, 102));
    SetLightColor(DIFFUSE, QColor(153, 153, 153));
    SetLightColor(SPECULAR, QColor(255, 255, 255));
    SetLightDir();
}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::SetLightColor(LIGHT_TYPE light_type, QColor color)
{
    switch (light_type)
    {
    case AMBITIENT:
        ui->Ambitient->setStyleSheet(GenerateStyleSheet(color));
        ambitient_color = color;
        break;
    case DIFFUSE:
        ui->Diffuse->setStyleSheet(GenerateStyleSheet(color));
        diffuse_color = color;
        break;
    case SPECULAR:
        ui->Specular->setStyleSheet(GenerateStyleSheet(color));
        specular_color = color;
        break;
    }

    ui->RenderWidget->SetLightColor(light_type, color);
}

void mainwindow::SetLightDir()
{
    glm::vec3 light_dir;
    float pitch = glm::radians(glm::clamp(static_cast<float>(ui->PitchSlider->value()), -89.9f, 89.9f));
    float yaw   = glm::radians(static_cast<float>(ui->YawDial->value()));

    light_dir.x = static_cast<float>(std::sin(pitch)) * static_cast<float>(std::cos(yaw));
    light_dir.y = static_cast<float>(std::cos(pitch));
    light_dir.z = static_cast<float>(std::sin(pitch)) * static_cast<float>(std::sin(yaw));
    ui->RenderWidget->SetLightDir(light_dir);
}


void mainwindow::on_action_open_file_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "","OBJ(*.obj)");
    if (!filePath.isEmpty())
        ui->RenderWidget->LoadModel(filePath);
}

void mainwindow::on_action_save_image_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "JPG(*.JPG)");
    if (!fileName.isEmpty())
        ui->RenderWidget->SaveImage(fileName);
}

void mainwindow::on_action_multithread_triggered()
{
    ui->RenderWidget->SetMultiThread();
}

void mainwindow::on_AmbitientColorSet_clicked()
{
    QColor color = QColorDialog::getColor(ambitient_color, this, "Select Ambitient Color");
    SetLightColor(AMBITIENT, color);
}

void mainwindow::on_DiffuseColorSet_clicked()
{
    QColor color = QColorDialog::getColor(diffuse_color, this, "Select Diffuse Color");
    SetLightColor(DIFFUSE, color);
}

void mainwindow::on_SpecularColorSet_clicked()
{
    QColor color = QColorDialog::getColor(specular_color, this, "Select Specular Color");
    SetLightColor(SPECULAR , color);
}

void mainwindow::on_PitchSlider_valueChanged(int value)
{
    SetLightDir();
}

void mainwindow::on_YawDial_valueChanged(int value)
{
    SetLightDir();
}

void mainwindow::on_FovSilder_valueChanged(int value)
{
    ui->RenderWidget->SetCameraPara(FOV, static_cast<float>(value));\
}

void mainwindow::on_NearSilder_valueChanged(int value)
{
    ui->RenderWidget->SetCameraPara(ZNEAR, value / 10.0f);
}




