#include "SRenderWidget.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Light light;
Material material;
Camera camera;

bool multi_thread = true;
float speed = 0.001;
float sensitity = 0.5;
int lastFrameTime = 1;
int deltaTime;

float scale = 0.01f;
glm::mat4 matrix(1.0f);
glm::mat4 transform(1.0f);

SRenderWidget::SRenderWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    connect(&timer, &QTimer::timeout, this, &SRenderWidget::Render);
    timer.start(1);

    camera = new Camera();
    SRender::getInstance().shader = new BlinnPhongShader();
    SRender::getInstance().shader->cameraPos = camera->eye;
    
    /*light.ambitient = glm::vec3(0.2f, 0.2f, 0.2f);
    light.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    light.dir = glm::vec3(1.0f, -1.0f, 1.0f);*/

}

SRenderWidget::~SRenderWidget()
{
    delete model;
}

void SRenderWidget::LoadModel(QString path)
{
    if (model) delete model;
    model = new Model(path);

    if (!model->load_success)
    {
        QMessageBox::critical(this, "Error", "Model loading error!");
        delete model;
        return;
    }

    emit SendModelData(model->face_cnt, model->vert_cnt);
}

void SRenderWidget::SaveImage(QString path)
{
    stbi_flip_vertically_on_write(true);
    stbi_write_jpg(path.toStdString().c_str(), 
        SRender::getInstance().frameBuffer->getWidth(), 
        SRender::getInstance().frameBuffer->getHeight(), 
        SRender::getInstance().frameBuffer->getChannel(), 
        SRender::getInstance().frameBuffer->getBuffer(), 
        0);
}

void SRenderWidget::SetLightColor(LIGHT_TYPE light_type, QColor color)
{
    switch (light_type)
    {
        case AMBITIENT: light.ambitient = glm::vec3(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f);
        case DIFFUSE  : light.diffuse   = glm::vec3(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f);
        case SPECULAR : light.specular  = glm::vec3(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f);
    }

}

void SRenderWidget::SetLightDir(glm::vec3& dir)
{
    light.dir = dir;
}

void SRenderWidget::SetMultiThread()
{
    multi_thread = !multi_thread;
}

void SRenderWidget::SetCameraPara(CAMERA_PARA para, float val)
{
    if (para == FOV)
    {
        camera->fov = val;
    }
    else if (para == ZNEAR)
    {
        camera->zNear = val;
    }
}

void SRenderWidget::paintEvent(QPaintEvent* event)
{
    set_canvas(SRender::getInstance().getBuffer(), 1920, 1080, QImage::Format_RGBA8888);
    if (canvas) {
        QPainter painter(this);
        QRectF target = rect();
        QRectF source(0, 0, 1920, 1080);
        painter.drawImage(target, *canvas, source);
    }
    QWidget::paintEvent(event);
}

void SRenderWidget::mousePressEvent(QMouseEvent* event)
{
    if (IN_RENDER_WIDGET)
    {
        CATCH_MOUSE_POS = true;
        pre_pos = event->pos();

        if (event->button() == Qt::LeftButton) CLICK_LEFT_BUTTON = true;
        if (event->button() == Qt::RightButton) CLICK_RIGHT_BUTTON = true;
    }
}

void SRenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    CATCH_MOUSE_POS = false;
    if (event->button() == Qt::LeftButton)
    {
        CLICK_LEFT_BUTTON = false;
    }
    else if (event->button() == Qt::RightButton)
    {
        CLICK_RIGHT_BUTTON = false;
    }
}

void SRenderWidget::mouseMoveEvent(QMouseEvent* event)
{
    // qt中 屏幕坐标左上角为原点  因此向下滑动 delta_y > 0, 需要加符号改变
    if (IN_RENDER_WIDGET && CATCH_MOUSE_POS)
    {   
        cur_pos = event->pos();
        float y_degree = static_cast<float>(pre_pos.x() - cur_pos.x());
        float x_degree = static_cast<float>(pre_pos.y() - cur_pos.y());
        if (CLICK_LEFT_BUTTON)
        {
            glm::mat4 tmp(1.0f);
            tmp = glm::rotate(tmp, glm::radians(x_degree * sensitity), glm::vec3(1, 0, 0));
            tmp = glm::rotate(tmp, glm::radians(y_degree * sensitity), glm::vec3(0, 1, 0));
            transform = glm::transpose(tmp) * transform;
        }
        else if (CLICK_RIGHT_BUTTON)
        {
            glm::vec3 trans(cur_pos.x() - pre_pos.x(), pre_pos.y() - cur_pos.y(), 0);
            trans *= speed;
            glm::mat4 tmp(1.0f);
            tmp = glm::translate(tmp, trans);
            transform = tmp * transform;
        }

        pre_pos = cur_pos;
    }

}

void SRenderWidget::wheelEvent(QWheelEvent* event)
{
    //TODO  变换矩阵问题  需单独设置一个变量存储用户交互所产生的结果
    if (event->delta() < 0)
    {
        scale *= 0.5;
    }
    else
    {
        scale *= 2;
    }
}

void SRenderWidget::enterEvent(QEvent*)
{
    IN_RENDER_WIDGET = true;
}

void SRenderWidget::leaveEvent(QEvent*)
{
    IN_RENDER_WIDGET = false;
}

void SRenderWidget::Render()
{
    if (model == nullptr) return;
    int nowTime = QTime::currentTime().msecsSinceStartOfDay();
    if (lastFrameTime != 0)
    {
        deltaTime = nowTime - lastFrameTime;
        ui.FPSLabel->setText(QString("FPS: ") + QString::number(1000 / deltaTime));
        lastFrameTime = nowTime;
    }

    SRender::getInstance().clearBuffer();

    // Calculate MVP Matrix
    //--------------------------------------------------------------
    matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));
    matrix = transform * matrix;

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 proj = camera->getProjection();
    //glm::mat4 proj = SRender::getInstance().perspective_ReversedZ(glm::radians(90.0f), 1.0f, 1.0f, 100.0f);
    //glm::mat4 proj = SRender::getInstance().ortho01(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);

    // Bind Data
    //-------------------------------------------------------------
    SRender::getInstance().shader->model = matrix;
    SRender::getInstance().shader->view = view;
    SRender::getInstance().shader->projection = proj;
    SRender::getInstance().shader->light = light;
    SRender::getInstance().shader->material.shininess = 150.0f;

    // Set Render Parameters
    //-------------------------------------------------------------
    SRender::getInstance().set_display_mode(FLAT);
    SRender::getInstance().set_multiThread(multi_thread);
    
    model->Draw();
    update();
    
}

void SRenderWidget::set_canvas(unsigned char* buffer, int w, int h, QImage::Format format, QImageCleanupFunction cleanupFunc, void* cleanupInfo)
{
    if (canvas) delete canvas;
    canvas = new QImage(buffer, w, h, format, cleanupFunc, cleanupInfo);
    *canvas = canvas->mirrored(false, true);
}
