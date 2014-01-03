/**********************************************************************
** Copyright (C) 2013 Peter Strömbäck <peter.stromback@yahoo.se>
**
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

#include "QPlot3D.h"
#include <limits>

static void Draw3DPlane(QVector3D topLeft, QVector3D bottomRight, QColor color) {
  QVector3D normal = QVector3D::crossProduct(topLeft,bottomRight);
  glNormal3f(normal.x(),normal.y(),normal.z());
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glBegin(GL_QUADS);
  glVertex3f(topLeft.x(),    topLeft.y(),    topLeft.z());
  glVertex3f(bottomRight.x(),topLeft.y(),    bottomRight.z());
  glVertex3f(bottomRight.x(),bottomRight.y(),bottomRight.z());
  glVertex3f(topLeft.x(),    bottomRight.y(),topLeft.z());
  glEnd();
}

static void Draw2DPlane(QVector2D topLeft, QVector2D bottomRight, QColor color) {
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glBegin(GL_QUADS);
  glVertex2f(topLeft.x(),    topLeft.y());
  glVertex2f(bottomRight.x(),topLeft.y());
  glVertex2f(bottomRight.x(),bottomRight.y());
  glVertex2f(topLeft.x(),    bottomRight.y());
  glEnd();
}


static void Draw2DLine(QVector2D from, QVector2D to, int lineWidth, QColor color) {
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glLineWidth(lineWidth);
  glBegin(GL_LINES);
  glVertex2f(from.x(),from.y());
  glVertex2f(to.x(),to.y());
  glEnd();
}

////////////////////////////////////////////////////////////////////////////////
// QRANGE
////////////////////////////////////////////////////////////////////////////////
QRange::QRange():
  min(-1.0,-1.0,-1.0), 
  max(1.0,1.0,1.0) 
{}

QRange::QRange(double _min, double _max):
  min(_min,_min,_min), 
  max(_max,_max,_max) 
{}

QVector3D QRange::center() const { 
  return 0.5*(max+min);
}

QVector3D QRange::delta() const {
  return max-min;
}

void QRange::setIfMin(QVector3D vec) {
  if(vec.x() < min.x()) min.setX(vec.x());
  if(vec.y() < min.y()) min.setY(vec.y());
  if(vec.z() < min.z()) min.setZ(vec.z());
}

void QRange::setIfMax(QVector3D vec) {
  if(vec.x() > max.x()) max.setX(vec.x());
  if(vec.y() > max.y()) max.setY(vec.y());
  if(vec.z() > max.z()) max.setZ(vec.z());
}

////////////////////////////////////////////////////////////////////////////////
// QCURVE3D
////////////////////////////////////////////////////////////////////////////////

QCurve3D::QCurve3D():
  mName(""),
  mColor(0,0,255),
  mLineWidth(1),
  mRange(std::numeric_limits<double>::max(),-std::numeric_limits<double>::max())
{
}

QCurve3D::QCurve3D(QString name): 
  mName(name),
  mColor(0,0,255),
  mLineWidth(1),
  mRange(std::numeric_limits<double>::max(),-std::numeric_limits<double>::max())
{
}

void QCurve3D::addData( const double& x, const double& y, const double& z) {
  addData(QVector3D(x,y,z));
}

void QCurve3D::addData( const QVector<double>& x, const QVector<double>& y, const QVector<double>& z) {
  int tSize = x.size();
  for (int i = 0; i < tSize; i++) {
    addData(x[i],y[i],z[i]);
  }
}

void QCurve3D::addData( const QVector<QVector3D>& data) {
  int tSize = data.size();
  for (int i = 0; i < tSize; i++) {
    addData(data[i]);
  }
}

void QCurve3D::addData(const QVector3D& data) {
  mRange.setIfMin(data);
  mRange.setIfMax(data);

  mVertices.push_back(data);
  mFaces.push_back(mVertices.size()-1);
}

QVector3D&  QCurve3D::operator[](int i) {
  return value(i);
}

const QVector3D&  QCurve3D::operator[](int i) const {
  return value(i);
}

void QCurve3D::draw() const {
  glVertexPointer(3,GL_FLOAT, 0, mVertices.constData());

  glLineWidth(mLineWidth);
  glColor3f(mColor.red()/255.0,mColor.green()/255.0,mColor.blue()/255.0);  
  glEnableClientState(GL_VERTEX_ARRAY);    
  glDrawElements(GL_LINE_STRIP,mFaces.count(),GL_UNSIGNED_SHORT, (GLushort*) mFaces.constData());
  glDisableClientState(GL_VERTEX_ARRAY);    
  glLineWidth(1);

}


////////////////////////////////////////////////////////////////////////////////
// QAXIS
////////////////////////////////////////////////////////////////////////////////
QAxis::QAxis():
  mRange(std::numeric_limits<double>::max(),-std::numeric_limits<double>::max()),
  mAxis(X_AXIS),
  mAdjustPlaneView(true),
  mShowPlane(true),
  mShowGrid(true),
  mShowAxis(true),
  mShowLabel(true),
  mShowAxisBox(false),
  mXLabel("X"),
  mYLabel("Y"),
  mPlaneColor(230,230,242),
  //  mGridColor(204,204,217),
  mGridColor(128,128,128),
  mLabelColor(0,0,0),
  mShowLowerTicks(false),
  mShowUpperTicks(false),
  mShowLeftTicks(false),
  mShowRightTicks(false),
  mTranslate(0.0),
  mScale(5.0),
  mLabelFont("Helvetica",12),
  mTicksFont("Helvetica",10)
  
{
}

void QAxis::setRange(QRange range) {

  mRange = range;

  if(mAxis == X_AXIS) 
    {
      mXTicks = getTicks(mRange.min.x(), mRange.max.x());
      mYTicks = getTicks(mRange.min.y(), mRange.max.y());
      mZTicks = getTicks(mRange.min.z(), mRange.max.z());
      mShowUpperTicks = true;
      mShowRightTicks = true;
    } 
  else if (mAxis == Y_AXIS)  
    {
      mXTicks = getTicks(mRange.min.y(), mRange.max.y());
      mYTicks = getTicks(mRange.min.z(), mRange.max.z());
      mZTicks = getTicks(mRange.min.x(), mRange.max.x());
    } 
  else 
    {
      mXTicks = getTicks(mRange.min.z(), mRange.max.z());
      mYTicks = getTicks(mRange.min.x(), mRange.max.x());
      mZTicks = getTicks(mRange.min.y(), mRange.max.y());
      mShowUpperTicks = true;
    }

  mTranslate = mZTicks[0];
}

QVector<double> QAxis::getTicks(double minValue, double maxValue)  const {
  QVector<double> tTicks;
  double step  = (maxValue-minValue)/mScale;
  double factor = pow(10.0, floor((log(step)/log(10.0))));

  double tmp = step/factor;
  if(tmp < mScale) {
    step = (int)(tmp*2)/2.0*factor;
  } else {
    step = (int)(tmp*0.5)/2.0*factor; 
  }
  int firstStep = floor(minValue/step);
  int lastStep  = ceil(maxValue/step);
  int tickCount = lastStep-firstStep+1;

  for (int i = 0; i < tickCount; i++) {
    tTicks.push_back((firstStep+i)*step);
  }
  return tTicks;
  
}

void QAxis::drawXTickLabel( QVector3D start, QVector3D stop, QString string ) const {
  
  QRect textSize = mPlot->textSize(string);

  const QVector3D tmpStart = mPlot->toScreenCoordinates(start.x(),start.y(),0.0);
  const QVector3D tmpStop  = mPlot->toScreenCoordinates(stop.x(),stop.y(),0.0);
  const QVector2D tStart(tmpStart.x(),tmpStart.y());
  const QVector2D tStop(tmpStop.x(),tmpStop.y());

  
  QVector2D r = tStop-tStart;
  QVector2D v = tStop;
  if(r.x() < 0) {
      v-QVector2D(textSize.width(),0.0);
    v += QVector2D(-textSize.width(),0.5*textSize.height());     
  } else 
    {
      v += QVector2D(0,0.5*textSize.height());     
    }

  glColor4f(mLabelColor.redF(),mLabelColor.greenF(),mLabelColor.blueF(),mLabelColor.alphaF());
  mPlot->renderTextAtScreenCoordinates(v.x(),v.y(),string,mTicksFont);  
}

void QAxis::drawAxisPlane() const {
  
  double minX = mXTicks[0];
  double maxX = mXTicks[mXTicks.size()-1];
  double minY = mYTicks[0];
  double maxY = mYTicks[mYTicks.size()-1];

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f,1.0f);

  // Plane
  if(mShowPlane) {
    Draw3DPlane(QVector3D(minX,minY,0), QVector3D(maxX, maxY,0), mPlaneColor);
  }

  double deltaX = mXTicks[1] - mXTicks[0];
  double deltaY = mYTicks[1] - mYTicks[0];

  // Grid
  for (int i = 0; i < mXTicks.size(); i++) {
    if(mShowGrid) {      
      mPlot->draw3DLine(QVector3D(mXTicks[i],minY,0), QVector3D(mXTicks[i],maxY,0), 2, mGridColor);
    }
    if(mShowAxis && mShowLowerTicks) {
      mPlot->draw3DLine(QVector3D(mXTicks[i],minY,0), QVector3D(mXTicks[i],minY-0.2*deltaY,0), 2, mLabelColor);
      drawXTickLabel(QVector3D(mXTicks[i],minY,0),    QVector3D(mXTicks[i],minY-0.5*deltaY,0), QString("%1").arg(mXTicks[i],3,'f',1));
    }
    if(mShowAxis && mShowUpperTicks) {
      mPlot->draw3DLine(QVector3D(mXTicks[i],maxY,0), QVector3D(mXTicks[i],maxY+0.2*deltaY,0), 2, mLabelColor);
      drawXTickLabel(QVector3D(mXTicks[i],maxY,0),    QVector3D(mXTicks[i],maxY+0.5*deltaY,0), QString("%1").arg(mXTicks[i],3,'f',1));
    }
  }

  for (int i = 1;i < mYTicks.size(); i++) {
    if(mShowGrid) {      
      mPlot->draw3DLine(QVector3D(minX,mYTicks[i],0), QVector3D(maxX,mYTicks[i] ,0), 2, mGridColor);
    }
    if(mShowAxis && mShowLeftTicks)  {
      mPlot->draw3DLine(QVector3D(minX,mYTicks[i],0), QVector3D(minX-0.2*deltaX,mYTicks[i] ,0), 2, mLabelColor);
      drawXTickLabel(QVector3D(minX,mYTicks[i],0), QVector3D(minX-0.5*deltaX,mYTicks[i] ,0), QString("%1").arg(mYTicks[i],3,'f',1));
    }            
    if(mShowAxis && mShowRightTicks) {
      mPlot->draw3DLine(QVector3D(maxX,mYTicks[i],0), QVector3D(maxX+0.2*deltaX,mYTicks[i] ,0), 2, mLabelColor);
      drawXTickLabel(QVector3D(maxX, mYTicks[i],0.0), QVector3D(maxX+0.5*deltaX,mYTicks[i], 0),QString("%1").arg(mYTicks[i],3,'f',1));
    }
  }	 

  if(mShowAxis &&  mShowLowerTicks) {
    mPlot->draw3DLine(QVector3D(minX,minY,0), QVector3D(maxX+0.5*deltaX,minY ,0), 3, mLabelColor);
  }
  if(mShowLabel && mShowLowerTicks) {
    mPlot->renderTextAtWorldCoordinates(QVector3D(0.5*(maxX+minX),minY-1.5*deltaY,0),mXLabel,mLabelFont);
  }
  if(mShowAxis && mShowUpperTicks) {
    mPlot->draw3DLine(QVector3D(minX,maxY,0), QVector3D(maxX+0.5*deltaX,maxY ,0), 3, mLabelColor);
  }
  if(mShowLabel && mShowUpperTicks) {
    mPlot->renderTextAtWorldCoordinates(QVector3D(0.5*(maxX+minX),maxY+1.5*deltaY,0),mXLabel,mLabelFont);
  }
  if(mShowAxis && mShowLeftTicks)  {
    mPlot->draw3DLine(QVector3D(minX,minY,0), QVector3D(minX, maxY+0.5*deltaY ,0), 3, mLabelColor);
  }
  if(mShowLabel && mShowLeftTicks) {    
    mPlot->renderTextAtWorldCoordinates(QVector3D(minX-1.5*deltaX,0.5*(maxY+minY),0),mYLabel,mLabelFont);   
  }
  if(mShowAxis && mShowRightTicks) {
    mPlot->draw3DLine(QVector3D(maxX,minY,0), QVector3D(maxX, maxY+0.5*deltaY ,0), 3, mLabelColor);
  }
  if(mShowLabel && mShowRightTicks) {
    mPlot->renderTextAtWorldCoordinates(QVector3D(maxX+1.5*deltaX,0.5*(maxY+minY),0),mYLabel,mLabelFont);   
  }


  glDisable(GL_POLYGON_OFFSET_FILL);

}

void QAxis::draw() const {
  if(mXTicks.isEmpty()) return;
  if(mYTicks.isEmpty()) return;
  if(mZTicks.isEmpty()) return;

  glPushMatrix();
  
  if(mAxis == X_AXIS) 
    {
    }
  else if (mAxis == Y_AXIS) 
    {
      glRotatef(90, 1,0,0);
      glRotatef(90, 0,1,0);    
    }
  else {
    glRotatef(90,  1,0,0);
    glRotatef(180, 0,1,0);        
    glRotatef(90,  0,0,1);
  }
  
  glTranslatef(0,0,mTranslate);
  drawAxisPlane();

  glPopMatrix();
} 
void QAxis::drawAxisBox() const {
  if(mXTicks.isEmpty()) return;
  if(mYTicks.isEmpty()) return;
  if(mZTicks.isEmpty()) return;

  glPushMatrix();


  if(mShowAxisBox) {
  if(mAxis == X_AXIS) 
    {
    }
  else if (mAxis == Y_AXIS) 
    {
      glRotatef(90, 1,0,0);
      glRotatef(90, 0,1,0);    
    }
  else {
    glRotatef(90,  1,0,0);
    glRotatef(180, 0,1,0);        
    glRotatef(90,  0,0,1);
  }

    // glTranslatef(0,0,-mTranslate);
    mPlot->draw3DLine(QVector3D(mXTicks[0],mYTicks[0],mZTicks[0]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[0]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[0]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[0]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[0]), QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[0]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[0]), QVector3D(mXTicks[0],mYTicks[0],mZTicks[0]),2,mLabelColor);
    
    mPlot->draw3DLine(QVector3D(mXTicks[0],mYTicks[0],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[mZTicks.size()-1]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]),2,mLabelColor);
    mPlot->draw3DLine(QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[0],mYTicks[0],mZTicks[mZTicks.size()-1]),2,mLabelColor);
  }
  glPopMatrix();  
}

void QAxis::setVisibleTicks(bool lower, bool right, bool upper, bool left ) {
  mShowLeftTicks  = left;
  mShowRightTicks = right;
  mShowLowerTicks = lower;	
  mShowUpperTicks = upper;
}

void QAxis::adjustPlaneView() {
  if(!mAdjustPlaneView) return;

  bool tFlipX = false;
  bool tFlipY = false;
  bool tFlipZ = false;

  // Translate the planes so that they are always in the background
  const QVector3D camPos = mPlot->cameraPositionInWorldCoordinates();
  if( ( camPos - 0.5*(QVector3D(mRange.max.x(), mRange.max.y(), mRange.max.z())+QVector3D(mRange.min.x(), mRange.min.y(), mRange.max.z())) ).length() >
      ( camPos - 0.5*(QVector3D(mRange.max.x(), mRange.max.y(), mRange.min.z())+QVector3D(mRange.min.x(), mRange.min.y(), mRange.min.z())) ).length() )   
    tFlipX = true;
  
  if( ( camPos - 0.5*(QVector3D(mRange.max.x(), mRange.max.y(), mRange.max.z())+QVector3D(mRange.max.x(), mRange.min.y(), mRange.min.z())) ).length() >
      ( camPos - 0.5*(QVector3D(mRange.min.x(), mRange.max.y(), mRange.max.z())+QVector3D(mRange.min.x(), mRange.min.y(), mRange.min.z())) ).length() )   
    tFlipY = true;
  
  if( ( camPos - 0.5*(QVector3D(mRange.max.x(), mRange.max.y(), mRange.max.z())+QVector3D(mRange.min.x(), mRange.max.y(), mRange.min.z())) ).length() >
      ( camPos - 0.5*(QVector3D(mRange.max.x(), mRange.min.y(), mRange.max.z())+QVector3D(mRange.min.x(), mRange.min.y(), mRange.min.z())) ).length() )
    tFlipZ = true;

  // Default translation of the plane
  mTranslate = mZTicks[0];

  // If it was THIS plane that flipped move it to the back.
  if ( (mAxis == X_AXIS && tFlipX) || (mAxis == Y_AXIS && tFlipY) || (mAxis == Z_AXIS && tFlipZ))
    {
      mTranslate = mZTicks[mZTicks.size()-1];
    }

  // Default ticks
  
  if(0.0 <= mPlot->elevation() && 90.0 > mPlot->elevation() ) {
    
    if(0.0 <= mPlot->azimuth() && 90.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(true,true,false,false);
	if( mAxis == Y_AXIS) setVisibleTicks(false,false, false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(false,false,true,false);
      }
    
    if(90.0 <= mPlot->azimuth() && 180.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,true,true,false);
	if( mAxis == Y_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(false,false,true,false);
      }
    
    if(180.0 <= mPlot->azimuth() && 270.0 > mPlot->azimuth() )
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,false,true,true);
	if( mAxis == Y_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(true,false,false,false);
      }
      
    if(270.0 <= mPlot->azimuth() && 360.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(true,false,false,true);
	if( mAxis == Y_AXIS) setVisibleTicks(false,false,false,true);
	if( mAxis == Z_AXIS) setVisibleTicks(false,false,false,false);
      }

  } else {
    if(0.0 <= mPlot->azimuth() && 90.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Y_AXIS) setVisibleTicks(true,false, false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(false,false,true,true);
      }
    
    if(90.0 <= mPlot->azimuth() && 180.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Y_AXIS) setVisibleTicks(true,false,false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(false,false,true,true);
      }
    
    if(180.0 <= mPlot->azimuth() && 270.0 > mPlot->azimuth() )
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Y_AXIS) setVisibleTicks(true,false,false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(true,false,false,true);
      }
      
    if(270.0 <= mPlot->azimuth() && 360.0 > mPlot->azimuth() ) 
      {
	if( mAxis == X_AXIS) setVisibleTicks(false,false,false,false);
	if( mAxis == Y_AXIS) setVisibleTicks(true,false,false,false);
	if( mAxis == Z_AXIS) setVisibleTicks(true,false,false,true);
      }
    

  }


}

////////////////////////////////////////////////////////////////////////////////
// QPLOT3D
////////////////////////////////////////////////////////////////////////////////
QPlot3D::QPlot3D(QWidget* parent): 
  QGLWidget(QGLFormat(QGL::SampleBuffers),parent),
  mBackgroundColor(Qt::white),
  mTranslate(0,0,-20),
  mRotation(0,0,0),  
  mShowAzimuthElevation(true),
  mShowLegend(true),
  mAxisEqual(false),
  mLegendFont("Helvetica", 12)
{


  setAzimuth(130);
  setElevation(30);

  mXAxis.setAxis(QAxis::X_AXIS);
  mXAxis.setPlot(this);
  setXLabel("X");

  mYAxis.setAxis(QAxis::Y_AXIS);
  mYAxis.setPlot(this);
  setYLabel("Y");

  mZAxis.setAxis(QAxis::Z_AXIS);
  mZAxis.setPlot(this);
  setZLabel("Z");

  setFont(QFont("Helvetica"));


  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
  

}

QPlot3D::~QPlot3D() {
}

void QPlot3D::showContextMenu(const QPoint& pos) {
  QPoint globalPos = this->mapToGlobal(pos);
  
  QMenu tMenu;

  QAction*  a1 = new QAction("Toggle Plane",this);
  connect(a1, SIGNAL(triggered()), &mXAxis,SLOT(togglePlane()));
  connect(a1, SIGNAL(triggered()), &mYAxis,SLOT(togglePlane()));
  connect(a1, SIGNAL(triggered()), &mZAxis,SLOT(togglePlane()));
  tMenu.addAction(a1);

  QAction*  a2 = new QAction("Toggle Grid",this);
  connect(a2, SIGNAL(triggered()), &mXAxis,SLOT(toggleGrid()));
  connect(a2, SIGNAL(triggered()), &mYAxis,SLOT(toggleGrid()));
  connect(a2, SIGNAL(triggered()), &mZAxis,SLOT(toggleGrid()));
  tMenu.addAction(a2);

  QAction*  a3 = new QAction("Toggle Axis",this);
  connect(a3, SIGNAL(triggered()), &mXAxis,SLOT(toggleAxis()));
  connect(a3, SIGNAL(triggered()), &mYAxis,SLOT(toggleAxis()));
  connect(a3, SIGNAL(triggered()), &mZAxis,SLOT(toggleAxis()));
  tMenu.addAction(a3);

  QAction*  a4 = new QAction("Toggle Label",this);
  connect(a4, SIGNAL(triggered()), &mXAxis,SLOT(toggleLabel()));
  connect(a4, SIGNAL(triggered()), &mYAxis,SLOT(toggleLabel()));
  connect(a4, SIGNAL(triggered()), &mZAxis,SLOT(toggleLabel()));
  tMenu.addAction(a4);

  QAction*  a5 = new QAction("Toggle Axis Box",this);
  connect(a5, SIGNAL(triggered()), &mXAxis,SLOT(toggleAxisBox()));
  connect(a5, SIGNAL(triggered()), &mYAxis,SLOT(toggleAxisBox()));
  connect(a5, SIGNAL(triggered()), &mZAxis,SLOT(toggleAxisBox()));
  tMenu.addAction(a5);

  QAction*  a6 = new QAction("Toggle Adjust Plane View",this);
  connect(a6, SIGNAL(triggered()), &mXAxis,SLOT(toggleAdjustView()));
  connect(a6, SIGNAL(triggered()), &mYAxis,SLOT(toggleAdjustView()));
  connect(a6, SIGNAL(triggered()), &mZAxis,SLOT(toggleAdjustView()));
  tMenu.addAction(a6);

  QAction*  a7 = new QAction("Toggle Axis Equal",this);
  connect(a7, SIGNAL(triggered()), this,SLOT(toggleAxisEqual()));
  tMenu.addAction(a7);

  tMenu.exec(globalPos);
  update();
}

void QPlot3D::initializeGL() {
  qglClearColor(mBackgroundColor);
    
  glShadeModel(GL_SMOOTH);

  glEnable(GL_MULTISAMPLE);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
}

void QPlot3D::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glTranslatef(mTranslate.x(),mTranslate.y(),mTranslate.z());

  glRotatef(mRotation.x()-90, 1.0, 0.0, 0.0);
  glRotatef(mRotation.y(), 0.0, 1.0, 0.0);
  glRotatef(mRotation.z(), 0.0, 0.0, 1.0);

  glScalef(mScale.x(),mScale.y(),mScale.z());
  
  QVector3D tCenter = mXAxis.range().center();
  glTranslatef(-tCenter.x(),-tCenter.y(),-tCenter.z());

  // DRAW AXIS
  mXAxis.draw();
  mYAxis.draw();
  mZAxis.draw();

  // DRAW CURVES
  const int nCurves = mCurves.size();
  for(int i = 0; i < nCurves; i++) {
    mCurves[i]->draw();
  }

  // DRAW AXIS BOX
  mXAxis.drawAxisBox();
  mYAxis.drawAxisBox();
  mZAxis.drawAxisBox();

  // DRAW LEGEND
  if(mShowLegend) {
    drawLegend();
  }

  // DRAW ELEVATION AZIMUTH TEXT BOX
  if(mShowAzimuthElevation) {  
    drawTextBox(10,height()-15,QString("Az: %1 El: %2").arg(azimuth(),3,'f',1).arg(elevation(),3,'f',1));
  }

  
}

void QPlot3D::drawLegend(){
  double textWidth  = 0;
  double textHeight = 0;
  int nrCurves = mCurves.size();
  for (int i = 0; i < nrCurves; i++) {
    QString string = mCurves[i]->name();
    if(fontMetrics().width(string) > textWidth) textWidth = fontMetrics().width(string);
    if(fontMetrics().height() > textHeight) textHeight = fontMetrics().height();
  }


  double tWidth  = 5 + 20 + 5 + textWidth + 5;
  double tHeight = 5 + nrCurves*textHeight + 5;
  double x0 = width()-tWidth-5; 
  double y0 = 5;

  enable2D();
  glEnable(GL_BLEND);
  // Draw box
  Draw2DPlane(QVector2D(x0,y0),             QVector2D(x0+tWidth,y0+tHeight),QColor(204,204,217,128));
  // Draw contour lines
  Draw2DLine(QVector2D(x0,y0),                QVector2D(x0+tWidth,y0),         1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0+tWidth,y0),         QVector2D(x0+tWidth,y0+tHeight), 1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0+tWidth,y0+tHeight), QVector2D(x0,y0+tHeight),        1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0,y0+tHeight),        QVector2D(x0,y0),                1, QColor(0,0,0,255));  
  glDisable(GL_BLEND);
  disable2D();

  y0 = 10;

  for (int i = 0; i < nrCurves; i++) {

    x0 = width()-tWidth-5; 

    enable2D();
    Draw2DLine(QVector2D(x0+5,   y0+0.5*textHeight), 
	       QVector2D(x0+5+20,y0+0.5*textHeight), 
	       mCurves[i]->lineWidth(), 
	       mCurves[i]->color());
    disable2D();

    x0 += 30;
    glColor4f(0,0,0,1);
    y0 += textHeight;
    renderTextAtScreenCoordinates((int)x0,(int)y0,mCurves[i]->name(),mLegendFont);
  }


}

void QPlot3D::drawTextBox(int x, int y, QString string, QFont font)  {
  const double textWidth  = fontMetrics().width(string);
  const double textHeight = fontMetrics().height();

  glEnable(GL_BLEND);
  enable2D();
  Draw2DPlane(QVector2D(x-5,y+5), QVector2D(x+10+textWidth, y-textHeight-5), QColor(204,204,217,128));
  disable2D();
  glDisable(GL_BLEND);

  glColor4f(0,0,0,1);
  renderTextAtScreenCoordinates(x,y,string,font);
}  


void QPlot3D::enable2D() {
  glPushMatrix();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,width(),height(),0,0.01,-10000.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
}

void QPlot3D::disable2D() {
  glPopMatrix();
  resizeGL(width(),height());
}

void QPlot3D::resizeGL(int width, int height) {
  glViewport(0,0,width,height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double zNear = 0.01;
  double zFar  = 10000.0;
  double aspect = (double)width/(double)height;
  double fW = tan( 25*3.141592/180.0)*zNear;
  double fH = fW/aspect;
  glFrustum(-fW,fW,-fH,fH,zNear,zFar);


  glMatrixMode(GL_MODELVIEW);
}

void QPlot3D::mousePressEvent(QMouseEvent *event)
{
    mLastMousePos = event->pos();


}

void QPlot3D::mouseMoveEvent(QMouseEvent *event)
{
  int dx = event->x() - mLastMousePos.x();
  int dy = event->y() - mLastMousePos.y();
  
  if (event->buttons() & Qt::LeftButton) {
    
    if(event->modifiers() == Qt::ControlModifier) {
      setPitch(pitch() + dx);     
    }else {
      if( (roll() + dy < 90) && (roll()+dy > -90)) 
	setRoll(roll() + dy);
      setYaw(yaw() + dx);
    }
  } else {
    setPan(pan() + QVector3D(dx/32.0,-dy/32.0,0.0));
  }
 
  mLastMousePos = event->pos();

  mXAxis.adjustPlaneView();
  mYAxis.adjustPlaneView();
  mZAxis.adjustPlaneView();

}

void QPlot3D::mouseDoubleClickEvent(QMouseEvent* event) {
  event->accept();
  mAxisEqual = !mAxisEqual;
  rescaleAxis();
}

void QPlot3D::wheelEvent(QWheelEvent* event) 
{
  event->accept();  
  setZoom( zoom() + (double)event->delta()/32);

  mXAxis.adjustPlaneView();
  mYAxis.adjustPlaneView();
  mZAxis.adjustPlaneView();

}

void QPlot3D::addCurve(QCurve3D* curve) { 
  mCurves.push_back(curve);  
  rescaleAxis();
  updateGL();
} 

void QPlot3D::setBackgroundColor(QColor color) { 
  mBackgroundColor = color;   
  makeCurrent(); 
  qglClearColor(mBackgroundColor); 
}

void QPlot3D::rescaleAxis() {
  QRange tRange = mXAxis.range();
  int tSize = mCurves.size();
  for(int i = 0; i < tSize; i++) {
    tRange.setIfMin(mCurves[i]->range());
    tRange.setIfMax(mCurves[i]->range());
  }
  mXAxis.setRange(tRange);
  mYAxis.setRange(tRange);
  mZAxis.setRange(tRange);
  
  if (mAxisEqual) 
    axisEqual();
  else 
    axisTight();

  mXAxis.adjustPlaneView();
  mYAxis.adjustPlaneView();
  mZAxis.adjustPlaneView();
}

void QPlot3D::axisEqual() {
  QVector3D delta = mXAxis.range().delta();
  double k = std::max(delta.x(),std::max(delta.y(),delta.z()));
  mScale.setX( 10.0/k);
  mScale.setY( 10.0/k);
  mScale.setZ( 10.0/k);
  updateGL();
}

void QPlot3D::axisTight() {
  QVector3D delta = mXAxis.range().delta();
  mScale.setX( 10.0/delta.x());
  mScale.setY( 10.0/delta.y());
  mScale.setZ( 10.0/delta.z());
  updateGL();
}

QVector3D QPlot3D::cameraPositionInWorldCoordinates() const {

  QVector3D tObjectCenter = mXAxis.range().center();
  
  QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, (mRotation.x()-90));
  QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, mRotation.y());
  QQuaternion q3 = QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, mRotation.z());
  QQuaternion q  = (q1*q2*q3).conjugate();
  
  return tObjectCenter + q.rotatedVector(-QVector3D(mTranslate.x()/mScale.x(),mTranslate.y()/mScale.y(),mTranslate.z()/mScale.z()));
}

void QPlot3D::renderTextAtWorldCoordinates(const QVector3D& vec, QString str, QFont font) {
  QVector3D sVec = toScreenCoordinates(vec);
  renderTextAtScreenCoordinates(sVec.x(),sVec.y(),str,font);  
}

void QPlot3D::renderTextAtScreenCoordinates(int x, int y, QString str, QFont font) {
  setFont(font);
  renderText(x,y,str);  
}
QVector3D QPlot3D::toScreenCoordinates(double worldX, double worldY, double worldZ) const {
  return toScreenCoordinates(QVector3D(worldX,worldY,worldZ));
}

QVector3D QPlot3D::toScreenCoordinates(const QVector3D& vec) const {
  // v' = P*M*v
  GLdouble m[16];
  GLdouble p[16];

  glGetDoublev(GL_MODELVIEW_MATRIX,&m[0]);
  glGetDoublev(GL_PROJECTION_MATRIX,&p[0]);

  GLint viewPort[4];
  viewPort[0]  = 0;
  viewPort[1]  = 0;
  viewPort[2]  = width();
  viewPort[3]  = height();

  // M*v
  const double f1 = m[0]*vec.x() + m[4]*vec.y() + m[8]*vec.z() + m[12];
  const double f2 = m[1]*vec.x() + m[5]*vec.y() + m[9]*vec.z() + m[13];
  const double f3 = m[2]*vec.x() + m[6]*vec.y() + m[10]*vec.z() + m[14];
  const double f4 = m[3]*vec.x() + m[7]*vec.y() + m[11]*vec.z() + m[15];

  // P*M*v
  double g1 = p[0]*f1 + p[4]*f2 + p[8]*f3  + p[12]*f4;
  double g2 = p[1]*f1 + p[5]*f2 + p[9]*f3  + p[13]*f4;
  double g3 = p[2]*f1 + p[6]*f2 + p[10]*f3 + p[14]*f4;
  double g4 = -f3;

  if(g4 == 0.0) return QVector3D(0.0,0.0,0.0);
  g1 /= g4;
  g2 /= g4;
  g3 /= g4;
       
  return QVector3D( (g1*0.5+0.5)*viewPort[2] + viewPort[0], 
  		    height()- ((g2*0.5+0.5)*viewPort[3] + viewPort[1]),
  		    (1.0+g3)*0.5);
  
}

void QPlot3D::setShowAxis(bool value) {
  mXAxis.setShowAxis(value);
  mXAxis.setShowLabel(value);

  mYAxis.setShowAxis(value);
  mYAxis.setShowLabel(value);

  mZAxis.setShowAxis(value);
  mZAxis.setShowLabel(value);
}

void QPlot3D::setShowAxisBox(bool value) {
  mXAxis.setShowAxisBox(value);
  mYAxis.setShowAxisBox(value);
  mZAxis.setShowAxisBox(value);
}

void QPlot3D::setShowGrid(bool value) {
  mXAxis.setShowPlane(value);
  mXAxis.setShowGrid(value);

  mYAxis.setShowPlane(value);
  mYAxis.setShowGrid(value);

  mZAxis.setShowPlane(value);
  mZAxis.setShowGrid(value);
}


void QPlot3D::draw3DLine(QVector3D from, QVector3D to, double lineWidth, QColor color) {

  const QVector3D tFrom = toScreenCoordinates(from);
  const QVector3D tTo   = toScreenCoordinates(to);

  const QVector3D v = tTo-tFrom;
  const QVector3D n1 = QVector3D::crossProduct(tTo,tFrom);
  const QVector3D n = QVector3D::crossProduct(v,n1).normalized();

  enable2D();  // Actually, set glOrtho...
  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  const QVector3D d = 0.5*lineWidth*n;
  const QVector3D v1 = tFrom - d;
  const QVector3D v2 = tTo   - d;
  const QVector3D v3 = tFrom;
  const QVector3D v4 = tTo;  
  const QVector3D v5 = tFrom + d;
  const QVector3D v6 = tTo   + d;

  float vertexVec[] = { 
    v1.x(),v1.y(),v1.z(),
    v2.x(),v2.y(),v2.z(),
    v3.x(),v3.y(),v3.z(),
    v4.x(),v4.y(),v4.z(),
    v5.x(),v5.y(),v5.z(),
    v6.x(),v6.y(),v6.z()
  };

  float colorVec[] = { 
    color.redF(), color.greenF(), color.blueF(), 0.0,
    color.redF(), color.greenF(), color.blueF(), 0.0,
    color.redF(), color.greenF(), color.blueF(), 1.0,
    color.redF(), color.greenF(), color.blueF(), 1.0,
    color.redF(), color.greenF(), color.blueF(), 0.0,
    color.redF(), color.greenF(), color.blueF(), 0.0,
  };

  glVertexPointer(3,GL_FLOAT, 0, vertexVec);
  glColorPointer(4,GL_FLOAT, 0, colorVec);
  glEnableClientState(GL_VERTEX_ARRAY);    
  glEnableClientState(GL_COLOR_ARRAY);    
  glDrawArrays(GL_TRIANGLE_STRIP,0,6);
  glDisableClientState(GL_COLOR_ARRAY);    
  glDisableClientState(GL_VERTEX_ARRAY);    
  

  glDisable(GL_BLEND);
  disable2D();

}

QRect QPlot3D::textSize(QString string) const {
  return QRect(0.0, 0.0, fontMetrics().width(string), fontMetrics().height());
}

bool QPlot3D::removeCurve(QCurve3D* curve) {
  return mCurves.removeOne(curve);
}
