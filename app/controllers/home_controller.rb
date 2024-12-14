class HomeController < ApplicationController
  def index
     @zones = Zone.all
     @sensor_readings = SensorReading.last
     @irrigation_events = IrrigationEvent.all
  end
end
