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

#include <limits>

ToolboxRoundness::ToolboxRoundness()
{
  _widget = new QWidget(nullptr);
  ui = new Ui::toolbox_roundness;

  ui->setupUi(_widget);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ToolboxPlugin::closed);

  connect(ui->pushButtonCalculate, &QPushButton::clicked, this,
          &ToolboxRoundness::calculateRoundness);

  connect(ui->pushButtonClear, &QPushButton::clicked, this, &ToolboxRoundness::onClearCurves);

  connect(ui->radioButtonLines, &QRadioButton::toggled, this, &ToolboxRoundness::formatToggle);
  connect(ui->radioButtonDots, &QRadioButton::toggled, this, &ToolboxRoundness::formatToggle);
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
  _plot_widget_B = new PJ::PlotWidgetBase(ui->framePlotPreviewB);

  auto preview_layout_A = new QHBoxLayout(ui->framePlotPreviewA);
  preview_layout_A->setMargin(6);
  preview_layout_A->addWidget(_plot_widget_A);

  auto preview_layout_B = new QHBoxLayout(ui->framePlotPreviewB);
  preview_layout_B->setMargin(6);
  preview_layout_B->addWidget(_plot_widget_B);

  _plot_widget_A->setAcceptDrops(true);
  _plot_widget_B->setAcceptDrops(false);

  _plot_widget_B->setModeXY(true);
  _plot_widget_B->setKeepRatioXY(true);

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
    _plot_widget_B->removeAllCurves();

    auto it = _plot_data->numeric.find("angle");
    if (it == _plot_data->numeric.end())
    {
        QMessageBox::warning(nullptr, tr("Didn't find 'angle'"),
                             QString("Couldn't angle"),
                             QMessageBox::Ok);
        return;
    }
    PlotData& angle_data = it->second;

    it = _plot_data->numeric.find("distance");
    if (it == _plot_data->numeric.end())
    {
        QMessageBox::warning(nullptr, tr("Didn't find 'distance'"),
                             QString("Couldn't distance"),
                             QMessageBox::Ok);
        return;
    }
    PlotData& dist_data = it->second;

    size_t min_index = 0;
    size_t max_index = angle_data.size() - 1;

    if (ui->radioZoomed->isChecked())
    {
        min_index = angle_data.getIndexFromX(_zoom_range.min);
        max_index = angle_data.getIndexFromX(_zoom_range.max);
    }

    auto& curve_dist = _local_data.getOrCreateScatterXY("distance_polar");
    curve_dist.clear();

    auto& curve_avg = _local_data.getOrCreateScatterXY("average_polar");
    curve_avg.clear();

    auto& curve_min = _local_data.getOrCreateScatterXY("min_polar");
    curve_min.clear();

    auto& curve_max = _local_data.getOrCreateScatterXY("max_polar");
    curve_max.clear();

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    double avg = 0;
    size_t t = 1;
    for (size_t i = min_index; i < max_index; i++)
    {
        min = dist_data[i].y < min ? dist_data[i].y : min;
        max = dist_data[i].y > max ? dist_data[i].y : max;

        const auto& a = angle_data[i].y * (M_PI / 180.0);
        const auto& d = dist_data[i].y;
        avg += (d - avg) / t++;
        curve_dist.pushBack({ d * ::cos(a), d * ::sin(a) });
    }

    for (double a = 0.0; a <= (2*M_PI); a += .1)
    {
        curve_min.pushBack({ min * ::cos(a), min * ::sin(a) });
        curve_max.pushBack({ max * ::cos(a), max * ::sin(a) });
        curve_avg.pushBack({ avg * ::cos(a), avg * ::sin(a) });
    }

    QColor color = Qt::transparent;
    auto colorHint = dist_data.attribute(COLOR_HINT);
    if (colorHint.isValid())
    {
        color = colorHint.value<QColor>();
    }

    ui->label_min->setText(QString::number(min));
    ui->label_max->setText(QString::number(max));
    ui->label_avg->setText(QString::number(avg));

    _plot_widget_B->addCurve("distance_polar", curve_dist, color);
    _plot_widget_B->addCurve("min_polar", curve_min);
    _plot_widget_B->addCurve("max_polar", curve_max);
    _plot_widget_B->addCurve("average_polar", curve_avg);
    _plot_widget_B->resetZoom();
}

void ToolboxRoundness::onClearCurves()
{
  _plot_widget_A->removeAllCurves();
  _plot_widget_A->resetZoom();

  _plot_widget_B->removeAllCurves();
  _plot_widget_B->resetZoom();

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

void ToolboxRoundness::formatToggle()
{
    if (_plot_widget_B)
    {
        if (ui->radioButtonDots->isChecked())
        {
            _plot_widget_B->changeCurvesStyle(PlotWidgetBase::DOTS);
        } else {
            _plot_widget_B->changeCurvesStyle(PlotWidgetBase::LINES);
        }
    }
}
