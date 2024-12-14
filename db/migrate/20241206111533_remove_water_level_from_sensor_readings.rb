class RemoveWaterLevelFromSensorReadings < ActiveRecord::Migration[8.0]
  def change
    remove_column :sensor_readings, :water_level, :float
  end
end
