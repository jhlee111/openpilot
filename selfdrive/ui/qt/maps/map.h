#pragma once

#include <optional>

#include <QGeoCoordinate>
#include <QGeoManeuver>
#include <QGeoRouteRequest>
#include <QGeoRouteSegment>
#include <QGeoRoutingManager>
#include <QGeoServiceProvider>
#include <QGestureEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMapboxGL>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScopedPointer>
#include <QString>
#include <QtGlobal>
#include <QTimer>
#include <QWheelEvent>
#include <QMap>
#include <QPixmap>

#include "cereal/messaging/messaging.h"
class MapInstructions : public QWidget {
  Q_OBJECT

private:
  QLabel *distance;
  QLabel *primary;
  QLabel *secondary;
  QLabel *icon_01;
  QHBoxLayout *lane_layout;
  QMap<QString, QVariant> last_banner;

public:
  MapInstructions(QWidget * parent=nullptr);

public slots:
  void updateDistance(float d);
  void updateInstructions(QMap<QString, QVariant> banner);
};

class MapETA : public QWidget {
  Q_OBJECT

private:
  QLabel *eta;
  QLabel *time;
  QLabel *time_unit;
  QLabel *distance;
  QLabel *distance_unit;

public:
  MapETA(QWidget * parent=nullptr);

public slots:
  void updateETA(float seconds, float seconds_typical, float distance);
};

class MapWindow : public QOpenGLWidget {
  Q_OBJECT

public:
  MapWindow(const QMapboxGLSettings &);
  ~MapWindow();

private:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) override;

  QMapboxGLSettings m_settings;
  QScopedPointer<QMapboxGL> m_map;

  void initLayers();

  void mousePressEvent(QMouseEvent *ev) final;
  void mouseMoveEvent(QMouseEvent *ev) final;
  void wheelEvent(QWheelEvent *ev) final;
  bool event(QEvent *event) final;
  bool gestureEvent(QGestureEvent *event);
  void pinchTriggered(QPinchGesture *gesture);

  bool m_sourceAdded = false;
  SubMaster *sm;
  QTimer* timer;

  // Panning
  QPointF m_lastPos;
  int pan_counter = 0;
  int zoom_counter = 0;

  // Position
  QMapbox::Coordinate last_position = QMapbox::Coordinate(37.7393118509158, -122.46471285025565);
  std::optional<float> last_bearing;

  // Route
  bool gps_ok = false;
  QGeoServiceProvider *geoservice_provider;
  QGeoRoutingManager *routing_manager;
  QGeoRoute route;
  QGeoRouteSegment segment;

  MapInstructions* map_instructions;
  MapETA* map_eta;

  QMapbox::Coordinate nav_destination;
  double last_maneuver_distance = 1000;

  // Route recompute
  QTimer* recompute_timer;
  int recompute_backoff = 0;
  int recompute_countdown = 0;
  void calculateRoute(QMapbox::Coordinate destination);
  void clearRoute();
  bool shouldRecompute();
  void updateETA();

private slots:
  void timerUpdate();
  void routeCalculated(QGeoRouteReply *reply);
  void recomputeRoute();

public slots:
  void offroadTransition(bool offroad);

signals:
  void distanceChanged(float distance);
  void instructionsChanged(QMap<QString, QVariant> banner);
  void ETAChanged(float seconds, float seconds_typical, float distance);
};

