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

  // ui->pushButtonClear->setIcon(LoadSvg(":/resources/svg/clear.svg", theme));
  return true;
}

void ToolboxRoundness::calculateRoundness()
{

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
  // const QMimeData* mimeData = event->mimeData();
  // QStringList mimeFormats = mimeData->formats();

  // for (const QString& format : mimeFormats)
  // {
  //   QByteArray encoded = mimeData->data(format);
  //   QDataStream stream(&encoded, QIODevice::ReadOnly);

  //   if (format != "curveslist/add_curve")
  //   {
  //     return;
  //   }

  //   QStringList curves;
  //   while (!stream.atEnd())
  //   {
  //     QString curve_name;
  //     stream >> curve_name;
  //     if (!curve_name.isEmpty())
  //     {
  //       curves.push_back(curve_name);
  //     }
  //   }
  //   _dragging_curves = curves;
  //   event->accept();
  // }
}

void ToolboxRoundness::onDropEvent(QDropEvent*)
{
  // _zoom_range.min = std::numeric_limits<double>::lowest();
  // _zoom_range.max = std::numeric_limits<double>::max();

  // for (auto& curve : _dragging_curves)
  // {
  //   std::string curve_id = curve.toStdString();
  //   PlotData& curve_data = _plot_data->getOrCreateNumeric(curve_id);

  //   _plot_widget_A->addCurve(curve_id, curve_data);
  //   _curve_names.push_back(curve_id);
  //   _zoom_range.min = std::min(_zoom_range.min, curve_data.front().x);
  //   _zoom_range.max = std::max(_zoom_range.max, curve_data.back().x);
  // }

  // ui->pushButtonSave->setEnabled(true);
  // ui->pushButtonCalculate->setEnabled(true);
  // ui->lineEditSuffix->setEnabled(true);

  // _dragging_curves.clear();
  // _plot_widget_A->resetZoom();
}

void ToolboxRoundness::onViewResized(const QRectF& rect)
{
  // _zoom_range.min = rect.left();
  // _zoom_range.max = rect.right();
}
