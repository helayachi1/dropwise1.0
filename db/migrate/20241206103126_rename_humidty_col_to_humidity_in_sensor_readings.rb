class RenameHumidtyColToHumidityInSensorReadings < ActiveRecord::Migration[8.0]
  def change
        rename_column :sensor_readings, :humidty, :humidity
  end
end
