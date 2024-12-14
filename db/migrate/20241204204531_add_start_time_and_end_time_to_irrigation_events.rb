class AddStartTimeAndEndTimeToIrrigationEvents < ActiveRecord::Migration[8.0]
  def change
    add_column :irrigation_events, :start_time, :datetime
    add_column :irrigation_events, :end_time, :datetime
  end
end
