/**********************************************************************
 **
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

#ifndef __Q_3D_PLOT_H__
#define __Q_3D_PLOT_H__

#include <QtCore>
#include <QtOpenGL>


/*!

 */

class QPlot3D;

/*!
  Class that represents a 3D range (similar to a bounding box).
  The class defines a minimum point and a maximal point in 3D space.
*/
class QRange {
 public:
  QRange();
  QRange(double _min, double _max);
  QVector3D center() const;
  QVector3D delta() const;
  void setIfMin(QVector3D in);
  void setIfMax(QVector3D in);
  void setIfMin(QRange in) { setIfMin(in.min); }
  void setIfMax(QRange in) { setIfMax(in.max); }
  QVector3D min;
  QVector3D max;
};

/*!
  The QCurve3D class is a container for the data representing a 3D-curve. 
  The class also encaspulates attributes associated with the 

  Example: 
  \code
  // Setup the curve
  QCurve aCurve("Simple Line");
  aCurve.addData(0.0, 0.0, 0.0);
  aCurve.addData(1.0, 1.0, 1.0);
  aCurve.setColor(Qt::green);
  aCurve.setLineWidth(3);

  // Add the curve to the plot
  mPlot->addCurve(&aCurve);

  // Change the value of the last point
  aCurve[aCurve.size()-1].setZ(3.0);
  
  \endcode
 */
class QCurve3D: public QObject{
  Q_OBJECT
   friend class QPlot3D;

 public:
  QCurve3D();
  QCurve3D(QString name);

  // Getters
  QColor color() const { return mColor; }
  double lineWidth() const { return mLineWidth; }
  QVector3D& value(int index)  { return mVertices[index]; }
  const QVector3D& value(int index) const { return mVertices[index]; }
  QRange range() const { return mRange; }
  QString name() const { return mName;}

  // Setters
  void setColor(QColor color) { mColor = color; }
  void setLineWidth(int value) { mLineWidth = value; }
  void setName(QString name) { mName = name; }

  // Misc
  void addData(const double& x, const double& y, const double& z);
  void addData(const QVector<double>& x, const QVector<double>& y, const QVector<double>& z);
  void addData(const QVector<QVector3D>& data);
  void addData(const QVector3D& data);
  void clear() { mVertices.clear(); }
  int  size() const { return mVertices.size(); }

  // Operators
  QVector3D& operator[](int i);  
  const QVector3D& operator[](int i) const;  

 protected:
  void draw() const;

 private:
  QString mName;
  QColor  mColor;
  int     mLineWidth;

  QVector<QVector3D> mVertices;
  QVector<GLushort>  mFaces;  
  QRange mRange;

};

/*!
  Class that represents the drawable axis plane.

  Example:
  \code
  // Don't draw plane
  mPlot->yAxis().setShowPlane(false);
  

  \endcode
 */
class QAxis: public QObject  {
  Q_OBJECT
  friend class QPlot3D;
 public:
  QAxis();
  enum Axis{
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2
  };

  void setRange(QRange range);
  void setAxis(Axis axis) { mAxis = axis; }
  void setAdjustPlaneView(bool value) { mAdjustPlaneView = value; }
  void setShowPlane(bool value) {mShowPlane = value; }
  void setShowGrid(bool value)  {mShowGrid = value; }
  void setShowAxis(bool value)  {mShowAxis = value; }
  void setShowLabel(bool value) {mShowLabel = value; }
  void setShowAxisBox(bool value)  {mShowAxisBox = value; }
  void setPlaneColor(QColor color) { mPlaneColor = color; }
  void setGridColor(QColor color) { mGridColor = color; }
  void setLabelColor(QColor color) { mLabelColor = color; }
  void setLabelFont(QFont font) { mLabelFont = font; }
  void setTicksFont(QFont font) { mTicksFont = font; }

  QRange range() const { return mRange; }
  bool   showPlane() const  {return mShowPlane; }
  bool   showGrid()  const  {return mShowGrid; }
  bool   showAxis()  const  {return mShowAxis; }
  bool   showLabel()  const  {return mShowLabel; }
  QString xLabel() const { return mXLabel; }
  QString yLabel() const { return mYLabel; }
  QColor planeColor() const { return mPlaneColor; }
  QColor gridColor() const { return mGridColor; }
  QColor labelColor() const { return mLabelColor; }
  QFont labelFont() const { return mLabelFont; }
  QFont ticksFont() const { return mTicksFont; }

 public slots:
  void adjustPlaneView();

  void togglePlane() {mShowPlane = !mShowPlane; }
  void toggleGrid()  {mShowGrid = !mShowGrid; }
  void toggleAxis()  {mShowAxis = !mShowAxis; }
  void toggleLabel() {mShowLabel = !mShowLabel; }
  void toggleAxisBox()  {mShowAxisBox = !mShowAxisBox; }
  void toggleAdjustView()  {mAdjustPlaneView = !mAdjustPlaneView; }

 protected: 
  void draw() const;
  void drawAxisBox() const;
  void setPlot(QPlot3D* plot) { mPlot = plot; }
  void setXLabel(QString label) { mXLabel = label; }
  void setYLabel(QString label) { mYLabel = label; }
  double mScale;
  
 private:
  void drawAxisPlane() const;
  void drawAxis(QVector<double> ticksX, QVector3D min, QVector3D max, QColor color,QVector3D normal) const;
  QVector<double> getTicks(double min, double max) const;
  void setVisibleTicks(bool lower, bool right, bool upper, bool left);
  void drawXTickLabel( QVector3D start, QVector3D stop, QString string ) const;

