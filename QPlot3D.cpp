#include "QPlot3D.h"
#include <limits>
#include <iostream>
#include <fstream>
#include "OpenGL/glu.h"

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
  if(data.x() < mRange.min.x()) mRange.min.setX( data.x());
  if(data.y() < mRange.min.y()) mRange.min.setY( data.y());
  if(data.z() < mRange.min.z()) mRange.min.setZ( data.z());

  if(data.x() > mRange.max.x()) mRange.max.setX( data.x());
  if(data.y() > mRange.max.y()) mRange.max.setY( data.y());
  if(data.z() > mRange.max.z()) mRange.max.setZ( data.z());

  mVertices.push_back(data);
  mFaces.push_back(mVertices.size()-1);
}

QVector3D&  QCurve3D::operator[](int i) {
  return getValue(i);
}

const QVector3D&  QCurve3D::operator[](int i) const {
  return getValue(i);
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
  mShowPlane(true),
  mShowGrid(true),
  mShowAxis(true),
  mShowLabel(true),
  mLabel(""),
  mPlaneColor(230,230,242),
  mGridColor(204,204,217),
  mAxisColor(0,0,0),
  mLabelColor(0,0,0)
{
}

void QAxis::drawPlane(double minX, double minY, double maxX, double maxY) const  {

  
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f,1.0f);

  // Plane
  if(mShowPlane) {
    glColor4f(mPlaneColor.redF(), mPlaneColor.greenF(),mPlaneColor.blueF(),mPlaneColor.alphaF());
    glBegin(GL_QUADS);
    glVertex3f(minX,minY,0);
    glNormal3f(0,0,1);
    glVertex3f(maxX,minY,0);
    glNormal3f(0,0,1);
    glVertex3f(maxX,maxY,0);
    glNormal3f(0,0,1);
    glVertex3f(minX,maxY,0);
    glNormal3f(0,0,1);
    glEnd();
  }


  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-1.0f,-1.0f);
  // Grid
  if(mShowGrid) {
    glColor4f(mGridColor.redF(), mGridColor.greenF(),mGridColor.blueF(),mGridColor.alphaF());
    glLineWidth(1);
    glBegin(GL_LINES);
    double tStep = (maxX-minX)/10.0;
    for (double x =minX+tStep; x < maxX; x +=tStep) {
      glVertex3f(x,minY,0);
      glVertex3f(x,maxY,0);
    }
    tStep = (maxY-minY)/10.0;
    for (double y = minY+tStep; y < maxY; y +=tStep) {
      glVertex3f(minX,y,0);
      glVertex3f(maxX,y,0);
    }	 
    glEnd();
  }

  // Axis
  if(mShowAxis) {
    glColor4f(mAxisColor.redF(), mAxisColor.greenF(),mAxisColor.blueF(),mAxisColor.alphaF());
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f(minX,minY,0);
    glVertex3f(maxX+ abs(maxX-minX)*0.1,minY,0);
    glEnd();
  }


  glDisable(GL_POLYGON_OFFSET_LINE);
  glDisable(GL_POLYGON_OFFSET_FILL);

  if( mShowLabel) {
    glColor4f(mLabelColor.redF(),mLabelColor.greenF(),mLabelColor.blueF(),mLabelColor.alphaF());
    mPlot->renderTextAtWorldCoord(QVector3D(maxX + abs(maxX-minX)*0.1,minY,0),mLabel);
  }

}

void QAxis::draw() const {
  glPushMatrix();

  QVector3D minRange = mRange.min;
  QVector3D maxRange = mRange.max;
  // glTranslatef(minRange.x(),minRange.y(),minRange.z());

  if(mAxis == X_AXIS) {
    glTranslatef(0,0,minRange.z());
    drawPlane(minRange.x(),minRange.y(), maxRange.x(),maxRange.y());
  } else if (mAxis == Y_AXIS) {
    glTranslatef(minRange.x(),0,0);
    glRotatef(90, 1,0,0);
    glRotatef(90, 0,1,0);    
    drawPlane(minRange.y(),minRange.z(), maxRange.y(),maxRange.z());
  } else {
    glTranslatef(0,minRange.y(),0);
    glRotatef(-90, 1,0,0);
    glRotatef(-90, 0,0,1);    
    drawPlane(minRange.z(),minRange.x(), maxRange.z(),maxRange.x());
  }

  glPopMatrix();

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
  mXAxis.setLabel("x");
  mYAxis.setAxis(QAxis::Y_AXIS);
  mYAxis.setPlot(this);
  mYAxis.setLabel("y");
  mZAxis.setAxis(QAxis::Z_AXIS);
  mZAxis.setPlot(this);
  mZAxis.setLabel("z");
  
}

