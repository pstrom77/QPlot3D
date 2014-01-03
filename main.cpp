/**********************************************************************
 **
** Copyright (C) 2013 Peter Strömbäck <peter.stromback@yahoo.se>
**
** Contact: http://www.qt-project.org/legal			       
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**********************************************************************/

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

  QCurve3D* curve() const { return mCurve; }
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
  fatBox.curve()->setLineWidth(5);

  QPlot3D plot;
  plot.addCurve(box1.curve());
  plot.addCurve(box2.curve());
  plot.addCurve(box3.curve());
  plot.addCurve(fatBox.curve());

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

#if 1
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
  plot2.setXLabel("North");
  plot2.setYLabel("East");
  plot2.setZLabel("Down");

  // Window logistics...
  plot2.resize(300,300);
  plot2.move(350,10);
  plot2.setFocus();
  plot2.show();

  //////////////////////////////////////////////////////////////////////  
  // Example 3: Changeing the  looks
  //////////////////////////////////////////////////////////////////////  
  // Setup a widget with four subplots...
  QWidget aWidget;
  QPlot3D tPlot1;
  QPlot3D tPlot2;
  QPlot3D tPlot3;
  QPlot3D tPlot4;
  QGridLayout* tLayout = new QGridLayout;
  aWidget.setLayout(tLayout);
  tLayout->addWidget(&tPlot1,0,0);
  tLayout->addWidget(&tPlot2,0,1);
  tLayout->addWidget(&tPlot3,1,0);
  tLayout->addWidget(&tPlot4,1,1);
  
  tPlot1.addCurve(&bigSpiral);
  tPlot2.addCurve(&bigSpiral);
  tPlot3.addCurve(&bigSpiral);
  tPlot4.addCurve(&bigSpiral);

  // Change settings
  tPlot1.setBackgroundColor(Qt::black);
  tPlot1.xAxis().setLabelColor(Qt::white);
  tPlot1.yAxis().setLabelColor(Qt::white);
  tPlot1.zAxis().setLabelColor(Qt::white);
  tPlot1.xAxis().setPlaneColor(Qt::gray);
  tPlot1.yAxis().setPlaneColor(Qt::gray);
  tPlot1.zAxis().setPlaneColor(Qt::gray);
  tPlot1.setShowLegend(false);
  tPlot1.setShowAzimuthElevation(false);

  tPlot2.xAxis().setShowPlane(false);
  tPlot2.yAxis().setShowPlane(false);
  tPlot2.zAxis().setShowPlane(false);
  tPlot2.setBackgroundColor(Qt::gray);
  tPlot2.xAxis().setGridColor(Qt::yellow);
  tPlot2.yAxis().setGridColor(Qt::yellow);
  tPlot2.zAxis().setGridColor(Qt::yellow);

  tPlot3.setXLabel("Forward");
  tPlot3.setYLabel("Right");
  tPlot3.setZLabel("Down");
  tPlot3.setShowAxis(false);
  tPlot3.setAdjustPlaneView(false);
  tPlot3.setShowAxisBox(true);

  tPlot4.setShowGrid(false);
  tPlot4.setShowAxis(false);
  tPlot4.setAzimuth(90);
  tPlot4.setElevation(-90);
  


  // Window logistics...
  aWidget.resize(600,600);
  aWidget.move(700,10);
  aWidget.setFocus();
  aWidget.show();


  //////////////////////////////////////////////////////////////////////
  // Example 4: Create and Remove plots
  //////////////////////////////////////////////////////////////////////
  QPlot3D plot4;
  plot4.resize(300,300);
  plot4.move(350,410);
  plot4.raise();
  plot4.show();

  QCurve3D curve1("Curve 1");
  curve1.addData(0,0,0);
  curve1.addData(1,1,1);
  plot4.addCurve(&curve1);
  QThread::sleep(1);

  QCurve3D curve2("Curve 2");
  curve2.addData(0,0,0);
  curve2.addData(-1,-1,0);
  plot4.addCurve(&curve2);
  QThread::sleep(1);

  plot4.removeCurve(&curve1);


#endif
  return app.exec();
  
    
}

