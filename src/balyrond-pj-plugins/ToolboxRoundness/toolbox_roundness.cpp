#include "toolbox_roundness.h"
#include "ui_toolbox_roundness.h"

#include <QDialogButtonBox>
#include <QEvent>
#include <QMimeData>
#include <QDebug>
#include <QDragEnterEvent>
#include <QSettings>

#include <Eigen/Dense>

#include "PlotJuggler/transform_function.h"
#include "PlotJuggler/svg_util.h"

#include <limits>

ToolboxRoundness::ToolboxRoundness() :
  curve_dist(_local_data.getOrCreateScatterXY("distance_polar")),
  curve_avg(_local_data.getOrCreateScatterXY("average_polar")),
  curve_min(_local_data.getOrCreateScatterXY("min_polar")),
  curve_max(_local_data.getOrCreateScatterXY("max_polar")),
  curve_lstsq(_local_data.getOrCreateScatterXY("least_squares"))
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

  connect(ui->checkBox_MCC, &QCheckBox::toggled, this, &ToolboxRoundness::curvesToggled);
  connect(ui->checkBox_MIC, &QCheckBox::toggled, this, &ToolboxRoundness::curvesToggled);
  connect(ui->checkBox_mean, &QCheckBox::toggled, this, &ToolboxRoundness::curvesToggled);
  connect(ui->checkBox_lsc, &QCheckBox::toggled, this, &ToolboxRoundness::curvesToggled);
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

    auto curve_iter = _plot_widget_A->curveList().begin();
    auto& angle_name = (*curve_iter++).src_name;
    auto it = _plot_data->numeric.find(angle_name);
    if (it == _plot_data->numeric.end())
    {
        QMessageBox::warning(nullptr, tr("Didn't find 'angle' timeseries"),
                             QString("Couldn't angle"),
                             QMessageBox::Ok);
        return;
    }
    PlotData& angle_data = it->second;

    auto& dist_name = (*curve_iter).src_name;
    it = _plot_data->numeric.find(dist_name);
    if (it == _plot_data->numeric.end())
    {
        QMessageBox::warning(nullptr, tr("Didn't find 'distance' timeseries"),
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

    curve_dist.clear();
    curve_avg.clear();
    curve_min.clear();
    curve_max.clear();
    curve_lstsq.clear();

    std::vector<std::tuple<double, double>> asdf;
    for (size_t i = min_index; i <= max_index; i++)
    {
        const auto& a = angle_data[i].y * (M_PI / 180.0);
        const auto& d = dist_data[i].y;

        double x = d * ::cos(a);
        double y = d * ::sin(a);
        curve_dist.pushBack({ x, y });
        asdf.push_back({ x, y });
    }

    auto lsqf = leastSquaresCircleFit(asdf);
    double xc = std::get<0>(lsqf);
    double yc = std::get<1>(lsqf);
    double r = std::get<2>(lsqf);

    // now that we know the true center,
    // we can find the min and max circles
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    double dfts = std::numeric_limits<double>::min(); // Deviation From True Circle
    double avg = 0;
    size_t t = 1;
    for (size_t i = 0; i < curve_dist.size(); i++)
    {
        double x = curve_dist.at(i).x;
        double y = curve_dist.at(i).y;
        double d = ::sqrt((x-xc)*(x-xc) + (y-yc)*(y-yc)) - r;
    
        min = y < min ? y : min;
        max = y > max ? y : max;
        dfts = ::abs(d) > dfts ? ::abs(d) : dfts;
        avg += (y - avg) / t++;
    }

    for (double a = 0.0; a <= (2*M_PI); a += .1)
    {
        curve_min.pushBack({ min * ::cos(a), min * ::sin(a) });
        curve_max.pushBack({ max * ::cos(a), max * ::sin(a) });
        curve_avg.pushBack({ avg * ::cos(a), avg * ::sin(a) });
        curve_lstsq.pushBack({r*::cos(a) + xc, r*::sin(a) + yc});
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
    ui->label_center->setText(QString::number(xc) + "," + QString::number(yc));
    ui->label_dfts->setText(QString::number(dfts));

    _plot_widget_B->addCurve("Measured", curve_dist, color);
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

  ui->label_min->setText(QString::number(0.0));
  ui->label_max->setText(QString::number(0.0));
  ui->label_avg->setText(QString::number(0.0));
  ui->label_center->setText(QString::number(0.0) + "," + QString::number(0.0));
  ui->label_dfts->setText(QString::number(0.0));

  ui->checkBox_MCC->setCheckState(Qt::Unchecked);
  ui->checkBox_MIC->setCheckState(Qt::Unchecked);
  ui->checkBox_mean->setCheckState(Qt::Unchecked);
  ui->checkBox_lsc->setCheckState(Qt::Unchecked);
  
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

void ToolboxRoundness::curvesToggled()
{
  if (_plot_widget_B)
  {
    if (ui->checkBox_MCC->isChecked())
    {
      _plot_widget_B->addCurve("MCC", curve_max);
    } else {
      _plot_widget_B->removeCurve("MCC");
    }

    if (ui->checkBox_MIC->isChecked())
    {
      _plot_widget_B->addCurve("MIC", curve_min);
    } else {
      _plot_widget_B->removeCurve("MIC");
    }

    if (ui->checkBox_mean->isChecked())
    {
      _plot_widget_B->addCurve("Mean", curve_avg);
    } else {
      _plot_widget_B->removeCurve("Mean");
    }

    if (ui->checkBox_lsc->isChecked())
    {
      _plot_widget_B->addCurve("LSC", curve_lstsq);
    } else {
      _plot_widget_B->removeCurve("LSC");
    }

    _plot_widget_B->resetZoom();
  }
}

std::tuple<double, double, double>
ToolboxRoundness::leastSquaresCircleFit(std::vector<std::tuple<double, double>> data)
{
   // https://eigen.tuxfamily.org/dox/group__LeastSquares.html
   // https://lucidar.me/en/mathematics/least-squares-fitting-of-circle/

    Eigen::MatrixXd A = Eigen::MatrixXd::Random(data.size(), 3);
    Eigen::VectorXd b = Eigen::VectorXd::Random(data.size());

    int row = 0;
    for (const auto& d : data)
    {
      double x = std::get<0>(d);
      double y = std::get<1>(d);

      A(row, 0) = x;
      A(row, 1) = y;
      A(row, 2) = 1.0;
      b(row) = x*x + y*y;
      row++;
    }

    Eigen::Vector3d s = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

    double xc = s(0) / 2.0;
    double yc = s(1) / 2.0;
    double r = ::sqrt(4*s(2) + s(0)*s(0) + s(1)*s(1)) / 2.0;

    // std::cout << xc << " " << yc << " " << r << "\n";

    return std::make_tuple(xc, yc, r);
}
