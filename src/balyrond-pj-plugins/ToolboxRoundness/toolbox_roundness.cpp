#include "toolbox_roundness.h"
#include "ui_toolbox_roundness.h"

#include <QDialogButtonBox>
#include <QEvent>
#include <QMimeData>
#include <QDebug>
#include <QDragEnterEvent>
#include <QSettings>

#include "PlotJuggler/transform_function.h"
#include "PlotJuggler/svg_util.h"

#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_marker.h>
#include <qwt_scale_engine.h>


class PolarData: public QwtSeriesData<QwtPointPolar>
{
public:
    PolarData( const QwtInterval &radialInterval,
               const QwtInterval &azimuthInterval, size_t size ):
        d_radialInterval( radialInterval ),
        d_azimuthInterval( azimuthInterval ),
        d_size( size )
    {
    }

    virtual size_t size() const
    {
        return d_size;
    }

    virtual QwtPointPolar sample( size_t i ) const
    {
        const double stepA = 4 * d_azimuthInterval.width() / d_size;
        const double a = d_azimuthInterval.minValue() + i * stepA;
        
        const double stepR = d_radialInterval.width() / d_size;
        const double r = d_radialInterval.minValue() + i * stepR;
        
        return QwtPointPolar( a, r );
    }

    virtual QRectF boundingRect() const
    {
        // if ( d_boundingRect.width() < 0.0 )
        //     d_boundingRect = qwtBoundingRect( *this );
        
        // return d_boundingRect;
        return qwtBoundingRect( *this );
    }

protected:
    QwtInterval d_radialInterval;
    QwtInterval d_azimuthInterval;
    size_t d_size;
};

const QwtInterval radialInterval( 0.0, 10.0 );
const QwtInterval azimuthInterval( 0.0, 360.0 );

class PPlot : public QwtPolarPlot
{
  public:
  PPlot(QWidget* parent)
  {
        setAutoReplot( false );
    setPlotBackground( Qt::white );

    // scales
    setScale( QwtPolar::Azimuth,
        azimuthInterval.minValue(), azimuthInterval.maxValue(),
        azimuthInterval.width() / 12 );

    setScaleMaxMinor( QwtPolar::Azimuth, 2 );
    setScale( QwtPolar::Radius,
        radialInterval.minValue(), radialInterval.maxValue() );

    // grids, axes

    d_grid = new QwtPolarGrid();
    d_grid->setPen( QPen( Qt::white ) );
    for ( int scaleId = 0; scaleId < 1; scaleId++ )
    {
        d_grid->showGrid( scaleId );
        d_grid->showMinorGrid( scaleId );

        QPen minorPen( Qt::gray );
#if 0
        minorPen.setStyle( Qt::DotLine );
#endif
        d_grid->setMinorGridPen( scaleId, minorPen );
    }
    d_grid->setAxisPen( QwtPolar::AxisAzimuth, QPen( Qt::black ) );

    d_grid->showAxis( QwtPolar::AxisAzimuth, true );
    d_grid->showAxis( QwtPolar::AxisLeft, false );
    d_grid->showAxis( QwtPolar::AxisRight, true );
    d_grid->showAxis( QwtPolar::AxisTop, true );
    d_grid->showAxis( QwtPolar::AxisBottom, false );
    d_grid->showGrid( QwtPolar::Azimuth, true );
    d_grid->showGrid( QwtPolar::Radius, true );
    d_grid->attach( this );

    QwtLegend *legend = new QwtLegend;
    insertLegend( legend,  QwtPolarPlot::BottomLegend );
  }

  private:
   QwtPolarGrid *d_grid;
};

ToolboxRoundness::ToolboxRoundness()
{
  _widget = new QWidget(nullptr);
  ui = new Ui::toolbox_roundness;

  ui->setupUi(_widget);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ToolboxPlugin::closed);

  connect(ui->pushButtonCalculate, &QPushButton::clicked, this,
          &ToolboxRoundness::calculateRoundness);

  connect(ui->pushButtonClear, &QPushButton::clicked, this, &ToolboxRoundness::onClearCurves);
}

