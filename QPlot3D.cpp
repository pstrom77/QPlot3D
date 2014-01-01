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

static void Draw3DLine(QVector3D from, QVector3D to, int lineWidth, QColor color) {
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glLineWidth(lineWidth);
  glBegin(GL_LINES);
  glVertex3f(from.x(),from.y(),from.z());
  glVertex3f(to.x(),to.y(),to.z());
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
  mGridColor(204,204,217),
  mLabelColor(0,0,0),
  mShowLowerTicks(false),
  mShowUpperTicks(false),
  mShowLeftTicks(false),
  mShowRightTicks(false),
  mTranslate(0.0)
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
  double step  = (maxValue-minValue)/5.0;

  double factor = pow(10.0, floor((log(step)/log(10.0))));

  double tmp = step/factor;
  if(tmp < 5) {
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
  glDisable(GL_POLYGON_OFFSET_FILL);


  double deltaX = mXTicks[1] - mXTicks[0];
  double deltaY = mYTicks[1] - mYTicks[0];

  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-1.0f,-1.0f);

  // Grid
  for (int i = 0; i < mXTicks.size(); i++) {
    if(mShowGrid) {      
      Draw3DLine(QVector3D(mXTicks[i],minY,0), QVector3D(mXTicks[i],maxY,0), 1, mGridColor);
      glColor4f(mLabelColor.redF(),mLabelColor.greenF(),mLabelColor.blueF(),mLabelColor.alphaF());
    }
    if(mShowAxis && mShowLowerTicks) {
      Draw3DLine(QVector3D(mXTicks[i],minY-0.3*deltaY,0), QVector3D(mXTicks[i],minY,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(mXTicks[i],minY-0.5*deltaY,0),QString("%1").arg(mXTicks[i],3,'f',1),10);
    }
    if(mShowAxis && mShowUpperTicks) {
      Draw3DLine(QVector3D(mXTicks[i],maxY,0), QVector3D(mXTicks[i],maxY+0.3*deltaY,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(mXTicks[i],maxY+0.5*deltaY,0),QString("%1").arg(mXTicks[i],3,'f',1),10);
    }
  }

  for (int i = 1;i < mYTicks.size(); i++) {
    if(mShowGrid) {      
      Draw3DLine(QVector3D(minX,mYTicks[i],0), QVector3D(maxX,mYTicks[i] ,0), 1, mGridColor);
      glColor4f(mLabelColor.redF(),mLabelColor.greenF(),mLabelColor.blueF(),mLabelColor.alphaF());
    }
    if(mShowAxis && mShowLeftTicks)  {
      Draw3DLine(QVector3D(minX-0.3*deltaX,mYTicks[i],0), QVector3D(minX,mYTicks[i] ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(minX-0.5*deltaX,mYTicks[i],0),QString("%1").arg(mYTicks[i],3,'f',1),10);
    }            
    if(mShowAxis && mShowRightTicks) {
      Draw3DLine(QVector3D(maxX,mYTicks[i],0), QVector3D(maxX+0.3*deltaX,mYTicks[i] ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(maxX+0.5*deltaX,mYTicks[i],0),QString("%1").arg(mYTicks[i],3,'f',1),10);
    }
  }	 

  // Polygon offset does not work with lines, manual offset to 
  // avoid z-fighting
  minX -= 0.01*deltaX;
  maxX += 0.01*deltaX;
  minY -= 0.01*deltaY;
  maxY += 0.01*deltaY;

  if(mShowAxis) {
    if(mShowLowerTicks) {
      Draw3DLine(QVector3D(minX,minY,0), QVector3D(maxX+0.5*deltaX,minY ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(0.5*(maxX+minX),minY-1.5*deltaY,0),mXLabel,12);
    }
    if(mShowUpperTicks) {
      Draw3DLine(QVector3D(minX,maxY,0), QVector3D(maxX+0.5*deltaX,maxY ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(0.5*(maxX+minX),maxY+1.5*deltaY,0),mXLabel,12);
    }
    if(mShowLeftTicks)  {
      Draw3DLine(QVector3D(minX,minY,0), QVector3D(minX, maxY+0.5*deltaY ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(minX-1.5*deltaX,0.5*(maxY+minY),0),mYLabel,12);   
    }
    if(mShowRightTicks) {
      Draw3DLine(QVector3D(maxX,minY,0), QVector3D(maxX, maxY+0.5*deltaY ,0), 1, mLabelColor);
      mPlot->renderTextAtWorldCoordinates(QVector3D(maxX+1.5*deltaX,0.5*(maxY+minY),0),mYLabel,12);   
    }
  }


  glDisable(GL_POLYGON_OFFSET_LINE);

}

void QAxis::draw() const {
  if(mXTicks.isEmpty()) return;
  if(mYTicks.isEmpty()) return;

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

  if(mShowAxisBox) {
    glTranslatef(0,0,-mTranslate);
    Draw3DLine(QVector3D(mXTicks[0],mYTicks[0],mZTicks[0]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[0]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[0]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[0]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[0]), QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[0]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[0]), QVector3D(mXTicks[0],mYTicks[0],mZTicks[0]),1,mLabelColor);
    
    Draw3DLine(QVector3D(mXTicks[0],mYTicks[0],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[mZTicks.size()-1]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[0],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[mXTicks.size()-1],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]),1,mLabelColor);
    Draw3DLine(QVector3D(mXTicks[0],mYTicks[mYTicks.size()-1],mZTicks[mZTicks.size()-1]), QVector3D(mXTicks[0],mYTicks[0],mZTicks[mZTicks.size()-1]),1,mLabelColor);
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
  mAxisEqual(false)
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

}

QPlot3D::~QPlot3D() {
}

void QPlot3D::initializeGL() {
  qglClearColor(mBackgroundColor);
    
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  // TOOD: Replace by sub-pixel polygon drawing instead...
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

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
  // Draw box
  Draw2DPlane(QVector2D(x0,y0),             QVector2D(x0+tWidth,y0+tHeight),QColor(204,204,217,128));
  // Draw contour lines
  Draw2DLine(QVector2D(x0,y0),                QVector2D(x0+tWidth,y0),         1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0+tWidth,y0),         QVector2D(x0+tWidth,y0+tHeight), 1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0+tWidth,y0+tHeight), QVector2D(x0,y0+tHeight),        1, QColor(0,0,0,255));
  Draw2DLine(QVector2D(x0,y0+tHeight),        QVector2D(x0,y0),                1, QColor(0,0,0,255));  
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
    renderText((int)x0,(int)y0,mCurves[i]->name());
  }


}

void QPlot3D::drawTextBox(int x, int y, QString string)  {
  const double textWidth  = fontMetrics().width(string);
  const double textHeight = fontMetrics().height();

  enable2D();
  Draw2DPlane(QVector2D(x-5,y+5), QVector2D(x+10+textWidth, y-textHeight-5), QColor(204,204,217,128));
  disable2D();

  QFont tFont  = font();
  tFont.setPixelSize(10);
  setFont(tFont);
  glColor4f(0,0,0,1);
  renderText(x,y,string);
}  


void QPlot3D::enable2D() {
  glPushMatrix();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,width(),height(),0,0,1);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glDisable(GL_DEPTH_TEST);
}

void QPlot3D::disable2D() {
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  resizeGL(width(),height());
}

void QPlot3D::resizeGL(int width, int height) {
  glViewport(0,0,width,height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double zNear = 0.01;
  double zFar  = 1000.0;
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
  } else if (event->buttons() & Qt::RightButton) {
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

void QPlot3D::renderTextAtWorldCoordinates(const QVector3D& vec, QString str, int fontSize) {
  QFont tFont  = font();
  tFont.setPixelSize(fontSize);
  setFont(tFont);
  QVector3D sVec = toScreenCoordinates(vec);
  renderText(sVec.x(),sVec.y(),str);  
}

QVector3D QPlot3D::toScreenCoordinates(const QVector3D& vec) {
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


