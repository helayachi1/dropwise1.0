class SensorReading < ApplicationRecord
	validates :temperature, numericality: true, allow_nil: true
	validates :humidity, numericality: true, allow_nil: true
end
