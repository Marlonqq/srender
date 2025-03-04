#pragma once
#include <qmetatype.h>
#include "Model.h"
#include "SRender.h"
#include "BlinnPhongShader.h"
#include "Camera.h"
#include <QWidget>
#include "ui_SRenderWidget.h"
#include <qpainter.h>
#include <qtimer.h>
#include <QtCore/qdatetime.h>
#include <qdebug.h>
#include <qimage.h>
#include <Qt3DInput/qmouseevent.h>
#include <qmouseeventtransition.h>
#include <qmessagebox.h>
class SRenderWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SRenderWidget(QWidget *parent = nullptr);
	~SRenderWidget();
	void LoadModel(QString path);
	void SaveImage(QString path);
	void SetLightColor(LIGHT_TYPE light_type, QColor color);
	void SetLightDir(glm::vec3& dir);
	void SetMultiThread();
	void SetCameraPara(CAMERA_PARA para, float val);

protected:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
	void enterEvent(QEvent*) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent*) Q_DECL_OVERRIDE;

signals:
	void SendModelData(int FaceCount, int VertCount);

public slots:
	void Render();

private:
	//----------------------------------------
	//            ”√ªßΩªª•
	//----------------------------------------
	QPoint pre_pos, cur_pos;
	bool CATCH_MOUSE_POS = false;
	bool IN_RENDER_WIDGET = false;
	bool CLICK_LEFT_BUTTON = false;
	bool CLICK_RIGHT_BUTTON = false;

private:
	Ui::SRenderWidgetClass ui;
	QImage* canvas = nullptr;
	QTimer timer;
	Camera* camera = nullptr;
	Model* model = nullptr;
	void set_canvas(unsigned char*, int, int, QImage::Format, QImageCleanupFunction cleanupFunc = Q_NULLPTR, void* cleanupInfo = Q_NULLPTR);

};



