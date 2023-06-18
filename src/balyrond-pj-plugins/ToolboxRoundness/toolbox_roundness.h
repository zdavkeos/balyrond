#pragma once

#include <QtPlugin>
#include <thread>
#include "PlotJuggler/toolbox_base.h"
#include "PlotJuggler/plotwidget_base.h"

namespace Ui
{
class toolbox_roundness;
}

class ToolboxRoundness : public PJ::ToolboxPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.Toolbox")
  Q_INTERFACES(PJ::ToolboxPlugin)

public:
  ToolboxRoundness();

  ~ToolboxRoundness() override;

  const char* name() const override
  {
    return "Roundness Analysis";
  }

  void init(PJ::PlotDataMapRef& src_data, PJ::TransformsMap& transform_map) override;

  std::pair<QWidget*, WidgetType> providedWidget() const override;

public slots:

  bool onShowWidget() override;

private:
  QWidget* _widget;
  Ui::toolbox_roundness* ui;


  QStringList _dragging_curves;

  PJ::PlotWidgetBase* _plot_widget_A = nullptr;
  PJ::PlotWidgetBase* _plot_widget_B = nullptr;

  PJ::PlotDataMapRef* _plot_data = nullptr;
  PJ::TransformsMap* _transforms = nullptr;

  PJ::PlotDataMapRef _local_data;

  Range _zoom_range;

  std::vector<std::string> _curve_names;

  PJ::PlotDataXY& curve_dist;
  PJ::PlotDataXY& curve_min;
  PJ::PlotDataXY& curve_max;
  PJ::PlotDataXY& curve_mcz;
  PJ::PlotDataXY& curve_lstsq;

  void calculateRoundness();

private slots:

  void onDragEnterEvent(QDragEnterEvent* event);
  void onDropEvent(QDropEvent* event);
  void onViewResized(const QRectF& rect);
  void onClearCurves();
  void formatToggle();
  void curvesToggled();
};
