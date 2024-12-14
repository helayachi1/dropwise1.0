class SensorReadingsController < ApplicationController
	def index
	 	@Sensor_readings = SensorReading.all
	end
end
