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

ToolboxRoundness::ToolboxRoundness()
{
  _widget = new QWidget(nullptr);
  ui = new Ui::toolbox_fft;

  ui->setupUi(_widget);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ToolboxPlugin::closed);

  connect(ui->pushButtonCalculate, &QPushButton::clicked, this,
          &ToolboxRoundness::calculateCurveRoundness);

  connect(ui->pushButtonSave, &QPushButton::clicked, this, &ToolboxRoundness::onSaveCurve);

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
  _plot_widget_B = new PJ::PlotWidgetBase(ui->framePlotPreviewB);

  auto preview_layout_A = new QHBoxLayout(ui->framePlotPreviewA);
  preview_layout_A->setMargin(6);
  preview_layout_A->addWidget(_plot_widget_A);

  auto preview_layout_B = new QHBoxLayout(ui->framePlotPreviewB);
  preview_layout_B->setMargin(6);
  preview_layout_B->addWidget(_plot_widget_B);

  _plot_widget_A->setAcceptDrops(true);

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

  ui->pushButtonClear->setIcon(LoadSvg(":/resources/svg/clear.svg", theme));
  return true;
}

void ToolboxRoundness::calculateCurveRoundness()
{
  _plot_widget_B->removeAllCurves();

  for (const auto& curve_id : _curve_names)
  {
    auto it = _plot_data->numeric.find(curve_id);
    if (it == _plot_data->numeric.end())
    {
      return;
    }
    PlotData& curve_data = it->second;

    if( curve_data.size() == 0)
    {
      return;
    }

    size_t min_index = 0;
    size_t max_index = curve_data.size() - 1;

    if (ui->radioZoomed->isChecked())
    {
      min_index = curve_data.getIndexFromX(_zoom_range.min);
      max_index = curve_data.getIndexFromX(_zoom_range.max);
    }

    size_t N = 1 + max_index - min_index;

    if (N & 1)
    {  // if not even, make it even
      N--;
      max_index--;
    }

    if (N < 8)
    {
      return;
    }

    double dT = (curve_data.at(max_index).x - curve_data.at(min_index).x) / double(N - 1);

    std::vector<kiss_fft_scalar> input;
    input.reserve(curve_data.size());

    double sum = 0;
    if(ui->checkAverage->isChecked())
    {
      for (size_t i = 0; i < N; i++)
      {
        sum += curve_data[i + min_index].y;
      }
    }
    double average = sum / double(N);

    for (size_t i = 0; i < N; i++)
    {
      size_t index = i + min_index;
      const auto& p = curve_data[index];
      input.push_back(static_cast<kiss_fft_scalar>(p.y - average));

      if (i != 0)
      {
        double dTi = (p.x - curve_data[index - 1].x);
        double diff = dTi - dT;
        // std_dev += diff*diff;
      }
    }
    // std_dev = sqrt(std_dev / double(N-1) );

    std::vector<kiss_fft_cpx> out(N / 2 + 1);

    auto config = kiss_fftr_alloc(N, false, nullptr, nullptr);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    kiss_fftr(config, input.data(), out.data());
    QApplication::restoreOverrideCursor();

    auto& curver_fft = _local_data.getOrCreateScatterXY(curve_id);
    curver_fft.clear();
    for (size_t i = 0; i < N / 2; i++)
    {
      kiss_fft_scalar Hz = i * (1.0 / dT) / double(N);
      kiss_fft_scalar amplitude = std::hypot(out[i].r, out[i].i) / N;
      curver_fft.pushBack({ Hz, amplitude });
    }

    QColor color = Qt::transparent;
    auto colorHint = curve_data.attribute(COLOR_HINT);
    if (colorHint.isValid())
    {
      color = colorHint.value<QColor>();
    }

    _plot_widget_B->addCurve(curve_id + "_Roundness", curver_fft, color);

    free(config);
  }

  _plot_widget_B->resetZoom();
}

void ToolboxRoundness::onClearCurves()
{
  _plot_widget_A->removeAllCurves();
  _plot_widget_A->resetZoom();

  _plot_widget_B->removeAllCurves();
  _plot_widget_B->resetZoom();

  ui->pushButtonSave->setEnabled(false);
  ui->pushButtonCalculate->setEnabled(false);

  ui->lineEditSuffix->setEnabled(false);
  ui->lineEditSuffix->setText("_Roundness");

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

  ui->pushButtonSave->setEnabled(true);
  ui->pushButtonCalculate->setEnabled(true);
  ui->lineEditSuffix->setEnabled(true);

  _dragging_curves.clear();
  _plot_widget_A->resetZoom();
}

void ToolboxRoundness::onViewResized(const QRectF& rect)
{
  _zoom_range.min = rect.left();
  _zoom_range.max = rect.right();
}

void ToolboxRoundness::onSaveCurve()
{
  auto suffix = ui->lineEditSuffix->text().toStdString();
  if (suffix.empty())
  {
    ui->lineEditSuffix->setText("_Roundness");
    suffix = "_Roundness";
  }
  for (const auto& curve_id : _curve_names)
  {
    auto it = _local_data.scatter_xy.find(curve_id);
    if (it == _local_data.scatter_xy.end())
    {
      continue;
    }
    auto& out_data = _plot_data->getOrCreateScatterXY(curve_id + suffix);
    out_data.clonePoints(it->second);

    // TODO out_data.setAttribute(PJ::DISABLE_LINKED_ZOOM, true);
    emit plotCreated(curve_id + suffix);
  }

  emit closed();
}
