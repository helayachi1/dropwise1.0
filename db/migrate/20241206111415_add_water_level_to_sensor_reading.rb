class AddWaterLevelToSensorReading < ActiveRecord::Migration[8.0]
  def change
    add_column :sensor_readings, :water_level, :float
  end
end
