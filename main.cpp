#include <QtCore>
#include "QPlot3D.h"

class Box {
public:
  Box(QString name, QVector3D center, QColor color) {    
    mCurve = new QCurve3D(name);
    mCurve->setColor(color);

    // Front
    mCurve->addData(center + QVector3D(-1.0,-1.0,1.0));
    mCurve->addData(center + QVector3D( 1.0,-1.0,1.0));
    mCurve->addData(center + QVector3D( 1.0, 1.0,1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0,1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0,1.0));

    // Left
    mCurve->addData(center + QVector3D(-1.0,-1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0, 1.0));

    // Back
    mCurve->addData(center + QVector3D(-1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0,-1.0,-1.0));

    // Bottom 
    mCurve->addData(center + QVector3D( 1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0,-1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0,-1.0,-1.0));

    // Right
    mCurve->addData(center + QVector3D(1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D(1.0,-1.0, 1.0));
    mCurve->addData(center + QVector3D(1.0,-1.0,-1.0));
    mCurve->addData(center + QVector3D(1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D(1.0, 1.0, 1.0));
    mCurve->addData(center + QVector3D(1.0,-1.0, 1.0));

    // Top
    mCurve->addData(center + QVector3D( 1.0, 1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0, 1.0));
    mCurve->addData(center + QVector3D(-1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0, 1.0,-1.0));
    mCurve->addData(center + QVector3D( 1.0, 1.0, 1.0));
  }

  ~Box() { delete mCurve; }

  QCurve3D* getCurve() const { return mCurve; }
private:
  QCurve3D* mCurve;
};


int main(int argc, char* argv[]) {
  QApplication app(argc,argv);

  //////////////////////////////////////////////////////////////////////  
  // Example 1: A lot of boxes...
  //////////////////////////////////////////////////////////////////////  
  Box box1("Box 1", QVector3D(5,0,0), Qt::red);
  Box box2("Box 2", QVector3D(-5,0,0), Qt::green);
  Box box3("Box 3", QVector3D(0,0,5), Qt::blue);
  Box fatBox("Fat Box", QVector3D(0,5,-5), Qt::black);
  fatBox.getCurve()->setLineWidth(5);

  QPlot3D plot;
  plot.addCurve(box1.getCurve());
  plot.addCurve(box2.getCurve());
  plot.addCurve(box3.getCurve());
  plot.addCurve(fatBox.getCurve());

  QCurve3D spiral("Spiral");
  // Add data to spiral curve
  for (double z= -5.0; z < 5.0;  z+=0.01 ) {
    spiral.addData(0.5*z*cos(z*3.141592),0.5*z*sin(z*3.1415192),z);
  }

  // Set color and linewidth
  spiral.setColor(Qt::blue);
  spiral.setLineWidth(2);
  // Add spiral curve to the plot 
  plot.addCurve(&spiral);
  // Set axis equal
  plot.setAxisEqual(true);

  // Resize and move window
  plot.resize(300,300);
  plot.move(10,10);
  plot.setFocus();
  plot.show();

  //////////////////////////////////////////////////////////////////////  
  // Example 2: A Spiral way of from origin...
  //////////////////////////////////////////////////////////////////////  
  QPlot3D plot2;
  QCurve3D bigSpiral("Big Spiral");
  for (double theta= 0.0; theta < 10.0;  theta+=0.01 ) {
    double z = theta*100;
    bigSpiral.addData(-10000+0.5*z*cos(theta*3.141592),-10000+0.5*z*sin(theta*3.1415192),-100+z);
  }
  bigSpiral.setColor(Qt::red);
  bigSpiral.setLineWidth(2);
  plot2.addCurve(&bigSpiral);

  // Change labels on axes from the default x,y,z
  plot2.getXAxis().setLabel("North");
  plot2.getYAxis().setLabel("East");
  plot2.getZAxis().setLabel("Down");

  // Window logistics...
  plot2.resize(300,300);
  plot2.move(350,10);
  plot2.setFocus();
  plot2.show();

  //////////////////////////////////////////////////////////////////////  
  // Example 3: Changeing the  looks
  //////////////////////////////////////////////////////////////////////  
  // Setup a widget with four subplots...
  QWidget* aWidget = new QWidget();
  QPlot3D tPlot1;
  QPlot3D tPlot2;
  QPlot3D tPlot3;
  QPlot3D tPlot4;
  QGridLayout* tLayout = new QGridLayout;
  aWidget->setLayout(tLayout);
  tLayout->addWidget(&tPlot1,0,0);
  tLayout->addWidget(&tPlot2,0,1);
  tLayout->addWidget(&tPlot3,1,0);
  tLayout->addWidget(&tPlot4,1,1);
  
  tPlot1.addCurve(&bigSpiral);
  tPlot2.addCurve(&bigSpiral);
  tPlot3.addCurve(&bigSpiral);
  tPlot4.addCurve(&bigSpiral);

  // Change settings
  tPlot1.setBackground(Qt::black);
  tPlot1.getXAxis().setLabelColor(Qt::white);
  tPlot1.getYAxis().setLabelColor(Qt::white);
  tPlot1.getZAxis().setLabelColor(Qt::white);
  tPlot1.getXAxis().setPlaneColor(Qt::gray);
  tPlot1.getYAxis().setPlaneColor(Qt::gray);
  tPlot1.getZAxis().setPlaneColor(Qt::gray);
  tPlot1.setShowLegend(false);
  tPlot1.setShowAzimuthElevation(false);

  tPlot2.getXAxis().setShowPlane(false);
  tPlot2.getYAxis().setShowPlane(false);
  tPlot2.getZAxis().setShowPlane(false);
  tPlot2.setBackground(Qt::gray);
  tPlot2.getXAxis().setGridColor(Qt::yellow);
  tPlot2.getYAxis().setGridColor(Qt::yellow);
  tPlot2.getZAxis().setGridColor(Qt::yellow);

  tPlot3.getXAxis().setLabel("Forward");
  tPlot3.getYAxis().setLabel("Right");
  tPlot3.getZAxis().setLabel("Down");
  tPlot3.hideAxes();

  tPlot4.getXAxis().setShowPlane(false);
  tPlot4.getYAxis().setShowPlane(false);
  tPlot4.getZAxis().setShowPlane(false);
  tPlot4.getXAxis().setShowGrid(false);
  tPlot4.getYAxis().setShowGrid(false);
  tPlot4.getZAxis().setShowGrid(false);
  tPlot4.getXAxis().setAxisColor(Qt::red);
  tPlot4.getYAxis().setAxisColor(Qt::green);
  tPlot4.getZAxis().setAxisColor(Qt::blue);
  tPlot4.setAzimuth(90);
  tPlot4.setElevation(-90);
  


  // Window logistics...
  aWidget->resize(600,600);
  aWidget->move(700,10);
  aWidget->setFocus();
  aWidget->show();

  return app.exec();
  
    
}

