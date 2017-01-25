#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>

#include "Dem.h"
#include "Point3.h"

using namespace QtDataVisualization;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface, QString sDEMPath, QString sSaveDir)
    : m_graph(surface)
{
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);
    m_DEMProxy = new QSurfaceDataProxy();
    m_DEMSeries = new QSurface3DSeries(m_DEMProxy);
    populateDEMProxy(sDEMPath,sSaveDir);
}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}

void SurfaceGraph::populateDEMProxy(QString sDEMPath, QString sSaveDir)
{
    DEM dem(sDEMPath.toStdString());
    iColumns = dem.getsizey();
    iRows = dem.getsizex();
    Point3 * p3Points = dem.getpoints();
    DEM::bounds graphBounds = dem.calculatebounds();
    fGXmin = graphBounds.fXMin;
    fGXmax = graphBounds.fXMax;
    fGZmin = graphBounds.fYMin;
    fGZmax = graphBounds.fYMax;

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(iRows);

    //Setup file stream if point matrix is being saved

    QFile qfPointMatrix(sSaveDir);
    QTextStream tsPointSave(&qfPointMatrix);
    QString sLocName = QString::fromStdString(dem.getname());
    if(sSaveDir != "No")
    {
        qfPointMatrix.open(QIODevice::WriteOnly|QFile::Text);
        tsPointSave << sLocName << ": (x,y,z)" << endl << endl;
    }

    for (int i = 0; i < iRows; i++)
    {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(iColumns);
        int iArrayIndex = 0;
        for(int j = 0; j < iColumns; j++)
        {
            float x = p3Points[j*iRows + i].x;
            float z = p3Points[j*iRows + i].y; //Z values in DEM structure are Y in Qt Surface Map and vice versa
            float y = p3Points[j*iRows + i].z;

            //POINT MATRIX SAVE
            tsPointSave << "(" << QString::number(x) << "," << QString::number(z) << "," << QString::number(y) << ") ";
            if(j == iColumns -1)
                tsPointSave << endl;
            //END POINT MATRIX SAVE

            if(y == -32767) // Have renderer ignore null points
                y = 0;
            (*newRow)[iArrayIndex++].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }
    m_DEMProxy->resetArray(dataArray);
}

void SurfaceGraph::enableDEMModel()
{
        m_DEMSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
        m_DEMSeries->setFlatShadingEnabled(true);

        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");
        m_graph->axisX()->setRange(fGXmin, fGXmax);
        m_graph->axisY()->setAutoAdjustRange(true);
        m_graph->axisZ()->setRange(fGZmin, fGZmax);
        m_graph->axisX()->setLabelAutoRotation(30);
        m_graph->axisY()->setLabelAutoRotation(90);
        m_graph->axisZ()->setLabelAutoRotation(30);

        m_graph->addSeries(m_DEMSeries);

        // Reset range sliders for Sqrt&Sin
        m_rangeMinX = fGXmin;
        m_rangeMinZ = fGZmin;
        m_stepX = (fGXmax - fGXmin) / float(iRows - 1);
        m_stepZ = (fGZmax - fGZmin) / float(iColumns - 1);
        m_axisMinSliderX->setMaximum(iRows - 2);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMaximum(iRows - 1);
        m_axisMaxSliderX->setValue(iRows - 1);
        m_axisMinSliderZ->setMaximum(iColumns - 2);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMaximum(iColumns - 1);
        m_axisMaxSliderZ->setValue(iColumns - 1);
}

void SurfaceGraph::adjustXMin(int min)
{
    float minX = m_stepX * float(min) + m_rangeMinX;

    int max = m_axisMaxSliderX->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderX->setValue(max);
    }
    float maxX = m_stepX * max + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
    float maxX = m_stepX * float(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderX->setValue(min);
    }
    float minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
    float minZ = m_stepZ * float(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderZ->setValue(max);
    }
    float maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
    float maxX = m_stepZ * float(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderZ->setValue(min);
    }
    float minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

void SurfaceGraph::setAxisXRange(float min, float max)
{
    m_graph->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(float min, float max)
{
    m_graph->axisZ()->setRange(min, max);
}

void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