 private:
  QPlot3D* mPlot;
  QRange mRange;
  Axis  mAxis;
  bool mAdjustPlaneView, mShowPlane, mShowGrid, mShowAxis, mShowLabel, mShowAxisBox;
  QString mXLabel, mYLabel;
  QColor mPlaneColor, mGridColor, mLabelColor;
  QVector<double> mXTicks, mYTicks, mZTicks;
  bool  mShowLowerTicks, mShowUpperTicks, mShowLeftTicks, mShowRightTicks;
  double mTranslate;
  QFont mLabelFont, mTicksFont;
};

/*!
  Class that represents the plot window.
  A QPlot3D is a continer for all the curves, axes and legend and responsible for adding curves, removeing curves and drawing curve, axes, and legends.

  User interactions like rotation, zooming and panning  with the mouse is included int QPlot3D.

  Example: 
  \code
  // Setup a plot
  QPlot3D mPlot;

  // Setup the curve
  QCurve aCurve("Simple Line");
  aCurve.addData(0.0, 0.0, 0.0);
  aCurve.addData(1.0, 1.0, 1.0);
  aCurve.setColor(Qt::green);
  aCurve.setLineWidth(3);

  // Add the curve to the plot
  mPlot.addCurve(&aCurve);
  
  // Remove legends
  mPlot.setShowLegend(false);
  
  // Change view
  mPlot.setAzimuth(45.0)
  mPlot.setElevation(45.0)
  
  // Raise the plot window.
  mPlot.show();
  mPlot.setFocus();

  
 */
class QPlot3D: public QGLWidget {
  Q_OBJECT
  friend class QAxis;
 public:
  QPlot3D(QWidget* parent=NULL);
  ~QPlot3D();

  void addCurve(QCurve3D* curve);
  bool removeCurve(QCurve3D* curve);
  void clear() { mCurves.clear(); }
  void setBackgroundColor(QColor color);
  void setLegendFont(QFont font) { mLegendFont = font; }
  QFont legendFont() const { return mLegendFont; }
    
  double    zoom()  const { return mTranslate.z(); }
  QVector3D pan()   const { return mTranslate;     }
  QColor    background() const { return mBackgroundColor; }
  double    azimuth() const { 
      double tAzimuth = -mRotation.z();
      return tAzimuth - floor(tAzimuth/360.0)*360.0;
  }
  double    elevation() const { 
    if (mRotation.x() > 180)
      return  mRotation.x() - floor(mRotation.x()/90.0)*90.0;
    if (mRotation.x() < -180)
      return  mRotation.x() - floor(mRotation.x()/90.0)*90.0;
    return mRotation.x();
  }
  QAxis&    xAxis() { return mXAxis; }
  QAxis&    yAxis() { return mYAxis; }
  QAxis&    zAxis() { return mZAxis; }



 public slots:
   void setZoom(double value)   { if(value < 0.0) mTranslate.setZ(value); updateGL(); }
   void setPan(QVector3D value) { mTranslate = value;     updateGL(); }
   void setShowAzimuthElevation(bool value) { mShowAzimuthElevation = value; }
   void setAzimuth(double value)   { mRotation.setZ(-value); }
   void setElevation(double value) { mRotation.setX(value); }
   void setShowLegend(bool value)  { mShowLegend = value; }
   void setAxisEqual(bool value)   { mAxisEqual = value; rescaleAxis();}
   void setShowAxis(bool value);
   void setShowAxisBox(bool value);
   void setShowGrid(bool value);
   void setXLabel(QString str) { mXAxis.setXLabel(str); mZAxis.setYLabel(str);}
   void setYLabel(QString str) { mYAxis.setXLabel(str); mXAxis.setYLabel(str);}
   void setZLabel(QString str) { mZAxis.setXLabel(str); mYAxis.setYLabel(str);}
   void setAdjustPlaneView(bool value) { mXAxis.setAdjustPlaneView(value);mYAxis.setAdjustPlaneView(value), mZAxis.setAdjustPlaneView(value);}
   void showContextMenu(const QPoint&);
   void toggleAxisEqual() {setAxisEqual(!mAxisEqual);}
   void replot() {updateGL();}

 private:
   double roll()  const { return mRotation.x();  }
   double pitch() const { return mRotation.y();  }
   double yaw()   const { return mRotation.z();  }
   void   drawLegend();
   void   enable2D();
   void   disable2D();
   void   draw3DLine(QVector3D from, QVector3D to, double lineWidth, QColor color);

 private slots:
   void setRoll(double value)   { mRotation.setX(value);  updateGL(); }
   void setPitch(double value)  { mRotation.setY(value);  updateGL(); }
   void setYaw(double value)    { mRotation.setZ(value);  updateGL(); }
   void rescaleAxis();
   void axisEqual();
   void axisTight();
   
 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  QRect textSize(QString) const;
  QVector3D toScreenCoordinates(const QVector3D& worldCoord) const ;
  QVector3D toScreenCoordinates(double worldX, double worldY, double worldZ) const ;
  void      renderTextAtWorldCoordinates(const QVector3D& vec, QString string, QFont font = QFont("Helvetica"));
  void      renderTextAtScreenCoordinates(int x, int y, QString string, QFont font = QFont("Helvetica"));
  QVector3D cameraPositionInWorldCoordinates() const;
  void   drawTextBox(int x, int y, QString string, QFont font = QFont("Helvetica"));

 private:
   QList<QCurve3D*> mCurves;
   QPoint mLastMousePos;
   QColor mBackgroundColor;

   QVector3D mTranslate;
   QVector3D mRotation;
   QVector3D mScale;

   bool mShowAzimuthElevation, mShowLegend, mAxisEqual;
   QAxis mXAxis, mYAxis, mZAxis;
   QFont mLegendFont;
};

#endif