ToolboxRoundness::~ToolboxRoundness()
{
  delete ui;
}

void ToolboxRoundness::init(PJ::PlotDataMapRef& src_data, PJ::TransformsMap& transform_map)
{
  _plot_data = &src_data;
  _transforms = &transform_map;

  _plot_widget_A = new PJ::PlotWidgetBase(ui->framePlotPreviewA);
  _plot_widget_B = new PPlot(ui->framePlotPreviewB);

  auto preview_layout_A = new QHBoxLayout(ui->framePlotPreviewA);
  preview_layout_A->setMargin(6);
  preview_layout_A->addWidget(_plot_widget_A);

  auto preview_layout_B = new QHBoxLayout(ui->framePlotPreviewB);
  preview_layout_B->setMargin(6);
  preview_layout_B->addWidget(_plot_widget_B);

  _plot_widget_A->setAcceptDrops(true);

  _plot_widget_B->setAutoReplot();

  connect(_plot_widget_A, &PlotWidgetBase::dragEnterSignal, this,
          &ToolboxRoundness::onDragEnterEvent);

  connect(_plot_widget_A, &PlotWidgetBase::dropSignal, this, &ToolboxRoundness::onDropEvent);

  connect(_plot_widget_A, &PlotWidgetBase::viewResized, this, &ToolboxRoundness::onViewResized);
}

std::pair<QWidget*, PJ::ToolboxPlugin::WidgetType> ToolboxRoundness::providedWidget() const
{
  return { _widget, PJ::ToolboxPlugin::FIXED };
}

bool ToolboxRoundness::onShowWidget()
{
  QSettings settings;
  QString theme = settings.value("StyleSheet::theme", "light").toString();

  return true;
}

void ToolboxRoundness::calculateRoundness()
{
    QwtPolarCurve *curve = new QwtPolarCurve();
    curve->setStyle( QwtPolarCurve::Lines );

    auto data = new PolarData(QwtInterval( 0.0, 10.0 ), QwtInterval( 0.0, 360.0 ), 1);

    curve->setData(data);

    curve->attach(_plot_widget_B);

    curve->setVisible(true);


}

void ToolboxRoundness::onClearCurves()
{
  _plot_widget_A->removeAllCurves();
  _plot_widget_A->resetZoom();

  // _plot_widget_B->removeAllCurves();
  _plot_widget_B->unzoom();

  ui->pushButtonCalculate->setEnabled(false);

  _curve_names.clear();
}

void ToolboxRoundness::onDragEnterEvent(QDragEnterEvent* event)
{
  const QMimeData* mimeData = event->mimeData();
  QStringList mimeFormats = mimeData->formats();

  for (const QString& format : mimeFormats)
  {
    QByteArray encoded = mimeData->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    if (format != "curveslist/add_curve")
    {
      return;
    }

    QStringList curves;
    while (!stream.atEnd())
    {
      QString curve_name;
      stream >> curve_name;
      if (!curve_name.isEmpty())
      {
        curves.push_back(curve_name);
      }
    }
    _dragging_curves = curves;
    event->accept();
  }
}

void ToolboxRoundness::onDropEvent(QDropEvent*)
{
  _zoom_range.min = std::numeric_limits<double>::lowest();
  _zoom_range.max = std::numeric_limits<double>::max();

  for (auto& curve : _dragging_curves)
  {
    std::string curve_id = curve.toStdString();
    PlotData& curve_data = _plot_data->getOrCreateNumeric(curve_id);

    _plot_widget_A->addCurve(curve_id, curve_data);
    _curve_names.push_back(curve_id);
    _zoom_range.min = std::min(_zoom_range.min, curve_data.front().x);
    _zoom_range.max = std::max(_zoom_range.max, curve_data.back().x);
  }

  if (_curve_names.size() > 1)
  {
    ui->pushButtonCalculate->setEnabled(true);
  }

  _dragging_curves.clear();
  _plot_widget_A->resetZoom();
}

void ToolboxRoundness::onViewResized(const QRectF& rect)
{
  _zoom_range.min = rect.left();
  _zoom_range.max = rect.right();
}