QPlot3D::~QPlot3D() {
}

void QPlot3D::initializeGL() {
  qglClearColor(mBackgroundColor);
    
  glEnable(GL_DEPTH_TEST);
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
  
  QVector3D tCenter = mXAxis.getRange().getCenter();
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
    drawTextBox(10,height()-15,QString("Az: %1 El: %2").arg(-mRotation.z(),3,'f',1).arg(mRotation.x(),3,'f',1));
  }

  
}

void QPlot3D::drawLegend(){
  double textWidth  = 0;
  double textHeight = 0;
  int nrCurves = mCurves.size();
  for (int i = 0; i < nrCurves; i++) {
    QString string = mCurves[i]->getName();
    if(fontMetrics().width(string) > textWidth) textWidth = fontMetrics().width(string);
    if(fontMetrics().height() > textHeight) textHeight = fontMetrics().height();
  }


  double tWidth  = 5 + 20 + 5 + textWidth + 5;
  double tHeight = 5 + nrCurves*textHeight + 5;
  double x0 = width()-tWidth-5; 
  double y0 = 5;
  enable2D();

  glColor4f(0.8,0.8,0.85,0.5);
  // glColor4f(0.5,0.5,0.5,0.5);
  glBegin(GL_QUADS);
  glVertex2f(x0,y0);
  glVertex2f(x0+tWidth, y0);
  glVertex2f(x0+tWidth, y0+tHeight);
  glVertex2f(x0,y0+tHeight);
  glEnd();

  glColor4f(0,0,0,1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x0,y0);
  glVertex2f(x0+tWidth, y0);
  glVertex2f(x0+tWidth, y0+tHeight);
  glVertex2f(x0,y0+tHeight);
  glVertex2f(x0,y0);
  glEnd();

  disable2D();

  y0 = 10;

  for (int i = 0; i < nrCurves; i++) {

    x0 = width()-tWidth-5; 

    QColor tColor = mCurves[i]->getColor();
    enable2D();
    glColor4f(tColor.redF(),tColor.greenF(),tColor.blueF(),1);
    glLineWidth(mCurves[i]->getLineWidth());
    glBegin(GL_LINES);
    glVertex2f(x0+5,y0+0.5*textHeight);
    glVertex2f(x0+5+20,y0+0.5*textHeight);
    glEnd();
    disable2D();
    glLineWidth(1);

    x0 += 30;
    glColor4f(0,0,0,1);
    y0 += textHeight;
    renderText((int)x0,(int)y0,mCurves[i]->getName());
  }


}

void QPlot3D::drawTextBox(int x, int y, QString string)  {
  const double textWidth = fontMetrics().width(string);
  const double textHeight = fontMetrics().height();

  enable2D();
  glColor4f(0.8,0.8,0.85,0.5);
  // glColor4f(0.5,0.5,0.5,0.5);
  glBegin(GL_QUADS);
  glVertex2f(x-5           ,y-textHeight-5);
  glVertex2f(x+10+textWidth,y-textHeight-5);
  glVertex2f(x+10+textWidth,y+5);
  glVertex2f(x-5           ,y+5);
  glEnd();
  disable2D();

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
      setPitch(getPitch() + dx);
    }else {
      setRoll(getRoll() + dy);
      setYaw(getYaw() + dx);
    }
  } else if (event->buttons() & Qt::RightButton) {
    setPan(getPan() + QVector3D((double)dx/32,-(double)dy/32,0.0));
  }
  mLastMousePos = event->pos();
}

void QPlot3D::mouseDoubleClickEvent(QMouseEvent* event) {
  mAxisEqual = !mAxisEqual;
  rescaleAxis();
}

void QPlot3D::wheelEvent(QWheelEvent* event) 
{
  event->accept();  
  setZoom( getZoom() + (double)event->delta()/32);
}

