class CreateSensorReadings < ActiveRecord::Migration[8.0]
  def change
    create_table :sensor_readings do |t|
      t.float :temperature
      t.float :humidty

      t.timestamps
    end
  end
end