void QPlot3D::addCurve(QCurve3D* curve) { 
  mCurves.push_back(curve);  

  // Readjust axes
  QRange tRange = mXAxis.getRange();
  if(curve->getRange().min.x() < tRange.min.x() ) tRange.min.setX(curve->getRange().min.x());
  if(curve->getRange().min.y() < tRange.min.y() ) tRange.min.setY(curve->getRange().min.y());
  if(curve->getRange().min.z() < tRange.min.z() ) tRange.min.setZ(curve->getRange().min.z());
  if(curve->getRange().max.x() > tRange.max.x() ) tRange.max.setX(curve->getRange().max.x());
  if(curve->getRange().max.y() > tRange.max.y() ) tRange.max.setY(curve->getRange().max.y());
  if(curve->getRange().max.z() > tRange.max.z() ) tRange.max.setZ(curve->getRange().max.z());
  
  mXAxis.setRange(tRange);
  mYAxis.setRange(tRange);
  mZAxis.setRange(tRange);

  // Rescale plot
  rescaleAxis();
  updateGL();
} 

void QPlot3D::rescaleAxis() {
  if (mAxisEqual) 
    axisEqual();
  else 
    axisTight();
}

void QPlot3D::axisEqual() {
  QRange tRange = mXAxis.getRange();

  QVector3D delta(tRange.max.x()-tRange.min.x(),tRange.max.y()-tRange.min.y(),tRange.max.z()-tRange.min.z());
  double k = std::max(delta.x(),std::max(delta.y(),delta.z()));
  mScale.setX( 10.0/k);
  mScale.setY( 10.0/k);
  mScale.setZ( 10.0/k);
  updateGL();
}

void QPlot3D::axisTight() {
  QRange tRange = mXAxis.getRange();

  QVector3D delta(tRange.max.x()-tRange.min.x(),tRange.max.y()-tRange.min.y(),tRange.max.z()-tRange.min.z());
  mScale.setX( 10.0/delta.x());
  mScale.setY( 10.0/delta.y());
  mScale.setZ( 10.0/delta.z());
  updateGL();
}

void QPlot3D::renderTextAtWorldCoord(const QVector3D& vec, QString str) {
  QVector3D sVec = toScreenCoord(vec);
  renderText(sVec.x(),sVec.y(),str);  
}

QVector3D QPlot3D::toScreenCoord(const QVector3D& vec) {
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
  double g1 = p[0]*f1 + p[4]*f2 + p[8]*f3 + p[12]*f4;
  double g2 = p[1]*f1 + p[5]*f2 + p[9]*f3 + p[13]*f4;
  double g3 = p[2]*f1 + p[6]*f2 + p[10]*f3 + p[14]*f4;
  double g4 = -f3;

  if(g4 == 0.0) return QVector3D(0.0,0.0,0.0);
  g1 /= g4;
  g2 /= g4;
  g3 /= g4;
       
  return QVector3D( (g1*0.5+0.5)*viewPort[2] + viewPort[0], 
		    height() - ((g2*0.5+0.5)*viewPort[3] + viewPort[1]),
		    (1.0+g3)*0.5);
      
}

void QPlot3D::hideAxes() {
  mXAxis.setShowPlane(false);
  mXAxis.setShowGrid(false);
  mXAxis.setShowAxis(false);
  mXAxis.setShowLabel(false);

  mYAxis.setShowPlane(false);
  mYAxis.setShowGrid(false);
  mYAxis.setShowAxis(false);
  mYAxis.setShowLabel(false);

  mZAxis.setShowPlane(false);
  mZAxis.setShowGrid(false);
  mZAxis.setShowAxis(false);
  mZAxis.setShowLabel(false);
}

void QPlot3D::showAxes() {
  mXAxis.setShowPlane(true);
  mXAxis.setShowGrid(true);
  mXAxis.setShowAxis(true);
  mXAxis.setShowLabel(true);

  mYAxis.setShowPlane(true);
  mYAxis.setShowGrid(true);
  mYAxis.setShowAxis(true);
  mYAxis.setShowLabel(true);

  mZAxis.setShowPlane(true);
  mZAxis.setShowGrid(true);
  mZAxis.setShowAxis(true);
  mZAxis.setShowLabel(true);
}
